#include "battle.h"
#include "unit.h"

#include "nat-macros.h"

enum
{
    STAT_HP,
    STAT_POW,
    STAT_SKL,
    STAT_SPD,
    STAT_DEF,
    STAT_RES,
    STAT_LCK,
    STAT_COUNT,
};

static inline int GrowthPointsToStat(int growth)
{
    // we are going to be doing some cheating
    // to scale growth points to stats, we are going to be doing (val * 0x28F6) >> 20 as an appromixation for dividing
    // by 100, which is slightly inaccurate. But 0.00018 extra stats for each gain shouldn't have too much of an impact.

    // I like the idea that it's fast, and this is supposed to be random anyway so who cares
    // (this is however not that necessary)

    return (growth * 0x28F6) >> 20;
}

static int GetFixedStatIncreaseBetween(int stat_level_from, int stat_level_to, int growth)
{
    int const from = GrowthPointsToStat(50 + growth * stat_level_from);
    int const to = GrowthPointsToStat(50 + growth * stat_level_to);

    return to - from;
}

static int GetFixedStatIncrease(int stat_level, int growth)
{
    return GetFixedStatIncreaseBetween(stat_level - 1, stat_level, growth);
}

static inline int GetUnitStatLevel(struct Unit * unit)
{
    int result = unit->level;

    // we add 20 if promoted so that the stat gains differ when promoted
    // otherwise level 1->2 unpromoted and level 1->2 promoted would gain the same stats, for example

    if ((UNIT_ATTRIBUTES(unit) & UNIT_ATTR_PROMOTED) != 0)
        result += 20;

    return result;
}

// replaces
LYN_REPLACE_CHECK(CheckBattleUnitLevelUp);
void CheckBattleUnitLevelUp(struct BattleUnit * bu)
{
    if (!CanBattleUnitGainExp(bu))
        return;

    if (bu->unit.exp >= 100)
    {
        // get stat level
        int const stat_level_before = GetUnitStatLevel(&bu->unit);

        bu->unit.exp -= 100;
        bu->unit.level++;

        if (bu->unit.level == UNIT_LEVEL_MAX)
        {
            bu->exp_gain -= bu->unit.exp;
            bu->unit.exp = 0xFF;
        }

        bu->change_hp = GetFixedStatIncrease(stat_level_before, bu->unit.pinfo->growth_hp);
        bu->change_pow = GetFixedStatIncrease(stat_level_before, bu->unit.pinfo->growth_pow);
        bu->change_skl = GetFixedStatIncrease(stat_level_before, bu->unit.pinfo->growth_skl);
        bu->change_spd = GetFixedStatIncrease(stat_level_before, bu->unit.pinfo->growth_spd);
        bu->change_def = GetFixedStatIncrease(stat_level_before, bu->unit.pinfo->growth_def);
        bu->change_res = GetFixedStatIncrease(stat_level_before, bu->unit.pinfo->growth_res);
        bu->change_lck = GetFixedStatIncrease(stat_level_before, bu->unit.pinfo->growth_lck);

        CheckBattleUnitStatCaps(GetUnit(bu->unit.id), bu);
    }
}

// replaces
LYN_REPLACE_CHECK(UnitAutolevelCore);
void UnitAutolevelCore(struct Unit * unit, fu8 jid, int level_count)
{
    if (level_count > 0)
    {
        struct JInfo const * const jinfo = GetJInfo(jid);

        // this isn't necessary going to cause the same results as if the stats were actually gained manually
        // but it's still deterministic so I don't really care
        // (because we assume those stats are going to be gained starting from the final level, rather than up to)

        int stat_level = unit->level;

        if ((jinfo->attributes & UNIT_ATTR_PROMOTED) != 0)
            stat_level += 20;

        unit->max_hp += GetFixedStatIncreaseBetween(stat_level, stat_level + level_count, unit->jinfo->growth_hp);
        unit->pow += GetFixedStatIncreaseBetween(stat_level, stat_level + level_count, unit->jinfo->growth_pow);
        unit->skl += GetFixedStatIncreaseBetween(stat_level, stat_level + level_count, unit->jinfo->growth_skl);
        unit->spd += GetFixedStatIncreaseBetween(stat_level, stat_level + level_count, unit->jinfo->growth_spd);
        unit->def += GetFixedStatIncreaseBetween(stat_level, stat_level + level_count, unit->jinfo->growth_def);
        unit->res += GetFixedStatIncreaseBetween(stat_level, stat_level + level_count, unit->jinfo->growth_res);
        unit->lck += GetFixedStatIncreaseBetween(stat_level, stat_level + level_count, unit->jinfo->growth_lck);
    }
}

// replaces
LYN_REPLACE_CHECK(UnitAutolevelPlayer);
void UnitAutolevelPlayer(struct Unit * unit)
{
    if (unit->pinfo->base_level < unit->level)
    {
        unit->max_hp += GetFixedStatIncreaseBetween(unit->pinfo->base_level, unit->level, unit->pinfo->growth_hp);
        unit->pow += GetFixedStatIncreaseBetween(unit->pinfo->base_level, unit->level, unit->pinfo->growth_pow);
        unit->skl += GetFixedStatIncreaseBetween(unit->pinfo->base_level, unit->level, unit->pinfo->growth_skl);
        unit->spd += GetFixedStatIncreaseBetween(unit->pinfo->base_level, unit->level, unit->pinfo->growth_spd);
        unit->def += GetFixedStatIncreaseBetween(unit->pinfo->base_level, unit->level, unit->pinfo->growth_def);
        unit->res += GetFixedStatIncreaseBetween(unit->pinfo->base_level, unit->level, unit->pinfo->growth_res);
        unit->lck += GetFixedStatIncreaseBetween(unit->pinfo->base_level, unit->level, unit->pinfo->growth_lck);
    }
}
