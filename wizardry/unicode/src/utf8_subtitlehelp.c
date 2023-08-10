#include "subtitlehelp.h"

#include "hardware.h"
#include "text.h"

#include "constants/videoalloc_global.h"

#include "nat-macros.h"

LYN_REPLACE_CHECK(InitSubtitleHelpText);
void InitSubtitleHelpText(struct SubtitleHelpProc * proc)
{
    // what changed: we use a prev variable to restore 'it' instead of subtracting 2
    // this makes this functions become encoding-agnostic

    static int const text_width = DISPLAY_WIDTH - 16;

    int line;

    char const * it = proc->string;

    InitSpriteTextFont(&proc->font, OBJ_VRAM0 + OBJCHR_SUBTITLEHELP_TEXT * CHR_SIZE, 0x10 + OBJPAL_SUBTITLEHELP_TEXT);
    SetTextFontGlyphs(TEXT_GLYPHS_TALK);

    ApplyPalette(Pal_Text + 0x10, 0x10 + OBJPAL_SUBTITLEHELP_TEXT);

    for (line = 0; line < 2; ++line)
    {
        InitSpriteText(proc->text + line);

        SpriteText_DrawBackgroundExt(proc->text + line, 0);
        Text_SetColor(proc->text + line, TEXT_COLOR_0123);
    }

    line = 0;

    while (*it > 1)
    {
        char const * prev = it;
        it = Text_DrawCharacter(proc->text + line, it);

        if (Text_GetCursor(proc->text + line) > text_width)
        {
            i32 width;

            it = prev;
            line += 1;

            GetCharTextLen(it, &width);

            Text_SetCursor(proc->text + line, Text_GetCursor(proc->text + 0) + 32 - width - text_width);
        }
    }

    proc->text_count = ((GetStringTextLen(proc->string) + 16) >> 5) + 1;
    proc->text_num = proc->text_count - 1;

    SetTextFont(NULL);
}
