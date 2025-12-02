#include "ClassIDHandler.h"
#include <cstdint>
#include <unordered_map>

// SDK
#include "../../SDK/class/I_BaseEntityDLL.h"
#include "../../SDK/class/ClientClass.h"
#include "../../SDK/class/BaseEntity.h"

// Utility
#include "../ConsoleLogging.h"



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
ClassID_t::ClassID_t(int* pDestination, const std::string& szClassName)
{
    m_pDestination = pDestination;
    m_szClassName  = szClassName;

    F::classIDHandler.RegisterClassID(this);
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void ClassIDHandler_t::RegisterClassID(ClassID_t* pClassID)
{
    auto it = m_mapClassNameToID.find(pClassID->m_szClassName);
    if (it != m_mapClassNameToID.end())
    {
        FAIL_LOG("Duplicate found for [ %s ]!", pClassID->m_szClassName.c_str());
        return;
    }

    m_mapClassNameToID.insert({ pClassID->m_szClassName, pClassID });
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool ClassIDHandler_t::Initialize()
{
    if (m_bInitialized == true)
        return true;

    ClientClass* pClientClass = I::IBaseClient->GetAllClasses();
    size_t       nIDsToFind   = m_mapClassNameToID.size();
    size_t       nIDsFound    = 0LLU;

    while (pClientClass != nullptr)
    {
        // Finding & storing class ID.
        auto it = m_mapClassNameToID.find(std::string(pClientClass->m_pNetworkName));
        if (it != m_mapClassNameToID.end())
        {
            *(it->second->m_pDestination) = pClientClass->m_ClassID;
            if(pClientClass->m_ClassID > 0)
            {
                nIDsFound++;
                LOG("initialized [ %s ] with class ID [ %d ]", pClientClass->m_pNetworkName, pClientClass->m_ClassID);
            }
            else
            {
                FAIL_LOG("Bullshit class ID [ %d ] for class [ %s ]", pClientClass->m_ClassID, pClientClass->m_pNetworkName);
            }
        }

        pClientClass = pClientClass->m_pNext;
    }

    
    // we found all the IDs!
    if (nIDsFound == nIDsToFind)
    {
        m_bInitialized = true;
        WIN_LOG("CACHED CLASS IDs");

        _ConstructStaticClassIDMap();

        return true;
    }

    FAIL_LOG("Failed to find all classes, [ %llu ] remaining", nIDsToFind - nIDsFound);
    return false;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool ClassIDHandler_t::IsInitialized() const
{
    return m_bInitialized;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
ClassIDStatic_t ClassIDHandler_t::GetStaticClassID(BaseEntity* pEnt)
{
    auto it = m_mapClassIDtoStaticClassID.find(pEnt->GetClientClass()->m_ClassID);
    if (it == m_mapClassIDtoStaticClassID.end())
        return ClassIDStatic_t::CLASSIDSTATIC_INVALID;

    return it->second;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
ClassIDStatic_t ClassIDHandler_t::GetStaticClassID(int iClassID)
{
    auto it = m_mapClassIDtoStaticClassID.find(iClassID);
    if (it == m_mapClassIDtoStaticClassID.end())
        return ClassIDStatic_t::CLASSIDSTATIC_INVALID;

    return it->second;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
ClassIDStatic_t ClassIDHandler_t::InvalidStaticClassID()
{
    return ClassIDStatic_t::CLASSIDSTATIC_INVALID;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
const std::unordered_map<int, ClassIDStatic_t>* ClassIDHandler_t::GetStaticClassIDMap()
{
    return &m_mapClassIDtoStaticClassID;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool ClassIDHandler_t::_ConstructStaticClassIDMap()
{
    // None of these maps need to be filled up from scratch here.
    m_mapClassNameToStaticClassID.clear(); m_mapClassIDtoStaticClassID.clear();


    m_mapClassNameToStaticClassID.emplace("CTFWearableRazorback", ClassIDStatic_t::ClassIDStatic_CTFWearableRazorback);
    m_mapClassNameToStaticClassID.emplace("CTFWearableDemoShield", ClassIDStatic_t::ClassIDStatic_CTFWearableDemoShield);
    m_mapClassNameToStaticClassID.emplace("CTFWearableLevelableItem", ClassIDStatic_t::ClassIDStatic_CTFWearableLevelableItem);
    m_mapClassNameToStaticClassID.emplace("CTFWearableCampaignItem", ClassIDStatic_t::ClassIDStatic_CTFWearableCampaignItem);
    m_mapClassNameToStaticClassID.emplace("CTFBaseRocket", ClassIDStatic_t::ClassIDStatic_CTFBaseRocket);
    m_mapClassNameToStaticClassID.emplace("CTFWeaponBaseMerasmusGrenade", ClassIDStatic_t::ClassIDStatic_CTFWeaponBaseMerasmusGrenade);
    m_mapClassNameToStaticClassID.emplace("CTFWeaponBaseMelee", ClassIDStatic_t::ClassIDStatic_CTFWeaponBaseMelee);
    m_mapClassNameToStaticClassID.emplace("CTFWeaponBaseGun", ClassIDStatic_t::ClassIDStatic_CTFWeaponBaseGun);
    m_mapClassNameToStaticClassID.emplace("CTFWeaponBaseGrenadeProj", ClassIDStatic_t::ClassIDStatic_CTFWeaponBaseGrenadeProj);
    m_mapClassNameToStaticClassID.emplace("CTFWeaponBase", ClassIDStatic_t::ClassIDStatic_CTFWeaponBase);
    m_mapClassNameToStaticClassID.emplace("CTFWearableRobotArm", ClassIDStatic_t::ClassIDStatic_CTFWearableRobotArm);
    m_mapClassNameToStaticClassID.emplace("CTFRobotArm", ClassIDStatic_t::ClassIDStatic_CTFRobotArm);
    m_mapClassNameToStaticClassID.emplace("CTFWrench", ClassIDStatic_t::ClassIDStatic_CTFWrench);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_ThrowableBreadMonster", ClassIDStatic_t::ClassIDStatic_CTFProjectile_ThrowableBreadMonster);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_ThrowableBrick", ClassIDStatic_t::ClassIDStatic_CTFProjectile_ThrowableBrick);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_ThrowableRepel", ClassIDStatic_t::ClassIDStatic_CTFProjectile_ThrowableRepel);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_Throwable", ClassIDStatic_t::ClassIDStatic_CTFProjectile_Throwable);
    m_mapClassNameToStaticClassID.emplace("CTFThrowable", ClassIDStatic_t::ClassIDStatic_CTFThrowable);
    m_mapClassNameToStaticClassID.emplace("CTFSyringeGun", ClassIDStatic_t::ClassIDStatic_CTFSyringeGun);
    m_mapClassNameToStaticClassID.emplace("CTFKatana", ClassIDStatic_t::ClassIDStatic_CTFKatana);
    m_mapClassNameToStaticClassID.emplace("CTFSword", ClassIDStatic_t::ClassIDStatic_CTFSword);
    m_mapClassNameToStaticClassID.emplace("CSniperDot", ClassIDStatic_t::ClassIDStatic_CSniperDot);
    m_mapClassNameToStaticClassID.emplace("CTFSniperRifleClassic", ClassIDStatic_t::ClassIDStatic_CTFSniperRifleClassic);
    m_mapClassNameToStaticClassID.emplace("CTFSniperRifleDecap", ClassIDStatic_t::ClassIDStatic_CTFSniperRifleDecap);
    m_mapClassNameToStaticClassID.emplace("CTFSniperRifle", ClassIDStatic_t::ClassIDStatic_CTFSniperRifle);
    m_mapClassNameToStaticClassID.emplace("CTFChargedSMG", ClassIDStatic_t::ClassIDStatic_CTFChargedSMG);
    m_mapClassNameToStaticClassID.emplace("CTFSMG", ClassIDStatic_t::ClassIDStatic_CTFSMG);
    m_mapClassNameToStaticClassID.emplace("CTFSlap", ClassIDStatic_t::ClassIDStatic_CTFSlap);
    m_mapClassNameToStaticClassID.emplace("CTFShovel", ClassIDStatic_t::ClassIDStatic_CTFShovel);
    m_mapClassNameToStaticClassID.emplace("CTFShotgunBuildingRescue", ClassIDStatic_t::ClassIDStatic_CTFShotgunBuildingRescue);
    m_mapClassNameToStaticClassID.emplace("CTFPEPBrawlerBlaster", ClassIDStatic_t::ClassIDStatic_CTFPEPBrawlerBlaster);
    m_mapClassNameToStaticClassID.emplace("CTFSodaPopper", ClassIDStatic_t::ClassIDStatic_CTFSodaPopper);
    m_mapClassNameToStaticClassID.emplace("CTFShotgun_Revenge", ClassIDStatic_t::ClassIDStatic_CTFShotgun_Revenge);
    m_mapClassNameToStaticClassID.emplace("CTFScatterGun", ClassIDStatic_t::ClassIDStatic_CTFScatterGun);
    m_mapClassNameToStaticClassID.emplace("CTFShotgun_Pyro", ClassIDStatic_t::ClassIDStatic_CTFShotgun_Pyro);
    m_mapClassNameToStaticClassID.emplace("CTFShotgun_HWG", ClassIDStatic_t::ClassIDStatic_CTFShotgun_HWG);
    m_mapClassNameToStaticClassID.emplace("CTFShotgun_Soldier", ClassIDStatic_t::ClassIDStatic_CTFShotgun_Soldier);
    m_mapClassNameToStaticClassID.emplace("CTFShotgun", ClassIDStatic_t::ClassIDStatic_CTFShotgun);
    m_mapClassNameToStaticClassID.emplace("CTFRocketPack", ClassIDStatic_t::ClassIDStatic_CTFRocketPack);
    m_mapClassNameToStaticClassID.emplace("CTFCrossbow", ClassIDStatic_t::ClassIDStatic_CTFCrossbow);
    m_mapClassNameToStaticClassID.emplace("CTFRocketLauncher_Mortar", ClassIDStatic_t::ClassIDStatic_CTFRocketLauncher_Mortar);
    m_mapClassNameToStaticClassID.emplace("CTFRocketLauncher_AirStrike", ClassIDStatic_t::ClassIDStatic_CTFRocketLauncher_AirStrike);
    m_mapClassNameToStaticClassID.emplace("CTFRocketLauncher_DirectHit", ClassIDStatic_t::ClassIDStatic_CTFRocketLauncher_DirectHit);
    m_mapClassNameToStaticClassID.emplace("CTFRocketLauncher", ClassIDStatic_t::ClassIDStatic_CTFRocketLauncher);
    m_mapClassNameToStaticClassID.emplace("CTFRevolver", ClassIDStatic_t::ClassIDStatic_CTFRevolver);
    m_mapClassNameToStaticClassID.emplace("CTFDRGPomson", ClassIDStatic_t::ClassIDStatic_CTFDRGPomson);
    m_mapClassNameToStaticClassID.emplace("CTFRaygun", ClassIDStatic_t::ClassIDStatic_CTFRaygun);
    m_mapClassNameToStaticClassID.emplace("CTFPistol_ScoutSecondary", ClassIDStatic_t::ClassIDStatic_CTFPistol_ScoutSecondary);
    m_mapClassNameToStaticClassID.emplace("CTFPistol_ScoutPrimary", ClassIDStatic_t::ClassIDStatic_CTFPistol_ScoutPrimary);
    m_mapClassNameToStaticClassID.emplace("CTFPistol_Scout", ClassIDStatic_t::ClassIDStatic_CTFPistol_Scout);
    m_mapClassNameToStaticClassID.emplace("CTFPistol", ClassIDStatic_t::ClassIDStatic_CTFPistol);
    m_mapClassNameToStaticClassID.emplace("CTFPipebombLauncher", ClassIDStatic_t::ClassIDStatic_CTFPipebombLauncher);
    m_mapClassNameToStaticClassID.emplace("CTFWeaponPDA_Spy", ClassIDStatic_t::ClassIDStatic_CTFWeaponPDA_Spy);
    m_mapClassNameToStaticClassID.emplace("CTFWeaponPDA_Engineer_Destroy", ClassIDStatic_t::ClassIDStatic_CTFWeaponPDA_Engineer_Destroy);
    m_mapClassNameToStaticClassID.emplace("CTFWeaponPDA_Engineer_Build", ClassIDStatic_t::ClassIDStatic_CTFWeaponPDA_Engineer_Build);
    m_mapClassNameToStaticClassID.emplace("CTFWeaponPDAExpansion_Teleporter", ClassIDStatic_t::ClassIDStatic_CTFWeaponPDAExpansion_Teleporter);
    m_mapClassNameToStaticClassID.emplace("CTFWeaponPDAExpansion_Dispenser", ClassIDStatic_t::ClassIDStatic_CTFWeaponPDAExpansion_Dispenser);
    m_mapClassNameToStaticClassID.emplace("CTFWeaponPDA", ClassIDStatic_t::ClassIDStatic_CTFWeaponPDA);
    m_mapClassNameToStaticClassID.emplace("CTFParticleCannon", ClassIDStatic_t::ClassIDStatic_CTFParticleCannon);
    m_mapClassNameToStaticClassID.emplace("CTFParachute_Secondary", ClassIDStatic_t::ClassIDStatic_CTFParachute_Secondary);
    m_mapClassNameToStaticClassID.emplace("CTFParachute_Primary", ClassIDStatic_t::ClassIDStatic_CTFParachute_Primary);
    m_mapClassNameToStaticClassID.emplace("CTFParachute", ClassIDStatic_t::ClassIDStatic_CTFParachute);
    m_mapClassNameToStaticClassID.emplace("CTFMinigun", ClassIDStatic_t::ClassIDStatic_CTFMinigun);
    m_mapClassNameToStaticClassID.emplace("CTFMedigunShield", ClassIDStatic_t::ClassIDStatic_CTFMedigunShield);
    m_mapClassNameToStaticClassID.emplace("CWeaponMedigun", ClassIDStatic_t::ClassIDStatic_CWeaponMedigun);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_MechanicalArmOrb", ClassIDStatic_t::ClassIDStatic_CTFProjectile_MechanicalArmOrb);
    m_mapClassNameToStaticClassID.emplace("CTFMechanicalArm", ClassIDStatic_t::ClassIDStatic_CTFMechanicalArm);
    m_mapClassNameToStaticClassID.emplace("CTFLunchBox_Drink", ClassIDStatic_t::ClassIDStatic_CTFLunchBox_Drink);
    m_mapClassNameToStaticClassID.emplace("CTFLunchBox", ClassIDStatic_t::ClassIDStatic_CTFLunchBox);
    m_mapClassNameToStaticClassID.emplace("CLaserDot", ClassIDStatic_t::ClassIDStatic_CLaserDot);
    m_mapClassNameToStaticClassID.emplace("CTFLaserPointer", ClassIDStatic_t::ClassIDStatic_CTFLaserPointer);
    m_mapClassNameToStaticClassID.emplace("CTFKnife", ClassIDStatic_t::ClassIDStatic_CTFKnife);
    m_mapClassNameToStaticClassID.emplace("CTFGasManager", ClassIDStatic_t::ClassIDStatic_CTFGasManager);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_JarGas", ClassIDStatic_t::ClassIDStatic_CTFProjectile_JarGas);
    m_mapClassNameToStaticClassID.emplace("CTFJarGas", ClassIDStatic_t::ClassIDStatic_CTFJarGas);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_Cleaver", ClassIDStatic_t::ClassIDStatic_CTFProjectile_Cleaver);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_JarMilk", ClassIDStatic_t::ClassIDStatic_CTFProjectile_JarMilk);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_Jar", ClassIDStatic_t::ClassIDStatic_CTFProjectile_Jar);
    m_mapClassNameToStaticClassID.emplace("CTFCleaver", ClassIDStatic_t::ClassIDStatic_CTFCleaver);
    m_mapClassNameToStaticClassID.emplace("CTFJarMilk", ClassIDStatic_t::ClassIDStatic_CTFJarMilk);
    m_mapClassNameToStaticClassID.emplace("CTFJar", ClassIDStatic_t::ClassIDStatic_CTFJar);
    m_mapClassNameToStaticClassID.emplace("CTFWeaponInvis", ClassIDStatic_t::ClassIDStatic_CTFWeaponInvis);
    m_mapClassNameToStaticClassID.emplace("CTFCannon", ClassIDStatic_t::ClassIDStatic_CTFCannon);
    m_mapClassNameToStaticClassID.emplace("CTFGrenadeLauncher", ClassIDStatic_t::ClassIDStatic_CTFGrenadeLauncher);
    m_mapClassNameToStaticClassID.emplace("CTFGrenadePipebombProjectile", ClassIDStatic_t::ClassIDStatic_CTFGrenadePipebombProjectile);
    m_mapClassNameToStaticClassID.emplace("CTFGrapplingHook", ClassIDStatic_t::ClassIDStatic_CTFGrapplingHook);
    m_mapClassNameToStaticClassID.emplace("CTFFlareGun_Revenge", ClassIDStatic_t::ClassIDStatic_CTFFlareGun_Revenge);
    m_mapClassNameToStaticClassID.emplace("CTFFlareGun", ClassIDStatic_t::ClassIDStatic_CTFFlareGun);
    m_mapClassNameToStaticClassID.emplace("CTFFlameRocket", ClassIDStatic_t::ClassIDStatic_CTFFlameRocket);
    m_mapClassNameToStaticClassID.emplace("CTFFlameThrower", ClassIDStatic_t::ClassIDStatic_CTFFlameThrower);
    m_mapClassNameToStaticClassID.emplace("CTFFists", ClassIDStatic_t::ClassIDStatic_CTFFists);
    m_mapClassNameToStaticClassID.emplace("CTFFireAxe", ClassIDStatic_t::ClassIDStatic_CTFFireAxe);
    m_mapClassNameToStaticClassID.emplace("CTFWeaponFlameBall", ClassIDStatic_t::ClassIDStatic_CTFWeaponFlameBall);
    m_mapClassNameToStaticClassID.emplace("CTFCompoundBow", ClassIDStatic_t::ClassIDStatic_CTFCompoundBow);
    m_mapClassNameToStaticClassID.emplace("CTFClub", ClassIDStatic_t::ClassIDStatic_CTFClub);
    m_mapClassNameToStaticClassID.emplace("CTFBuffItem", ClassIDStatic_t::ClassIDStatic_CTFBuffItem);
    m_mapClassNameToStaticClassID.emplace("CTFStickBomb", ClassIDStatic_t::ClassIDStatic_CTFStickBomb);
    m_mapClassNameToStaticClassID.emplace("CTFBreakableSign", ClassIDStatic_t::ClassIDStatic_CTFBreakableSign);
    m_mapClassNameToStaticClassID.emplace("CTFBottle", ClassIDStatic_t::ClassIDStatic_CTFBottle);
    m_mapClassNameToStaticClassID.emplace("CTFBreakableMelee", ClassIDStatic_t::ClassIDStatic_CTFBreakableMelee);
    m_mapClassNameToStaticClassID.emplace("CTFBonesaw", ClassIDStatic_t::ClassIDStatic_CTFBonesaw);
    m_mapClassNameToStaticClassID.emplace("CTFBall_Ornament", ClassIDStatic_t::ClassIDStatic_CTFBall_Ornament);
    m_mapClassNameToStaticClassID.emplace("CTFStunBall", ClassIDStatic_t::ClassIDStatic_CTFStunBall);
    m_mapClassNameToStaticClassID.emplace("CTFBat_Giftwrap", ClassIDStatic_t::ClassIDStatic_CTFBat_Giftwrap);
    m_mapClassNameToStaticClassID.emplace("CTFBat_Wood", ClassIDStatic_t::ClassIDStatic_CTFBat_Wood);
    m_mapClassNameToStaticClassID.emplace("CTFBat_Fish", ClassIDStatic_t::ClassIDStatic_CTFBat_Fish);
    m_mapClassNameToStaticClassID.emplace("CTFBat", ClassIDStatic_t::ClassIDStatic_CTFBat);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_EnergyRing", ClassIDStatic_t::ClassIDStatic_CTFProjectile_EnergyRing);
    m_mapClassNameToStaticClassID.emplace("CTFDroppedWeapon", ClassIDStatic_t::ClassIDStatic_CTFDroppedWeapon);
    m_mapClassNameToStaticClassID.emplace("CTFWeaponSapper", ClassIDStatic_t::ClassIDStatic_CTFWeaponSapper);
    m_mapClassNameToStaticClassID.emplace("CTFWeaponBuilder", ClassIDStatic_t::ClassIDStatic_CTFWeaponBuilder);
    m_mapClassNameToStaticClassID.emplace("C_TFWeaponBuilder", ClassIDStatic_t::ClassIDStatic_C_TFWeaponBuilder);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_Rocket", ClassIDStatic_t::ClassIDStatic_CTFProjectile_Rocket);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_Flare", ClassIDStatic_t::ClassIDStatic_CTFProjectile_Flare);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_EnergyBall", ClassIDStatic_t::ClassIDStatic_CTFProjectile_EnergyBall);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_GrapplingHook", ClassIDStatic_t::ClassIDStatic_CTFProjectile_GrapplingHook);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_HealingBolt", ClassIDStatic_t::ClassIDStatic_CTFProjectile_HealingBolt);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_Arrow", ClassIDStatic_t::ClassIDStatic_CTFProjectile_Arrow);
    m_mapClassNameToStaticClassID.emplace("CMannVsMachineStats", ClassIDStatic_t::ClassIDStatic_CMannVsMachineStats);
    m_mapClassNameToStaticClassID.emplace("CTFTankBoss", ClassIDStatic_t::ClassIDStatic_CTFTankBoss);
    m_mapClassNameToStaticClassID.emplace("CTFBaseBoss", ClassIDStatic_t::ClassIDStatic_CTFBaseBoss);
    m_mapClassNameToStaticClassID.emplace("CBossAlpha", ClassIDStatic_t::ClassIDStatic_CBossAlpha);
    m_mapClassNameToStaticClassID.emplace("NextBotCombatCharacter", ClassIDStatic_t::ClassIDStatic_NextBotCombatCharacter);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_SpellKartBats", ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellKartBats);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_SpellKartOrb", ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellKartOrb);
    m_mapClassNameToStaticClassID.emplace("CTFHellZap", ClassIDStatic_t::ClassIDStatic_CTFHellZap);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_SpellLightningOrb", ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellLightningOrb);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_SpellTransposeTeleport", ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellTransposeTeleport);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_SpellMeteorShower", ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellMeteorShower);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_SpellSpawnBoss", ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellSpawnBoss);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_SpellMirv", ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellMirv);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_SpellPumpkin", ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellPumpkin);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_SpellSpawnHorde", ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellSpawnHorde);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_SpellSpawnZombie", ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellSpawnZombie);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_SpellBats", ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellBats);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_SpellFireball", ClassIDStatic_t::ClassIDStatic_CTFProjectile_SpellFireball);
    m_mapClassNameToStaticClassID.emplace("CTFSpellBook", ClassIDStatic_t::ClassIDStatic_CTFSpellBook);
    m_mapClassNameToStaticClassID.emplace("CHightower_TeleportVortex", ClassIDStatic_t::ClassIDStatic_CHightower_TeleportVortex);
    m_mapClassNameToStaticClassID.emplace("CTeleportVortex", ClassIDStatic_t::ClassIDStatic_CTeleportVortex);
    m_mapClassNameToStaticClassID.emplace("CZombie", ClassIDStatic_t::ClassIDStatic_CZombie);
    m_mapClassNameToStaticClassID.emplace("CMerasmusDancer", ClassIDStatic_t::ClassIDStatic_CMerasmusDancer);
    m_mapClassNameToStaticClassID.emplace("CMerasmus", ClassIDStatic_t::ClassIDStatic_CMerasmus);
    m_mapClassNameToStaticClassID.emplace("CHeadlessHatman", ClassIDStatic_t::ClassIDStatic_CHeadlessHatman);
    m_mapClassNameToStaticClassID.emplace("CEyeballBoss", ClassIDStatic_t::ClassIDStatic_CEyeballBoss);
    m_mapClassNameToStaticClassID.emplace("CTFBotHintEngineerNest", ClassIDStatic_t::ClassIDStatic_CTFBotHintEngineerNest);
    m_mapClassNameToStaticClassID.emplace("CBotNPCMinion", ClassIDStatic_t::ClassIDStatic_CBotNPCMinion);
    m_mapClassNameToStaticClassID.emplace("CBotNPC", ClassIDStatic_t::ClassIDStatic_CBotNPC);
    m_mapClassNameToStaticClassID.emplace("CPasstimeGun", ClassIDStatic_t::ClassIDStatic_CPasstimeGun);
    m_mapClassNameToStaticClassID.emplace("CTFViewModel", ClassIDStatic_t::ClassIDStatic_CTFViewModel);
    m_mapClassNameToStaticClassID.emplace("CRobotDispenser", ClassIDStatic_t::ClassIDStatic_CRobotDispenser);
    m_mapClassNameToStaticClassID.emplace("CTFRobotDestruction_Robot", ClassIDStatic_t::ClassIDStatic_CTFRobotDestruction_Robot);
    m_mapClassNameToStaticClassID.emplace("CTFReviveMarker", ClassIDStatic_t::ClassIDStatic_CTFReviveMarker);
    m_mapClassNameToStaticClassID.emplace("CTFPumpkinBomb", ClassIDStatic_t::ClassIDStatic_CTFPumpkinBomb);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_BallOfFire", ClassIDStatic_t::ClassIDStatic_CTFProjectile_BallOfFire);
    m_mapClassNameToStaticClassID.emplace("CTFBaseProjectile", ClassIDStatic_t::ClassIDStatic_CTFBaseProjectile);
    m_mapClassNameToStaticClassID.emplace("CTFPointManager", ClassIDStatic_t::ClassIDStatic_CTFPointManager);
    m_mapClassNameToStaticClassID.emplace("CBaseObjectUpgrade", ClassIDStatic_t::ClassIDStatic_CBaseObjectUpgrade);
    m_mapClassNameToStaticClassID.emplace("CTFRobotDestructionLogic", ClassIDStatic_t::ClassIDStatic_CTFRobotDestructionLogic);
    m_mapClassNameToStaticClassID.emplace("CTFRobotDestruction_RobotGroup", ClassIDStatic_t::ClassIDStatic_CTFRobotDestruction_RobotGroup);
    m_mapClassNameToStaticClassID.emplace("CTFRobotDestruction_RobotSpawn", ClassIDStatic_t::ClassIDStatic_CTFRobotDestruction_RobotSpawn);
    m_mapClassNameToStaticClassID.emplace("CTFPlayerDestructionLogic", ClassIDStatic_t::ClassIDStatic_CTFPlayerDestructionLogic);
    m_mapClassNameToStaticClassID.emplace("CPlayerDestructionDispenser", ClassIDStatic_t::ClassIDStatic_CPlayerDestructionDispenser);
    m_mapClassNameToStaticClassID.emplace("CTFMinigameLogic", ClassIDStatic_t::ClassIDStatic_CTFMinigameLogic);
    m_mapClassNameToStaticClassID.emplace("CTFHalloweenMinigame_FallingPlatforms", ClassIDStatic_t::ClassIDStatic_CTFHalloweenMinigame_FallingPlatforms);
    m_mapClassNameToStaticClassID.emplace("CTFHalloweenMinigame", ClassIDStatic_t::ClassIDStatic_CTFHalloweenMinigame);
    m_mapClassNameToStaticClassID.emplace("CTFMiniGame", ClassIDStatic_t::ClassIDStatic_CTFMiniGame);
    m_mapClassNameToStaticClassID.emplace("CTFPowerupBottle", ClassIDStatic_t::ClassIDStatic_CTFPowerupBottle);
    m_mapClassNameToStaticClassID.emplace("CTFItem", ClassIDStatic_t::ClassIDStatic_CTFItem);
    m_mapClassNameToStaticClassID.emplace("CHalloweenSoulPack", ClassIDStatic_t::ClassIDStatic_CHalloweenSoulPack);
    m_mapClassNameToStaticClassID.emplace("CTFGenericBomb", ClassIDStatic_t::ClassIDStatic_CTFGenericBomb);
    m_mapClassNameToStaticClassID.emplace("CBonusRoundLogic", ClassIDStatic_t::ClassIDStatic_CBonusRoundLogic);
    m_mapClassNameToStaticClassID.emplace("CTFGameRulesProxy", ClassIDStatic_t::ClassIDStatic_CTFGameRulesProxy);
    m_mapClassNameToStaticClassID.emplace("CTETFParticleEffect", ClassIDStatic_t::ClassIDStatic_CTETFParticleEffect);
    m_mapClassNameToStaticClassID.emplace("CTETFExplosion", ClassIDStatic_t::ClassIDStatic_CTETFExplosion);
    m_mapClassNameToStaticClassID.emplace("CTETFBlood", ClassIDStatic_t::ClassIDStatic_CTETFBlood);
    m_mapClassNameToStaticClassID.emplace("CTFFlameManager", ClassIDStatic_t::ClassIDStatic_CTFFlameManager);
    m_mapClassNameToStaticClassID.emplace("CHalloweenGiftPickup", ClassIDStatic_t::ClassIDStatic_CHalloweenGiftPickup);
    m_mapClassNameToStaticClassID.emplace("CBonusDuckPickup", ClassIDStatic_t::ClassIDStatic_CBonusDuckPickup);
    m_mapClassNameToStaticClassID.emplace("CHalloweenPickup", ClassIDStatic_t::ClassIDStatic_CHalloweenPickup);
    m_mapClassNameToStaticClassID.emplace("CCaptureFlagReturnIcon", ClassIDStatic_t::ClassIDStatic_CCaptureFlagReturnIcon);
    m_mapClassNameToStaticClassID.emplace("CCaptureFlag", ClassIDStatic_t::ClassIDStatic_CCaptureFlag);
    m_mapClassNameToStaticClassID.emplace("CBonusPack", ClassIDStatic_t::ClassIDStatic_CBonusPack);
    m_mapClassNameToStaticClassID.emplace("CTFTeam", ClassIDStatic_t::ClassIDStatic_CTFTeam);
    m_mapClassNameToStaticClassID.emplace("CTFTauntProp", ClassIDStatic_t::ClassIDStatic_CTFTauntProp);
    m_mapClassNameToStaticClassID.emplace("CTFPlayerResource", ClassIDStatic_t::ClassIDStatic_CTFPlayerResource);
    m_mapClassNameToStaticClassID.emplace("CTFPlayer", ClassIDStatic_t::ClassIDStatic_CTFPlayer);
    m_mapClassNameToStaticClassID.emplace("CTFRagdoll", ClassIDStatic_t::ClassIDStatic_CTFRagdoll);
    m_mapClassNameToStaticClassID.emplace("CTEPlayerAnimEvent", ClassIDStatic_t::ClassIDStatic_CTEPlayerAnimEvent);
    m_mapClassNameToStaticClassID.emplace("CTFPasstimeLogic", ClassIDStatic_t::ClassIDStatic_CTFPasstimeLogic);
    m_mapClassNameToStaticClassID.emplace("CPasstimeBall", ClassIDStatic_t::ClassIDStatic_CPasstimeBall);
    m_mapClassNameToStaticClassID.emplace("CTFObjectiveResource", ClassIDStatic_t::ClassIDStatic_CTFObjectiveResource);
    m_mapClassNameToStaticClassID.emplace("CTFGlow", ClassIDStatic_t::ClassIDStatic_CTFGlow);
    m_mapClassNameToStaticClassID.emplace("CTEFireBullets", ClassIDStatic_t::ClassIDStatic_CTEFireBullets);
    m_mapClassNameToStaticClassID.emplace("CTFBuffBanner", ClassIDStatic_t::ClassIDStatic_CTFBuffBanner);
    m_mapClassNameToStaticClassID.emplace("CTFAmmoPack", ClassIDStatic_t::ClassIDStatic_CTFAmmoPack);
    m_mapClassNameToStaticClassID.emplace("CObjectTeleporter", ClassIDStatic_t::ClassIDStatic_CObjectTeleporter);
    m_mapClassNameToStaticClassID.emplace("CObjectSentrygun", ClassIDStatic_t::ClassIDStatic_CObjectSentrygun);
    m_mapClassNameToStaticClassID.emplace("CTFProjectile_SentryRocket", ClassIDStatic_t::ClassIDStatic_CTFProjectile_SentryRocket);
    m_mapClassNameToStaticClassID.emplace("CObjectSapper", ClassIDStatic_t::ClassIDStatic_CObjectSapper);
    m_mapClassNameToStaticClassID.emplace("CObjectCartDispenser", ClassIDStatic_t::ClassIDStatic_CObjectCartDispenser);
    m_mapClassNameToStaticClassID.emplace("CObjectDispenser", ClassIDStatic_t::ClassIDStatic_CObjectDispenser);
    m_mapClassNameToStaticClassID.emplace("CMonsterResource", ClassIDStatic_t::ClassIDStatic_CMonsterResource);
    m_mapClassNameToStaticClassID.emplace("CFuncRespawnRoomVisualizer", ClassIDStatic_t::ClassIDStatic_CFuncRespawnRoomVisualizer);
    m_mapClassNameToStaticClassID.emplace("CFuncRespawnRoom", ClassIDStatic_t::ClassIDStatic_CFuncRespawnRoom);
    m_mapClassNameToStaticClassID.emplace("CFuncPasstimeGoal", ClassIDStatic_t::ClassIDStatic_CFuncPasstimeGoal);
    m_mapClassNameToStaticClassID.emplace("CFuncForceField", ClassIDStatic_t::ClassIDStatic_CFuncForceField);
    m_mapClassNameToStaticClassID.emplace("CCaptureZone", ClassIDStatic_t::ClassIDStatic_CCaptureZone);
    m_mapClassNameToStaticClassID.emplace("CCurrencyPack", ClassIDStatic_t::ClassIDStatic_CCurrencyPack);
    m_mapClassNameToStaticClassID.emplace("CBaseObject", ClassIDStatic_t::ClassIDStatic_CBaseObject);
    m_mapClassNameToStaticClassID.emplace("CTestTraceline", ClassIDStatic_t::ClassIDStatic_CTestTraceline);
    m_mapClassNameToStaticClassID.emplace("CTEWorldDecal", ClassIDStatic_t::ClassIDStatic_CTEWorldDecal);
    m_mapClassNameToStaticClassID.emplace("CTESpriteSpray", ClassIDStatic_t::ClassIDStatic_CTESpriteSpray);
    m_mapClassNameToStaticClassID.emplace("CTESprite", ClassIDStatic_t::ClassIDStatic_CTESprite);
    m_mapClassNameToStaticClassID.emplace("CTESparks", ClassIDStatic_t::ClassIDStatic_CTESparks);
    m_mapClassNameToStaticClassID.emplace("CTESmoke", ClassIDStatic_t::ClassIDStatic_CTESmoke);
    m_mapClassNameToStaticClassID.emplace("CTEShowLine", ClassIDStatic_t::ClassIDStatic_CTEShowLine);
    m_mapClassNameToStaticClassID.emplace("CTEProjectedDecal", ClassIDStatic_t::ClassIDStatic_CTEProjectedDecal);
    m_mapClassNameToStaticClassID.emplace("CTEPlayerDecal", ClassIDStatic_t::ClassIDStatic_CTEPlayerDecal);
    m_mapClassNameToStaticClassID.emplace("CTEPhysicsProp", ClassIDStatic_t::ClassIDStatic_CTEPhysicsProp);
    m_mapClassNameToStaticClassID.emplace("CTEParticleSystem", ClassIDStatic_t::ClassIDStatic_CTEParticleSystem);
    m_mapClassNameToStaticClassID.emplace("CTEMuzzleFlash", ClassIDStatic_t::ClassIDStatic_CTEMuzzleFlash);
    m_mapClassNameToStaticClassID.emplace("CTELargeFunnel", ClassIDStatic_t::ClassIDStatic_CTELargeFunnel);
    m_mapClassNameToStaticClassID.emplace("CTEKillPlayerAttachments", ClassIDStatic_t::ClassIDStatic_CTEKillPlayerAttachments);
    m_mapClassNameToStaticClassID.emplace("CTEImpact", ClassIDStatic_t::ClassIDStatic_CTEImpact);
    m_mapClassNameToStaticClassID.emplace("CTEGlowSprite", ClassIDStatic_t::ClassIDStatic_CTEGlowSprite);
    m_mapClassNameToStaticClassID.emplace("CTEShatterSurface", ClassIDStatic_t::ClassIDStatic_CTEShatterSurface);
    m_mapClassNameToStaticClassID.emplace("CTEFootprintDecal", ClassIDStatic_t::ClassIDStatic_CTEFootprintDecal);
    m_mapClassNameToStaticClassID.emplace("CTEFizz", ClassIDStatic_t::ClassIDStatic_CTEFizz);
    m_mapClassNameToStaticClassID.emplace("CTEExplosion", ClassIDStatic_t::ClassIDStatic_CTEExplosion);
    m_mapClassNameToStaticClassID.emplace("CTEEnergySplash", ClassIDStatic_t::ClassIDStatic_CTEEnergySplash);
    m_mapClassNameToStaticClassID.emplace("CTEEffectDispatch", ClassIDStatic_t::ClassIDStatic_CTEEffectDispatch);
    m_mapClassNameToStaticClassID.emplace("CTEDynamicLight", ClassIDStatic_t::ClassIDStatic_CTEDynamicLight);
    m_mapClassNameToStaticClassID.emplace("CTEDecal", ClassIDStatic_t::ClassIDStatic_CTEDecal);
    m_mapClassNameToStaticClassID.emplace("CTEClientProjectile", ClassIDStatic_t::ClassIDStatic_CTEClientProjectile);
    m_mapClassNameToStaticClassID.emplace("CTEBubbleTrail", ClassIDStatic_t::ClassIDStatic_CTEBubbleTrail);
    m_mapClassNameToStaticClassID.emplace("CTEBubbles", ClassIDStatic_t::ClassIDStatic_CTEBubbles);
    m_mapClassNameToStaticClassID.emplace("CTEBSPDecal", ClassIDStatic_t::ClassIDStatic_CTEBSPDecal);
    m_mapClassNameToStaticClassID.emplace("CTEBreakModel", ClassIDStatic_t::ClassIDStatic_CTEBreakModel);
    m_mapClassNameToStaticClassID.emplace("CTEBloodStream", ClassIDStatic_t::ClassIDStatic_CTEBloodStream);
    m_mapClassNameToStaticClassID.emplace("CTEBloodSprite", ClassIDStatic_t::ClassIDStatic_CTEBloodSprite);
    m_mapClassNameToStaticClassID.emplace("CTEBeamSpline", ClassIDStatic_t::ClassIDStatic_CTEBeamSpline);
    m_mapClassNameToStaticClassID.emplace("CTEBeamRingPoint", ClassIDStatic_t::ClassIDStatic_CTEBeamRingPoint);
    m_mapClassNameToStaticClassID.emplace("CTEBeamRing", ClassIDStatic_t::ClassIDStatic_CTEBeamRing);
    m_mapClassNameToStaticClassID.emplace("CTEBeamPoints", ClassIDStatic_t::ClassIDStatic_CTEBeamPoints);
    m_mapClassNameToStaticClassID.emplace("CTEBeamLaser", ClassIDStatic_t::ClassIDStatic_CTEBeamLaser);
    m_mapClassNameToStaticClassID.emplace("CTEBeamFollow", ClassIDStatic_t::ClassIDStatic_CTEBeamFollow);
    m_mapClassNameToStaticClassID.emplace("CTEBeamEnts", ClassIDStatic_t::ClassIDStatic_CTEBeamEnts);
    m_mapClassNameToStaticClassID.emplace("CTEBeamEntPoint", ClassIDStatic_t::ClassIDStatic_CTEBeamEntPoint);
    m_mapClassNameToStaticClassID.emplace("CTEBaseBeam", ClassIDStatic_t::ClassIDStatic_CTEBaseBeam);
    m_mapClassNameToStaticClassID.emplace("CTEArmorRicochet", ClassIDStatic_t::ClassIDStatic_CTEArmorRicochet);
    m_mapClassNameToStaticClassID.emplace("CTEMetalSparks", ClassIDStatic_t::ClassIDStatic_CTEMetalSparks);
    m_mapClassNameToStaticClassID.emplace("CSteamJet", ClassIDStatic_t::ClassIDStatic_CSteamJet);
    m_mapClassNameToStaticClassID.emplace("CSmokeStack", ClassIDStatic_t::ClassIDStatic_CSmokeStack);
    m_mapClassNameToStaticClassID.emplace("DustTrail", ClassIDStatic_t::ClassIDStatic_DustTrail);
    m_mapClassNameToStaticClassID.emplace("CFireTrail", ClassIDStatic_t::ClassIDStatic_CFireTrail);
    m_mapClassNameToStaticClassID.emplace("SporeTrail", ClassIDStatic_t::ClassIDStatic_SporeTrail);
    m_mapClassNameToStaticClassID.emplace("SporeExplosion", ClassIDStatic_t::ClassIDStatic_SporeExplosion);
    m_mapClassNameToStaticClassID.emplace("RocketTrail", ClassIDStatic_t::ClassIDStatic_RocketTrail);
    m_mapClassNameToStaticClassID.emplace("SmokeTrail", ClassIDStatic_t::ClassIDStatic_SmokeTrail);
    m_mapClassNameToStaticClassID.emplace("CPropVehicleDriveable", ClassIDStatic_t::ClassIDStatic_CPropVehicleDriveable);
    m_mapClassNameToStaticClassID.emplace("ParticleSmokeGrenade", ClassIDStatic_t::ClassIDStatic_ParticleSmokeGrenade);
    m_mapClassNameToStaticClassID.emplace("CParticleFire", ClassIDStatic_t::ClassIDStatic_CParticleFire);
    m_mapClassNameToStaticClassID.emplace("MovieExplosion", ClassIDStatic_t::ClassIDStatic_MovieExplosion);
    m_mapClassNameToStaticClassID.emplace("CTEGaussExplosion", ClassIDStatic_t::ClassIDStatic_CTEGaussExplosion);
    m_mapClassNameToStaticClassID.emplace("CEnvQuadraticBeam", ClassIDStatic_t::ClassIDStatic_CEnvQuadraticBeam);
    m_mapClassNameToStaticClassID.emplace("CEmbers", ClassIDStatic_t::ClassIDStatic_CEmbers);
    m_mapClassNameToStaticClassID.emplace("CEnvWind", ClassIDStatic_t::ClassIDStatic_CEnvWind);
    m_mapClassNameToStaticClassID.emplace("CPrecipitation", ClassIDStatic_t::ClassIDStatic_CPrecipitation);
    m_mapClassNameToStaticClassID.emplace("CBaseTempEntity", ClassIDStatic_t::ClassIDStatic_CBaseTempEntity);
    m_mapClassNameToStaticClassID.emplace("CWeaponIFMSteadyCam", ClassIDStatic_t::ClassIDStatic_CWeaponIFMSteadyCam);
    m_mapClassNameToStaticClassID.emplace("CWeaponIFMBaseCamera", ClassIDStatic_t::ClassIDStatic_CWeaponIFMBaseCamera);
    m_mapClassNameToStaticClassID.emplace("CWeaponIFMBase", ClassIDStatic_t::ClassIDStatic_CWeaponIFMBase);
    m_mapClassNameToStaticClassID.emplace("CTFWearableVM", ClassIDStatic_t::ClassIDStatic_CTFWearableVM);
    m_mapClassNameToStaticClassID.emplace("CTFWearable", ClassIDStatic_t::ClassIDStatic_CTFWearable);
    m_mapClassNameToStaticClassID.emplace("CTFWearableItem", ClassIDStatic_t::ClassIDStatic_CTFWearableItem);
    m_mapClassNameToStaticClassID.emplace("CEconWearable", ClassIDStatic_t::ClassIDStatic_CEconWearable);
    m_mapClassNameToStaticClassID.emplace("CBaseAttributableItem", ClassIDStatic_t::ClassIDStatic_CBaseAttributableItem);
    m_mapClassNameToStaticClassID.emplace("CEconEntity", ClassIDStatic_t::ClassIDStatic_CEconEntity);
    m_mapClassNameToStaticClassID.emplace("CHandleTest", ClassIDStatic_t::ClassIDStatic_CHandleTest);
    m_mapClassNameToStaticClassID.emplace("CTeamplayRoundBasedRulesProxy", ClassIDStatic_t::ClassIDStatic_CTeamplayRoundBasedRulesProxy);
    m_mapClassNameToStaticClassID.emplace("CTeamRoundTimer", ClassIDStatic_t::ClassIDStatic_CTeamRoundTimer);
    m_mapClassNameToStaticClassID.emplace("CSpriteTrail", ClassIDStatic_t::ClassIDStatic_CSpriteTrail);
    m_mapClassNameToStaticClassID.emplace("CSpriteOriented", ClassIDStatic_t::ClassIDStatic_CSpriteOriented);
    m_mapClassNameToStaticClassID.emplace("CSprite", ClassIDStatic_t::ClassIDStatic_CSprite);
    m_mapClassNameToStaticClassID.emplace("CRagdollPropAttached", ClassIDStatic_t::ClassIDStatic_CRagdollPropAttached);
    m_mapClassNameToStaticClassID.emplace("CRagdollProp", ClassIDStatic_t::ClassIDStatic_CRagdollProp);
    m_mapClassNameToStaticClassID.emplace("CPoseController", ClassIDStatic_t::ClassIDStatic_CPoseController);
    m_mapClassNameToStaticClassID.emplace("CGameRulesProxy", ClassIDStatic_t::ClassIDStatic_CGameRulesProxy);
    m_mapClassNameToStaticClassID.emplace("CInfoLadderDismount", ClassIDStatic_t::ClassIDStatic_CInfoLadderDismount);
    m_mapClassNameToStaticClassID.emplace("CFuncLadder", ClassIDStatic_t::ClassIDStatic_CFuncLadder);
    m_mapClassNameToStaticClassID.emplace("CEnvDetailController", ClassIDStatic_t::ClassIDStatic_CEnvDetailController);
    m_mapClassNameToStaticClassID.emplace("CWorld", ClassIDStatic_t::ClassIDStatic_CWorld);
    m_mapClassNameToStaticClassID.emplace("CWaterLODControl", ClassIDStatic_t::ClassIDStatic_CWaterLODControl);
    m_mapClassNameToStaticClassID.emplace("CWaterBullet", ClassIDStatic_t::ClassIDStatic_CWaterBullet);
    m_mapClassNameToStaticClassID.emplace("CVoteController", ClassIDStatic_t::ClassIDStatic_CVoteController);
    m_mapClassNameToStaticClassID.emplace("CVGuiScreen", ClassIDStatic_t::ClassIDStatic_CVGuiScreen);
    m_mapClassNameToStaticClassID.emplace("CPropJeep", ClassIDStatic_t::ClassIDStatic_CPropJeep);
    m_mapClassNameToStaticClassID.emplace("CPropVehicleChoreoGeneric", ClassIDStatic_t::ClassIDStatic_CPropVehicleChoreoGeneric);
    m_mapClassNameToStaticClassID.emplace("CTest_ProxyToggle_Networkable", ClassIDStatic_t::ClassIDStatic_CTest_ProxyToggle_Networkable);
    m_mapClassNameToStaticClassID.emplace("CTesla", ClassIDStatic_t::ClassIDStatic_CTesla);
    m_mapClassNameToStaticClassID.emplace("CTeamTrainWatcher", ClassIDStatic_t::ClassIDStatic_CTeamTrainWatcher);
    m_mapClassNameToStaticClassID.emplace("CBaseTeamObjectiveResource", ClassIDStatic_t::ClassIDStatic_CBaseTeamObjectiveResource);
    m_mapClassNameToStaticClassID.emplace("CTeam", ClassIDStatic_t::ClassIDStatic_CTeam);
    m_mapClassNameToStaticClassID.emplace("CSun", ClassIDStatic_t::ClassIDStatic_CSun);
    m_mapClassNameToStaticClassID.emplace("CParticlePerformanceMonitor", ClassIDStatic_t::ClassIDStatic_CParticlePerformanceMonitor);
    m_mapClassNameToStaticClassID.emplace("CSpotlightEnd", ClassIDStatic_t::ClassIDStatic_CSpotlightEnd);
    m_mapClassNameToStaticClassID.emplace("CSlideshowDisplay", ClassIDStatic_t::ClassIDStatic_CSlideshowDisplay);
    m_mapClassNameToStaticClassID.emplace("CShadowControl", ClassIDStatic_t::ClassIDStatic_CShadowControl);
    m_mapClassNameToStaticClassID.emplace("CSceneEntity", ClassIDStatic_t::ClassIDStatic_CSceneEntity);
    m_mapClassNameToStaticClassID.emplace("CRopeKeyframe", ClassIDStatic_t::ClassIDStatic_CRopeKeyframe);
    m_mapClassNameToStaticClassID.emplace("CRagdollManager", ClassIDStatic_t::ClassIDStatic_CRagdollManager);
    m_mapClassNameToStaticClassID.emplace("CPhysicsPropMultiplayer", ClassIDStatic_t::ClassIDStatic_CPhysicsPropMultiplayer);
    m_mapClassNameToStaticClassID.emplace("CPhysBoxMultiplayer", ClassIDStatic_t::ClassIDStatic_CPhysBoxMultiplayer);
    m_mapClassNameToStaticClassID.emplace("CBasePropDoor", ClassIDStatic_t::ClassIDStatic_CBasePropDoor);
    m_mapClassNameToStaticClassID.emplace("CDynamicProp", ClassIDStatic_t::ClassIDStatic_CDynamicProp);
    m_mapClassNameToStaticClassID.emplace("CPointWorldText", ClassIDStatic_t::ClassIDStatic_CPointWorldText);
    m_mapClassNameToStaticClassID.emplace("CPointCommentaryNode", ClassIDStatic_t::ClassIDStatic_CPointCommentaryNode);
    m_mapClassNameToStaticClassID.emplace("CPointCamera", ClassIDStatic_t::ClassIDStatic_CPointCamera);
    m_mapClassNameToStaticClassID.emplace("CPlayerResource", ClassIDStatic_t::ClassIDStatic_CPlayerResource);
    m_mapClassNameToStaticClassID.emplace("CPlasma", ClassIDStatic_t::ClassIDStatic_CPlasma);
    m_mapClassNameToStaticClassID.emplace("CPhysMagnet", ClassIDStatic_t::ClassIDStatic_CPhysMagnet);
    m_mapClassNameToStaticClassID.emplace("CPhysicsProp", ClassIDStatic_t::ClassIDStatic_CPhysicsProp);
    m_mapClassNameToStaticClassID.emplace("CPhysBox", ClassIDStatic_t::ClassIDStatic_CPhysBox);
    m_mapClassNameToStaticClassID.emplace("CParticleSystem", ClassIDStatic_t::ClassIDStatic_CParticleSystem);
    m_mapClassNameToStaticClassID.emplace("CMaterialModifyControl", ClassIDStatic_t::ClassIDStatic_CMaterialModifyControl);
    m_mapClassNameToStaticClassID.emplace("CLightGlow", ClassIDStatic_t::ClassIDStatic_CLightGlow);
    m_mapClassNameToStaticClassID.emplace("CInfoOverlayAccessor", ClassIDStatic_t::ClassIDStatic_CInfoOverlayAccessor);
    m_mapClassNameToStaticClassID.emplace("CFuncTrackTrain", ClassIDStatic_t::ClassIDStatic_CFuncTrackTrain);
    m_mapClassNameToStaticClassID.emplace("CFuncSmokeVolume", ClassIDStatic_t::ClassIDStatic_CFuncSmokeVolume);
    m_mapClassNameToStaticClassID.emplace("CFuncRotating", ClassIDStatic_t::ClassIDStatic_CFuncRotating);
    m_mapClassNameToStaticClassID.emplace("CFuncReflectiveGlass", ClassIDStatic_t::ClassIDStatic_CFuncReflectiveGlass);
    m_mapClassNameToStaticClassID.emplace("CFuncOccluder", ClassIDStatic_t::ClassIDStatic_CFuncOccluder);
    m_mapClassNameToStaticClassID.emplace("CFuncMonitor", ClassIDStatic_t::ClassIDStatic_CFuncMonitor);
    m_mapClassNameToStaticClassID.emplace("CFunc_LOD", ClassIDStatic_t::ClassIDStatic_CFunc_LOD);
    m_mapClassNameToStaticClassID.emplace("CTEDust", ClassIDStatic_t::ClassIDStatic_CTEDust);
    m_mapClassNameToStaticClassID.emplace("CFunc_Dust", ClassIDStatic_t::ClassIDStatic_CFunc_Dust);
    m_mapClassNameToStaticClassID.emplace("CFuncConveyor", ClassIDStatic_t::ClassIDStatic_CFuncConveyor);
    m_mapClassNameToStaticClassID.emplace("CBreakableSurface", ClassIDStatic_t::ClassIDStatic_CBreakableSurface);
    m_mapClassNameToStaticClassID.emplace("CFuncAreaPortalWindow", ClassIDStatic_t::ClassIDStatic_CFuncAreaPortalWindow);
    m_mapClassNameToStaticClassID.emplace("CFish", ClassIDStatic_t::ClassIDStatic_CFish);
    m_mapClassNameToStaticClassID.emplace("CEntityFlame", ClassIDStatic_t::ClassIDStatic_CEntityFlame);
    m_mapClassNameToStaticClassID.emplace("CFireSmoke", ClassIDStatic_t::ClassIDStatic_CFireSmoke);
    m_mapClassNameToStaticClassID.emplace("CEnvTonemapController", ClassIDStatic_t::ClassIDStatic_CEnvTonemapController);
    m_mapClassNameToStaticClassID.emplace("CEnvScreenEffect", ClassIDStatic_t::ClassIDStatic_CEnvScreenEffect);
    m_mapClassNameToStaticClassID.emplace("CEnvScreenOverlay", ClassIDStatic_t::ClassIDStatic_CEnvScreenOverlay);
    m_mapClassNameToStaticClassID.emplace("CEnvProjectedTexture", ClassIDStatic_t::ClassIDStatic_CEnvProjectedTexture);
    m_mapClassNameToStaticClassID.emplace("CEnvParticleScript", ClassIDStatic_t::ClassIDStatic_CEnvParticleScript);
    m_mapClassNameToStaticClassID.emplace("CFogController", ClassIDStatic_t::ClassIDStatic_CFogController);
    m_mapClassNameToStaticClassID.emplace("CEntityParticleTrail", ClassIDStatic_t::ClassIDStatic_CEntityParticleTrail);
    m_mapClassNameToStaticClassID.emplace("CEntityDissolve", ClassIDStatic_t::ClassIDStatic_CEntityDissolve);
    m_mapClassNameToStaticClassID.emplace("CDynamicLight", ClassIDStatic_t::ClassIDStatic_CDynamicLight);
    m_mapClassNameToStaticClassID.emplace("CColorCorrectionVolume", ClassIDStatic_t::ClassIDStatic_CColorCorrectionVolume);
    m_mapClassNameToStaticClassID.emplace("CColorCorrection", ClassIDStatic_t::ClassIDStatic_CColorCorrection);
    m_mapClassNameToStaticClassID.emplace("CBreakableProp", ClassIDStatic_t::ClassIDStatic_CBreakableProp);
    m_mapClassNameToStaticClassID.emplace("CBasePlayer", ClassIDStatic_t::ClassIDStatic_CBasePlayer);
    m_mapClassNameToStaticClassID.emplace("CBaseFlex", ClassIDStatic_t::ClassIDStatic_CBaseFlex);
    m_mapClassNameToStaticClassID.emplace("CBaseEntity", ClassIDStatic_t::ClassIDStatic_CBaseEntity);
    m_mapClassNameToStaticClassID.emplace("CBaseDoor", ClassIDStatic_t::ClassIDStatic_CBaseDoor);
    m_mapClassNameToStaticClassID.emplace("CBaseCombatCharacter", ClassIDStatic_t::ClassIDStatic_CBaseCombatCharacter);
    m_mapClassNameToStaticClassID.emplace("CBaseAnimatingOverlay", ClassIDStatic_t::ClassIDStatic_CBaseAnimatingOverlay);
    m_mapClassNameToStaticClassID.emplace("CBoneFollower", ClassIDStatic_t::ClassIDStatic_CBoneFollower);
    m_mapClassNameToStaticClassID.emplace("CBaseAnimating", ClassIDStatic_t::ClassIDStatic_CBaseAnimating);
    m_mapClassNameToStaticClassID.emplace("CInfoLightingRelative", ClassIDStatic_t::ClassIDStatic_CInfoLightingRelative);
    m_mapClassNameToStaticClassID.emplace("CAI_BaseNPC", ClassIDStatic_t::ClassIDStatic_CAI_BaseNPC);
    m_mapClassNameToStaticClassID.emplace("CBeam", ClassIDStatic_t::ClassIDStatic_CBeam);
    m_mapClassNameToStaticClassID.emplace("CBaseViewModel", ClassIDStatic_t::ClassIDStatic_CBaseViewModel);
    m_mapClassNameToStaticClassID.emplace("CBaseProjectile", ClassIDStatic_t::ClassIDStatic_CBaseProjectile);
    m_mapClassNameToStaticClassID.emplace("CBaseParticleEntity", ClassIDStatic_t::ClassIDStatic_CBaseParticleEntity);
    m_mapClassNameToStaticClassID.emplace("CBaseGrenade", ClassIDStatic_t::ClassIDStatic_CBaseGrenade);
    m_mapClassNameToStaticClassID.emplace("CBaseCombatWeapon", ClassIDStatic_t::ClassIDStatic_CBaseCombatWeapon);


    for (auto& [szClassName, iStaticClassId] : m_mapClassNameToStaticClassID)
    {
        auto it = m_mapClassNameToID.find(szClassName);
        if (it == m_mapClassNameToID.end())
            continue;

        int* pClassID = it->second->m_pDestination;
        if (pClassID == nullptr)
            continue;

        if (*pClassID <= 0)
            continue;

        m_mapClassIDtoStaticClassID.emplace(*pClassID, iStaticClassId);
        LOG("Registered dynamic class ID [ %d for %s ] against static class ID : %d", *pClassID, szClassName.c_str(), iStaticClassId);
    }


    // m_mapClassIDtoStaticClassID size && m_mapClassNameToStaticClassID may or may not be equal!!!
    // 
    // NOTE : Since these ClientClass->ClassId's are only initialized when loaded into a map, not all classIDs get
    // initialized by the game in each map load, but the useful / important / relevant ones get initialized in most cases.
    // So its is not important that m_mapClassNameToStaticClassID and m_mapClassIDtoStaticClassID be equal.
    // But the following condition should always be true. 
    // m_mapClassNameToID.size() == m_mapClassIDtoStaticClassID.size()
    // cause m_mapClassNameToID and m_mapClassIDtoStaticClassID only have the classes that the game filled up.!
    if (m_mapClassNameToID.size() != m_mapClassIDtoStaticClassID.size())
    {
        FAIL_LOG("'m_mapClassNameToStaticClassID' and 'm_mapClassIDtoStaticClassID' size doesn't match! Some entires have been missed!");
        return false;
    }


    WIN_LOG("Constructed dynamic class Id's to static class ID map");
    return true;
}