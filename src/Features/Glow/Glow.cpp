#include "Glow.h"
#include <unordered_map>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <string.h>

// UTILITY
#include "../../Utility/ConsoleLogging.h"
#include "../../Utility/ClassIDHandler/ClassIDHandler.h"
#include "../ImGui/NotificationSystem/NotificationSystem.h"
#include "../Aimbot/AimbotHelper.h"
#include "../DMEHandler/DMEHandler.h"
#include "../../Utility/Profiler/Profiler.h"
#include "../../Utility/math.h"
#include "../../Utility/Signature Handler/signatures.h"

// SDK
#include "../../SDK/class/Source Entity.h"
#include "../../SDK/class/BaseEntity.h"
#include "../../SDK/class/IVEngineClient.h"
#include "../../SDK/class/IMaterial.h"
#include "../../SDK/class/IMaterialVar.h"
#include "../../SDK/class/IMaterialSystem.h"
#include "../../SDK/class/ITexture.h"
#include "../../SDK/class/Basic Structures.h"
#include "../../SDK/class/IMatRenderContext.h"
#include "../../SDK/class/IStudioRender.h"
#include "../../SDK/class/IVRenderView.h"
#include "../../SDK/class/IMatRenderContext.h"
#include "../../SDK/class/viewSetup.h"

#include "../Entity Iterator/EntityIterator.h"
#include "../FeatureHandler.h"
#include "../../Libraries/Utility/Utility.h"



/*
* TEST CASE : 
tr_walkway 200 ms backtrack. all record glow enabled.

TIMES :
Total       : [ 1, 1.4 ] ms
FILTER      : ~200 us
FIRST DRAW  : [ 400, 500] us
SECOND DRAW : ~300 us

NEW TIMES : 
Total       : < 0.8 ms
FILTER      : [ 50, 150] us
FIRST DRAW  : Depends on entities on screen. ( drawing fewer entites now )
SECOND DRAW : Depends on entities on screen. ( drawing fewer entities & drawing on smaller render target now )
*/


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
Glow_t::Glow_t()
{
    m_vecGlowEntities.clear();
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Glow_t::Run(CViewSetup* pViewSetup)
{
    // Initialize materials & render targets.
    if (m_bInitialized == false) m_bInitialized = _Initialize();
    if (m_bInitialized == false) return;


    BaseEntity* pLocalPlayer = I::IClientEntityList->GetClientEntity(I::iEngine->GetLocalPlayer());
    if (pLocalPlayer == nullptr || F::entityIterator.GetLocalPlayerInfo().m_iLifeState != lifeState_t::LIFE_ALIVE)
        return;



    // Setup mateiral blur ammounts.
    m_pBlurFilterX->FindVar("$bloomamount", NULL, false)->SetFloatValue(Features::Glow::Glow::Glow_Blur.GetData().m_flVal);
    m_pBlurFilterY->FindVar("$bloomamount", NULL, false)->SetFloatValue(Features::Glow::Glow::Glow_Blur.GetData().m_flVal);


    // Screen size... ( to be used at multiple places throughout. )
    int iScreenWidth = 0, iScreenHeight = 0; I::iEngine->GetScreenSize(iScreenWidth, iScreenHeight);



    PROFILER_START_SCOPE_NAMED("Glow");
    {
        PROFILER_RECORD_FUNCTION_NAMED("Entity Filter", Glow);

        _ConstructGlowEntityList(m_vecGlowEntities, pViewSetup);
        
        // Handling local player seperatly.
        if (Features::Glow::Players::Glow_LocalPlayer.IsActive() == true)
        {
            m_vecGlowEntities.push_back(GlowEntity_t(pLocalPlayer, Features::Glow::Players::Glow_LocalPlayerClr.GetData().GetAsBytes()));
        }

        // If we leave without ending profiler scope, would trigger assert.
        if (m_vecGlowEntities.empty() == true)
        {
            PROFILER_END_SCOPE_NAMED("Glow");
            return;
        }
    }


    vec   vOriginalClrMod; I::iVRenderView->GetColorModulation(&vOriginalClrMod.x);
    float flOriginalBlend = I::iVRenderView->GetBlend();

    
    // NOTE : All models drawn with mateiral override.
    I::iStudioRender->ForcedMaterialOverride(m_pGlowMaterial, OverrideType_t::OVERRIDE_NORMAL);
    IMatRenderContext* pRenderContext = I::iMaterialSystem->GetRenderContext();
    {
        PROFILER_RECORD_FUNCTION_NAMED("First Draw", Glow);

        pRenderContext->OverrideDepthEnable(true, false);

        // we wanna draw, but we don't want it to be visible.
        I::iVRenderView->SetBlend(0.0f);

        // Stencil settings for first draw.
        pRenderContext->SetStencilEnable         (true);
        pRenderContext->SetStencilFailOperation  (STENCILOPERATION_KEEP);
        pRenderContext->SetStencilZFailOperation (STENCILOPERATION_REPLACE);
        pRenderContext->SetStencilPassOperation  (STENCILOPERATION_REPLACE);
        pRenderContext->SetStencilCompareFunction(STENCILCOMPARISONFUNCTION_ALWAYS);
        pRenderContext->SetStencilReferenceValue (1);
        pRenderContext->SetStencilTestMask       (0xFFFFFFFF);
        pRenderContext->SetStencilWriteMask      (0xFFFFFFFF);


        m_bDrawingModels = true;
        m_bShouldBlend   = false;

        // Does this STUDIO_NOSHADOWS flag helps with performance? It feels like it should.
        for (GlowEntity_t& glowEnt : m_vecGlowEntities)
            glowEnt.m_pEnt->DrawModel(STUDIO_RENDER | STUDIO_NOSHADOWS);

        m_bShouldBlend   = true;
        m_bDrawingModels = false;
        pRenderContext->OverrideDepthEnable(false, false);
    }



    // 2nd draw... ( to our rendertarget, so we can modify and blur it :) )
    {
        PROFILER_RECORD_FUNCTION_NAMED("SecondDraw", Glow);

        pRenderContext->PushRenderTargetAndViewport();

        // Setup our custom render target.
        pRenderContext->SetRenderTarget(m_pGlowBuffer1);
        pRenderContext->Viewport(0, 0, iScreenWidth, iScreenHeight);
        
        // Clear glowbuffer1.
        pRenderContext->ClearColor4ub(0, 0, 0, 0);
        pRenderContext->ClearBuffers(true, false, false);


        // We don't want stencil for this draw.
        pRenderContext->SetStencilEnable(false);


        m_bDrawingModels = true;
        m_bShouldBlend   = true;

        for (GlowEntity_t& glowEnt : m_vecGlowEntities)
        {
            Vec4 vClr    = glowEnt.m_clr.GetAsVec4();
            vec  vClrRGB = vClr.XYZ();
 
            I::iVRenderView->SetColorModulation(&vClrRGB);
            I::iVRenderView->SetBlend(vClr.w);

            // Must use noshadow, else the glow will get dark at some parts of maps. thx "whydoIhax".
            glowEnt.m_pEnt->DrawModel(STUDIO_RENDER | STUDIO_NOSHADOWS);
        }
        
        m_bShouldBlend   = false;
        m_bDrawingModels = false;


        pRenderContext->PopRenderTargetAndViewport();
    }


    // No more drawmodel calls, restore everything.
    I::iVRenderView->SetBlend(flOriginalBlend);
    I::iVRenderView->SetColorModulation(&vOriginalClrMod);
    I::iStudioRender->ForcedMaterialOverride(nullptr, OverrideType_t::OVERRIDE_NORMAL);

    int iViewPortX = 0, iViewPortY = 0, iViewPortWidth = 0, iViewPortHeight = 0;
    pRenderContext->GetViewport(iViewPortX, iViewPortY, iViewPortWidth, iViewPortHeight);


    // Now we blur.
    if(Features::Glow::Glow::Glow_BlurEnabled.IsActive() == true)
    {
        pRenderContext->PushRenderTargetAndViewport();

        // Dumping glowbuffer1 ( has colored models drawn on it. ) to glowbuffer and blur it 
        // simultaneously.
        pRenderContext->SetRenderTarget(m_pGlowBuffer2);
        pRenderContext->Viewport(0, 0, iScreenWidth, iScreenHeight);


        pRenderContext->DrawScreenSpaceRectangle(
            m_pBlurFilterX, // ( this mateiral has basetexture as glowbuffer1 ( which has models. ) so it should draw models to rendertarget ( glowbuffer2 )
            0, 0, iViewPortWidth, iViewPortHeight,
            0.0f, -0.5f, static_cast<float>(m_iBufferWidth - 1), static_cast<float>(m_iBufferHeight - 1),
            m_iBufferWidth, m_iBufferHeight
        );

        pRenderContext->SetRenderTarget(m_pGlowBuffer1);

        pRenderContext->DrawScreenSpaceRectangle(
            m_pBlurFilterY,
            0, 0, iViewPortWidth, iViewPortHeight,
            0.0f, -0.5f, static_cast<float>(m_iBufferWidth - 1), static_cast<float>(m_iBufferHeight - 1),
            m_iBufferWidth, m_iBufferHeight
        );

        pRenderContext->PopRenderTargetAndViewport();
    }

    
    // Now we dump the glowbuffer1 to backbuffer
    pRenderContext->SetStencilEnable         (true);
    pRenderContext->SetStencilFailOperation  (STENCILOPERATION_KEEP);
    pRenderContext->SetStencilZFailOperation (STENCILOPERATION_KEEP);
    pRenderContext->SetStencilPassOperation  (STENCILOPERATION_KEEP);
    pRenderContext->SetStencilCompareFunction(STENCILCOMPARISONFUNCTION_EQUAL);
    pRenderContext->SetStencilReferenceValue (0);
    pRenderContext->SetStencilTestMask       (0xFFFFFFFF);
    pRenderContext->SetStencilWriteMask      (0xFFFFFFFF);


    pRenderContext->DrawScreenSpaceRectangle(
        m_pMatHaloAddToScreen,
        0, 0, iViewPortWidth, iViewPortHeight,
        0.0f, -0.5f, static_cast<float>(m_iBufferWidth), static_cast<float>(m_iBufferHeight),
        m_iBufferWidth, m_iBufferHeight
    );

    pRenderContext->SetStencilEnable(false);

    PROFILER_END_SCOPE_NAMED("Glow");
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool Glow_t::DrawingGlow() const
{
    return m_bDrawingModels;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool Glow_t::ShouldBlend() const
{
    return m_bShouldBlend;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool Glow_t::_Initialize()
{
    // To Initialize 
    // 
    // BlurBuffer1 : where we will be drawing our colored model.
    // BlurBuffer2 : to temporarily store our colored models while blurring it simulatneously.
    // BlurFilterX -> ( $BaseTexture : BlurBuffer1 ) : to blur the buffer in X direction.
    // BlurFilterY -> ( $BaseTexture : BlurBuffer2 ) : to blur the buffer in Y direction.
    // FullScreenRT.
    // GlowMateiral

    m_pRTFullScreen = I::iMaterialSystem->FindTexture("_rt_FullFrameFB", TEXTURE_GROUP_RENDER_TARGET);
    if (m_pRTFullScreen == nullptr)
    {
        FAIL_LOG("Failed to find texture \"_rt_FullFrameFB\"");
        return false;
    }

    m_iBufferWidth  = m_pRTFullScreen->GetActualWidth()  / 2;
    m_iBufferHeight = m_pRTFullScreen->GetActualHeight() / 2;

    // Glow buffer 1
    if (m_pGlowBuffer1 == nullptr)
    {
        m_pGlowBuffer1 = reinterpret_cast<ITexture*>(I::iMaterialSystem->CreateNamedRenderTargetTextureEx(
            "GlowBuffer1",
            //m_pRTFullScreen->GetActualWidth(), m_pRTFullScreen->GetActualHeight(),
            m_iBufferWidth, m_iBufferHeight,
            RT_SIZE_LITERAL,
            IMAGE_FORMAT_RGB888,
            MATERIAL_RT_DEPTH_SHARED,
            TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_EIGHTBITALPHA,
            CREATERENDERTARGETFLAGS_HDR
        ));
    }


    // Glow buffer 2
    if(m_pGlowBuffer2 == nullptr)
    {
        m_pGlowBuffer2 = reinterpret_cast<ITexture*>(I::iMaterialSystem->CreateNamedRenderTargetTextureEx(
            "GlowBuffer2",
            //m_pRTFullScreen->GetActualWidth(), m_pRTFullScreen->GetActualHeight(),
            m_iBufferWidth, m_iBufferHeight,
            RT_SIZE_LITERAL,
            IMAGE_FORMAT_RGB888,
            MATERIAL_RT_DEPTH_SHARED,
            TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_EIGHTBITALPHA,
            CREATERENDERTARGETFLAGS_HDR
        ));
    }


    if (m_pGlowBuffer1 == nullptr || m_pGlowBuffer2 == nullptr)
    {
        FAIL_LOG("Failed to create glowbuffer texture. GlowBuffer1 -> %p || GlowBuffer2 -> %p", m_pGlowBuffer1, m_pGlowBuffer2);
        return false;
    }

    
    // Blur Filter X
    if(m_pBlurFilterX == nullptr)
    {
        static const char* s_szBlurFilterXVMT = R"( "BlurFilterX" { "$basetexture" "GlowBuffer1" "$bloomamount" 1})";

        KeyValues* kv = new KeyValues;
        kv->LoadFromBuffer("BlurFilterX", s_szBlurFilterXVMT);
        m_pBlurFilterX = I::iMaterialSystem->CreateMaterial("BlurFilterX", kv);
    }

    // Blur Filter Y
    if(m_pBlurFilterY == nullptr)
    {
        static const char* s_szBlurFilterYVMT = R"( "BlurFilterY" { "$basetexture" "GlowBuffer2" "$bloomamount" 1})";

        KeyValues* kv = new KeyValues;
        kv->LoadFromBuffer("BlurFilterY", s_szBlurFilterYVMT);
        m_pBlurFilterY = I::iMaterialSystem->CreateMaterial("BlurFilterY", kv);
    }


    if (m_pBlurFilterX == nullptr || m_pBlurFilterY == nullptr)
    {
        FAIL_LOG("Failed to create blurFilter materials. BlurFilterX -> %p || BlurFilterY -> %p", m_pBlurFilterX, m_pBlurFilterY);
        return false;
    }


    if (m_pMatHaloAddToScreen == nullptr)
    {
        static const char* s_szMatHaloAddToScreenVMT = R"( "UnlitGeneric" { "$basetexture" "GlowBuffer1" "$additive" 1 "$C0_X" "1.0"})";

        KeyValues* kv = new KeyValues;
        kv->LoadFromBuffer("GlowMaterialHalo", s_szMatHaloAddToScreenVMT);
        m_pMatHaloAddToScreen = I::iMaterialSystem->CreateMaterial("GlowMaterialHalo", kv);
    }


    if (m_pMatHaloAddToScreen == nullptr)
    {
        FAIL_LOG("Failed to create MatHaloAddToScreen!");
        return false;
    }


    // Glow material...
    if(m_pGlowMaterial == nullptr)
    {
        m_pGlowMaterial = I::iMaterialSystem->FindMaterial("dev/glow_color", TEXTURE_GROUP_OTHER, true, NULL);
        
        if (m_pGlowMaterial == nullptr)
        {
            FAIL_LOG("Failed to find material \"dev/glow_color\"");
            return false;
        }
    }
    

    Render::notificationSystem.PushBack("Initialized glow handler");
    WIN_LOG("Initialized glow handler");
    return true;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Glow_t::_ConstructGlowEntityList(std::vector<GlowEntity_t>& vecGlowEntities, CViewSetup* pViewSetup)
{
    // A array to acess color for a given classID in O(1) time.
    // NOTE : this array will be filled in the first call to this fn, due to the check below. So no memset here.
    static GlowClrPair_t s_glowClrLUT[(int64_t)ClassIDStatic_t::CLASSIDSTATIC_COUNT];


    // Filling up glow clr LUT.
    static std::chrono::time_point s_lastLUTUpdateTime = std::chrono::high_resolution_clock::now();
    std::chrono::time_point        iTimeNow            = std::chrono::high_resolution_clock::now();
    int64_t                        iTimeSinceLUTUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(iTimeNow - s_lastLUTUpdateTime).count();
    if (iTimeSinceLUTUpdate > 1000LL) // each second
    {
        _FillGlowClrLUT(s_glowClrLUT, sizeof(s_glowClrLUT));
        s_lastLUTUpdateTime = iTimeNow;
    }


    // now iterate entity list and collect entities to glow.
    vecGlowEntities.clear();
    int nEntities     = I::IClientEntityList->GetHighestEntityIndex();
    int iFriendlyTeam = F::entityIterator.GetLocalPlayerInfo().m_iTeam; 
    for (int iEntIndex = 0; iEntIndex < nEntities; iEntIndex++)
    {
        BaseEntity* pEnt = I::IClientEntityList->GetClientEntity(iEntIndex);

        if (pEnt == nullptr)
            continue;

        if (pEnt->IsDormant() == true)
            continue;


        // Entity info...
        bool            bFriendly = pEnt->m_iTeamNum() == iFriendlyTeam;
        ClassIDStatic_t iClassID  = F::classIDHandler.GetStaticClassID(pEnt);
        if (iClassID == F::classIDHandler.InvalidStaticClassID())
            continue;

        // color for this entity's glow.
        GlowClrPair_t   clrPair   = s_glowClrLUT[static_cast<int32_t>(iClassID)];
        uint32_t*       pClr      = reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(&clrPair) + (static_cast<uintptr_t>(bFriendly) * 4LLU));

        // No clr set for this entity.
        if (*pClr == 0U)
            continue;


        if (_IsVisible(pViewSetup, pEnt) == false)
            continue;


        // if bFriendly if 0 pick up first 4 bytes ( RGBA_t for enemy ) else pickup next 4 bytes ( RGBA_t for friendly entities )
        vecGlowEntities.push_back(GlowEntity_t(pEnt, *reinterpret_cast<RGBA_t*>(pClr)));
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Glow_t::_FillGlowClrLUT(GlowClrPair_t* pGlowClrLUT, uint64_t iSize)
{
    memset(pGlowClrLUT, 0, iSize);


    // Players...
    if (Features::Glow::Players::Glow_EnemyPlayer.IsActive() == true)
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFPlayer].enemyClr            = Features::Glow::Players::Glow_EnemyPlayerClr.GetData().GetAsBytes();

    if (Features::Glow::Players::Glow_TeamPlayer.IsActive() == true)
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFPlayer].friendlyClr         = Features::Glow::Players::Glow_TeamPlayerClr.GetData().GetAsBytes();


    // Dispensers...
    if (Features::Glow::Building::Glow_EnemyDispenser.IsActive() == true)
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CObjectDispenser].enemyClr     = Features::Glow::Building::Glow_EnemyDispenserClr.GetData().GetAsBytes();

    if (Features::Glow::Building::Glow_TeamDispenser.IsActive() == true)
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CObjectDispenser].friendlyClr  = Features::Glow::Building::Glow_TeamDispenserClr.GetData().GetAsBytes();

    // Teleporters...
    if (Features::Glow::Building::Glow_EnemyTeleporter.IsActive() == true)
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CObjectTeleporter].enemyClr    = Features::Glow::Building::Glow_EnemyTeleporterClr.GetData().GetAsBytes();

    if (Features::Glow::Building::Glow_TeamTeleporter.IsActive() == true)
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CObjectTeleporter].friendlyClr = Features::Glow::Building::Glow_TeamTeleporterClr.GetData().GetAsBytes();

    // Sentries...
    if (Features::Glow::Building::Glow_EnemySentry.IsActive() == true)
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CObjectSentrygun].enemyClr     = Features::Glow::Building::Glow_EnemySentryClr.GetData().GetAsBytes();

    if (Features::Glow::Building::Glow_TeamSentry.IsActive() == true)
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CObjectSentrygun].friendlyClr  = Features::Glow::Building::Glow_TeamSentryClr.GetData().GetAsBytes();
    
    
    // Projectiles...
    if (Features::Glow::Projectiles::Glow_EnemyProjectile.IsActive() == true)
    {
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_ThrowableBreadMonster].enemyClr  = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_ThrowableBrick].enemyClr         = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_ThrowableRepel].enemyClr         = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_Throwable].enemyClr              = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_Cleaver].enemyClr                = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_JarMilk].enemyClr                = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_Jar].enemyClr                    = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_JarGas].enemyClr                 = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_MechanicalArmOrb].enemyClr       = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_Rocket].enemyClr                 = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_Flare].enemyClr                  = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_EnergyBall].enemyClr             = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_GrapplingHook].enemyClr          = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_HealingBolt].enemyClr            = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_Arrow].enemyClr                  = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_EnergyRing].enemyClr             = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellKartBats].enemyClr          = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellKartOrb].enemyClr           = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellLightningOrb].enemyClr      = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellTransposeTeleport].enemyClr = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellMeteorShower].enemyClr      = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellSpawnBoss].enemyClr         = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellMirv].enemyClr              = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellPumpkin].enemyClr           = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellSpawnHorde].enemyClr        = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellSpawnZombie].enemyClr       = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellBats].enemyClr              = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellFireball].enemyClr          = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_BallOfFire].enemyClr             = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SentryRocket].enemyClr           = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFStickBomb].enemyClr                         = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFGrenadePipebombProjectile].enemyClr         = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFBall_Ornament].enemyClr                     = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFStunBall].enemyClr                          = Features::Glow::Projectiles::Glow_EnemyProjectileClr.GetData().GetAsBytes();
    }
    
    if (Features::Glow::Projectiles::Glow_TeamProjectile.IsActive() == true)
    {
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_ThrowableBreadMonster].friendlyClr  = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_ThrowableBrick].friendlyClr         = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_ThrowableRepel].friendlyClr         = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_Throwable].friendlyClr              = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_Cleaver].friendlyClr                = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_JarMilk].friendlyClr                = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_Jar].friendlyClr                    = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_JarGas].friendlyClr                 = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_MechanicalArmOrb].friendlyClr       = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_Rocket].friendlyClr                 = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_Flare].friendlyClr                  = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_EnergyBall].friendlyClr             = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_GrapplingHook].friendlyClr          = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_HealingBolt].friendlyClr            = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_Arrow].friendlyClr                  = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_EnergyRing].friendlyClr             = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellKartBats].friendlyClr          = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellKartOrb].friendlyClr           = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellLightningOrb].friendlyClr      = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellTransposeTeleport].friendlyClr = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellMeteorShower].friendlyClr      = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellSpawnBoss].friendlyClr         = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellMirv].friendlyClr              = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellPumpkin].friendlyClr           = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellSpawnHorde].friendlyClr        = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellSpawnZombie].friendlyClr       = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellBats].friendlyClr              = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellFireball].friendlyClr          = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_BallOfFire].friendlyClr             = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFProjectile_SentryRocket].friendlyClr           = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFStickBomb].friendlyClr                         = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFGrenadePipebombProjectile].friendlyClr         = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFBall_Ornament].friendlyClr                     = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFStunBall].friendlyClr                          = Features::Glow::Projectiles::Glow_TeamProjectileClr.GetData().GetAsBytes();
    }


    // Misc...
    if (Features::Glow::Misc::Glow_Ammo.IsActive() == true)
    {
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFAmmoPack].enemyClr    = Features::Glow::Misc::Glow_AmmoClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFAmmoPack].friendlyClr = Features::Glow::Misc::Glow_AmmoClr.GetData().GetAsBytes();
    }

    if (Features::Glow::Misc::Glow_Animating.IsActive() == true)
    {
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CBaseAnimating].enemyClr    = Features::Glow::Misc::Glow_AnimatingClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CBaseAnimating].friendlyClr = Features::Glow::Misc::Glow_AnimatingClr.GetData().GetAsBytes();
    }

    if (Features::Glow::Misc::Glow_ViewModel.IsActive() == true)
    {
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFViewModel].enemyClr    = Features::Glow::Misc::Glow_ViewModelClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFViewModel].friendlyClr = Features::Glow::Misc::Glow_ViewModelClr.GetData().GetAsBytes();
    }

    if (Features::Glow::Misc::Glow_DroppedWpn.IsActive() == true)
    {
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFDroppedWeapon].enemyClr    = Features::Glow::Misc::Glow_DroppedWpnClr.GetData().GetAsBytes();
        pGlowClrLUT[(uint64_t)ClassIDStatic_t::ClassIDStatic_CTFDroppedWeapon].friendlyClr = Features::Glow::Misc::Glow_DroppedWpnClr.GetData().GetAsBytes();
    }
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool Glow_t::_IsVisible(CViewSetup* pViewSetup, BaseEntity* pEnt)
{
    ICollideable_t*      pCollideable = pEnt->GetCollideable();
    I_client_renderable* pRenderable  = pEnt->GetClientRenderable();
    vec                  vMin         = pRenderable->GetRenderOrigin() + pCollideable->OBBMinsPreScaled();
    vec                  vMax         = pRenderable->GetRenderOrigin() + pCollideable->OBBMaxsPreScaled();

    vec vViewAngles;
    Maths::AngleVectors(pViewSetup->angles, &vViewAngles);
    vViewAngles.NormalizeInPlace();

    // Angle from screen center to entity min.
    vec   vEyeToMin  = (vMin - pViewSetup->origin).Normalize();
    float flDotMin   = vViewAngles.Dot(vEyeToMin);
    float flMinAngle = RAD2DEG(acosf(flDotMin));

    // Angle from screen center to entity max.
    vec   vEyeToMax  = (vMax - pViewSetup->origin).Normalize();
    float flDotMax   = vViewAngles.Dot(vEyeToMax);
    float flMaxAngle = RAD2DEG(acosf(flDotMax));

    
    float flFOV =  pViewSetup->fov * 0.75f; // you don't need entities near the edge to glow. do you?
    return flMinAngle <= flFOV || flMaxAngle <= flFOV;
}
