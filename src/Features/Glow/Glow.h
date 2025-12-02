//=========================================================================
//                      GLOW
//=========================================================================
// by      : INSANE
// created : 21/11/2025
// 
// purpose : Simple glow mechanism to make entities glow. Mimics source engine's glow logic(kinda).
//-------------------------------------------------------------------------
#pragma once
#include <vector>

#include "../../SDK/class/Basic Structures.h"
#include "../FeatureHandler.h"


/*
Credit to "whydoIhax" @ UnknownCheats.com 
"https://www.unknowncheats.me/forum/team-fortress-2-a/700159-simple-glow-outline.html"
Nice stuff bro!.
*/


class BaseEntity;
class ITexture;
class IMaterial;
struct CViewSetup;


///////////////////////////////////////////////////////////////////////////
class Glow_t
{
public:
    Glow_t();

    void Run(CViewSetup* pViewSetup);
    bool DrawingGlow() const;
    bool ShouldBlend() const;

private:
    struct GlowEntity_t 
    { 
        GlowEntity_t(BaseEntity* pEnt, RGBA_t clr) : m_pEnt(pEnt), m_clr(clr) {}
        BaseEntity* m_pEnt = nullptr;
        RGBA_t      m_clr;
    };
    struct GlowClrPair_t 
    {
        GlowClrPair_t()                                    : enemyClr(0LU),      friendlyClr(0LU)         {}
        GlowClrPair_t(RGBA_t clrEnemy, RGBA_t clrFriendly) : enemyClr(clrEnemy), friendlyClr(clrFriendly) {}

        RGBA_t enemyClr, friendlyClr; 
    };

    bool _Initialize();
    bool m_bInitialized = false;


    void _ConstructGlowEntityList(std::vector<GlowEntity_t>& vecGlowEntities, CViewSetup* pViewSetup);
    void _FillGlowClrLUT         (GlowClrPair_t* pGlowClrLUT, uint64_t iSize);
    bool _IsVisible              (CViewSetup* pViewSetup, BaseEntity* pEnt);


    // To notify at other hooks.
    bool m_bDrawingModels = false;
    bool m_bShouldBlend   = false;


    // All entities that will be given a glow effect.
    std::vector<GlowEntity_t> m_vecGlowEntities;


    // GlowBuffer1 & GlowBuffer2 dimensions...
    // We use quator size render targets cause after blur its all gonna
    // look the same.
    int        m_iBufferHeight       = 0;
    int        m_iBufferWidth        = 0;

    // Render targets for blurring.
    ITexture*  m_pRTFullScreen       = nullptr;
    ITexture*  m_pGlowBuffer1        = nullptr;
    ITexture*  m_pGlowBuffer2        = nullptr;

    IMaterial* m_pBlurFilterX        = nullptr;
    IMaterial* m_pBlurFilterY        = nullptr;
    IMaterial* m_pGlowMaterial       = nullptr;
    IMaterial* m_pMatHaloAddToScreen = nullptr;
};
///////////////////////////////////////////////////////////////////////////

DECLARE_FEATURE_OBJECT(glow, Glow_t)

DEFINE_TAB    (Glow, 8)

DEFINE_SECTION(Glow, "Glow", 1)
DEFINE_FEATURE(Glow_BlurEnabled,                       "Enable Blur",          bool, Glow, Glow, 0,  true)
DEFINE_FEATURE(Glow_Blur,                                     "Blur", FloatSlider_t, Glow, Glow, 1,  FloatSlider_t(1.0f, 0.0f, 20.0f))
DEFINE_FEATURE(Glow_EnableGameGlow,                      "Game Glow",          bool, Glow, Glow, 2, true, FeatureFlag_None, "Let game draw it's own glow?")


DEFINE_SECTION(Players, "Glow", 2)
DEFINE_FEATURE(Glow_EnemyPlayer,                     "Enemy Players",          bool, Players, Glow, 1,  false)
DEFINE_FEATURE(Glow_EnemyPlayerClr,              "Enemy Players Clr",   ColorData_t, Players, Glow, 2,  ColorData_t((unsigned char)255, 255, 255, 255))
DEFINE_FEATURE(Glow_TeamPlayer,                       "Team Players",          bool, Players, Glow, 3,  false)
DEFINE_FEATURE(Glow_TeamPlayerClr,                "Team Players Clr",   ColorData_t, Players, Glow, 4,  ColorData_t((unsigned char)255, 255, 255, 255))
DEFINE_FEATURE(Glow_LocalPlayer,                       "LocalPlayer",          bool, Players, Glow, 5,  false)
DEFINE_FEATURE(Glow_LocalPlayerClr,                "LocalPlayer Clr",   ColorData_t, Players, Glow, 6,  ColorData_t((unsigned char)255, 255, 255, 255))


DEFINE_SECTION(Building, "Glow", 3)
// Sentries...
DEFINE_FEATURE(Glow_EnemySentry,                      "Enemy Sentry",          bool, Building, Glow, 1,  false)
DEFINE_FEATURE(Glow_EnemySentryClr,               "Enemy Sentry Clr",   ColorData_t, Building, Glow, 2,  ColorData_t((unsigned char)255, 255, 255, 255))
DEFINE_FEATURE(Glow_TeamSentry,                        "Team Sentry",          bool, Building, Glow, 3,  false)
DEFINE_FEATURE(Glow_TeamSentryClr,                 "Team Sentry Clr",   ColorData_t, Building, Glow, 4,  ColorData_t((unsigned char)255, 255, 255, 255))

// Dispensers...
DEFINE_FEATURE(Glow_EnemyDispenser,                "Enemy Dispenser",          bool, Building, Glow, 5, false)
DEFINE_FEATURE(Glow_EnemyDispenserClr,         "Enemy Dispenser Clr",   ColorData_t, Building, Glow, 6, ColorData_t((unsigned char)255, 255, 255, 255))
DEFINE_FEATURE(Glow_TeamDispenser,                 "Team  Dispenser",          bool, Building, Glow, 7, false)
DEFINE_FEATURE(Glow_TeamDispenserClr,          "Team  Dispenser Clr",   ColorData_t, Building, Glow, 8, ColorData_t((unsigned char)255, 255, 255, 255))

// Teleporters...
DEFINE_FEATURE(Glow_EnemyTeleporter,              "Enemy Teleporter",          bool, Building, Glow, 9, false)
DEFINE_FEATURE(Glow_EnemyTeleporterClr,       "Enemy Teleporter Clr",   ColorData_t, Building, Glow, 10, ColorData_t((unsigned char)255, 255, 255, 255))
DEFINE_FEATURE(Glow_TeamTeleporter,               "Team  Teleporter",          bool, Building, Glow, 11, false)
DEFINE_FEATURE(Glow_TeamTeleporterClr,        "Team  Teleporter Clr",   ColorData_t, Building, Glow, 12, ColorData_t((unsigned char)255, 255, 255, 255))


DEFINE_SECTION(Projectiles, "Glow", 4)
DEFINE_FEATURE(Glow_TeamProjectile,         "Team Projectiles",        bool, Projectiles, Glow, 1, false)
DEFINE_FEATURE(Glow_TeamProjectileClr,   "Team Projectile Clr", ColorData_t, Projectiles, Glow, 2, ColorData_t((unsigned char)255, 255, 255, 255))

DEFINE_FEATURE(Glow_EnemyProjectile,       "Enemy Projectiles",        bool, Projectiles, Glow, 3, false)
DEFINE_FEATURE(Glow_EnemyProjectileClr, "Enemy Projectile Clr", ColorData_t, Projectiles, Glow, 4, ColorData_t((unsigned char)255, 255, 255, 255))



DEFINE_SECTION(Misc, "Glow", 5)
DEFINE_FEATURE(Glow_Ammo,                                 "Ammo Box",          bool, Misc, Glow, 1, false)
DEFINE_FEATURE(Glow_AmmoClr,                          "Ammo Box Clr",   ColorData_t, Misc, Glow, 2, ColorData_t((unsigned char)255, 255, 255, 255))

DEFINE_FEATURE(Glow_Animating,       "CBaseAnimating ( no derived )",          bool, Misc, Glow, 3, false)
DEFINE_FEATURE(Glow_AnimatingClr,               "CBaseAnimating Clr",   ColorData_t, Misc, Glow, 4, ColorData_t((unsigned char)255, 255, 255, 255))

DEFINE_FEATURE(Glow_ViewModel,                          "View Model",          bool, Misc, Glow, 5, false, FeatureFlag_None, "Must use same viewmodel FOV as eye FOV. ( My bad )")
DEFINE_FEATURE(Glow_ViewModelClr,                   "View Model Clr",   ColorData_t, Misc, Glow, 6, ColorData_t((unsigned char)255, 255, 255, 255))

DEFINE_FEATURE(Glow_DroppedWpn,                       "Dropped Wpns",          bool, Misc, Glow, 7, false)
DEFINE_FEATURE(Glow_DroppedWpnClr,                "Dropped Wpns Clr",   ColorData_t, Misc, Glow, 8, ColorData_t((unsigned char)255, 255, 255, 255))