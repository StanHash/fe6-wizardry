#include "text.h"

#include "stan-failscreen.h"

struct UnicodeGlyphEnt
{
    u32 character;
    struct Glyph const* glyph;
};

struct UnicodeFontInfo
{
    struct UnicodeGlyphEnt const* beg;
    struct UnicodeGlyphEnt const* end;
};

extern struct UnicodeFontInfo const UnicodeFontInfoTable[];

u32 Utf8DecodeCharacter(char const** strptr)
{
    u32 byte_0, byte_1, byte_2, byte_3;

    byte_0 = *(*strptr)++;

    switch (byte_0 & 0xF0)
    {
        default:
            return byte_0;

        case 0x80: case 0x90: case 0xA0: case 0xB0:
            // continuation byte
            goto error;

        case 0xC0: case 0xD0:
            byte_0 = 0x1F & byte_0;
            byte_1 = 0x3F & *(*strptr)++;

            return (byte_0 << 6) + (byte_1);

        case 0xE0:
            byte_0 = 0x0F & byte_0;
            byte_1 = 0x3F & *(*strptr)++;
            byte_2 = 0x3F & *(*strptr)++;

            return (byte_0 << 12) + (byte_1 << 6) + (byte_2);

        case 0xF0:
            if ((byte_0 & 0x08) != 0)
            {
                goto error;
            }

            byte_0 = 0x07 & byte_0;
            byte_1 = 0x3F & *(*strptr)++;
            byte_2 = 0x3F & *(*strptr)++;
            byte_3 = 0x3F & *(*strptr)++;

            return (byte_0 << 18) + (byte_1 << 12) + (byte_2 << 6) + (byte_3);
    }

error:
    DebugPrintStr("==============================\n");
    DebugPrintStr("====  UTF-8 DECODE ERROR  ====\n");
    DebugPrintStr("==============================\n");
    DebugPrintStr(" Failed to decode character\n");
    DebugPrintStr(" Bytes found: ");
    DebugPrintNumberHex(byte_0, 2);
    DebugPrintStr(" ");
    DebugPrintNumberHex(**strptr, 2);
    DebugPrintStr(" ...\n");
    DebugPrintStr(" Address: ");
    DebugPrintNumberHex(((int) (*strptr)) - 1, 7);
    DebugPrintStr("\n");
    FailScreen();
}

struct Glyph const* Utf8GetGlyph(u32 character)
{
    struct UnicodeFontInfo const* unicode_font_info = (struct UnicodeFontInfo const*) gActiveFont->glyphs;

    // binary search!

    // with vanilla glyph sets, this seems to take a maximum of 10 iterations to find the correct glyph
    // which is I think pretty good? a hash table may still be faster idk.

    // int iterations = 0;

    u32 l = 0;
    u32 r = (unicode_font_info->end - unicode_font_info->beg) - 1;
    struct UnicodeGlyphEnt const* a = unicode_font_info->beg;

    while (l <= r)
    {
        u32 m = (l + r) / 2;

        if (a[m].character < character)
        {
            l = m + 1;
        }
        else if (a[m].character > character)
        {
            r = m - 1;
        }
        else
        {
            // DebugPrintStr("Utf8GetGlyph iterations: ");
            // DebugPrintNumber(iterations, 3);
            // DebugPrintStr("\n");

            return a[m].glyph;
        }

        // iterations++;
    }

    // failed to find character in sorted list

    DebugPrintStr("==============================\n");
    DebugPrintStr("=====  UTF-8 DRAW ERROR  =====\n");
    DebugPrintStr("==============================\n");
    DebugPrintStr(" Failed to draw character\n");
    DebugPrintStr(" No glyph found\n");
    DebugPrintStr(" Character: U+");
    DebugPrintNumberHex(character, (character < 0x100 ? 2 : (character < 0x1000 ? 3 : 4)));
    DebugPrintStr("\n");
    FailScreen();
}

// replaces
void SetTextFontGlyphs(int glyph_set)
{
    gActiveFont->glyphs = (struct Glyph const* const*) &UnicodeFontInfoTable[glyph_set];
}

// replaces
char const* GetCharTextLen(char const* str, int* out_width)
{
    u32 character = Utf8DecodeCharacter(&str);

    if (character < 0x20)
    {
        *out_width = 0;
        return str;
    }

    *out_width = Utf8GetGlyph(character)->width;

    return str;
}

// replaces
int GetStringTextLen(char const* str)
{
    int result = 0;

    while (*str > 1)
    {
        int tmp;
        str = GetCharTextLen(str, &tmp);

        result += tmp;
    }

    return result;
}

// replaces
char const* Text_DrawCharacter(struct Text* text, char const* str)
{
    u32 character = Utf8DecodeCharacter(&str);

    gActiveFont->draw_glyph(text, Utf8GetGlyph(character));

    return str;
}

// replaces
void Text_DrawString(struct Text* text, char const* str)
{
    while (*str > 1)
        str = Text_DrawCharacter(text, str);
}
