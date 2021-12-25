#include "text.h"

#include "stan-failscreen.h"

struct UnicodePageInfo
{
    u32 beg, end;
    struct Glyph const* const* glyph_lut;
};

extern struct UnicodePageInfo const* const UnicodePageInfoTable[];

u32 Utf8DecodeCharacter(char const** strptr)
{
    u32 b0, b1, b2, b3;

    b0 = *(*strptr)++;

    switch (b0 & 0xF0)
    {
        default:
            return b0;

        case 0x80: case 0x90: case 0xA0: case 0xB0:
            // continuation byte
            goto error;

        case 0xC0: case 0xD0:
            b0 = 0x1F & b0;
            b1 = 0x3F & *(*strptr)++;

            return (b0 << 6) + (b1);

        case 0xE0:
            b0 = 0x0F & b0;
            b1 = 0x3F & *(*strptr)++;
            b2 = 0x3F & *(*strptr)++;

            return (b0 << 12) + (b1 << 6) + (b2);

        case 0xF0:
            if ((b0 & 0x08) != 0)
            {
                goto error;
            }

            b0 = 0x07 & b0;
            b1 = 0x3F & *(*strptr)++;
            b2 = 0x3F & *(*strptr)++;
            b3 = 0x3F & *(*strptr)++;

            return (b0 << 18) + (b1 << 12) + (b2 << 6) + (b3);
    }

error:
    DebugPrintStr("==============================\n");
    DebugPrintStr("====  UTF-8 DECODE ERROR  ====\n");
    DebugPrintStr("==============================\n");
    DebugPrintStr(" Failed to decode character\n");
    DebugPrintStr(" Bytes found: ");
    DebugPrintNumberHex(b0, 2);
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
    struct UnicodePageInfo const* unicode_page_info = (struct UnicodePageInfo const*) gActiveFont->glyphs;

    while (unicode_page_info->glyph_lut != NULL)
    {
        if (character >= unicode_page_info->beg && character <= unicode_page_info->end)
        {
            return unicode_page_info->glyph_lut[character - unicode_page_info->beg];
        }

        unicode_page_info++;
    }

    DebugPrintStr("==============================\n");
    DebugPrintStr("=====  UTF-8 DRAW ERROR  =====\n");
    DebugPrintStr("==============================\n");
    DebugPrintStr(" Failed to draw character\n");
    DebugPrintStr(" No valid glyph found\n");
    DebugPrintStr(" Character: U+");
    DebugPrintNumberHex(character, (character < 0x100 ? 2 : (character < 0x1000 ? 3 : 4)));
    DebugPrintStr("\n");
    FailScreen();
}

// replaces
void SetTextFontGlyphs(int glyph_set)
{
    gActiveFont->glyphs = (struct Glyph const* const*) UnicodePageInfoTable[glyph_set];
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

    if (character < 0x20)
        return str;

    gActiveFont->draw_glyph(text, Utf8GetGlyph(character));

    return str;
}

// replaces
void Text_DrawString(struct Text* text, char const* str)
{
    while (*str > 1)
        str = Text_DrawCharacter(text, str);
}
