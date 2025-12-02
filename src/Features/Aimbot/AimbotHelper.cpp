#include "AimbotHelper.h"

// SDK
#include "../../SDK/class/BaseWeapon.h"
#include "../../SDK/class/Source Entity.h"
#include "../../SDK/class/IVEngineClient.h"
#include "../../SDK/class/IVDebugOverlay.h"
#include "../../SDK/class/IRender.h"

// Utility & UI
#include "../Graphics Engine V2/Draw Objects/Circle/Circle.h"
#include "../Graphics Engine V2/Draw Objects/Box/Box.h"
#include "../../Utility/ConsoleLogging.h"
#include "../../Utility/ClassIDHandler/ClassIDHandler.h"
#include "../../Utility/Profiler/Profiler.h"

#include "../Entity Iterator/EntityIterator.h"

// AIMBOTS
#include "AimbotHitscanV2/AimbotHitscanV2.h"
#include "Aimbot Projectile/AimbotProjectile.h"
#include "Aimbot Melee/AimbotMelee.h"


//=========================================================================
//                     PUBLIC METHODS
//=========================================================================
void AimbotHelper_t::Run(BaseEntity* pLocalPlayer, baseWeapon* pActiveWeapon, CUserCmd* pCmd, bool* pCreateMoveResult)
{
    PROFILER_RECORD_FUNCTION(CreateMove);

    // We alive?
    if (pLocalPlayer->m_lifeState() != lifeState_t::LIFE_ALIVE)
        return;

    auto  iProjectileType = pActiveWeapon->GetTFWeaponInfo()->GetWeaponData(0)->m_iProjectile;
    float flAimbotFOV     = 0.0f;
    bool  bTargetFound    = false;
    if(pActiveWeapon->getSlot() == WPN_SLOT_MELLE)
    {
        // Smack em niggas!
        bTargetFound = F::aimbotMelee.RunV3(pLocalPlayer, pActiveWeapon, pCmd, pCreateMoveResult);

        if(Features::Aimbot::Melee_Aimbot::MeleeAimbot.IsDisabled() == false)
        {
            flAimbotFOV = Features::Aimbot::Melee_Aimbot::MeleeAimbot_FOV.GetData().m_flVal;
        }
    }
    else if (iProjectileType != TF_PROJECTILE_BULLET && iProjectileType != TF_PROJECTILE_NONE)
    {
        // surface-to-air freedom delivery system :)
        bTargetFound = F::aimbotProjectile.Run(pLocalPlayer, pActiveWeapon, pCmd, pCreateMoveResult);

        if(Features::Aimbot::Aimbot_Projectile::ProjAimbot_Enable.IsDisabled() == false)
        {
            flAimbotFOV = Features::Aimbot::Aimbot_Projectile::ProjAimbot_FOV.GetData().m_flVal;
        }
    }
    else
    {
        F::aimbotHitscanV2.Run(pCmd, pLocalPlayer, pActiveWeapon, pCreateMoveResult);

        //bTargetFound = F::aimbotHitscan.Run(pLocalPlayer, pActiveWeapon, pCmd, pCreateMoveResult);

        if(Features::Aimbot::AimbotHitscanV2::AimbotHitscan_Enable.IsActive() == true)
        {
            flAimbotFOV = Features::Aimbot::AimbotHitscanV2::AimbotHitscan_FOV.GetData().m_flVal;
        }
    }


    _DrawFOVCircle(flAimbotFOV, bTargetFound);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void AimbotHelper_t::NotifyGameFOV(const float flFOV)
{
    m_flGameFOV = flFOV;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void AimbotHelper_t::_DrawFOVCircle(const float FOV, bool bTargetFound)
{
    if (m_flGameFOV < 0.0f)
        return;

    int iScreenHeight = 0, iScreenWidth = 0; I::iEngine->GetScreenSize(iScreenWidth, iScreenHeight);
    float flFOVCircleRadius = (tanf(DEG2RAD(FOV / 2.0f)) / tanf(DEG2RAD(m_flGameFOV / 2.0f))) * (static_cast<float>(iScreenWidth));

    flFOVCircleRadius = Maths::MAX<float>(flFOVCircleRadius, 0.0f);

    // if circles too big to fit in screen then don't bother drawing it.
    if (flFOVCircleRadius > sqrtf(static_cast<float>(iScreenHeight * iScreenHeight + iScreenWidth * iScreenWidth)))
        return;

    static Circle2D_t* pCircle = nullptr;
    if (pCircle == nullptr)
    {
        pCircle = new Circle2D_t();
        pCircle->SetColor(255, 255, 255, 255);
        pCircle->SetThickness(2.0f);
        pCircle->SetRGBAnimSpeed(1.0f);
        pCircle->SetDrawInLobby(false);
    }
    
    vec vScreenCenter(static_cast<float>(iScreenWidth) / 2.0f, static_cast<float>(iScreenHeight) / 2.0f, 0.0f);
    pCircle->SetVertex(
        vec(vScreenCenter.x - flFOVCircleRadius, vScreenCenter.y - flFOVCircleRadius, 0.0f),
        vec(vScreenCenter.x + flFOVCircleRadius, vScreenCenter.y + flFOVCircleRadius, 0.0f));
    
    pCircle->SetThickness(2.0f);
}
