//=========================================================================
//                      TICK MANIPULATION HELPER
//=========================================================================
// by      : INSANE
// created : 11/05/2025
// 
// purpose : Handles "SendPacket"'s value & runs fakelag.
//-------------------------------------------------------------------------
#pragma once
#include "../FeatureHandler.h"


class  BaseEntity;
class  baseWeapon;
class  CUserCmd;
struct ModelRenderInfo_t;
struct matrix3x4_t;



///////////////////////////////////////////////////////////////////////////
class TickManipHelper_t
{
public:
    void Run(BaseEntity* pLocalPlayer, baseWeapon* pActiveWeapon, CUserCmd* pCmd, bool* pSendPacket, bool* pCreateMoveResult);
    void Reset();

    bool         ModifyRealBoneAngles()      const;
    bool         ShouldDrawSecondModel()     const;
    bool         ShouldDrawSecondModelGlow() const;
    matrix3x4_t* GetFakeAngleBones();

    bool         CalculatingBones() const;

private:

    // Calculating & storing bones.
    bool   _ShouldRecordSecondModelBones(const bool bSendPacket) const;
    void   _StoreBones(const qangle& qEyeAngle, matrix3x4_t* pDestination, BaseEntity* pLocalPlayer);

    matrix3x4_t m_fakeAngleBones[MAX_STUDIO_BONES];
    bool        m_bCalculatingBones = false;

    
    void _DetectAndHandleShots(BaseEntity* pLocalPlayer, baseWeapon* pActiveWeapon, CUserCmd* pCmd, bool* pSendPacket);
    qangle m_qOriginalAngles;
    float  m_flOrigForwardMove = 0.0f;
    float  m_flOrigSideMove    = 0.0f;


    // UI
    void _DrawWidget() const;
    int m_iTicksChocked = 0;
};
///////////////////////////////////////////////////////////////////////////

DECLARE_FEATURE_OBJECT(tickManipHelper, TickManipHelper_t)

DEFINE_TAB(Misc, 12)
DEFINE_SECTION(FakeLag, "Misc", 1)
DEFINE_FEATURE(FakeLag_Enable,       "Enable",     bool,        FakeLag, Misc, 1, false, FeatureFlag_None)
DEFINE_FEATURE(FakeLag_ChockedTicks, "Ticks",      IntSlider_t, FakeLag, Misc, 2, IntSlider_t(0, 0, 24), FeatureFlag_None)
DEFINE_FEATURE(FakeLag_Draw,         "Draw Model", bool,        FakeLag, Misc, 3, false, FeatureFlag_None)
DEFINE_FEATURE(FakeLag_Glow,         "Glow Model", bool,        FakeLag, Misc, 4, false, FeatureFlag_None)
DEFINE_FEATURE(FakeLag_DrawWidget,   "Draw Info",  bool,        FakeLag, Misc, 5, false, FeatureFlag_None)