#include "save.h"
#include "save_core.h"

#include <assert.h>

#include "nat-macros.h"

LYN_REPLACE_CHECK(InitGlobalSaveInfo);
void InitGlobalSaveInfo(void)
{
    struct GlobalSaveInfo info;

    static_assert(sizeof(struct GlobalSaveInfo) == 0x20, "GlobalSaveInfo malformed");
    static_assert(offsetof(struct GlobalSaveInfo, cleared_playthroughs) == 0x10, "GlobalSaveInfo malformed");
    static_assert(offsetof(struct GlobalSaveInfo, checksum) == 0x1C, "GlobalSaveInfo malformed");

    WipeSram();

    info.name[0] = 'A';
    info.name[1] = 'G';
    info.name[2] = 'B';
    info.name[3] = '-';
    info.name[4] = 'F';
    info.name[5] = 'E';
    info.name[6] = '6';
    info.name[7] = '\0';
    info.magic32 = SAVE_MAGIC32;
    info.magic16 = SAVE_MAGIC16;

    info.completed = TRUE;
    info.completed_true = TRUE;
    info.completed_hard = TRUE;
    info.completed_true_hard = TRUE;
    info.unk_0E_4 = 0;
    info.last_suspend_slot = 0;
    info.last_game_save_id = 0;

    for (int i = 0; i < MAX_CLEARED_PLAYTHROUGHS; i++)
        info.cleared_playthroughs[i] = 1 + i;

    WriteGlobalSaveInfo(&info);
}
