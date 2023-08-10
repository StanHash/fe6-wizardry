#include "armfunc.h"
#include "faction.h"
#include "map.h"
#include "move.h"
#include "oam.h"
#include "sprite.h"
#include "trap.h"
#include "unit.h"
#include "unitsprite.h"
#include "util.h"

#include "constants/unitsprites.h"

#include "nat-macros.h"

enum
{
    UNITSPRITE_FRAME_A = 0,
    UNITSPRITE_FRAME_B = 32,
    UNITSPRITE_FRAME_C = 36,
    UNITSPRITE_FRAME_D = 68,
    UNITSPRITE_FRAME_END = 72,
};

#define hp_bar_info _pad_0x0A[0]

#define OBCHR_HPBARS 18

extern u8 const Img_HpBars[];

static void ApplyHpBarsGraphics(void)
{
    Decompress(Img_HpBars, gBuf);
    Copy2dChr(gBuf, OBJ_VRAM0 + CHR_SIZE * OBCHR_HPBARS, 6, 4);
}

LYN_REPLACE_CHECK(ApplySystemObjectsGraphics);
void ApplySystemObjectsGraphics(void)
{
    Decompress(Img_SystemObjects, gBuf);
    Copy2dChr(gBuf, OBJ_VRAM0 + CHR_SIZE * OBJCHR_SYSTEM_OBJECTS, 18, 4);

    ApplyPalettes(Pal_SystemObjects, 0x10 + OBJPAL_SYSTEM_OBJECTS, 2);

    // NOTE: if cloud weather, don't load hp bars graphics
    // because weather graphics take up all of the space

    if (gPlaySt.weather != WEATHER_CLOUDS)
        ApplyHpBarsGraphics();
}

LYN_REPLACE_CHECK(RefreshUnitSprites);
void RefreshUnitSprites(void)
{
    struct UnitSprite * map_sprite;

    struct Trap * trap;
    int i;

    gNextFreeUnitSprite = &gUnitSprites[0];

    gNextFreeUnitSprite->next = NULL;
    gNextFreeUnitSprite->y = 0x400;

    gNextFreeUnitSprite = &gUnitSprites[1];

    for (i = 1; i < FACTION_PURPLE + 6; ++i)
    {
        struct Unit * unit = GetUnit(i);

        if (!unit)
            continue;

        if (!unit->pinfo)
            continue;

        unit->map_sprite = NULL;

        if ((unit->flags & (UNIT_FLAG_HIDDEN | UNIT_FLAG_CONCEALED)) != 0)
            continue;

        if (gMapUnit[unit->y][unit->x] == 0)
            continue;

        map_sprite = AddUnitSprite(unit->y * 16);

        map_sprite->y = unit->y * 16;
        map_sprite->x = unit->x * 16;
        map_sprite->oam2 = UseUnitSprite(GetUnitMapSprite(unit)) + OAM2_PAL(GetUnitDisplayedSpritePalette(unit));
        map_sprite->hp_bar_info = gPlaySt.weather != WEATHER_CLOUDS ? 0x0B - Div(unit->hp * 0x0B, unit->max_hp) : 0;
        map_sprite->config = GetUnitSpriteInfo(GetUnitMapSprite(unit)).size;

        if ((unit->flags & UNIT_FLAG_SEEN) != 0)
            map_sprite->config += 3;

        unit->map_sprite = map_sprite;
    }

    for (trap = GetTrap(0); trap->kind != TRAP_NONE; ++trap)
    {
        u16 oam2;

        switch (trap->kind)
        {
            case TRAP_BALLISTA:
                oam2 = UseUnitSprite(UNITSPRITE_BALLISTA) - 0x4000;
                break;

            case TRAP_LONGBALLISTA:
                oam2 = UseUnitSprite(UNITSPRITE_LONGBALLISTA) - 0x4000;
                break;

            case TRAP_KILLERBALLISTA:
                oam2 = UseUnitSprite(UNITSPRITE_KILLERBALLISTA) - 0x4000;
                break;

            default:
                continue;
        }

        map_sprite = AddUnitSprite(trap->y * 16);

        map_sprite->y = trap->y * 16;
        map_sprite->x = trap->x * 16;
        map_sprite->oam2 = oam2;
        map_sprite->hp_bar_info = 0;
        map_sprite->config = GetUnitSpriteInfo(UNITSPRITE_BALLISTA).size;
    }

    if (gUnitSpriteSyncRequest != 0)
    {
        ForceSyncUnitSpriteSheet();
    }
}

static u8 const s_hpbar_chr_off[0x0B] = {
    0 + CHR_LINE * 0, 2 + CHR_LINE * 0, 4 + CHR_LINE * 0, // first row
    0 + CHR_LINE * 1, 2 + CHR_LINE * 1, 4 + CHR_LINE * 1, // second row
    0 + CHR_LINE * 2, 2 + CHR_LINE * 2, 4 + CHR_LINE * 2, // third row
    0 + CHR_LINE * 3, 2 + CHR_LINE * 3,                   // fourth row
};

LYN_REPLACE_CHECK(PutUnitSpritesOam);
void PutUnitSpritesOam(void)
{
    struct UnitSprite * it = gUnitSprites->next;

    PutUnitSpriteIconsOam();

    for (; it != NULL; it = it->next)
    {
        if ((it->config & 0x80) != 0)
            continue;

        int x = it->x - gBmSt.camera.x;
        int y = it->y - gBmSt.camera.y;

        if (x < -16 || x > DISPLAY_WIDTH)
            continue;

        if (y < -32 || y > DISPLAY_HEIGHT)
            continue;

        if (it->hp_bar_info != 0)
        {
            PutOamHi(
                OAM1_X(0x200 + x), OAM0_Y(0x100 + y + 9), Sprite_16x8,
                OAM2_CHR(OBCHR_HPBARS + s_hpbar_chr_off[it->hp_bar_info - 1]) + OAM2_LAYER(2));
        }

        switch (it->config)
        {
            case UNITSPRITE_16x16:
                PutOamHi(OAM1_X(0x200 + x), OAM0_Y(0x100 + y), Sprite_16x16, it->oam2 + OAM2_LAYER(2));
                break;

            case UNITSPRITE_16x32:
                PutOamHi(OAM1_X(0x200 + x), OAM0_Y(0x100 + y - 0x10), Sprite_16x32, it->oam2 + OAM2_LAYER(2));
                break;

            case UNITSPRITE_32x32:
                PutOamHi(OAM1_X(0x200 + x - 8), OAM0_Y(0x100 + y - 0x10), Sprite_32x32, it->oam2 + OAM2_LAYER(2));
                break;

            case UNITSPRITE_16x16 + 3:
                PutOamHi(OAM1_X(0x200 + x), OAM0_Y(0x100 + y), Sprite_16x16, it->oam2 + OAM2_LAYER(3));
                break;

            case UNITSPRITE_16x32 + 3:
                PutOamHi(OAM1_X(0x200 + x), OAM0_Y(0x100 + y - 0x10), Sprite_16x32, it->oam2 + OAM2_LAYER(3));
                break;

            case UNITSPRITE_32x32 + 3:
                PutOamHi(OAM1_X(0x200 + x - 8), OAM0_Y(0x100 + y - 0x10), Sprite_32x32, it->oam2 + OAM2_LAYER(3));
                break;
        }
    }
}
