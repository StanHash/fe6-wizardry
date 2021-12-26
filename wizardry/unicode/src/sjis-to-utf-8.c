#include "msg.h"

#include "stan-failscreen.h"

#include <string.h>

extern char* const* const Utf8TranscoderMsgStringPtr;

struct SjisToUnicodeEnt
{
    /* 00 */ u16 single_char;
    /* 02 */ u8 byterange_beg, byterange_end;
    /* 04 */ u16 const* lut;
};

extern struct SjisToUnicodeEnt const Utf8TranscoderSjisToUnicodeTable[];

static u32 CvtSjis(char const** strptr)
{
    u32 b0 = *(*strptr)++;

    struct SjisToUnicodeEnt const* ent = &Utf8TranscoderSjisToUnicodeTable[b0];

    if (ent->single_char != 0)
    {
        return ent->single_char;
    }

    u32 b1 = *(*strptr)++;

    if (b1 < ent->byterange_beg || b1 > ent->byterange_end || ent->lut[b1] == 0)
    {
        DebugPrintStr("==============================\n");
        DebugPrintStr("===  UTF-8 TRANSCODE ERROR ===\n");
        DebugPrintStr("==============================\n");
        DebugPrintStr(" Failed to convert character\n");
        DebugPrintStr(" No Unicode character mapped\n");
        DebugPrintStr(" Bytes found: ");
        DebugPrintNumberHex(b0, 2);
        DebugPrintStr(" ");
        DebugPrintNumberHex(b1, 2);
        DebugPrintStr(" ...\n");
        DebugPrintStr(" Address: ");
        DebugPrintNumberHex(((int) (*strptr)) - 2, 7);
        DebugPrintStr("\n");
        FailScreen();
    }

    return ent->lut[b1];
}

static void WriteUtf8(char** dst, u32 character)
{
    switch (character)
    {
        case 0x00 ... 0x7F:
            *(*dst)++ = character;
            return;

        case 0x80 ... 0x7FF:
            *(*dst)++ = 0xC0 + ((character) >> 6);
            *(*dst)++ = 0x80 + ((character) & 0x3F);
            return;

        case 0x800 ... 0xFFFF:
            *(*dst)++ = 0xE0 + ((character >> 12));
            *(*dst)++ = 0x80 + ((character >> 6) & 0x3F);
            *(*dst)++ = 0x80 + ((character) & 0x3F);
            return;

        default:
            *(*dst)++ = 0xF0 + ((character >> 18));
            *(*dst)++ = 0x80 + ((character >> 12) & 0x3F);
            *(*dst)++ = 0x80 + ((character >> 6) & 0x3F);
            *(*dst)++ = 0x80 + ((character) & 0x3F);
            return;
    }
}

static void Sjis2Utf8(char* buf, int len)
{
    // int orig_len = strlen(buf) + 1;
    int orig_len = 0;
    while (buf[orig_len] != '\0')
    {
        orig_len += 1;
    }
    orig_len += 1;

    // memmove(buf + len - orig_len, buf, orig_len);
    for (int i = orig_len - 1; i >= 0; i--)
    {
        buf[len - orig_len + i] = buf[i];
    }

    char* dst = buf;
    char const* src = buf + len - orig_len;

    while (src != buf + len)
    {
        switch (*src)
        {
            case 0x00 ... 0x0F:
            case 0x11 ... 0x1C:
                *dst++ = *src++;
                break;

            case 0x10:
                *dst++ = *src++;
                *dst++ = *src++;
                *dst++ = *src++;
                break;

            case 0x80:
                *dst++ = *src++;

                switch (*src)
                {
                    case 0x00 ... 0x11:
                        *dst++ = *src++;
                        break;
                }

                break;

            default:
                WriteUtf8(&dst, CvtSjis(&src));
                continue;
        }
    }
}

// replaces
void DecodeStringRam(char const* src, char* dst)
{
    extern void(*DecodeStringRamFunc)(char const* src, char* dst);

    DecodeStringRamFunc(src, dst);

    if (dst == *Utf8TranscoderMsgStringPtr)
    {
        Sjis2Utf8(dst, 0x1000);
    }
}
