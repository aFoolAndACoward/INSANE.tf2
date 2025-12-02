#include <cstdint>

#include "../Utility/Hook Handler/Hook_t.h"
#include "../Utility/ConsoleLogging.h"

// SDK
#include "../SDK/class/Basic Structures.h"
#include "../SDK/class/GlowManager.h"
#include "../SDK/class/viewSetup.h"
#include "../SDK/class/BaseEntity.h"
#include "../SDK/class/IVEngineClient.h"
#include "../SDK/class/IVRenderView.h"
#include "../SDK/class/IStudioRender.h"
#include "../SDK/class/IMaterial.h"
#include "../SDK/class/IMaterialSystem.h"
#include "../SDK/class/IMatRenderContext.h"
#include "../SDK/class/ITexture.h"
#include "../SDK/class/IMaterialVar.h"
#include "../SDK/class/Source Entity.h"

#include "../Features/Glow/Glow.h"


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
MAKE_HOOK(CGlowObjectManager_ApplyEntityGlowEffect, "48 8B C4 48 89 58 ? 48 89 50 ? 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 0F 29 70", __fastcall, CLIENT_DLL, void*,
    CGlowObjectManager* pGlowManager, CViewSetup* pViewSetup, int nSplitScreenSlots, void** pContext)
{
    void* result = nullptr;
    if(Features::Glow::Glow::Glow_EnableGameGlow.IsActive() == true)
    {
        // This can ruin the visuals, entities looks ass with double glow.
        result = Hook::CGlowObjectManager_ApplyEntityGlowEffect::O_CGlowObjectManager_ApplyEntityGlowEffect(pGlowManager, pViewSetup, nSplitScreenSlots, pContext);
    }


    F::glow.Run(pViewSetup);


    return result;
}