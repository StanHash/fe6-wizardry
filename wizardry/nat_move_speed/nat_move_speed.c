#include "bm.h"
#include "mu.h"
#include "unit.h"

#include "nat-macros.h"

static u16 const s_walk_speeds[2] = {
    4 << 4, // normal
    2 << 4, // slow
};

LYN_REPLACE_CHECK(GetMuQ4MovementSpeed);
u16 GetMuQ4MovementSpeed(struct MuProc * mu)
{
    if (mu->move_config != 0)
        return mu->move_config;

    if (mu->fast_walk_b)
        return 16 << 4;

    if (gPlaySt.config_walk_speed == 0)
    {
        return s_walk_speeds[GetJInfo(mu->jid)->walk_speed];
    }

    return 8 << 4;
}
