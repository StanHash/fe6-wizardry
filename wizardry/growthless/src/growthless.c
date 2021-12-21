#include "battle.h"

void NuCheckBattleUnitLevelUp(struct BattleUnit* bu)
{
    if (!CanBattleUnitGainExp(bu))
        return;

    if (bu->unit.exp >= 100)
    {
        bu->unit.exp -= 100;
        bu->unit.level++;

        if (bu->unit.level == UNIT_LEVEL_MAX)
        {
            bu->exp_gain -= bu->unit.exp;
            bu->unit.exp = 0xFF;
        }

        bu->change_hp = 0;
        bu->change_pow = 0;
        bu->change_skl = 0;
        bu->change_spd = 0;
        bu->change_def = 0;
        bu->change_res = 0;
        bu->change_lck = 0;

        CheckBattleUnitStatCaps(GetUnit(bu->unit.id), bu);
    }
}
