
#include "unit.h"

int GetUnitAid(struct Unit* unit)
{
    if (UNIT_ATTRIBUTES(unit) & UNIT_ATTR_MOUNTED)
        return 25;

    return unit->pow;
}
