/*
 * Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "trial_of_the_crusader.h"
#include "Player.h"
#include "GridNotifiers.h"

#include "CreatureTextMgr.h"
static inline uint8 GetHealthPCT(Unit const* hTarget) { if (!hTarget || hTarget->isDead()) return 100; return (hTarget->GetHealth()*100/hTarget->GetMaxHealth()); }
static inline uint8 GetManaPCT(Unit const* hTarget) { if (!hTarget || hTarget->isDead() || hTarget->GetMaxPower(POWER_MANA) <= 1) return 100; return (hTarget->GetPower(POWER_MANA)*100/(hTarget->GetMaxPower(POWER_MANA))); }
//转中文UTF8
const char* _StringToUTF8b(const char*   pASCIIBuf)
{
#ifdef WIN32
	DWORD     UniCodeLen = MultiByteToWideChar(CP_ACP, 0, pASCIIBuf, -1, 0, 0);
	std::vector <wchar_t>   vWCH(UniCodeLen);
	MultiByteToWideChar(CP_ACP, 0, pASCIIBuf, -1, &vWCH[0], UniCodeLen);
	DWORD   dwUtf8Len = WideCharToMultiByte(CP_UTF8, 0, &vWCH[0], UniCodeLen, NULL, NULL, NULL, NULL);
	char* _StringConversionStorage = new char[dwUtf8Len + 1];
	WideCharToMultiByte(CP_UTF8, 0, &vWCH[0], UniCodeLen, _StringConversionStorage, dwUtf8Len, NULL, NULL);
	return &_StringConversionStorage[0];
#else
	return &pASCIIBuf[0];
#endif

}

enum Yells
{
    SAY_KILL_PLAYER     = 6
};

enum AIs
{
    AI_MELEE    = 0,
    AI_RANGED   = 1,
    AI_HEALER   = 2,
    AI_PET      = 3
};

enum Spells
{
    // generic
    SPELL_ANTI_AOE                  = 68595,
    SPELL_PVP_TRINKET               = 65547,

    // druid healer
    SPELL_LIFEBLOOM                 = 66093,
    SPELL_NOURISH                   = 66066,
    SPELL_REGROWTH                  = 66067,
    SPELL_REJUVENATION              = 66065,
    SPELL_TRANQUILITY               = 66086,
    SPELL_BARKSKIN                  = 65860,
    SPELL_THORNS                    = 66068,
    SPELL_NATURE_GRASP              = 66071,

    // shaman healer
    SPELL_HEALING_WAVE              = 66055,
    SPELL_RIPTIDE                   = 66053,
    SPELL_SPIRIT_CLEANSE            = 66056, //friendly only
    SPELL_HEROISM                   = 65983,
    SPELL_BLOODLUST                 = 65980,
    SPELL_HEX                       = 66054,
    SPELL_EARTH_SHIELD              = 66063,
    SPELL_EARTH_SHOCK               = 65973,
    AURA_EXHAUSTION                 = 57723,
    AURA_SATED                      = 57724,

    // paladin healer
    SPELL_HAND_OF_FREEDOM           = 68757,
    SPELL_DIVINE_SHIELD             = 66010,
    SPELL_CLEANSE                   = 66116,
    SPELL_FLASH_OF_LIGHT            = 66113,
    SPELL_HOLY_LIGHT                = 66112,
    SPELL_HOLY_SHOCK                = 66114,
    SPELL_HAND_OF_PROTECTION        = 66009,
    SPELL_HAMMER_OF_JUSTICE         = 66613,
    SPELL_FORBEARANCE               = 25771,

    // priest healer
    SPELL_RENEW                     = 66177,
    SPELL_SHIELD                    = 66099,
    SPELL_FLASH_HEAL                = 66104,
    SPELL_DISPEL                    = 65546,
    SPELL_PSYCHIC_SCREAM            = 65543,
    SPELL_MANA_BURN                 = 66100,
    SPELL_PENANCE                   = 66097,

    // priest dps
    SPELL_SILENCE                   = 65542,
    SPELL_VAMPIRIC_TOUCH            = 65490,
    SPELL_SW_PAIN                   = 65541,
    SPELL_MIND_FLAY                 = 65488,
    SPELL_MIND_BLAST                = 65492,
    SPELL_HORROR                    = 65545,
    SPELL_DISPERSION                = 65544,
    SPELL_SHADOWFORM                = 16592,

    // warlock
    SPELL_HELLFIRE                   = 65816,
    SPELL_CORRUPTION                 = 65810,
    SPELL_CURSE_OF_AGONY             = 65814,
    SPELL_CURSE_OF_EXHAUSTION        = 65815,
    SPELL_FEAR                       = 65809,
    SPELL_SEARING_PAIN               = 65819,
    SPELL_SHADOW_BOLT                = 65821,
    SPELL_UNSTABLE_AFFLICTION        = 65812,
    SPELL_UNSTABLE_AFFLICTION_DISPEL = 65813,
    SPELL_SUMMON_FELHUNTER           = 67514,

    // mage
    SPELL_ARCANE_BARRAGE            = 65799,
    SPELL_ARCANE_BLAST              = 65791,
    SPELL_ARCANE_EXPLOSION          = 65800,
    SPELL_BLINK                     = 65793,
    SPELL_COUNTERSPELL              = 65790,
    SPELL_FROST_NOVA                = 65792,
    SPELL_FROSTBOLT                 = 65807,
    SPELL_ICE_BLOCK                 = 65802,
    SPELL_POLYMORPH                 = 65801,

    // hunter
    SPELL_AIMED_SHOT                = 65883,
    SPELL_DETERRENCE                = 65871,
    SPELL_DISENGAGE                 = 65869,
    SPELL_EXPLOSIVE_SHOT            = 65866,
    SPELL_FROST_TRAP                = 65880,
    SPELL_SHOOT                     = 65868,
    SPELL_STEADY_SHOT               = 65867,
    SPELL_WING_CLIP                 = 66207,
    SPELL_WYVERN_STING              = 65877,
    SPELL_CALL_PET                  = 67777,

    // druid dps
    SPELL_CYCLONE                   = 65859,
    SPELL_ENTANGLING_ROOTS          = 65857,
    SPELL_FAERIE_FIRE               = 65863,
    SPELL_FORCE_OF_NATURE           = 65861,
    SPELL_INSECT_SWARM              = 65855,
    SPELL_MOONFIRE                  = 65856,
    SPELL_STARFIRE                  = 65854,
    SPELL_WRATH                     = 65862,

    // warrior
    SPELL_BLADESTORM                = 65947,
    SPELL_INTIMIDATING_SHOUT        = 65930,
    SPELL_MORTAL_STRIKE             = 65926,
    SPELL_CHARGE                    = 68764,
    SPELL_DISARM                    = 65935,
    SPELL_OVERPOWER                 = 65924,
    SPELL_SUNDER_ARMOR              = 65936,
    SPELL_SHATTERING_THROW          = 65940,
    SPELL_RETALIATION               = 65932,

    // death knight
    SPELL_CHAINS_OF_ICE             = 66020,
    SPELL_DEATH_COIL                = 66019,
    SPELL_DEATH_GRIP                = 66017,
    SPELL_FROST_STRIKE              = 66047,
    SPELL_ICEBOUND_FORTITUDE        = 66023,
    SPELL_ICY_TOUCH                 = 66021,
    SPELL_STRANGULATE               = 66018,
    SPELL_DEATH_GRIP_PULL           = 64431,    // used at spellscript

    // rogue
    SPELL_FAN_OF_KNIVES             = 65955,
    SPELL_BLIND                     = 65960,
    SPELL_CLOAK                     = 65961,
    SPELL_BLADE_FLURRY              = 65956,
    SPELL_SHADOWSTEP                = 66178,
    SPELL_HEMORRHAGE                = 65954,
    SPELL_EVISCERATE                = 65957,
    SPELL_WOUND_POISON              = 65962,

    // shaman dps (some spells taken from shaman healer)
    SPELL_LAVA_LASH                 = 65974,
    SPELL_STORMSTRIKE               = 65970,
    SPELL_WINDFURY                  = 65976,

    // paladin dps
    SPELL_AVENGING_WRATH            = 66011,
    SPELL_CRUSADER_STRIKE           = 66003,
    SPELL_DIVINE_STORM              = 66006,
    SPELL_HAMMER_OF_JUSTICE_RET     = 66007,
    SPELL_JUDGEMENT_OF_COMMAND      = 66005,
    SPELL_REPENTANCE                = 66008,
    SPELL_SEAL_OF_COMMAND           = 66004,

    // warlock pet
    SPELL_DEVOUR_MAGIC              = 67518,
    SPELL_SPELL_LOCK                = 67519,

    // hunter pet
    SPELL_CLAW                      = 67793
};

enum Events
{
    // generic
    EVENT_THREAT                    = 1,
    EVENT_REMOVE_CC                 = 2,

    // druid healer
    EVENT_LIFEBLOOM                 = 1,
    EVENT_NOURISH                   = 2,
    EVENT_REGROWTH                  = 3,
    EVENT_REJUVENATION              = 4,
    EVENT_TRANQUILITY               = 5,
    EVENT_HEAL_BARKSKIN                  = 6,
    EVENT_THORNS                    = 7,
    EVENT_NATURE_GRASP              = 8,

    // shaman healer
    EVENT_HEALING_WAVE              = 1,
    EVENT_RIPTIDE                   = 2,
    EVENT_SPIRIT_CLEANSE            = 3,
    EVENT_HEAL_BLOODLUST_HEROISM    = 4,
    EVENT_HEX                       = 5,
    EVENT_EARTH_SHIELD              = 6,
    EVENT_HEAL_EARTH_SHOCK          = 7,

    // paladin healer
    EVENT_HAND_OF_FREEDOM           = 1,
    EVENT_HEAL_DIVINE_SHIELD        = 2,
    EVENT_CLEANSE                   = 3,
    EVENT_FLASH_OF_LIGHT            = 4,
    EVENT_HOLY_LIGHT                = 5,
    EVENT_HOLY_SHOCK                = 6,
    EVENT_HEAL_HAND_OF_PROTECTION   = 7,
    EVENT_HAMMER_OF_JUSTICE         = 8,

    // priest healer
    EVENT_RENEW                     = 1,
    EVENT_SHIELD                    = 2,
    EVENT_FLASH_HEAL                = 3,
    EVENT_HEAL_DISPEL               = 4,
    EVENT_HEAL_PSYCHIC_SCREAM       = 5,
    EVENT_MANA_BURN                 = 6,
    EVENT_PENANCE                   = 7,

    // priest dps
    EVENT_SILENCE                   = 1,
    EVENT_VAMPIRIC_TOUCH            = 2,
    EVENT_SW_PAIN                   = 3,
    EVENT_MIND_BLAST                = 4,
    EVENT_HORROR                    = 5,
    EVENT_DISPERSION                = 6,
    EVENT_DPS_DISPEL                = 7,
    EVENT_DPS_PSYCHIC_SCREAM        = 8,

    // warlock
    EVENT_HELLFIRE                  = 1,
    EVENT_CORRUPTION                = 2,
    EVENT_CURSE_OF_AGONY            = 3,
    EVENT_CURSE_OF_EXHAUSTION       = 4,
    EVENT_FEAR                      = 5,
    EVENT_SEARING_PAIN              = 6,
    EVENT_UNSTABLE_AFFLICTION       = 7,

    // mage
    EVENT_ARCANE_BARRAGE            = 1,
    EVENT_ARCANE_BLAST              = 2,
    EVENT_ARCANE_EXPLOSION          = 3,
    EVENT_BLINK                     = 4,
    EVENT_COUNTERSPELL              = 5,
    EVENT_FROST_NOVA                = 6,
    EVENT_ICE_BLOCK                 = 7,
    EVENT_POLYMORPH                 = 8,

    // hunter
    EVENT_AIMED_SHOT                = 1,
    EVENT_DETERRENCE                = 2,
    EVENT_DISENGAGE                 = 3,
    EVENT_EXPLOSIVE_SHOT            = 4,
    EVENT_FROST_TRAP                = 5,
    EVENT_STEADY_SHOT               = 6,
    EVENT_WING_CLIP                 = 7,
    EVENT_WYVERN_STING              = 8,

    // druid dps
    EVENT_CYCLONE                   = 1,
    EVENT_ENTANGLING_ROOTS          = 2,
    EVENT_FAERIE_FIRE               = 3,
    EVENT_FORCE_OF_NATURE           = 4,
    EVENT_INSECT_SWARM              = 5,
    EVENT_MOONFIRE                  = 6,
    EVENT_STARFIRE                  = 7,
    EVENT_DPS_BARKSKIN              = 8,

    // warrior
    EVENT_BLADESTORM                = 1,
    EVENT_INTIMIDATING_SHOUT        = 2,
    EVENT_MORTAL_STRIKE             = 3,
    EVENT_WARR_CHARGE               = 4,
    EVENT_DISARM                    = 5,
    EVENT_OVERPOWER                 = 6,
    EVENT_SUNDER_ARMOR              = 7,
    EVENT_SHATTERING_THROW          = 8,
    EVENT_RETALIATION               = 9,

    // death knight
    EVENT_CHAINS_OF_ICE             = 1,
    EVENT_DEATH_COIL                = 2,
    EVENT_DEATH_GRIP                = 3,
    EVENT_FROST_STRIKE              = 4,
    EVENT_ICEBOUND_FORTITUDE        = 5,
    EVENT_ICY_TOUCH                 = 6,
    EVENT_STRANGULATE               = 7,

    // rogue
    EVENT_FAN_OF_KNIVES             = 1,
    EVENT_BLIND                     = 2,
    EVENT_CLOAK                     = 3,
    EVENT_BLADE_FLURRY              = 4,
    EVENT_SHADOWSTEP                = 5,
    EVENT_HEMORRHAGE                = 6,
    EVENT_EVISCERATE                = 7,
    EVENT_WOUND_POISON              = 8,

    // shaman dps
    EVENT_DPS_EARTH_SHOCK           = 1,
    EVENT_LAVA_LASH                 = 2,
    EVENT_STORMSTRIKE               = 3,
    EVENT_DPS_BLOODLUST_HEROISM     = 4,
    EVENT_DEPLOY_TOTEM              = 5,
    EVENT_WINDFURY                  = 6,

    // paladin dps
    EVENT_AVENGING_WRATH            = 1,
    EVENT_CRUSADER_STRIKE           = 2,
    EVENT_DIVINE_STORM              = 3,
    EVENT_HAMMER_OF_JUSTICE_RET     = 4,
    EVENT_JUDGEMENT_OF_COMMAND      = 5,
    EVENT_REPENTANCE                = 6,
    EVENT_DPS_HAND_OF_PROTECTION    = 7,
    EVENT_DPS_DIVINE_SHIELD         = 8,

    // warlock pet
    EVENT_DEVOUR_MAGIC              = 1,
    EVENT_SPELL_LOCK                = 2
};

class boss_toc_champion_controller : public CreatureScript
{
    public:
        boss_toc_champion_controller() : CreatureScript("boss_toc_champion_controller") { }

        struct boss_toc_champion_controllerAI : public ScriptedAI
        {
            boss_toc_champion_controllerAI(Creature* creature) : ScriptedAI(creature), _summons(me)
            {
                Initialize();
                _instance = creature->GetInstanceScript();
            }

            void Initialize()
            {
                _championsNotStarted = 0;
                _championsFailed = 0;
                _championsKilled = 0;
                _inProgress = false;
            }

            void Reset() override
            {
                Initialize();
            }

            std::vector<uint32> SelectChampions(Team playerTeam)
            {
                std::vector<uint32> vHealersEntries;
                vHealersEntries.clear();
                vHealersEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_DRUID_RESTORATION : NPC_ALLIANCE_DRUID_RESTORATION);
                vHealersEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_PALADIN_HOLY : NPC_ALLIANCE_PALADIN_HOLY);
                vHealersEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_PRIEST_DISCIPLINE : NPC_ALLIANCE_PRIEST_DISCIPLINE);
                vHealersEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_SHAMAN_RESTORATION : NPC_ALLIANCE_SHAMAN_RESTORATION);

                std::vector<uint32> vOtherEntries;
                vOtherEntries.clear();
                vOtherEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_DEATH_KNIGHT : NPC_ALLIANCE_DEATH_KNIGHT);
                vOtherEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_HUNTER : NPC_ALLIANCE_HUNTER);
                vOtherEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_MAGE : NPC_ALLIANCE_MAGE);
                vOtherEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_ROGUE : NPC_ALLIANCE_ROGUE);
                vOtherEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_WARLOCK : NPC_ALLIANCE_WARLOCK);
                vOtherEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_WARRIOR : NPC_ALLIANCE_WARRIOR);

                uint8 healersSubtracted = 2;
                if (_instance->instance->GetSpawnMode() == RAID_DIFFICULTY_25MAN_NORMAL || _instance->instance->GetSpawnMode() == RAID_DIFFICULTY_25MAN_HEROIC)
                    healersSubtracted = 1;
                for (uint8 i = 0; i < healersSubtracted; ++i)
                {
                    uint8 pos = urand(0, vHealersEntries.size() - 1);
                    switch (vHealersEntries[pos])
                    {
                        case NPC_ALLIANCE_DRUID_RESTORATION:
                            vOtherEntries.push_back(NPC_ALLIANCE_DRUID_BALANCE);
                            break;
                        case NPC_HORDE_DRUID_RESTORATION:
                            vOtherEntries.push_back(NPC_HORDE_DRUID_BALANCE);
                            break;
                        case NPC_ALLIANCE_PALADIN_HOLY:
                            vOtherEntries.push_back(NPC_ALLIANCE_PALADIN_RETRIBUTION);
                            break;
                        case NPC_HORDE_PALADIN_HOLY:
                            vOtherEntries.push_back(NPC_HORDE_PALADIN_RETRIBUTION);
                            break;
                        case NPC_ALLIANCE_PRIEST_DISCIPLINE:
                            vOtherEntries.push_back(NPC_ALLIANCE_PRIEST_SHADOW);
                            break;
                        case NPC_HORDE_PRIEST_DISCIPLINE:
                            vOtherEntries.push_back(NPC_HORDE_PRIEST_SHADOW);
                            break;
                        case NPC_ALLIANCE_SHAMAN_RESTORATION:
                            vOtherEntries.push_back(NPC_ALLIANCE_SHAMAN_ENHANCEMENT);
                            break;
                        case NPC_HORDE_SHAMAN_RESTORATION:
                            vOtherEntries.push_back(NPC_HORDE_SHAMAN_ENHANCEMENT);
                            break;
                        default:
                            break;
                    }
                    vHealersEntries.erase(vHealersEntries.begin() + pos);
                }

                if (_instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL || _instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_HEROIC)
                    for (uint8 i = 0; i < 4; ++i)
                        vOtherEntries.erase(vOtherEntries.begin() + urand(0, vOtherEntries.size() - 1));

                std::vector<uint32> vChampionEntries;
                vChampionEntries.clear();
                for (uint8 i = 0; i < vHealersEntries.size(); ++i)
                    vChampionEntries.push_back(vHealersEntries[i]);
                for (uint8 i = 0; i < vOtherEntries.size(); ++i)
                    vChampionEntries.push_back(vOtherEntries[i]);

                return vChampionEntries;
            }

            void SummonChampions(Team playerTeam)
            {
                std::vector<Position> vChampionJumpOrigin;
                if (playerTeam == ALLIANCE)
                    for (uint8 i = 0; i < 5; i++)
                        vChampionJumpOrigin.push_back(FactionChampionLoc[i]);
                else
                    for (uint8 i = 5; i < 10; i++)
                        vChampionJumpOrigin.push_back(FactionChampionLoc[i]);

                std::vector<Position> vChampionJumpTarget;
                for (uint8 i = 10; i < 20; i++)
                    vChampionJumpTarget.push_back(FactionChampionLoc[i]);
                std::vector<uint32> vChampionEntries = SelectChampions(playerTeam);

                for (uint8 i = 0; i < vChampionEntries.size(); ++i)
                {
                    uint8 pos = urand(0, vChampionJumpTarget.size()-1);
                    if (Creature* champion = me->SummonCreature(vChampionEntries[i], vChampionJumpOrigin[urand(0, vChampionJumpOrigin.size()-1)], TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        _summons.Summon(champion);
                        champion->SetReactState(REACT_PASSIVE);
                        champion->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                        if (playerTeam == ALLIANCE)
                        {
                            champion->SetHomePosition(vChampionJumpTarget[pos].GetPositionX(), vChampionJumpTarget[pos].GetPositionY(), vChampionJumpTarget[pos].GetPositionZ(), 0);
                            champion->GetMotionMaster()->MoveJump(vChampionJumpTarget[pos], 20.0f, 20.0f);
                            champion->SetOrientation(0);
                        }
                        else
                        {
                            champion->SetHomePosition((ToCCommonLoc[1].GetPositionX()*2)-vChampionJumpTarget[pos].GetPositionX(), vChampionJumpTarget[pos].GetPositionY(), vChampionJumpTarget[pos].GetPositionZ(), 3);
                            champion->GetMotionMaster()->MoveJump((ToCCommonLoc[1].GetPositionX() * 2) - vChampionJumpTarget[pos].GetPositionX(), vChampionJumpTarget[pos].GetPositionY(), vChampionJumpTarget[pos].GetPositionZ(), vChampionJumpTarget[pos].GetOrientation(), 20.0f, 20.0f);
                            champion->SetOrientation(3);
                        }
                    }
                    vChampionJumpTarget.erase(vChampionJumpTarget.begin()+pos);
                }
            }

            void SetData(uint32 uiType, uint32 uiData) override
            {
                switch (uiType)
                {
                    case 0:
                        SummonChampions((Team)uiData);
                        break;
                    case 1:
                        for (SummonList::iterator i = _summons.begin(); i != _summons.end(); ++i)
                        {
                            if (Creature* summon = ObjectAccessor::GetCreature(*me, *i))
                            {
                                summon->SetReactState(REACT_AGGRESSIVE);
                                summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                            }
                        }
                        break;
                    case 2:
                        switch (uiData)
                        {
                            case FAIL:
                                _championsFailed++;
                                if (_championsFailed + _championsKilled >= _summons.size())
                                {
                                    _instance->SetBossState(BOSS_CRUSADERS, FAIL);
                                    _summons.DespawnAll();
                                    me->DespawnOrUnsummon();
                                }
                                break;
                            case IN_PROGRESS:
                                if (!_inProgress)
                                {
                                    _championsNotStarted = 0;
                                    _championsFailed = 0;
                                    _championsKilled = 0;
                                    _inProgress = true;
                                    _summons.DoZoneInCombat();
                                    _instance->SetBossState(BOSS_CRUSADERS, IN_PROGRESS);
                                }
                                break;
                            case DONE:
                                _championsKilled++;
                                if (_championsKilled == 1)
                                    _instance->SetBossState(BOSS_CRUSADERS, SPECIAL);
                                else if (_championsKilled >= _summons.size())
                                {
                                    _instance->SetBossState(BOSS_CRUSADERS, DONE);
                                    _summons.DespawnAll();
                                    me->DespawnOrUnsummon();
                                }
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
            }
            private:
                InstanceScript* _instance;
                SummonList _summons;
                uint32 _championsNotStarted;
                uint32 _championsFailed;
                uint32 _championsKilled;
                bool   _inProgress;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_toc_champion_controllerAI>(creature);
        }
};

struct boss_faction_championsAI : public BossAI
{
    boss_faction_championsAI(Creature* creature, uint32 aitype) : BossAI(creature, BOSS_CRUSADERS)
    {
        _aiType = aitype;
    }

    void Reset() override
    {
        _events.ScheduleEvent(EVENT_THREAT, 5*IN_MILLISECONDS);
        if (IsHeroic() && (_aiType != AI_PET))
            _events.ScheduleEvent(EVENT_REMOVE_CC, 5*IN_MILLISECONDS);
    }

    void JustReachedHome() override
    {
        if (Creature* pChampionController = ObjectAccessor::GetCreature((*me), instance->GetGuidData(NPC_CHAMPIONS_CONTROLLER)))
            pChampionController->AI()->SetData(2, FAIL);
        me->DespawnOrUnsummon();
    }

    float CalculateThreat(float distance, float armor, uint32 health)
    {
        float dist_mod = (_aiType == AI_MELEE || _aiType == AI_PET) ? 15.0f / (15.0f + distance) : 1.0f;
        float armor_mod = (_aiType == AI_MELEE || _aiType == AI_PET) ? armor / 16635.0f : 0.0f;
        float eh = (health + 1) * (1.0f + armor_mod);
        return dist_mod * 30000.0f / eh;
    }

    void UpdateThreat()
    {
        std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
        for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
        {
            Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
            if (unit && me->getThreatManager().getThreat(unit))
            {
                if (unit->GetTypeId() == TYPEID_PLAYER)
                {
                    float threat = CalculateThreat(me->GetDistance2d(unit), (float)unit->GetArmor(), unit->GetHealth());
                    me->getThreatManager().modifyThreatPercent(unit, -100);
                    me->AddThreat(unit, 1000000.0f * threat);
                }
            }
        }
    }

    void UpdatePower()
    {
        if (me->getPowerType() == POWER_MANA)
            me->ModifyPower(POWER_MANA, me->GetMaxPower(POWER_MANA) / 3);
    }

    void RemoveCC()
    {
        me->RemoveAurasByType(SPELL_AURA_MOD_STUN);
        me->RemoveAurasByType(SPELL_AURA_MOD_FEAR);
        me->RemoveAurasByType(SPELL_AURA_MOD_ROOT);
        me->RemoveAurasByType(SPELL_AURA_MOD_PACIFY);
        me->RemoveAurasByType(SPELL_AURA_MOD_CONFUSE);
        //DoCast(me, SPELL_PVP_TRINKET);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (_aiType != AI_PET)
            if (Creature* pChampionController = ObjectAccessor::GetCreature((*me), instance->GetGuidData(NPC_CHAMPIONS_CONTROLLER)))
                pChampionController->AI()->SetData(2, DONE);
    }

    void EnterCombat(Unit* /*who*/) override
    {
        DoCast(me, SPELL_ANTI_AOE, true);
        _EnterCombat();
        if (Creature* pChampionController = ObjectAccessor::GetCreature((*me), instance->GetGuidData(NPC_CHAMPIONS_CONTROLLER)))
            pChampionController->AI()->SetData(2, IN_PROGRESS);
    }

    void KilledUnit(Unit* who) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER)
        {
            Map::PlayerList const &players = me->GetMap()->GetPlayers();
            uint32 TeamInInstance = 0;

            if (!players.isEmpty())
                if (Player* player = players.begin()->GetSource())
                    TeamInInstance = player->GetTeam();

            if (TeamInInstance == ALLIANCE)
            {
                if (Creature* varian = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_VARIAN)))
                    varian->AI()->Talk(SAY_KILL_PLAYER);
            }
            else
                if (Creature* garrosh = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_GARROSH)))
                    garrosh->AI()->Talk(SAY_KILL_PLAYER);

        }
    }

    Creature* SelectRandomFriendlyMissingBuff(uint32 spell)
    {
        std::list<Creature*> lst = DoFindFriendlyMissingBuff(40.0f, spell);
        std::list<Creature*>::const_iterator itr = lst.begin();
        if (lst.empty())
            return NULL;
        advance(itr, rand32() % lst.size());
        return (*itr);
    }

    Unit* SelectEnemyCaster(bool /*casting*/)
    {
        std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
        std::list<HostileReference*>::const_iterator iter;
        for (iter = tList.begin(); iter!=tList.end(); ++iter)
        {
            Unit* target = ObjectAccessor::GetUnit(*me, (*iter)->getUnitGuid());
            if (target && target->getPowerType() == POWER_MANA)
                return target;
        }
        return NULL;
    }

    uint32 EnemiesInRange(float distance)
    {
        std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
        std::list<HostileReference*>::const_iterator iter;
        uint32 count = 0;
        for (iter = tList.begin(); iter != tList.end(); ++iter)
        {
            Unit* target = ObjectAccessor::GetUnit(*me, (*iter)->getUnitGuid());
                if (target && me->GetDistance2d(target) < distance)
                    ++count;
        }
        return count;
    }

    void AttackStart(Unit* who) override
    {
        if (!who)
            return;

        if (me->Attack(who, true))
        {
            me->AddThreat(who, 10.0f);
            me->SetInCombatWith(who);
            who->SetInCombatWith(me);

            if (_aiType == AI_MELEE || _aiType == AI_PET)
                DoStartMovement(who);
            else
                DoStartMovement(who, 20.0f);
            SetCombatMovement(true);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_THREAT:
                    UpdatePower();
                    UpdateThreat();
                    _events.ScheduleEvent(EVENT_THREAT, 4*IN_MILLISECONDS);
                    return;
                case EVENT_REMOVE_CC:
                    if (me->HasBreakableByDamageCrowdControlAura())
                    {
                        RemoveCC();
                        _events.RescheduleEvent(EVENT_REMOVE_CC, 2*MINUTE*IN_MILLISECONDS);
                    }
                    else
                        _events.RescheduleEvent(EVENT_REMOVE_CC, 3*IN_MILLISECONDS);
                    return;
                default:
                    return;
            }
        }

        if (_aiType == AI_MELEE || _aiType == AI_PET)
            DoMeleeAttackIfReady();
    }

    private:
        uint32 _aiType;
        // make sure that every bosses separate events dont mix with these _events
        EventMap _events;
};

/********************************************************************
                            HEALERS
********************************************************************/
class npc_toc_druid : public CreatureScript
{
    public:
        npc_toc_druid() : CreatureScript("npc_toc_druid") { }

        struct npc_toc_druidAI : public boss_faction_championsAI
        {
            npc_toc_druidAI(Creature* creature) : boss_faction_championsAI(creature, AI_HEALER) { }

            void Reset() override
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_LIFEBLOOM, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_NOURISH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_REGROWTH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_REJUVENATION, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_TRANQUILITY, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEAL_BARKSKIN, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_THORNS, 2*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_NATURE_GRASP, urand(3*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                SetEquipmentSlots(false, 51799, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_LIFEBLOOM:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_LIFEBLOOM);
                            events.ScheduleEvent(EVENT_LIFEBLOOM, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_NOURISH:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_NOURISH);
                            events.ScheduleEvent(EVENT_NOURISH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_REGROWTH:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_REGROWTH);
                            events.ScheduleEvent(EVENT_REGROWTH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_REJUVENATION:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_REJUVENATION);
                            events.ScheduleEvent(EVENT_REJUVENATION, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_TRANQUILITY:
                            DoCastAOE(SPELL_TRANQUILITY);
                            events.ScheduleEvent(EVENT_TRANQUILITY, urand(15*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_BARKSKIN:
                            if (HealthBelowPct(30))
                            {
                                DoCast(me, SPELL_BARKSKIN);
                                events.RescheduleEvent(EVENT_HEAL_BARKSKIN, 60*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_HEAL_BARKSKIN, 3*IN_MILLISECONDS);
                            return;
                        case EVENT_THORNS:
                            if (Creature* target = SelectRandomFriendlyMissingBuff(SPELL_THORNS))
                                DoCast(target, SPELL_THORNS);
                            events.ScheduleEvent(EVENT_THORNS, urand(25*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_NATURE_GRASP:
                            DoCast(me, SPELL_NATURE_GRASP);
                            events.ScheduleEvent(EVENT_NATURE_GRASP, 60*IN_MILLISECONDS);
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_toc_druidAI>(creature);
        }
};

class npc_toc_shaman : public CreatureScript
{
    public:
        npc_toc_shaman() : CreatureScript("npc_toc_shaman") { }

        struct npc_toc_shamanAI : public boss_faction_championsAI
        {
            npc_toc_shamanAI(Creature* creature) : boss_faction_championsAI(creature, AI_HEALER) { }

            void Reset() override
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_HEALING_WAVE, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_RIPTIDE, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SPIRIT_CLEANSE, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEAL_BLOODLUST_HEROISM, 20*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_HEX, urand(5*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_EARTH_SHIELD, 1*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_HEAL_EARTH_SHOCK, urand(5*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                SetEquipmentSlots(false, 49992, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HEALING_WAVE:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_HEALING_WAVE);
                            events.ScheduleEvent(EVENT_HEALING_WAVE, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            return;
                        case EVENT_RIPTIDE:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_RIPTIDE);
                            events.ScheduleEvent(EVENT_RIPTIDE, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_SPIRIT_CLEANSE:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_SPIRIT_CLEANSE);
                            events.ScheduleEvent(EVENT_SPIRIT_CLEANSE, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_BLOODLUST_HEROISM:
                            if (me->getFaction()) // alliance = 1
                            {
                                if (!me->HasAura(AURA_EXHAUSTION))
                                    DoCastAOE(SPELL_HEROISM);
                            }
                            else
                            {
                                if (!me->HasAura(AURA_SATED))
                                    DoCastAOE(SPELL_BLOODLUST);
                            }
                            events.ScheduleEvent(EVENT_HEAL_BLOODLUST_HEROISM, 5*MINUTE*IN_MILLISECONDS);
                            return;
                        case EVENT_HEX:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_HEX);
                            events.ScheduleEvent(EVENT_HEX, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_EARTH_SHIELD:
                            if (Creature* target = SelectRandomFriendlyMissingBuff(SPELL_EARTH_SHIELD))
                                DoCast(target, SPELL_EARTH_SHIELD);
                            events.ScheduleEvent(EVENT_EARTH_SHIELD, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_EARTH_SHOCK:
                            if (Unit* target = SelectEnemyCaster(true))
                                DoCast(target, SPELL_EARTH_SHOCK);
                            events.ScheduleEvent(EVENT_HEAL_EARTH_SHOCK, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_toc_shamanAI>(creature);
        }
};

class npc_toc_paladin : public CreatureScript
{
    public:
        npc_toc_paladin() : CreatureScript("npc_toc_paladin") { }

        struct npc_toc_paladinAI : public boss_faction_championsAI
        {
            npc_toc_paladinAI(Creature* creature) : boss_faction_championsAI(creature, AI_HEALER) { }

            void Reset() override
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_HAND_OF_FREEDOM, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEAL_DIVINE_SHIELD, 20*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CLEANSE, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FLASH_OF_LIGHT, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HOLY_LIGHT, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HOLY_SHOCK, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEAL_HAND_OF_PROTECTION, urand(30*IN_MILLISECONDS, 60*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HAMMER_OF_JUSTICE, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                SetEquipmentSlots(false, 50771, 47079, EQUIP_NO_CHANGE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HAND_OF_FREEDOM:
                            if (Unit* target = SelectRandomFriendlyMissingBuff(SPELL_HAND_OF_FREEDOM))
                                DoCast(target, SPELL_HAND_OF_FREEDOM);
                            events.ScheduleEvent(EVENT_HAND_OF_FREEDOM, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_DIVINE_SHIELD:
                            if (HealthBelowPct(30) && !me->HasAura(SPELL_FORBEARANCE))
                            {
                                DoCast(me, SPELL_DIVINE_SHIELD);
                                events.RescheduleEvent(EVENT_HEAL_DIVINE_SHIELD, 5*MINUTE*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_HEAL_DIVINE_SHIELD, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_CLEANSE:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_CLEANSE);
                            events.ScheduleEvent(EVENT_CLEANSE, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_FLASH_OF_LIGHT:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_FLASH_OF_LIGHT);
                            events.ScheduleEvent(EVENT_FLASH_OF_LIGHT, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            return;
                        case EVENT_HOLY_LIGHT:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_HOLY_LIGHT);
                            events.ScheduleEvent(EVENT_HOLY_LIGHT, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_HOLY_SHOCK:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_HOLY_SHOCK);
                            events.ScheduleEvent(EVENT_HOLY_SHOCK, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_HAND_OF_PROTECTION:
                            if (Unit* target = DoSelectLowestHpFriendly(30.0f))
                            {
                                if (!target->HasAura(SPELL_FORBEARANCE))
                                {
                                    DoCast(target, SPELL_HAND_OF_PROTECTION);
                                    events.RescheduleEvent(EVENT_HEAL_HAND_OF_PROTECTION, 5*MINUTE*IN_MILLISECONDS);
                                }
                                else
                                    events.RescheduleEvent(EVENT_HEAL_HAND_OF_PROTECTION, 3*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_HEAL_HAND_OF_PROTECTION, 10*IN_MILLISECONDS);
                            return;
                        case EVENT_HAMMER_OF_JUSTICE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 15.0f, true))
                                DoCast(target, SPELL_HAMMER_OF_JUSTICE);
                            events.ScheduleEvent(EVENT_HAMMER_OF_JUSTICE, 40*IN_MILLISECONDS);
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_toc_paladinAI>(creature);
        }
};

class npc_toc_priest : public CreatureScript
{
    public:
        npc_toc_priest() : CreatureScript("npc_toc_priest") { }

        struct npc_toc_priestAI : public boss_faction_championsAI
        {
            npc_toc_priestAI(Creature* creature) : boss_faction_championsAI(creature, AI_HEALER) { }

            void Reset() override
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_RENEW, urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SHIELD, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FLASH_HEAL, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEAL_DISPEL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEAL_PSYCHIC_SCREAM, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_MANA_BURN, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_PENANCE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                SetEquipmentSlots(false, 49992, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RENEW:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_RENEW);
                            events.ScheduleEvent(EVENT_RENEW, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            return;
                        case EVENT_SHIELD:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_SHIELD);
                            events.ScheduleEvent(EVENT_SHIELD, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_FLASH_HEAL:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_FLASH_HEAL);
                            events.ScheduleEvent(EVENT_FLASH_HEAL, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_DISPEL:
                            if (Unit* target = urand(0, 1) ? SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true) : DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_DISPEL);
                            events.ScheduleEvent(EVENT_HEAL_DISPEL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_PSYCHIC_SCREAM:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCastAOE(SPELL_PSYCHIC_SCREAM);
                            events.ScheduleEvent(EVENT_HEAL_PSYCHIC_SCREAM, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_MANA_BURN:
                            if (Unit* target = SelectEnemyCaster(false))
                                DoCast(target, SPELL_MANA_BURN);
                            events.ScheduleEvent(EVENT_MANA_BURN, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_PENANCE:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_PENANCE);
                            events.ScheduleEvent(EVENT_PENANCE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_toc_priestAI>(creature);
        }
};

/********************************************************************
                            RANGED
********************************************************************/
class npc_toc_shadow_priest : public CreatureScript
{
    public:
        npc_toc_shadow_priest() : CreatureScript("npc_toc_shadow_priest") { }

        struct npc_toc_shadow_priestAI : public boss_faction_championsAI
        {
            npc_toc_shadow_priestAI(Creature* creature) : boss_faction_championsAI(creature, AI_RANGED) { }

            void Reset() override
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_SILENCE, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_VAMPIRIC_TOUCH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SW_PAIN, urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_MIND_BLAST, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HORROR, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DISPERSION, urand(20*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DPS_DISPEL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DPS_PSYCHIC_SCREAM, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                SetEquipmentSlots(false, 50040, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
                DoCast(me, SPELL_SHADOWFORM);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SILENCE:
                            if (Unit* target = SelectEnemyCaster(true))
                                DoCast(target, SPELL_SILENCE);
                            events.ScheduleEvent(EVENT_SILENCE, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_VAMPIRIC_TOUCH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true))
                                DoCast(target, SPELL_VAMPIRIC_TOUCH);
                            events.ScheduleEvent(EVENT_VAMPIRIC_TOUCH, urand(10*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_SW_PAIN:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                                DoCast(target, SPELL_SW_PAIN);
                            events.ScheduleEvent(EVENT_SW_PAIN, urand(10*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_MIND_BLAST:
                            DoCastVictim(SPELL_MIND_BLAST);
                            events.ScheduleEvent(EVENT_MIND_BLAST, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_HORROR:
                            DoCastVictim(SPELL_HORROR);
                            events.ScheduleEvent(EVENT_HORROR, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_DISPERSION:
                            if (HealthBelowPct(40))
                            {
                                DoCast(me, SPELL_DISPERSION);
                                events.RescheduleEvent(EVENT_DISPERSION, 180*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_DISPERSION, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_DPS_DISPEL:
                            if (Unit* target = urand(0, 1) ? SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true) : DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_DISPEL);
                            events.ScheduleEvent(EVENT_DPS_DISPEL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_DPS_PSYCHIC_SCREAM:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCastAOE(SPELL_PSYCHIC_SCREAM);
                            events.ScheduleEvent(EVENT_DPS_PSYCHIC_SCREAM, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
                DoSpellAttackIfReady(SPELL_MIND_FLAY);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_toc_shadow_priestAI>(creature);
        }
};

class npc_toc_warlock : public CreatureScript
{
    public:
        npc_toc_warlock() : CreatureScript("npc_toc_warlock") { }

        struct npc_toc_warlockAI : public boss_faction_championsAI
        {
            npc_toc_warlockAI(Creature* creature) : boss_faction_championsAI(creature, AI_RANGED) { }

            void Reset() override
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_HELLFIRE, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CORRUPTION, urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CURSE_OF_AGONY, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CURSE_OF_EXHAUSTION, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FEAR, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SEARING_PAIN, urand(5*IN_MILLISECONDS, 12*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_UNSTABLE_AFFLICTION, urand(7*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                SetEquipmentSlots(false, 49992, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            }

            void EnterCombat(Unit* who) override
            {
                boss_faction_championsAI::EnterCombat(who);
                DoCast(SPELL_SUMMON_FELHUNTER);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HELLFIRE:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCastAOE(SPELL_HELLFIRE);
                            events.ScheduleEvent(EVENT_HELLFIRE, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_CORRUPTION:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f))
                                DoCast(target, SPELL_CORRUPTION);
                            events.ScheduleEvent(EVENT_CORRUPTION, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_CURSE_OF_AGONY:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f))
                                DoCast(target, SPELL_CURSE_OF_AGONY);
                            events.ScheduleEvent(EVENT_CURSE_OF_AGONY, urand(20*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_CURSE_OF_EXHAUSTION:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f))
                                DoCast(target, SPELL_CURSE_OF_EXHAUSTION);
                            events.ScheduleEvent(EVENT_CURSE_OF_EXHAUSTION, urand(20*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_FEAR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 20.0f, true))
                                DoCast(target, SPELL_FEAR);
                            events.ScheduleEvent(EVENT_FEAR, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_SEARING_PAIN:
                            DoCastVictim(SPELL_SEARING_PAIN);
                            events.ScheduleEvent(EVENT_SEARING_PAIN, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_UNSTABLE_AFFLICTION:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true))
                                DoCast(target, SPELL_UNSTABLE_AFFLICTION);
                            events.ScheduleEvent(EVENT_UNSTABLE_AFFLICTION, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
                DoSpellAttackIfReady(SPELL_SHADOW_BOLT);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_toc_warlockAI>(creature);
        }
};

class npc_toc_mage : public CreatureScript
{
    public:
        npc_toc_mage() : CreatureScript("npc_toc_mage") { }

        struct npc_toc_mageAI : public boss_faction_championsAI
        {
            npc_toc_mageAI(Creature* creature) : boss_faction_championsAI(creature, AI_RANGED) { }

            void Reset() override
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_ARCANE_BARRAGE, urand(1*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ARCANE_BLAST, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ARCANE_EXPLOSION, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_BLINK, urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_COUNTERSPELL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FROST_NOVA, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ICE_BLOCK, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_POLYMORPH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                SetEquipmentSlots(false, 47524, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARCANE_BARRAGE:
                            DoCastVictim(SPELL_ARCANE_BARRAGE);
                            events.ScheduleEvent(EVENT_ARCANE_BARRAGE, urand(5*IN_MILLISECONDS, 7*IN_MILLISECONDS));
                            return;
                        case EVENT_ARCANE_BLAST:
                            DoCastVictim(SPELL_ARCANE_BLAST);
                            events.ScheduleEvent(EVENT_ARCANE_BLAST, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_ARCANE_EXPLOSION:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCastAOE(SPELL_ARCANE_EXPLOSION);
                            events.ScheduleEvent(EVENT_ARCANE_EXPLOSION, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_BLINK:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCast(SPELL_BLINK);
                            events.ScheduleEvent(EVENT_BLINK, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_COUNTERSPELL:
                            if (Unit* target = SelectEnemyCaster(true))
                                DoCast(target, SPELL_COUNTERSPELL);
                            events.ScheduleEvent(EVENT_COUNTERSPELL, 24*IN_MILLISECONDS);
                            return;
                        case EVENT_FROST_NOVA:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCastAOE(SPELL_FROST_NOVA);
                            events.ScheduleEvent(EVENT_FROST_NOVA, 25*IN_MILLISECONDS);
                            return;
                        case EVENT_ICE_BLOCK:
                            if (HealthBelowPct(30))
                            {
                                DoCast(SPELL_ICE_BLOCK);
                                events.RescheduleEvent(EVENT_ICE_BLOCK, 5*MINUTE*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_ICE_BLOCK, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_POLYMORPH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_POLYMORPH);
                            events.ScheduleEvent(EVENT_POLYMORPH, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
                DoSpellAttackIfReady(SPELL_FROSTBOLT);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_toc_mageAI>(creature);
        }
};

class npc_toc_hunter : public CreatureScript
{
    public:
        npc_toc_hunter() : CreatureScript("npc_toc_hunter") { }

        struct npc_toc_hunterAI : public boss_faction_championsAI
        {
            npc_toc_hunterAI(Creature* creature) : boss_faction_championsAI(creature, AI_RANGED) { }

            void Reset() override
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_AIMED_SHOT, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DETERRENCE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DISENGAGE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_EXPLOSIVE_SHOT, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FROST_TRAP, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_STEADY_SHOT, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_WING_CLIP, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_WYVERN_STING, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                SetEquipmentSlots(false, 47156, EQUIP_NO_CHANGE, 48711);
            }

            void EnterCombat(Unit* who) override
            {
                boss_faction_championsAI::EnterCombat(who);
                DoCast(SPELL_CALL_PET);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_AIMED_SHOT:
                            DoCastVictim(SPELL_AIMED_SHOT);
                            events.ScheduleEvent(EVENT_AIMED_SHOT, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_DETERRENCE:
                            if (HealthBelowPct(30))
                            {
                                DoCast(SPELL_DETERRENCE);
                                events.RescheduleEvent(EVENT_DETERRENCE, 150*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_DETERRENCE, 10*IN_MILLISECONDS);
                            return;
                        case EVENT_DISENGAGE:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCast(SPELL_DISENGAGE);
                            events.ScheduleEvent(EVENT_DISENGAGE, 30*IN_MILLISECONDS);
                            return;
                        case EVENT_EXPLOSIVE_SHOT:
                            DoCastVictim(SPELL_EXPLOSIVE_SHOT);
                            events.ScheduleEvent(EVENT_EXPLOSIVE_SHOT, urand(6*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_FROST_TRAP:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCastAOE(SPELL_FROST_TRAP);
                            events.ScheduleEvent(EVENT_FROST_TRAP, 30*IN_MILLISECONDS);
                            return;
                        case EVENT_STEADY_SHOT:
                            DoCastVictim(SPELL_STEADY_SHOT);
                            events.ScheduleEvent(EVENT_STEADY_SHOT, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_WING_CLIP:
                            if (Unit* target = me->GetVictim())
                            {
                                if (me->GetDistance2d(target) < 6.0f)
                                    DoCast(target, SPELL_WING_CLIP);
                            }
                            events.ScheduleEvent(EVENT_WING_CLIP, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_WYVERN_STING:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_WYVERN_STING);
                            events.ScheduleEvent(EVENT_WYVERN_STING, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
                DoSpellAttackIfReady(SPELL_SHOOT);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_toc_hunterAI>(creature);
        }
};

class npc_toc_boomkin : public CreatureScript
{
    public:
        npc_toc_boomkin() : CreatureScript("npc_toc_boomkin") { }

        struct npc_toc_boomkinAI : public boss_faction_championsAI
        {
            npc_toc_boomkinAI(Creature* creature) : boss_faction_championsAI(creature, AI_RANGED) { }

            void Reset() override
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_CYCLONE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ENTANGLING_ROOTS, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FAERIE_FIRE, urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FORCE_OF_NATURE, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_INSECT_SWARM, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_MOONFIRE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_STARFIRE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DPS_BARKSKIN, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));

                SetEquipmentSlots(false, 50966, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CYCLONE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_CYCLONE);
                            events.ScheduleEvent(EVENT_CYCLONE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_ENTANGLING_ROOTS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true))
                                DoCast(target, SPELL_ENTANGLING_ROOTS);
                            events.ScheduleEvent(EVENT_ENTANGLING_ROOTS, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_FAERIE_FIRE:
                            DoCastVictim(SPELL_FAERIE_FIRE);
                            events.ScheduleEvent(EVENT_FAERIE_FIRE, urand(30*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_FORCE_OF_NATURE:
                            DoCastVictim(SPELL_FORCE_OF_NATURE);
                            events.ScheduleEvent(EVENT_FORCE_OF_NATURE, 180*IN_MILLISECONDS);
                            return;
                        case EVENT_INSECT_SWARM:
                            DoCastVictim(SPELL_INSECT_SWARM);
                            events.ScheduleEvent(EVENT_INSECT_SWARM, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_MOONFIRE:
                            DoCastVictim(SPELL_MOONFIRE);
                            events.ScheduleEvent(EVENT_MOONFIRE, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_STARFIRE:
                            DoCastVictim(SPELL_STARFIRE);
                            events.ScheduleEvent(EVENT_STARFIRE, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_DPS_BARKSKIN:
                            if (HealthBelowPct(30))
                            {
                                DoCast(me, SPELL_BARKSKIN);
                                events.RescheduleEvent(EVENT_DPS_BARKSKIN, 60*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_DPS_BARKSKIN, 5*IN_MILLISECONDS);
                            return;
                        default:
                            return;
                    }
                }
                DoSpellAttackIfReady(SPELL_WRATH);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_toc_boomkinAI>(creature);
        }
};

/********************************************************************
                            MELEE
********************************************************************/
class npc_toc_warrior : public CreatureScript
{
    public:
        npc_toc_warrior() : CreatureScript("npc_toc_warrior") { }

        struct npc_toc_warriorAI : public boss_faction_championsAI
        {
            npc_toc_warriorAI(Creature* creature) : boss_faction_championsAI(creature, AI_MELEE) { }

            void Reset() override
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_BLADESTORM, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_INTIMIDATING_SHOUT, urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_WARR_CHARGE, 1*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_DISARM, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_OVERPOWER, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SUNDER_ARMOR, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SHATTERING_THROW, urand(20*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_RETALIATION, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                SetEquipmentSlots(false, 47427, 46964, EQUIP_NO_CHANGE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BLADESTORM:
                            DoCastVictim(SPELL_BLADESTORM);
                            events.ScheduleEvent(EVENT_BLADESTORM, 150*IN_MILLISECONDS);
                            return;
                        case EVENT_INTIMIDATING_SHOUT:
                            DoCastAOE(SPELL_INTIMIDATING_SHOUT);
                            events.ScheduleEvent(EVENT_INTIMIDATING_SHOUT, 120*IN_MILLISECONDS);
                            return;
                        case EVENT_MORTAL_STRIKE:
                            DoCastVictim(SPELL_MORTAL_STRIKE);
                            events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_WARR_CHARGE:
                            DoCastVictim(SPELL_CHARGE);
                            events.ScheduleEvent(EVENT_WARR_CHARGE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_DISARM:
                            DoCastVictim(SPELL_DISARM);
                            events.ScheduleEvent(EVENT_DISARM, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_OVERPOWER:
                            DoCastVictim(SPELL_OVERPOWER);
                            events.ScheduleEvent(EVENT_OVERPOWER, urand(20*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_SUNDER_ARMOR:
                            DoCastVictim(SPELL_SUNDER_ARMOR);
                            events.ScheduleEvent(EVENT_SUNDER_ARMOR, urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            return;
                        case EVENT_SHATTERING_THROW:
                            if (Unit* target = me->GetVictim())
                            {
                                if (target->HasAuraWithMechanic(1 << MECHANIC_IMMUNE_SHIELD))
                                {
                                    DoCast(target, SPELL_SHATTERING_THROW);
                                    events.RescheduleEvent(EVENT_SHATTERING_THROW, 5*MINUTE*IN_MILLISECONDS);
                                    return;
                                }
                            }
                            events.RescheduleEvent(EVENT_SHATTERING_THROW, 3*IN_MILLISECONDS);
                            return;
                        case EVENT_RETALIATION:
                            if (HealthBelowPct(50))
                            {
                                DoCast(SPELL_RETALIATION);
                                events.RescheduleEvent(EVENT_RETALIATION, 5*MINUTE*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_RETALIATION, 5*IN_MILLISECONDS);
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_toc_warriorAI>(creature);
        }
};

class npc_toc_dk : public CreatureScript
{
    public:
        npc_toc_dk() : CreatureScript("npc_toc_dk") { }

        struct npc_toc_dkAI : public boss_faction_championsAI
        {
            npc_toc_dkAI(Creature* creature) : boss_faction_championsAI(creature, AI_MELEE) { }

            void Reset() override
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_CHAINS_OF_ICE, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DEATH_COIL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DEATH_GRIP, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FROST_STRIKE, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ICEBOUND_FORTITUDE, urand(25*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ICY_TOUCH, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_STRANGULATE, urand(5*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                SetEquipmentSlots(false, 47518, 51021, EQUIP_NO_CHANGE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHAINS_OF_ICE:
                            DoCastVictim(SPELL_CHAINS_OF_ICE);
                            events.ScheduleEvent(EVENT_CHAINS_OF_ICE, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_DEATH_COIL:
                            DoCastVictim(SPELL_DEATH_COIL);
                            events.ScheduleEvent(EVENT_DEATH_COIL, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_DEATH_GRIP:
                            if (Unit* target = me->GetVictim())
                            {
                                if (me->IsInRange(target, 5.0f, 30.0f, false))
                                {
                                    DoCast(target, SPELL_DEATH_GRIP);
                                    events.RescheduleEvent(EVENT_DEATH_GRIP, 35*IN_MILLISECONDS);
                                    return;
                                }
                            }
                            events.RescheduleEvent(EVENT_DEATH_GRIP, 3*IN_MILLISECONDS);
                            return;
                        case EVENT_FROST_STRIKE:
                            DoCastVictim(SPELL_FROST_STRIKE);
                            events.ScheduleEvent(EVENT_FROST_STRIKE, urand(6*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_ICEBOUND_FORTITUDE:
                            if (HealthBelowPct(50))
                            {
                                DoCast(SPELL_ICEBOUND_FORTITUDE);
                                events.RescheduleEvent(EVENT_ICEBOUND_FORTITUDE, 60*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_ICEBOUND_FORTITUDE, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_ICY_TOUCH:
                            DoCastVictim(SPELL_ICY_TOUCH);
                            events.ScheduleEvent(EVENT_ICY_TOUCH, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_STRANGULATE:
                            if (Unit* target = SelectEnemyCaster(false))
                            {
                                DoCast(target, SPELL_STRANGULATE);
                                events.RescheduleEvent(EVENT_STRANGULATE, 120*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_STRANGULATE, 5*IN_MILLISECONDS);
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_toc_dkAI>(creature);
        }
};

class npc_toc_rogue : public CreatureScript
{
    public:
        npc_toc_rogue() : CreatureScript("npc_toc_rogue") { }

        struct npc_toc_rogueAI : public boss_faction_championsAI
        {
            npc_toc_rogueAI(Creature* creature) : boss_faction_championsAI(creature, AI_MELEE) { }

            void Reset() override
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_FAN_OF_KNIVES, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_BLIND, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CLOAK, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_BLADE_FLURRY, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SHADOWSTEP, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEMORRHAGE, urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_EVISCERATE, urand(20*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_WOUND_POISON, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                SetEquipmentSlots(false, 47422, 49982, EQUIP_NO_CHANGE);
                me->setPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 100);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FAN_OF_KNIVES:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCastAOE(SPELL_FAN_OF_KNIVES);
                            events.ScheduleEvent(EVENT_FAN_OF_KNIVES, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_BLIND:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_BLIND);
                            events.ScheduleEvent(EVENT_BLIND, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_CLOAK:
                            if (HealthBelowPct(50))
                            {
                                DoCast(SPELL_CLOAK);
                                events.RescheduleEvent(EVENT_CLOAK, 90*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_CLOAK, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_BLADE_FLURRY:
                            if (EnemiesInRange(10.0f) >= 2)
                            {
                                DoCast(SPELL_BLADE_FLURRY);
                                events.RescheduleEvent(EVENT_BLADE_FLURRY, 120*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_BLADE_FLURRY, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_SHADOWSTEP:
                            if (Unit* target = me->GetVictim())
                            {
                                if (me->IsInRange(target, 10.0f, 40.0f, false))
                                {
                                    DoCast(target, SPELL_SHADOWSTEP);
                                    events.RescheduleEvent(EVENT_SHADOWSTEP, 30*IN_MILLISECONDS);
                                    return;
                                }
                            }
                            events.RescheduleEvent(EVENT_SHADOWSTEP, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_HEMORRHAGE:
                            DoCastVictim(SPELL_HEMORRHAGE);
                            events.ScheduleEvent(EVENT_HEMORRHAGE, urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_EVISCERATE:
                            DoCastVictim(SPELL_EVISCERATE);
                            events.ScheduleEvent(EVENT_EVISCERATE, urand(30*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_WOUND_POISON:
                            DoCastVictim(SPELL_WOUND_POISON);
                            events.ScheduleEvent(EVENT_WOUND_POISON, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_toc_rogueAI>(creature);
        }
};

class npc_toc_enh_shaman : public CreatureScript
{
    public:
        npc_toc_enh_shaman() : CreatureScript("npc_toc_enh_shaman") { }

        struct npc_toc_enh_shamanAI : public boss_faction_championsAI
        {
            npc_toc_enh_shamanAI(Creature* creature) : boss_faction_championsAI(creature, AI_MELEE)
            {
                Initialize();
            }

            void Initialize()
            {
                _totemCount = 0;
                _totemOldCenterX = me->GetPositionX();
                _totemOldCenterY = me->GetPositionY();
            }

            void Reset() override
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_DPS_EARTH_SHOCK, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_LAVA_LASH, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_STORMSTRIKE, urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DPS_BLOODLUST_HEROISM, 20*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_DEPLOY_TOTEM, 1*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_WINDFURY, urand(20*IN_MILLISECONDS, 50*IN_MILLISECONDS));

                Initialize();
                SetEquipmentSlots(false, 51803, 48013, EQUIP_NO_CHANGE);
                summons.DespawnAll();
            }

            void JustSummoned(Creature* summoned) override
            {
                summons.Summon(summoned);
            }

            void SummonedCreatureDespawn(Creature* /*pSummoned*/) override
            {
                --_totemCount;
            }

            void DeployTotem()
            {
                _totemCount = 4;
                _totemOldCenterX = me->GetPositionX();
                _totemOldCenterY = me->GetPositionY();
                /*
                -Windfury (16% melee haste)
                -Grounding (redirects one harmful magic spell to the totem)

                -Healing Stream (unable to find amount of healing in our logs)

                -Tremor (prevents fear effects)
                -Strength of Earth (155 strength and agil for the opposing team)

                -Searing (average ~3500 damage on a random target every ~3.5 seconds)
                */
            }

            void JustDied(Unit* killer) override
            {
                boss_faction_championsAI::JustDied(killer);
                summons.DespawnAll();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DPS_EARTH_SHOCK:
                            if (Unit* target = SelectEnemyCaster(true))
                                DoCast(target, SPELL_EARTH_SHOCK);
                            events.ScheduleEvent(EVENT_DPS_EARTH_SHOCK, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_LAVA_LASH:
                            DoCastVictim(SPELL_LAVA_LASH);
                            events.ScheduleEvent(EVENT_LAVA_LASH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_STORMSTRIKE:
                            DoCastVictim(SPELL_STORMSTRIKE);
                            events.ScheduleEvent(EVENT_STORMSTRIKE, urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_DPS_BLOODLUST_HEROISM:
                            if (me->getFaction()) //Am i alliance?
                            {
                                if (!me->HasAura(AURA_EXHAUSTION))
                                    DoCastAOE(SPELL_HEROISM);
                            }
                            else
                            {
                                if (!me->HasAura(AURA_SATED))
                                    DoCastAOE(SPELL_BLOODLUST);
                            }
                            events.ScheduleEvent(EVENT_DPS_BLOODLUST_HEROISM, 5*MINUTE*IN_MILLISECONDS);
                            return;
                        case EVENT_DEPLOY_TOTEM:
                            if (_totemCount < 4 || me->GetDistance2d(_totemOldCenterX, _totemOldCenterY) > 20.0f)
                                DeployTotem();
                            events.ScheduleEvent(EVENT_DEPLOY_TOTEM, 1*IN_MILLISECONDS);
                            return;
                        case EVENT_WINDFURY:
                            DoCastVictim(SPELL_WINDFURY);
                            events.ScheduleEvent(EVENT_WINDFURY, urand(20*IN_MILLISECONDS, 60*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
            }
            private:
                uint8  _totemCount;
                float  _totemOldCenterX, _totemOldCenterY;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_toc_enh_shamanAI>(creature);
        }
};

class npc_toc_retro_paladin : public CreatureScript
{
    public:
        npc_toc_retro_paladin() : CreatureScript("npc_toc_retro_paladin") { }

        struct npc_toc_retro_paladinAI : public boss_faction_championsAI
        {
            npc_toc_retro_paladinAI(Creature* creature) : boss_faction_championsAI(creature, AI_MELEE) { }

            void Reset() override
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_AVENGING_WRATH, urand(25*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CRUSADER_STRIKE, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DIVINE_STORM, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HAMMER_OF_JUSTICE_RET, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_JUDGEMENT_OF_COMMAND, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_REPENTANCE, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DPS_HAND_OF_PROTECTION, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DPS_DIVINE_SHIELD, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                SetEquipmentSlots(false, 47519, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            }

            void EnterCombat(Unit* who) override
            {
                boss_faction_championsAI::EnterCombat(who);
                DoCast(SPELL_SEAL_OF_COMMAND);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_AVENGING_WRATH:
                            DoCast(SPELL_AVENGING_WRATH);
                            events.ScheduleEvent(EVENT_AVENGING_WRATH, 180*IN_MILLISECONDS);
                            return;
                        case EVENT_CRUSADER_STRIKE:
                            DoCastVictim(SPELL_CRUSADER_STRIKE);
                            events.ScheduleEvent(EVENT_CRUSADER_STRIKE, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_DIVINE_STORM:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCast(SPELL_DIVINE_STORM);
                            events.ScheduleEvent(EVENT_DIVINE_STORM, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_HAMMER_OF_JUSTICE_RET:
                            DoCastVictim(SPELL_HAMMER_OF_JUSTICE_RET);
                            events.ScheduleEvent(EVENT_HAMMER_OF_JUSTICE_RET, 40*IN_MILLISECONDS);
                            return;
                        case EVENT_JUDGEMENT_OF_COMMAND:
                            DoCastVictim(SPELL_JUDGEMENT_OF_COMMAND);
                            events.ScheduleEvent(EVENT_JUDGEMENT_OF_COMMAND, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_REPENTANCE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_REPENTANCE);
                            events.ScheduleEvent(EVENT_REPENTANCE, 60*IN_MILLISECONDS);
                            return;
                        case EVENT_DPS_HAND_OF_PROTECTION:
                            if (Unit* target = DoSelectLowestHpFriendly(30.0f))
                            {
                                if (!target->HasAura(SPELL_FORBEARANCE))
                                {
                                    DoCast(target, SPELL_HAND_OF_PROTECTION);
                                    events.RescheduleEvent(EVENT_DPS_HAND_OF_PROTECTION, 5*MINUTE*IN_MILLISECONDS);
                                }
                                else
                                    events.RescheduleEvent(EVENT_DPS_HAND_OF_PROTECTION, 5*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_DPS_HAND_OF_PROTECTION, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_DPS_DIVINE_SHIELD:
                            if (HealthBelowPct(30) && !me->HasAura(SPELL_FORBEARANCE))
                            {
                                DoCast(me, SPELL_DIVINE_SHIELD);
                                events.RescheduleEvent(EVENT_DPS_DIVINE_SHIELD, 5*MINUTE*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_DPS_DIVINE_SHIELD, 5*IN_MILLISECONDS);
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_toc_retro_paladinAI>(creature);
        }
};

class npc_toc_pet_warlock : public CreatureScript
{
    public:
        npc_toc_pet_warlock() : CreatureScript("npc_toc_pet_warlock") { }

        struct npc_toc_pet_warlockAI : public boss_faction_championsAI
        {
            npc_toc_pet_warlockAI(Creature* creature) : boss_faction_championsAI(creature, AI_PET) { }

            void Reset() override
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_DEVOUR_MAGIC, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SPELL_LOCK, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DEVOUR_MAGIC:
                            DoCastVictim(SPELL_DEVOUR_MAGIC);
                            events.ScheduleEvent(EVENT_DEVOUR_MAGIC, urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_SPELL_LOCK:
                            DoCast(SPELL_SPELL_LOCK);
                            events.ScheduleEvent(EVENT_SPELL_LOCK, urand(24*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_toc_pet_warlockAI>(creature);
        }
};

class npc_toc_pet_hunter : public CreatureScript
{
    public:
        npc_toc_pet_hunter() : CreatureScript("npc_toc_pet_hunter") { }

        struct npc_toc_pet_hunterAI : public boss_faction_championsAI
        {
            npc_toc_pet_hunterAI(Creature* creature) : boss_faction_championsAI(creature, AI_PET)
            {
                Initialize();
            }

            void Initialize()
            {
                _clawTimer = urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS);
            }

            void Reset() override
            {
                boss_faction_championsAI::Reset();
                Initialize();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                boss_faction_championsAI::UpdateAI(diff);

                if (_clawTimer <= diff)
                {
                    DoCastVictim(SPELL_CLAW);
                    _clawTimer = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS);
                }
                else
                    _clawTimer -= diff;
            }
            private:
                uint32 _clawTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_toc_pet_hunterAI>(creature);
        }
};

class spell_faction_champion_warl_unstable_affliction : public SpellScriptLoader
{
    public:
        spell_faction_champion_warl_unstable_affliction() : SpellScriptLoader("spell_faction_champion_warl_unstable_affliction") { }

        class spell_faction_champion_warl_unstable_affliction_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_faction_champion_warl_unstable_affliction_AuraScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_UNSTABLE_AFFLICTION_DISPEL))
                    return false;
                return true;
            }

            void HandleDispel(DispelInfo* dispelInfo)
            {
                if (Unit* caster = GetCaster())
                    caster->CastSpell(dispelInfo->GetDispeller(), SPELL_UNSTABLE_AFFLICTION_DISPEL, true, NULL, GetEffect(EFFECT_0));
            }

            void Register() override
            {
                AfterDispel += AuraDispelFn(spell_faction_champion_warl_unstable_affliction_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_faction_champion_warl_unstable_affliction_AuraScript();
        }
};

class spell_faction_champion_death_grip : public SpellScriptLoader
{
    public:
        spell_faction_champion_death_grip() : SpellScriptLoader("spell_faction_champion_death_grip") { }

        class spell_faction_champion_death_grip_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_faction_champion_death_grip_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DEATH_GRIP_PULL))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    if (Unit* caster = GetCaster())
                        target->CastSpell(caster, SPELL_DEATH_GRIP_PULL);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_faction_champion_death_grip_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }

        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_faction_champion_death_grip_SpellScript();
        }
};

class spell_toc_bloodlust : public SpellScriptLoader
{
    public:
        spell_toc_bloodlust() : SpellScriptLoader("spell_toc_bloodlust") { }

        class spell_toc_bloodlust_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_toc_bloodlust_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(AURA_SATED))
                    return false;
                return true;
            }

            void RemoveInvalidTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(Trinity::UnitAuraCheck(true, AURA_SATED));
            }

            void ApplyDebuff()
            {
                if (Unit* target = GetHitUnit())
                    target->CastSpell(target, AURA_SATED, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_toc_bloodlust_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_toc_bloodlust_SpellScript::RemoveInvalidTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
                AfterHit += SpellHitFn(spell_toc_bloodlust_SpellScript::ApplyDebuff);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_toc_bloodlust_SpellScript();
        }
};

class spell_toc_heroism : public SpellScriptLoader
{
    public:
        spell_toc_heroism() : SpellScriptLoader("spell_toc_heroism") { }

        class spell_toc_heroism_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_toc_heroism_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(AURA_EXHAUSTION))
                    return false;
                return true;
            }

            void RemoveInvalidTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(Trinity::UnitAuraCheck(true, AURA_EXHAUSTION));
            }

            void ApplyDebuff()
            {
                if (Unit* target = GetHitUnit())
                    target->CastSpell(target, AURA_EXHAUSTION, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_toc_heroism_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_toc_heroism_SpellScript::RemoveInvalidTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
                AfterHit += SpellHitFn(spell_toc_heroism_SpellScript::ApplyDebuff);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_toc_heroism_SpellScript();
        }
};

//hxsd
class class_npczs : public CreatureScript
{
public:
    class_npczs() : CreatureScript("class_npczs") { }

 bool OnGossipHello(Player* player, Creature* creature)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("挑战"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF );
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("招募"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("解散"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(907, creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		me->setFaction(14);
		me->SetLevel(lvl);
		me->SetMaxHealth(lvl * 60);
		me->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			me->SetMaxHealth(lvl * 100);
			me->SetHealth(lvl * 100);

		}


		if (lvl > 40)
		{
			me->SetMaxHealth(lvl * 200);
			me->SetHealth(lvl * 200);

		}

		if (lvl > 70)
		{
			me->SetMaxHealth(lvl * 300);
			me->SetHealth(lvl * 300);

		}



		if (lvl > 80)
		{
			me->SetMaxHealth(lvl * 400);
			me->SetHealth(lvl * 400);

		}

		if (lvl > 85)
		{
			me->SetMaxHealth(lvl * 500);
			me->SetHealth(lvl * 500);

		}

		//me->UpdateMaxHealth();
		me->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			

		
		//me->SetBonusDamage(dmg);
		dmg=dmg/2;me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
		/*me->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);*/
		me->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
		me->UpdateDamagePhysical(BASE_ATTACK);
	    
	    me->UpdateDamagePhysical(OFF_ATTACK);
	     me->UpdateAttackPowerAndDamage();
		//me->AI()->EnterEvadeMode();
 break;
}
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+1:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		int eid = int(creature->GetEntry());
		int fl=int(irand(1,8));
		std::list<Creature*> summons;
		QueryResult result = WorldDatabase.PQuery("SELECT count(id) as sl FROM pknpc where cid= %u;",player->GetGUID());if (result){Field* fields = result->Fetch();if (fields[0].GetUInt32()>=4) {player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));return false;} }if (summons.size() >= 4)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));
			return false;
}

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() >=1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有这个招募者了！"));
			return false;
}



		SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(713);
		TempSummon *summon = player->GetMap()->SummonCreature(eid, *player, properties, 0, player);
		if (!summon)
			return true;
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		((Guardian*)summon)->InitStatsForLevel(lvl);
		summon->setFaction(player->getFaction());
		((Minion*)summon)->SetFollowAngle(fl);
                summon->GetMotionMaster()->MoveFollow(player, fl, summon->GetFollowAngle());
                
                summon->SetLevel(lvl);
		summon->SetMaxHealth(lvl * 60);
		summon->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			summon->SetMaxHealth(lvl * 100);
			summon->SetHealth(lvl * 100);

		}
		if (lvl > 40)
		{
			summon->SetMaxHealth(lvl * 200);
			summon->SetHealth(lvl * 200);

		}
		if (lvl > 70)
		{
			summon->SetMaxHealth(lvl * 300);
			summon->SetHealth(lvl * 300);

		}
		if (lvl > 80)
		{
			summon->SetMaxHealth(lvl * 500);
			summon->SetHealth(lvl * 500);

		}
		if (lvl > 85)
		{
			summon->SetMaxHealth(lvl * 1000);
			summon->SetHealth(lvl * 1000);

		}
		summon->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			
		dmg=dmg/2+1;summon->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
		/*summon->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);
		*/summon->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
	     summon->UpdateAttackPowerAndDamage();WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);WorldDatabase.PExecute("INSERT INTO pknpc (cid,pknpcid) VALUES (%u, %u);" , player->GetGUID(),eid );
			//summon->AI()->EnterEvadeMode();

break;
}		
		
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+2:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;
		int eid = int(creature->GetEntry());
		std::list<Creature*> summons;

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() < 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("当前没有这个招募者！"));
			return false;
}


		if (summons.size() >= 1)
		{
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		player->GetSession()->SendNotification(_StringToUTF8b("解散成功！"));WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);
			return true;
}

}			


            return true;
}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_bd_onyxiaAI (creature);
    }

    struct boss_bd_onyxiaAI : public ScriptedAI
    {
        boss_bd_onyxiaAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        EventMap events;
        uint32 m_uiDistancesCheckTimer;
        uint32 m_uiPowerTimer;

        void Reset()
        {
            me->setPowerType(POWER_RAGE);
            if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            //events.Reset();
        }

      

        
   void UpdatePower()
    {
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)
me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));

if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)
me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));

if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)
me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));

    }

    void JustDied(Unit* killer)
    {
    if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
    }


        

        void EnterCombat(Unit* /*who*/)
        {
             if (irand(0,58)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            m_uiPowerTimer = 2000;
            m_uiDistancesCheckTimer = 10000;
            
            
            
        }

/*        void EnterEvadeMode()
        {
            //me->DespawnOrUnsummon(100);
        }*/

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

             uint8 lvl = me->getLevel();
             Unit* opponent = me->GetVictim();


if (!opponent)
return;
float meleedist = me->GetDistance(opponent);
	    if (_isnan(meleedist))
		return;
            if (meleedist>30) 
                return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;


    uint32 SPELLA =40504;
    uint32 SPELLB =78;
    uint32 SPELLC =100;
    uint32 SPELLD =2457;

if (lvl>=10 && irand(0,8)==1)
{
    SPELLA =5308;
    SPELLB =12294;
    //SPELLC =355;
    SPELLD =71;	
}

if (lvl>=20 && irand(0,8)==1)
{
    SPELLA =7386;
    SPELLB =7394;
    SPELLC =6343;
    SPELLD =2565;	
}

if (lvl>=30 && irand(0,8)==1)
{
    SPELLA =1680;
    SPELLB =20243;
    //SPELLC =676;
    SPELLD =55694;	
}

if (lvl>=40 && irand(0,8)==1)
{
    SPELLA =1715;
    SPELLB =845;
    SPELLC =40599;
    SPELLD =6673;	
}

if (lvl>=50 && irand(0,8)==1)
{
    SPELLA =12323;
    //SPELLB =5246;
    SPELLC =46924;
    SPELLD =871;	
}

if (lvl>=60 && irand(0,8)==1)
{
    //SPELLA =46968;
    SPELLB =40504;
    SPELLC =12328;
    SPELLD =23920;	
}

if (lvl>=70 && irand(0,8)==1)
{
    SPELLA =64382;
    SPELLB =40599;
    SPELLC =65924;
    SPELLD =65932;	
}

if (lvl>=76 && irand(0,8)==1)
{
    SPELLA =65926;
    SPELLB =65936;
    SPELLC =65940;
    SPELLD =2457;	
}

if (lvl>=80 && irand(0,8)==1)
{
    SPELLA =65947;
    SPELLB =65930;
    SPELLC =68764;
    SPELLD =2457;	
}


            if (m_uiPowerTimer <= diff && GetHealthPCT(me) < 30)
            {
               DoCast(me, 61371);
               m_uiPowerTimer = 2000;
            }
            else m_uiPowerTimer -= diff;

            if (m_uiDistancesCheckTimer <= diff  && GetHealthPCT(me) < 10)
            {
                    me->AddAura(5016, me);

                m_uiDistancesCheckTimer = 5000;
            }
            else m_uiDistancesCheckTimer -= diff;if (irand(0,3)==0) {if (irand(0,180)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}}

            //events.Update(diff);

if (irand(0, 30) == 0) DoCastVictim(SPELLA);
else if (irand(0, 30) == 1) DoCastVictim(SPELLB);
else if (irand(0, 30) == 2) DoCastVictim(SPELLC);
else if (irand(0, 30) == 3) DoCastVictim(SPELLC);
else if (irand(0, 30) == 4) DoCast(me, SPELLD);
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
/*            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CLASSA:
                    DoCastVictim(SPELLA);
                    
                    events.ScheduleEvent(EVENT_CLASSA, urand(2000, 4000));
                    break;                       

                    case EVENT_CLASSB:
                    DoCastVictim(SPELLB);
                    
                    break;

                    case EVENT_CLASSC:
                    DoCastVictim(SPELLC);
                    if (irand(0,6)==1) Talk(1);
                    
                    if (irand(0,12)==1) DoCast(me,SPELLD);

                    
                    break;

                    default:
                    break;
                }
            }*/

if (me->HasUnitState(UNIT_STATE_CASTING)) return;            DoMeleeAttackIfReady();
        }
    };
};

class class_npcqs : public CreatureScript
{
public:
    class_npcqs() : CreatureScript("class_npcqs") { }

 bool OnGossipHello(Player* player, Creature* creature)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("挑战"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF );
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("招募"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("解散"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(907, creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		me->setFaction(14);
		me->SetLevel(lvl);
		me->SetMaxHealth(lvl * 60);
		me->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			me->SetMaxHealth(lvl * 100);
			me->SetHealth(lvl * 100);

		}


		if (lvl > 40)
		{
			me->SetMaxHealth(lvl * 200);
			me->SetHealth(lvl * 200);

		}

		if (lvl > 70)
		{
			me->SetMaxHealth(lvl * 300);
			me->SetHealth(lvl * 300);

		}



		if (lvl > 80)
		{
			me->SetMaxHealth(lvl * 400);
			me->SetHealth(lvl * 400);

		}

		if (lvl > 85)
		{
			me->SetMaxHealth(lvl * 500);
			me->SetHealth(lvl * 500);

		}

		//me->UpdateMaxHealth();
		me->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			

		
		//me->SetBonusDamage(dmg);
		dmg=dmg/2;me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
		/*me->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);*/
		me->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
		me->UpdateDamagePhysical(BASE_ATTACK);
	    me->UpdateDamagePhysical(OFF_ATTACK);
	     me->UpdateAttackPowerAndDamage();
		//me->AI()->EnterEvadeMode();
 break;
}
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+1:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		int eid = int(creature->GetEntry());
		int fl=int(irand(1,8));
		std::list<Creature*> summons;
		QueryResult result = WorldDatabase.PQuery("SELECT count(id) as sl FROM pknpc where cid= %u;",player->GetGUID());if (result){Field* fields = result->Fetch();if (fields[0].GetUInt32()>=4) {player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));return false;} }if (summons.size() >= 4)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));
			return false;
}

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() >= 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有这个招募者了！"));
			return false;
}



		SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(713);
		TempSummon *summon = player->GetMap()->SummonCreature(eid, *player, properties, 0, player);
		if (!summon)
			return true;
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		((Guardian*)summon)->InitStatsForLevel(lvl);
		summon->setFaction(player->getFaction());
		((Minion*)summon)->SetFollowAngle(fl);
                summon->GetMotionMaster()->MoveFollow(player, fl, summon->GetFollowAngle());
                
                summon->SetLevel(lvl);
		summon->SetMaxHealth(lvl * 60);
		summon->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			summon->SetMaxHealth(lvl * 100);
			summon->SetHealth(lvl * 100);

		}
		if (lvl > 40)
		{
			summon->SetMaxHealth(lvl * 200);
			summon->SetHealth(lvl * 200);

		}
		if (lvl > 70)
		{
			summon->SetMaxHealth(lvl * 300);
			summon->SetHealth(lvl * 300);

		}
		if (lvl > 80)
		{
			summon->SetMaxHealth(lvl * 500);
			summon->SetHealth(lvl * 500);

		}
		if (lvl > 85)
		{
			summon->SetMaxHealth(lvl * 1000);
			summon->SetHealth(lvl * 1000);

		}
		summon->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			
		dmg=dmg/2+1;summon->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
		/*summon->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);
		*/summon->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
	     summon->UpdateAttackPowerAndDamage();WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);WorldDatabase.PExecute("INSERT INTO pknpc (cid,pknpcid) VALUES (%u, %u);" , player->GetGUID(),eid );
			//summon->AI()->EnterEvadeMode();

break;
}		
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+2:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;
		int eid = int(creature->GetEntry());
		std::list<Creature*> summons;

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() < 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("当前没有这个招募者！"));
			return false;
}


		if (summons.size() >= 1)
		{
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		player->GetSession()->SendNotification(_StringToUTF8b("解散成功！"));WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);
			return true;
}

}			


            return true;
}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_bd_onyxiaAI (creature);
    }

    struct boss_bd_onyxiaAI : public ScriptedAI
    {
        boss_bd_onyxiaAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        EventMap events;
        uint32 m_uiDistancesCheckTimer;
        uint32 m_uiPowerTimer;

        void Reset()
        {
            if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            //events.Reset();
        }

      

        
   void UpdatePower()
    {
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)
me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));

if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)
me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));

if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)
me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
    }

    void JustDied(Unit* killer)
    {
    if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
    }


        

        void EnterCombat(Unit* /*who*/)
        {
            if (irand(0,58)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            m_uiPowerTimer = 2000;
            m_uiDistancesCheckTimer = 10000;
            
            
            
        }

/*        void EnterEvadeMode()
        {
            //me->DespawnOrUnsummon(100);
        }*/

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

             uint8 lvl = me->getLevel();
             Unit* opponent = me->GetVictim();


if (!opponent)
return;
float meleedist = me->GetDistance(opponent);
	    if (_isnan(meleedist))
		return;
            if (meleedist>30) 
                return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;


   uint32 SPELLA =20271;
    uint32 SPELLB =35395;
    uint32 SPELLC =853;
    uint32 SPELLD =54043;

if (lvl>=10 && irand(0,8)==1)
{
    SPELLA =48819;
    SPELLB =10326;
    SPELLC =25780;
    SPELLD =25780;	
}

if (lvl>=20 && irand(0,8)==1)
{
    SPELLA =2812;
    SPELLB =53595;
     SPELLC =61411;
    SPELLD =31801;	
}

if (lvl>=30 && irand(0,8)==1)
{
    SPELLA =40599;
    SPELLB =53385;
    SPELLC =24245;
    SPELLD =635;	
}

if (lvl>=40 && irand(0,8)==1)
{
    SPELLA =53600;
    SPELLB =10326;
    SPELLC =879;
    SPELLD =20154;	
}

if (lvl>=50 && irand(0,8)==1)
{
    SPELLA =35395;
    SPELLB =48818;
    SPELLC =53385;
    SPELLD =48936;	
}

if (lvl>=60 && irand(0,8)==1)
{
    SPELLA =35395;
    SPELLB =20271;
    SPELLC =53385;
    SPELLD =48785;	
}

if (lvl>=70 && irand(0,8)==1)
{
    SPELLA =35395;
    SPELLB =53385;
    SPELLC =20271;
    SPELLD =48801;	
}

if (lvl>=76 && irand(0,8)==1)
{
    SPELLA =66005;
    SPELLB =60504;
    SPELLC =60599;
    SPELLD =19740;	
}

if (lvl>=80 && irand(0,8)==1)
{
    SPELLA =66007;
    SPELLB =66006;
    SPELLC =66003;
    SPELLD =66011;	
}








            if (m_uiPowerTimer <= diff && GetHealthPCT(me) < 30)
            {
               DoCast(me, 61371);
               m_uiPowerTimer = 2000;
            }
            else m_uiPowerTimer -= diff;

            if (m_uiDistancesCheckTimer <= diff  && GetHealthPCT(me) < 10)
            {
                    me->AddAura(5016, me);

                m_uiDistancesCheckTimer = 5000;
            }
            else m_uiDistancesCheckTimer -= diff;if (irand(0,3)==0) {if (irand(0,180)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}}

            //events.Update(diff);

if (irand(0, 30) == 0) DoCastVictim(SPELLA);
else if (irand(0, 30) == 1) DoCastVictim(SPELLB);
else if (irand(0, 30) == 2) DoCastVictim(SPELLC);
else if (irand(0, 30) == 3) DoCastVictim(SPELLC);
else if (irand(0, 30) == 4) DoCast(me, SPELLD);
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
            /*while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CLASSA:
                    DoCastVictim(SPELLA);
                    
                    events.ScheduleEvent(EVENT_CLASSA, urand(2000, 4000));
                    break;                       

                    case EVENT_CLASSB:
                    DoCastVictim(SPELLB);
                    
                    break;

                    case EVENT_CLASSC:
                    DoCastVictim(SPELLC);
                    if (irand(0,6)==1) Talk(1);
                    
                    if (irand(0,12)==1) DoCast(me,SPELLD);

                    
                    break;

                    default:
                    break;
                }
            }*/

if (me->HasUnitState(UNIT_STATE_CASTING)) return;            DoMeleeAttackIfReady();
        }
    };
};


class class_npcfs : public CreatureScript
{
public:
    class_npcfs() : CreatureScript("class_npcfs") { }

 bool OnGossipHello(Player* player, Creature* creature)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("挑战"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF );
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("招募"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("解散"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(907, creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		me->setFaction(14);
		me->SetLevel(lvl);
		me->SetMaxHealth(lvl * 60);
		me->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			me->SetMaxHealth(lvl * 100);
			me->SetHealth(lvl * 100);

		}


		if (lvl > 40)
		{
			me->SetMaxHealth(lvl * 200);
			me->SetHealth(lvl * 200);

		}

		if (lvl > 70)
		{
			me->SetMaxHealth(lvl * 300);
			me->SetHealth(lvl * 300);

		}



		if (lvl > 80)
		{
			me->SetMaxHealth(lvl * 400);
			me->SetHealth(lvl * 400);

		}

		if (lvl > 85)
		{
			me->SetMaxHealth(lvl * 500);
			me->SetHealth(lvl * 500);

		}

		//me->UpdateMaxHealth();
		me->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			

		
		//me->SetBonusDamage(dmg);
		dmg=dmg/2;me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
/*		me->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);*/
		me->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
		me->UpdateDamagePhysical(BASE_ATTACK);
	    me->UpdateDamagePhysical(OFF_ATTACK);
	     me->UpdateAttackPowerAndDamage();
		//me->AI()->EnterEvadeMode();
 break;
}
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+1:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		int eid = int(creature->GetEntry());
		int fl=int(irand(1,8));
		std::list<Creature*> summons;
		QueryResult result = WorldDatabase.PQuery("SELECT count(id) as sl FROM pknpc where cid= %u;",player->GetGUID());if (result){Field* fields = result->Fetch();if (fields[0].GetUInt32()>=4) {player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));return false;} }if (summons.size() >= 4)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));
			return false;
}

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() >= 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有这个招募者了！"));
			return false;
}



		SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(713);
		TempSummon *summon = player->GetMap()->SummonCreature(eid, *player, properties, 0, player);
		if (!summon)
			return true;
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		((Guardian*)summon)->InitStatsForLevel(lvl);
		summon->setFaction(player->getFaction());
		((Minion*)summon)->SetFollowAngle(fl);
                summon->GetMotionMaster()->MoveFollow(player, fl, summon->GetFollowAngle());
                
                summon->SetLevel(lvl);
		summon->SetMaxHealth(lvl * 60);
		summon->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			summon->SetMaxHealth(lvl * 100);
			summon->SetHealth(lvl * 100);

		}
		if (lvl > 40)
		{
			summon->SetMaxHealth(lvl * 200);
			summon->SetHealth(lvl * 200);

		}
		if (lvl > 70)
		{
			summon->SetMaxHealth(lvl * 300);
			summon->SetHealth(lvl * 300);

		}
		if (lvl > 80)
		{
			summon->SetMaxHealth(lvl * 500);
			summon->SetHealth(lvl * 500);

		}
		if (lvl > 85)
		{
			summon->SetMaxHealth(lvl * 1000);
			summon->SetHealth(lvl * 1000);

		}
		summon->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			
		dmg=dmg/2+1;summon->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
		/*summon->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);
		*/summon->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
	     summon->UpdateAttackPowerAndDamage();WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);WorldDatabase.PExecute("INSERT INTO pknpc (cid,pknpcid) VALUES (%u, %u);" , player->GetGUID(),eid );
			//summon->AI()->EnterEvadeMode();

break;
}		
		
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+2:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;
		int eid = int(creature->GetEntry());
		std::list<Creature*> summons;

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() < 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("当前没有这个招募者！"));
			return false;
}


		if (summons.size() >= 1)
		{
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		player->GetSession()->SendNotification(_StringToUTF8b("解散成功！"));WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);
			return true;
}

}	

            return true;
}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_bd_onyxiaAI (creature);
    }

    struct boss_bd_onyxiaAI : public ScriptedAI
    {
        boss_bd_onyxiaAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        EventMap events;
        uint32 m_uiDistancesCheckTimer;
        uint32 m_uiPowerTimer;

        void Reset()
        {
            if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            //events.Reset();
        }

      

        
   void UpdatePower()
    {
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)
me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));

if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)
me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));

if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)
me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
    }

    void JustDied(Unit* killer)
    {
    if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
    }


        

        void EnterCombat(Unit* /*who*/)
        {
             if (irand(0,58)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            m_uiPowerTimer = 2000;
            m_uiDistancesCheckTimer = 10000;
            
            
            
        }

/*        void EnterEvadeMode()
        {
            //me->DespawnOrUnsummon(100);
        }*/

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

             uint8 lvl = me->getLevel();
             Unit* opponent = me->GetVictim();


if (!opponent)
return;
float meleedist = me->GetDistance(opponent);
	    if (_isnan(meleedist))
		return;
            if (meleedist>30) 
                return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;


    uint32 SPELLA =133;
    uint32 SPELLB =44614;
    uint32 SPELLC =122;
    uint32 SPELLD =45438;

if (lvl>=10 && irand(0,8)==1)
{
    SPELLA =30451;
    SPELLB =11366;
    SPELLC =44425;
    SPELLD =45438;	
}

if (lvl>=20 && irand(0,8)==1)
{
    SPELLA =116;
    SPELLB =2139;
    SPELLC =1449;
    SPELLD =12051;	
}

if (lvl>=30 && irand(0,8)==1)
{
    SPELLA =30455;
    SPELLB =5143;
    SPELLC =42896;
    SPELLD =30482;	
}

if (lvl>=40 && irand(0,8)==1)
{
    SPELLA =42873;
    SPELLB =38692;
    SPELLC =2948;
    SPELLD =11426;	
}

if (lvl>=50 && irand(0,8)==1)
{
    SPELLA =10;
    SPELLB =31661;
    SPELLC =44572;
    SPELLD =55342;	
}

if (lvl>=60 && irand(0,8)==1)
{
    SPELLA =55359;
    SPELLB =47610;
    SPELLC =42940;
    SPELLD =475;	
}

if (lvl>=70 && irand(0,8)==1)
{
    SPELLA =40504;
    SPELLB =42939;
    SPELLC =42841;
    SPELLD =6117;	
}

if (lvl>=76 && irand(0,8)==1)
{
    SPELLA =65802;
    SPELLB =65807;
    SPELLC =42846;
    SPELLD =43046;	
}

if (lvl>=80 && irand(0,8)==1)
{
    SPELLA =65799;
    SPELLB =65791;
    SPELLC =65800;
    SPELLD =65802;	
}



            if (m_uiPowerTimer <= diff && GetHealthPCT(me) < 30)
            {
               DoCast(me, 61371);
               m_uiPowerTimer = 2000;
            }
            else m_uiPowerTimer -= diff;

            if (m_uiDistancesCheckTimer <= diff  && GetHealthPCT(me) < 10)
            {
                    me->AddAura(5016, me);

                m_uiDistancesCheckTimer = 5000;
            }
            else m_uiDistancesCheckTimer -= diff;if (irand(0,3)==0) {if (irand(0,180)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}}

            //events.Update(diff);

if (irand(0, 30) == 0) DoCastVictim(SPELLA);
else if (irand(0, 30) == 1) DoCastVictim(SPELLB);
else if (irand(0, 30) == 2) DoCastVictim(SPELLC);
else if (irand(0, 30) == 3) DoCastVictim(SPELLC);
else if (irand(0, 30) == 4) DoCast(me, SPELLD);
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
         /*   while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CLASSA:
                    DoCastVictim(SPELLA);
                    
                    events.ScheduleEvent(EVENT_CLASSA, urand(2000, 4000));
                    break;                       

                    case EVENT_CLASSB:
                    DoCastVictim(SPELLB);
                    
                    break;

                    case EVENT_CLASSC:
                    DoCastVictim(SPELLC);
                    if (irand(0,6)==1) Talk(1);
                    
                    if (irand(0,12)==1) DoCast(me,SPELLD);

                    
                    break;

                    default:
                    break;
                }
            }*/
if (me->HasUnitState(UNIT_STATE_CASTING)) return;if (irand(0,2)==0) DoMeleeAttackIfReady(); else DoSpellAttackIfReady(133);
        }
    };
};


class class_npcss : public CreatureScript
{
public:
    class_npcss() : CreatureScript("class_npcss") { }

 bool OnGossipHello(Player* player, Creature* creature)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("挑战"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF );
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("招募"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("解散"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(907, creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		me->setFaction(14);
		me->SetLevel(lvl);
		me->SetMaxHealth(lvl * 60);
		me->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			me->SetMaxHealth(lvl * 100);
			me->SetHealth(lvl * 100);

		}


		if (lvl > 40)
		{
			me->SetMaxHealth(lvl * 200);
			me->SetHealth(lvl * 200);

		}

		if (lvl > 70)
		{
			me->SetMaxHealth(lvl * 300);
			me->SetHealth(lvl * 300);

		}



		if (lvl > 80)
		{
			me->SetMaxHealth(lvl * 400);
			me->SetHealth(lvl * 400);

		}

		if (lvl > 85)
		{
			me->SetMaxHealth(lvl * 500);
			me->SetHealth(lvl * 500);

		}

		//me->UpdateMaxHealth();
		me->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			

		
		//me->SetBonusDamage(dmg);
		dmg=dmg/2;me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
		/*me->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);*/
		me->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
		me->UpdateDamagePhysical(BASE_ATTACK);
	    me->UpdateDamagePhysical(OFF_ATTACK);
	     me->UpdateAttackPowerAndDamage();
		//me->AI()->EnterEvadeMode();
 break;
}
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+1:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		int eid = int(creature->GetEntry());
		int fl=int(irand(1,8));
		std::list<Creature*> summons;
		QueryResult result = WorldDatabase.PQuery("SELECT count(id) as sl FROM pknpc where cid= %u;",player->GetGUID());if (result){Field* fields = result->Fetch();if (fields[0].GetUInt32()>=4) {player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));return false;} }if (summons.size() >= 4)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));
			return false;
}

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() >= 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有这个招募者了！"));
			return false;
}



		SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(713);
		TempSummon *summon = player->GetMap()->SummonCreature(eid, *player, properties, 0, player);
		if (!summon)
			return true;
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		((Guardian*)summon)->InitStatsForLevel(lvl);
		summon->setFaction(player->getFaction());
		((Minion*)summon)->SetFollowAngle(fl);
                summon->GetMotionMaster()->MoveFollow(player, fl, summon->GetFollowAngle());
                
                summon->SetLevel(lvl);
		summon->SetMaxHealth(lvl * 60);
		summon->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			summon->SetMaxHealth(lvl * 100);
			summon->SetHealth(lvl * 100);

		}
		if (lvl > 40)
		{
			summon->SetMaxHealth(lvl * 200);
			summon->SetHealth(lvl * 200);

		}
		if (lvl > 70)
		{
			summon->SetMaxHealth(lvl * 300);
			summon->SetHealth(lvl * 300);

		}
		if (lvl > 80)
		{
			summon->SetMaxHealth(lvl * 500);
			summon->SetHealth(lvl * 500);

		}
		if (lvl > 85)
		{
			summon->SetMaxHealth(lvl * 1000);
			summon->SetHealth(lvl * 1000);

		}
		summon->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			
		dmg=dmg/2+1;summon->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
		/*summon->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);
		*/summon->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
	     summon->UpdateAttackPowerAndDamage();WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);WorldDatabase.PExecute("INSERT INTO pknpc (cid,pknpcid) VALUES (%u, %u);" , player->GetGUID(),eid );
			//summon->AI()->EnterEvadeMode();

break;
}		
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+2:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;
		int eid = int(creature->GetEntry());
		std::list<Creature*> summons;

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() < 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("当前没有这个招募者！"));
			return false;
}


		if (summons.size() >= 1)
		{
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		player->GetSession()->SendNotification(_StringToUTF8b("解散成功！"));WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);
			return true;
}

}			


            return true;
}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_bd_onyxiaAI (creature);
    }

    struct boss_bd_onyxiaAI : public ScriptedAI
    {
        boss_bd_onyxiaAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        EventMap events;
        uint32 m_uiDistancesCheckTimer;
        uint32 m_uiPowerTimer;

        void Reset()
        {
            if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            //events.Reset();
        }

      

        
   void UpdatePower()
    {
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)
me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));

if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)
me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));

if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)
me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
    }

    void JustDied(Unit* killer)
    {
    if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
    }


        

        void EnterCombat(Unit* /*who*/)
        {
            if (irand(0,58)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            m_uiPowerTimer = 2000;
            m_uiDistancesCheckTimer = 10000;
            
            
            
        }

/*        void EnterEvadeMode()
        {
            //me->DespawnOrUnsummon(100);
        }*/

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

             uint8 lvl = me->getLevel();
             Unit* opponent = me->GetVictim();


if (!opponent)
return;
float meleedist = me->GetDistance(opponent);
	    if (_isnan(meleedist))
		return;
            if (meleedist>30) 
                return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;


    uint32 SPELLA =686;
    uint32 SPELLB =172;
    uint32 SPELLC =689;
    uint32 SPELLD =696;

if (lvl>=10 && irand(0,8)==1)
{
    SPELLA =30108;
    SPELLB =29722;
    SPELLC =17962;
    SPELLD =755;	
}

if (lvl>=20 && irand(0,8)==1)
{
    SPELLA =348;
    SPELLB =6353;
    SPELLC =47838;
    SPELLD =688;	
}

if (lvl>=30 && irand(0,8)==1)
{
    SPELLA =1120;
    SPELLB =59172;
    SPELLC =5740;
    SPELLD =50589;
}

if (lvl>=40 && irand(0,8)==1)
{
    SPELLA =1949;
    SPELLB =47864;
    SPELLC =47897;
   SPELLD =132;
}

if (lvl>=50 && irand(0,8)==1)
{
    SPELLA =5138;
    SPELLB =30283;
    SPELLC = 47860;
    SPELLD =47889;	
}

if (lvl>=60 && irand(0,8)==1)
{
    SPELLA =40504;
    SPELLB =1490;
    SPELLC =40599;
    //SPELLD =119898;	
}

if (lvl>=70 && irand(0,8)==1)
{
    SPELLA =59092;
    SPELLB =30405;
    SPELLC =47811;
    SPELLD =60004;	
}

if (lvl>=76 && irand(0,8)==1)
{
    SPELLA =65810;
    SPELLB =65814;
    SPELLC =65815;
    SPELLD =61371;	
}

if (lvl>=80 && irand(0,8)==1)
{
    
    SPELLA =47809;
    SPELLB =65821;
    SPELLC =65812;
    //SPELLD =67514;	
}



            if (m_uiPowerTimer <= diff && GetHealthPCT(me) < 30)
            {
               DoCast(me, 61371);
               m_uiPowerTimer = 2000;
            }
            else m_uiPowerTimer -= diff;

            if (m_uiDistancesCheckTimer <= diff  && GetHealthPCT(me) < 10)
            {
                    me->AddAura(5016, me);

                m_uiDistancesCheckTimer = 5000;
            }
            else m_uiDistancesCheckTimer -= diff;if (irand(0,3)==0) {if (irand(0,180)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}}

          //  events.Update(diff);

if (irand(0, 30) == 0) DoCastVictim(SPELLA);
else if (irand(0, 30) == 1) DoCastVictim(SPELLB);
else if (irand(0, 30) == 2) DoCastVictim(SPELLC);
else if (irand(0, 30) == 3) DoCastVictim(SPELLC);
else if (irand(0, 30) == 4) DoCast(me, SPELLD);
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
           /* while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CLASSA:
                    DoCastVictim(SPELLA);
                    
                    events.ScheduleEvent(EVENT_CLASSA, urand(2000, 4000));
                    break;                       

                    case EVENT_CLASSB:
                    DoCastVictim(SPELLB);
                    
                    break;

                    case EVENT_CLASSC:
                    DoCastVictim(SPELLC);
                    if (irand(0,6)==1) Talk(1);
                    
                    if (irand(0,12)==1) DoCast(me,SPELLD);

                    
                    break;

                    default:
                    break;
                }
            }*/

if (me->HasUnitState(UNIT_STATE_CASTING)) return;if (irand(0,2)==0) DoMeleeAttackIfReady(); else             DoSpellAttackIfReady(686);
        }
    };
};

class class_npcms : public CreatureScript
{
public:
    class_npcms() : CreatureScript("class_npcms") { }

 bool OnGossipHello(Player* player, Creature* creature)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("挑战"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF );
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("招募"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("解散"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(907, creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		me->setFaction(14);
		me->SetLevel(lvl);
		me->SetMaxHealth(lvl * 60);
		me->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			me->SetMaxHealth(lvl * 100);
			me->SetHealth(lvl * 100);

		}


		if (lvl > 40)
		{
			me->SetMaxHealth(lvl * 200);
			me->SetHealth(lvl * 200);

		}

		if (lvl > 70)
		{
			me->SetMaxHealth(lvl * 300);
			me->SetHealth(lvl * 300);

		}



		if (lvl > 80)
		{
			me->SetMaxHealth(lvl * 400);
			me->SetHealth(lvl * 400);

		}

		if (lvl > 85)
		{
			me->SetMaxHealth(lvl * 500);
			me->SetHealth(lvl * 500);

		}

		//me->UpdateMaxHealth();
		me->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			

		
		//me->SetBonusDamage(dmg);
		dmg=dmg/2;me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
/*		me->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);*/
		me->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
		me->UpdateDamagePhysical(BASE_ATTACK);
	    me->UpdateDamagePhysical(OFF_ATTACK);
	     me->UpdateAttackPowerAndDamage();
		//me->AI()->EnterEvadeMode();
 break;
}
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+1:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		int eid = int(creature->GetEntry());
		int fl=int(irand(1,8));
		std::list<Creature*> summons;
		QueryResult result = WorldDatabase.PQuery("SELECT count(id) as sl FROM pknpc where cid= %u;",player->GetGUID());if (result){Field* fields = result->Fetch();if (fields[0].GetUInt32()>=4) {player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));return false;} }if (summons.size() >= 4)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));
			return false;
}

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() >= 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有这个招募者了！"));
			return false;
}



		SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(713);
		TempSummon *summon = player->GetMap()->SummonCreature(eid, *player, properties, 0, player);
		if (!summon)
			return true;
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		((Guardian*)summon)->InitStatsForLevel(lvl);
		summon->setFaction(player->getFaction());
		((Minion*)summon)->SetFollowAngle(fl);
                summon->GetMotionMaster()->MoveFollow(player, fl, summon->GetFollowAngle());
                
                summon->SetLevel(lvl);
		summon->SetMaxHealth(lvl * 60);
		summon->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			summon->SetMaxHealth(lvl * 100);
			summon->SetHealth(lvl * 100);

		}
		if (lvl > 40)
		{
			summon->SetMaxHealth(lvl * 200);
			summon->SetHealth(lvl * 200);

		}
		if (lvl > 70)
		{
			summon->SetMaxHealth(lvl * 300);
			summon->SetHealth(lvl * 300);

		}
		if (lvl > 80)
		{
			summon->SetMaxHealth(lvl * 500);
			summon->SetHealth(lvl * 500);

		}
		if (lvl > 85)
		{
			summon->SetMaxHealth(lvl * 1000);
			summon->SetHealth(lvl * 1000);

		}
		summon->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			
		dmg=dmg/2+1;summon->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
		/*summon->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);
		*/summon->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
	     summon->UpdateAttackPowerAndDamage();WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);WorldDatabase.PExecute("INSERT INTO pknpc (cid,pknpcid) VALUES (%u, %u);" , player->GetGUID(),eid );
			//summon->AI()->EnterEvadeMode();

break;
}		
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+2:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;
		int eid = int(creature->GetEntry());
		std::list<Creature*> summons;

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() < 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("当前没有这个招募者！"));
			return false;
}


		if (summons.size() >= 1)
		{
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		player->GetSession()->SendNotification(_StringToUTF8b("解散成功！"));WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);
			return true;
}

}			


            return true;
}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_bd_onyxiaAI (creature);
    }

    struct boss_bd_onyxiaAI : public ScriptedAI
    {
        boss_bd_onyxiaAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        EventMap events;
        uint32 m_uiDistancesCheckTimer;
        uint32 m_uiPowerTimer;

        void Reset()
        {
            if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            //events.Reset();
        }

      

        
   void UpdatePower()
    {
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)
me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));

if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)
me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));

if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)
me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
    }

    void JustDied(Unit* killer)
    {
    if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
    }


        

        void EnterCombat(Unit* /*who*/)
        {
             if (irand(0,58)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            m_uiPowerTimer = 2000;
            m_uiDistancesCheckTimer = 10000;
            
            
            
        }

/*        void EnterEvadeMode()
        {
            //me->DespawnOrUnsummon(100);
        }*/

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

             uint8 lvl = me->getLevel();
             Unit* opponent = me->GetVictim();


if (!opponent)
return;
float meleedist = me->GetDistance(opponent);
	    if (_isnan(meleedist))
		return;
            if (meleedist>30) 
                return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;


   uint32 SPELLA =585;
    uint32 SPELLB =589;
    uint32 SPELLC =48125;
    uint32 SPELLD =2061;

if (lvl>=10 && irand(0,8)==1)
{
    SPELLA =48127;
    SPELLB =15407;
    SPELLC =48300;
    SPELLD =21562;	
}

if (lvl>=20 && irand(0,8)==1)
{
    SPELLA =14914;
    SPELLB =2944;
    SPELLC =48160;
    SPELLD =586;	
}

if (lvl>=30 && irand(0,8)==1)
{
    SPELLA =34914;
    SPELLB =9484;
    SPELLC =34433;
    SPELLD =2050;	
}

if (lvl>=40 && irand(0,8)==1)
{
    SPELLA =48158;
    SPELLB =40504;
    SPELLC =48123;
    SPELLD =6346;
}

if (lvl>=50 && irand(0,8)==1)
{
    SPELLA =15487;
   SPELLB =48156;
    SPELLC =32375;
    //SPELLD =40504;	
}

if (lvl>=60 && irand(0,8)==1)
{
    SPELLA =64044;
    SPELLB =15487;
    SPELLC =40599;
    SPELLD =10060;	
}

if (lvl>=76 && irand(0,8)==1)
{
    SPELLA =65488;
    SPELLB =65492;
    SPELLC =10890;
    SPELLD =48072;	
}

if (lvl>=80 && irand(0,8)==1)
{
    SPELLA =65542;
    SPELLB =65490;
    SPELLC =65541;
    SPELLD =66177;	
}


            if (m_uiPowerTimer <= diff && GetHealthPCT(me) < 30)
            {
               DoCast(me, 61371);
               m_uiPowerTimer = 2000;
            }
            else m_uiPowerTimer -= diff;

            if (m_uiDistancesCheckTimer <= diff  && GetHealthPCT(me) < 10)
            {
                    me->AddAura(5016, me);

                m_uiDistancesCheckTimer = 5000;
            }
            else m_uiDistancesCheckTimer -= diff;if (irand(0,3)==0) {if (irand(0,180)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}}

            //events.Update(diff);

if (irand(0, 30) == 0) DoCastVictim(SPELLA);
else if (irand(0, 30) == 1) DoCastVictim(SPELLB);
else if (irand(0, 30) == 2) DoCastVictim(SPELLC);
else if (irand(0, 30) == 3) DoCastVictim(SPELLC);
else if (irand(0, 30) == 4) DoCast(me, SPELLD);
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
           /* while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CLASSA:
                    DoCastVictim(SPELLA);
                    
                    events.ScheduleEvent(EVENT_CLASSA, urand(2000, 4000));
                    break;                       

                    case EVENT_CLASSB:
                    DoCastVictim(SPELLB);
                    
                    break;

                    case EVENT_CLASSC:
                    DoCastVictim(SPELLC);
                    if (irand(0,6)==1) Talk(1);
                    
                    if (irand(0,12)==1) DoCast(me,SPELLD);

                    
                    break;

                    default:
                    break;
                }
            }*/

if (me->HasUnitState(UNIT_STATE_CASTING)) return;if (irand(0,2)==0) DoMeleeAttackIfReady(); else             DoSpellAttackIfReady(585);
        }
    };
};

class class_npclr : public CreatureScript
{
public:
    class_npclr() : CreatureScript("class_npclr") { }

 bool OnGossipHello(Player* player, Creature* creature)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("挑战"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF );
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("招募"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("解散"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(907, creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		me->setFaction(14);
		me->SetLevel(lvl);
		me->SetMaxHealth(lvl * 60);
		me->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			me->SetMaxHealth(lvl * 100);
			me->SetHealth(lvl * 100);

		}


		if (lvl > 40)
		{
			me->SetMaxHealth(lvl * 200);
			me->SetHealth(lvl * 200);

		}

		if (lvl > 70)
		{
			me->SetMaxHealth(lvl * 300);
			me->SetHealth(lvl * 300);

		}



		if (lvl > 80)
		{
			me->SetMaxHealth(lvl * 400);
			me->SetHealth(lvl * 400);

		}

		if (lvl > 85)
		{
			me->SetMaxHealth(lvl * 500);
			me->SetHealth(lvl * 500);

		}

		//me->UpdateMaxHealth();
		me->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			

		
		//me->SetBonusDamage(dmg);
		dmg=dmg/2;me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
/*		me->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);*/
		me->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
		me->UpdateDamagePhysical(BASE_ATTACK);
	    me->UpdateDamagePhysical(OFF_ATTACK);
	     me->UpdateAttackPowerAndDamage();
		//me->AI()->EnterEvadeMode();
 break;
}
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+1:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		int eid = int(creature->GetEntry());
		int fl=int(irand(1,8));
		std::list<Creature*> summons;
		QueryResult result = WorldDatabase.PQuery("SELECT count(id) as sl FROM pknpc where cid= %u;",player->GetGUID());if (result){Field* fields = result->Fetch();if (fields[0].GetUInt32()>=4) {player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));return false;} }if (summons.size() >= 4)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));
			return false;
}

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() >= 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有这个招募者了！"));
			return false;
}



		SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(713);
		TempSummon *summon = player->GetMap()->SummonCreature(eid, *player, properties, 0, player);
		if (!summon)
			return true;
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		((Guardian*)summon)->InitStatsForLevel(lvl);
		summon->setFaction(player->getFaction());
		((Minion*)summon)->SetFollowAngle(fl);
                summon->GetMotionMaster()->MoveFollow(player, fl, summon->GetFollowAngle());
                
                summon->SetLevel(lvl);
		summon->SetMaxHealth(lvl * 60);
		summon->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			summon->SetMaxHealth(lvl * 100);
			summon->SetHealth(lvl * 100);

		}
		if (lvl > 40)
		{
			summon->SetMaxHealth(lvl * 200);
			summon->SetHealth(lvl * 200);

		}
		if (lvl > 70)
		{
			summon->SetMaxHealth(lvl * 300);
			summon->SetHealth(lvl * 300);

		}
		if (lvl > 80)
		{
			summon->SetMaxHealth(lvl * 500);
			summon->SetHealth(lvl * 500);

		}
		if (lvl > 85)
		{
			summon->SetMaxHealth(lvl * 1000);
			summon->SetHealth(lvl * 1000);

		}
		summon->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			
		dmg=dmg/2+1;summon->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
		/*summon->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);
		*/summon->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
	     summon->UpdateAttackPowerAndDamage();WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);WorldDatabase.PExecute("INSERT INTO pknpc (cid,pknpcid) VALUES (%u, %u);" , player->GetGUID(),eid );
			//summon->AI()->EnterEvadeMode();

break;
}		
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+2:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;
		int eid = int(creature->GetEntry());
		std::list<Creature*> summons;

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() < 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("当前没有这个招募者！"));
			return false;
}


		if (summons.size() >= 1)
		{
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		player->GetSession()->SendNotification(_StringToUTF8b("解散成功！"));WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);
			return true;
}

}			


            return true;
}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_bd_onyxiaAI (creature);
    }

    struct boss_bd_onyxiaAI : public ScriptedAI
    {
        boss_bd_onyxiaAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        EventMap events;
        uint32 m_uiDistancesCheckTimer;
        uint32 m_uiPowerTimer;

        void Reset()
        {
        	me->setPowerType(POWER_FOCUS);
            if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            //events.Reset();
        }

      

        
   void UpdatePower()
    {
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)
me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));

if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)
me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));

if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)
me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
    }

    void JustDied(Unit* killer)
    {
    if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
    }


        

        void EnterCombat(Unit* /*who*/)
        {
            if (irand(0,58)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            m_uiPowerTimer = 2000;
            m_uiDistancesCheckTimer = 10000;
            
            
            
        }

/*        void EnterEvadeMode()
        {
            //me->DespawnOrUnsummon(100);
        }*/

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

             uint8 lvl = me->getLevel();
             Unit* opponent = me->GetVictim();


if (!opponent)
return;
float meleedist = me->GetDistance(opponent);
	    if (_isnan(meleedist))
		return;
            if (meleedist>30) 
                return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;


    uint32 SPELLA =2643;
    uint32 SPELLB =1499;
    uint32 SPELLC =34490;
    uint32 SPELLD =2457;

if (lvl>=10 && irand(0,8)==1)
{
    SPELLA =3044;
    SPELLB =56641;
    SPELLC =5116;
    //SPELLD =883;	
}

if (lvl>=20 && irand(0,8)==1)
{
    SPELLA =3043;
    SPELLB =3045;
    SPELLC =19386;
    SPELLD =13165;	
}

if (lvl>=30 && irand(0,8)==1)
{
    SPELLA =19577;
    SPELLB =19801;
   SPELLC =58434;
    SPELLD =40504;	
}

if (lvl>=40 && irand(0,8)==1)
{
    SPELLA =49052;
    SPELLB =19801;
    SPELLC =13809;
    //SPELLD =109260;	
}

if (lvl>=50 && irand(0,8)==1)
{
    SPELLA =49052;
    SPELLB =2974;
    SPELLC =3045;
    SPELLD =13159;	
}

if (lvl>=60 && irand(0,8)==1)
{
    SPELLA =53209;
    SPELLB =49056;
    SPELLC =34600;
    SPELLD =5118;
}

if (lvl>=70 && irand(0,8)==1)
{
    SPELLA =63672;
    SPELLB =60053;
    SPELLC =34600;
    SPELLD =19263;	
}

if (lvl>=75 && irand(0,8)==1)
{
    SPELLA =49001;
    SPELLB =49045;
    SPELLC =53338;
    SPELLD =51753;	
}

if (lvl>=76 && irand(0,8)==1)
{
    SPELLA =65883;
    SPELLB =65866;
    SPELLC =65867;
   // SPELLD =67777;	
}


if (lvl>=80 && irand(0,8)==1)
{
    SPELLA =65880;
    SPELLB =66207;
    SPELLC =65877;
    SPELLD =65871;	
}

            if (m_uiPowerTimer <= diff && GetHealthPCT(me) < 30)
            {
               DoCast(me, 61371);
               m_uiPowerTimer = 2000;
            }
            else m_uiPowerTimer -= diff;

            if (m_uiDistancesCheckTimer <= diff  && GetHealthPCT(me) < 10)
            {
                    me->AddAura(5016, me);

                m_uiDistancesCheckTimer = 5000;
            }
            else m_uiDistancesCheckTimer -= diff;if (irand(0,3)==0) {if (irand(0,180)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}}

           // events.Update(diff);

if (irand(0, 30) == 0) DoCastVictim(SPELLA);
else if (irand(0, 30) == 1) DoCastVictim(SPELLB);
else if (irand(0, 30) == 2) DoCastVictim(SPELLC);
else if (irand(0, 30) == 3) DoCastVictim(SPELLC);
else if (irand(0, 30) == 4) DoCast(me, SPELLD);
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
           /* while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CLASSA:
                    DoCastVictim(SPELLA);
                    
                    events.ScheduleEvent(EVENT_CLASSA, urand(2000, 4000));
                    break;                       

                    case EVENT_CLASSB:
                    DoCastVictim(SPELLB);
                    
                    break;

                    case EVENT_CLASSC:
                    DoCastVictim(SPELLC);
                    if (irand(0,6)==1) Talk(1);
                    
                    if (irand(0,12)==1) DoCast(me,SPELLD);

                    
                    break;

                    default:
                    break;
                }
            }
*/
if (me->HasUnitState(UNIT_STATE_CASTING)) return;if (irand(0,2)==0) DoMeleeAttackIfReady(); else             DoSpellAttackIfReady(75);
        }
    };
};

class class_npcdk : public CreatureScript
{
public:
    class_npcdk() : CreatureScript("class_npcdk") { }

 bool OnGossipHello(Player* player, Creature* creature)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("挑战"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF );
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("招募"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("解散"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(907, creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		me->setFaction(14);
		me->SetLevel(lvl);
		me->SetMaxHealth(lvl * 60);
		me->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			me->SetMaxHealth(lvl * 100);
			me->SetHealth(lvl * 100);

		}


		if (lvl > 40)
		{
			me->SetMaxHealth(lvl * 200);
			me->SetHealth(lvl * 200);

		}

		if (lvl > 70)
		{
			me->SetMaxHealth(lvl * 300);
			me->SetHealth(lvl * 300);

		}



		if (lvl > 80)
		{
			me->SetMaxHealth(lvl * 400);
			me->SetHealth(lvl * 400);

		}

		if (lvl > 85)
		{
			me->SetMaxHealth(lvl * 500);
			me->SetHealth(lvl * 500);

		}

		//me->UpdateMaxHealth();
		me->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			

		
		//me->SetBonusDamage(dmg);
		dmg=dmg/2;me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
/*		me->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);*/
		me->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
		me->UpdateDamagePhysical(BASE_ATTACK);
	    me->UpdateDamagePhysical(OFF_ATTACK);
	     me->UpdateAttackPowerAndDamage();
		//me->AI()->EnterEvadeMode();
 break;
}
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+1:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		int eid = int(creature->GetEntry());
		int fl=int(irand(1,8));
		std::list<Creature*> summons;
		QueryResult result = WorldDatabase.PQuery("SELECT count(id) as sl FROM pknpc where cid= %u;",player->GetGUID());if (result){Field* fields = result->Fetch();if (fields[0].GetUInt32()>=4) {player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));return false;} }if (summons.size() >= 4)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));
			return false;
}

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() >= 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有这个招募者了！"));
			return false;
}



		SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(713);
		TempSummon *summon = player->GetMap()->SummonCreature(eid, *player, properties, 0, player);
		if (!summon)
			return true;
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		((Guardian*)summon)->InitStatsForLevel(lvl);
		summon->setFaction(player->getFaction());
		((Minion*)summon)->SetFollowAngle(fl);
                summon->GetMotionMaster()->MoveFollow(player, fl, summon->GetFollowAngle());
                
                summon->SetLevel(lvl);
		summon->SetMaxHealth(lvl * 60);
		summon->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			summon->SetMaxHealth(lvl * 100);
			summon->SetHealth(lvl * 100);

		}
		if (lvl > 40)
		{
			summon->SetMaxHealth(lvl * 200);
			summon->SetHealth(lvl * 200);

		}
		if (lvl > 70)
		{
			summon->SetMaxHealth(lvl * 300);
			summon->SetHealth(lvl * 300);

		}
		if (lvl > 80)
		{
			summon->SetMaxHealth(lvl * 500);
			summon->SetHealth(lvl * 500);

		}
		if (lvl > 85)
		{
			summon->SetMaxHealth(lvl * 1000);
			summon->SetHealth(lvl * 1000);

		}
		summon->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			
		dmg=dmg/2+1;summon->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
		/*summon->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);
		*/summon->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
	     summon->UpdateAttackPowerAndDamage();WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);WorldDatabase.PExecute("INSERT INTO pknpc (cid,pknpcid) VALUES (%u, %u);" , player->GetGUID(),eid );
			//summon->AI()->EnterEvadeMode();

break;
}		
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+2:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;
		int eid = int(creature->GetEntry());
		std::list<Creature*> summons;

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() < 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("当前没有这个招募者！"));
			return false;
}


		if (summons.size() >= 1)
		{
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		player->GetSession()->SendNotification(_StringToUTF8b("解散成功！"));WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);
			return true;
}

}			


            return true;
}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_bd_onyxiaAI (creature);
    }

    struct boss_bd_onyxiaAI : public ScriptedAI
    {
        boss_bd_onyxiaAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        EventMap events;
        uint32 m_uiDistancesCheckTimer;
        uint32 m_uiPowerTimer;

        void Reset()
        {
                        me->setPowerType(POWER_RUNIC_POWER);
            me->SetMaxPower(POWER_RUNIC_POWER, me->GetCreatePowers(POWER_RUNIC_POWER));
            if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            //events.Reset();
        }

      

        
   void UpdatePower()
    {
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)
me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));

if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)
me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));

if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)
me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
    }

    void JustDied(Unit* killer)
    {
    if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
    }


        

        void EnterCombat(Unit* /*who*/)
        {
            if (irand(0,58)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            m_uiPowerTimer = 2000;
            m_uiDistancesCheckTimer = 10000;
            
            
            
        }

/*        void EnterEvadeMode()
        {
            //me->DespawnOrUnsummon(100);
        }*/

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

             uint8 lvl = me->getLevel();
             Unit* opponent = me->GetVictim();


if (!opponent)
return;
float meleedist = me->GetDistance(opponent);
	    if (_isnan(meleedist))
		return;
            if (meleedist>30) 
                return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;


    uint32 SPELLA =40504;
    uint32 SPELLB =78;
    uint32 SPELLC =100;
    uint32 SPELLD =2457;

if (lvl>=10 && irand(0,8)==1)
{
    SPELLA =45477;
    SPELLB =47541;
    SPELLC =355;
    SPELLD =48266;	
}

if (lvl>=20 && irand(0,8)==1)
{
    SPELLA =45462;
    SPELLB =45902;
    SPELLC =49143;
    SPELLD =51052;	
}

if (lvl>=30 && irand(0,8)==1)
{
    SPELLA =1680;
    SPELLB =20243;
    SPELLC =676;
    //SPELLD =49039;	
}

if (lvl>=40 && irand(0,8)==1)
{
    SPELLA =1715;
    SPELLB =49998;
    SPELLC =48721;
    SPELLD =6673;	
}

if (lvl>=50 && irand(0,8)==1)
{
    SPELLA =50842;
    SPELLB =48721;
    SPELLC =47528;
    SPELLD =48263;	
}

if (lvl>=60 && irand(0,8)==1)
{
    SPELLA =45524;
    SPELLB =47476;
    SPELLC =56222;
    SPELLD =48792;	
}

if (lvl>=70 && irand(0,8)==1)
{
    SPELLA =49020;
    SPELLB =43265;
   SPELLC =55271;
    SPELLD =49028;	
}

if (lvl>=76 && irand(0,8)==1)
{
    SPELLA =40504;
    SPELLB =40504;
    SPELLC =40599;
    SPELLD =48266;	
}

if (lvl>=80 && irand(0,8)==1)
{
    SPELLA =66020;
    SPELLB =66019;
    SPELLC =66047;
    SPELLD =66023;	
}

            if (m_uiPowerTimer <= diff && GetHealthPCT(me) < 30)
            {
               DoCast(me, 61371);
               m_uiPowerTimer = 2000;
            }
            else m_uiPowerTimer -= diff;

            if (m_uiDistancesCheckTimer <= diff  && GetHealthPCT(me) < 10)
            {
                    me->AddAura(5016, me);

                m_uiDistancesCheckTimer = 5000;
            }
            else m_uiDistancesCheckTimer -= diff;if (irand(0,3)==0) {if (irand(0,180)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}}

            //events.Update(diff);

if (irand(0, 30) == 0) DoCastVictim(SPELLA);
else if (irand(0, 30) == 1) DoCastVictim(SPELLB);
else if (irand(0, 30) == 2) DoCastVictim(SPELLC);
else if (irand(0, 30) == 3) DoCastVictim(SPELLC);
else if (irand(0, 30) == 4) DoCast(me, SPELLD);
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
           /* while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CLASSA:
                    DoCastVictim(SPELLA);
                    
                    events.ScheduleEvent(EVENT_CLASSA, urand(2000, 4000));
                    break;                       

                    case EVENT_CLASSB:
                    DoCastVictim(SPELLB);
                    
                    break;

                    case EVENT_CLASSC:
                    DoCastVictim(SPELLC);
                    if (irand(0,6)==1) Talk(1);
                    
                    if (irand(0,12)==1) DoCast(me,SPELLD);

                    
                    break;

                    default:
                    break;
                }
            }*/

if (me->HasUnitState(UNIT_STATE_CASTING)) return;            DoMeleeAttackIfReady();
        }
    };
};

class class_npcdly : public CreatureScript
{
public:
    class_npcdly() : CreatureScript("class_npcdly") { }

 bool OnGossipHello(Player* player, Creature* creature)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("挑战"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF );
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("招募"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("解散"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(907, creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		me->setFaction(14);
		me->SetLevel(lvl);
		me->SetMaxHealth(lvl * 60);
		me->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			me->SetMaxHealth(lvl * 100);
			me->SetHealth(lvl * 100);

		}


		if (lvl > 40)
		{
			me->SetMaxHealth(lvl * 200);
			me->SetHealth(lvl * 200);

		}

		if (lvl > 70)
		{
			me->SetMaxHealth(lvl * 300);
			me->SetHealth(lvl * 300);

		}



		if (lvl > 80)
		{
			me->SetMaxHealth(lvl * 400);
			me->SetHealth(lvl * 400);

		}

		if (lvl > 85)
		{
			me->SetMaxHealth(lvl * 500);
			me->SetHealth(lvl * 500);

		}

		//me->UpdateMaxHealth();
		me->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			

		
		//me->SetBonusDamage(dmg);
		dmg=dmg/2;me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
/*		me->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);*/
		me->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
		me->UpdateDamagePhysical(BASE_ATTACK);
	    me->UpdateDamagePhysical(OFF_ATTACK);
	     me->UpdateAttackPowerAndDamage();
		//me->AI()->EnterEvadeMode();
 break;
}
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+1:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		int eid = int(creature->GetEntry());
		int fl=int(irand(1,8));
		std::list<Creature*> summons;
		QueryResult result = WorldDatabase.PQuery("SELECT count(id) as sl FROM pknpc where cid= %u;",player->GetGUID());if (result){Field* fields = result->Fetch();if (fields[0].GetUInt32()>=4) {player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));return false;} }if (summons.size() >= 4)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));
			return false;
}

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() >= 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有这个招募者了！"));
			return false;
}



		SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(713);
		TempSummon *summon = player->GetMap()->SummonCreature(eid, *player, properties, 0, player);
		if (!summon)
			return true;
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		((Guardian*)summon)->InitStatsForLevel(lvl);
		summon->setFaction(player->getFaction());
		((Minion*)summon)->SetFollowAngle(fl);
                summon->GetMotionMaster()->MoveFollow(player, fl, summon->GetFollowAngle());
                
                summon->SetLevel(lvl);
		summon->SetMaxHealth(lvl * 60);
		summon->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			summon->SetMaxHealth(lvl * 100);
			summon->SetHealth(lvl * 100);

		}
		if (lvl > 40)
		{
			summon->SetMaxHealth(lvl * 200);
			summon->SetHealth(lvl * 200);

		}
		if (lvl > 70)
		{
			summon->SetMaxHealth(lvl * 300);
			summon->SetHealth(lvl * 300);

		}
		if (lvl > 80)
		{
			summon->SetMaxHealth(lvl * 500);
			summon->SetHealth(lvl * 500);

		}
		if (lvl > 85)
		{
			summon->SetMaxHealth(lvl * 1000);
			summon->SetHealth(lvl * 1000);

		}
		summon->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			
		dmg=dmg/2+1;summon->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
		/*summon->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);
		*/summon->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
	     summon->UpdateAttackPowerAndDamage();WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);WorldDatabase.PExecute("INSERT INTO pknpc (cid,pknpcid) VALUES (%u, %u);" , player->GetGUID(),eid );
			//summon->AI()->EnterEvadeMode();

break;
}		
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+2:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;
		int eid = int(creature->GetEntry());
		std::list<Creature*> summons;

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() < 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("当前没有这个招募者！"));
			return false;
}


		if (summons.size() >= 1)
		{
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		player->GetSession()->SendNotification(_StringToUTF8b("解散成功！"));WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);
			return true;
}

}			


            return true;
}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_bd_onyxiaAI (creature);
    }

    struct boss_bd_onyxiaAI : public ScriptedAI
    {
        boss_bd_onyxiaAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        EventMap events;
        uint32 m_uiDistancesCheckTimer;
        uint32 m_uiPowerTimer;

        void Reset()
        {
            if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            //events.Reset();
        }

      

        
   void UpdatePower()
    {
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)
me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));

if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)
me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));

if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)
me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
    }

    void JustDied(Unit* killer)
    {
    if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
    }


        

        void EnterCombat(Unit* /*who*/)
        {
            if (irand(0,58)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            m_uiPowerTimer = 2000;
            m_uiDistancesCheckTimer = 10000;
            
            
            
        }

/*        void EnterEvadeMode()
        {
            //me->DespawnOrUnsummon(100);
        }*/

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

             uint8 lvl = me->getLevel();
             Unit* opponent = me->GetVictim();


if (!opponent)
return;
float meleedist = me->GetDistance(opponent);
	    if (_isnan(meleedist))
		return;
            if (meleedist>30) 
                return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;


    uint32 SPELLA =5176;
    uint32 SPELLB =8921;
    uint32 SPELLC =22568;
    uint32 SPELLD =774;

if (lvl>=10 && irand(0,8)==1)
{
    SPELLA =339;
    SPELLB =2912;
    SPELLC =5487;
    SPELLD =5232;
}

if (lvl>=20 && irand(0,8)==1)
{
    SPELLA =770;
    SPELLB =53312;
    SPELLC =770;
    SPELLD =2090;
}

if (lvl>=30 && irand(0,8)==1)
{
    SPELLA =48465;
    SPELLB =48468;
    SPELLC =48467;
    SPELLD =33763;	
}

if (lvl>=40 && irand(0,8)==1)
{
    SPELLA =6795;
    SPELLB =9747;
    SPELLC =9880;
    SPELLD =9880;	
}

if (lvl>=50 && irand(0,8)==1)
{
    SPELLA =6785;
    SPELLB =52610;
    SPELLC =48574;
    SPELLD =2782;	
}

if (lvl>=60 && irand(0,8)==1)
{
    SPELLA =48511;
    SPELLB =26986;
    SPELLC =2637;
    SPELLD =1126;	
}

if (lvl>=70 && irand(0,8)==1)
{
    SPELLA =53201;
    SPELLB =9835;
    SPELLC =48505;
    SPELLD =5229;	
}

if (lvl>=76 && irand(0,8)==1)
{
    SPELLA =22570;
    SPELLB =40504;
    SPELLC =5211;
    //SPELLD =124974;	
}

if (lvl>=80 && irand(0,8)==1)
{
    SPELLA =65859;
    SPELLB =65854;
    SPELLC =65856;
    SPELLD =66093;	
}

            if (m_uiPowerTimer <= diff && GetHealthPCT(me) < 30)
            {
               DoCast(me, 61371);
               m_uiPowerTimer = 2000;
            }
            else m_uiPowerTimer -= diff;

            if (m_uiDistancesCheckTimer <= diff  && GetHealthPCT(me) < 10)
            {
                    me->AddAura(5016, me);

                m_uiDistancesCheckTimer = 5000;
            }
            else m_uiDistancesCheckTimer -= diff;if (irand(0,3)==0) {if (irand(0,180)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}}

           // events.Update(diff);

if (irand(0, 30) == 0) DoCastVictim(SPELLA);
else if (irand(0, 30) == 1) DoCastVictim(SPELLB);
else if (irand(0, 30) == 2) DoCastVictim(SPELLC);
else if (irand(0, 30) == 3) DoCastVictim(SPELLC);
else if (irand(0, 30) == 4) DoCast(me, SPELLD);
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
            /*while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CLASSA:
                    DoCastVictim(SPELLA);
                    
                    events.ScheduleEvent(EVENT_CLASSA, urand(2000, 4000));
                    break;                       

                    case EVENT_CLASSB:
                    DoCastVictim(SPELLB);
                    
                    break;

                    case EVENT_CLASSC:
                    DoCastVictim(SPELLC);
                    if (irand(0,6)==1) Talk(1);
                    
                    if (irand(0,12)==1) DoCast(me,SPELLD);

                    
                    break;

                    default:
                    break;
                }
            }*/
 if (me->HasUnitState(UNIT_STATE_CASTING)) return;if (irand(0,2)==0) DoMeleeAttackIfReady(); else DoSpellAttackIfReady(8921);
//            DoMeleeAttackIfReady();
        }
    };
};

class class_npcdz : public CreatureScript
{
public:
    class_npcdz() : CreatureScript("class_npcdz") { }

 bool OnGossipHello(Player* player, Creature* creature)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("挑战"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF );
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("招募"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("解散"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(907, creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		me->setFaction(14);
		me->SetLevel(lvl);
		me->SetMaxHealth(lvl * 60);
		me->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			me->SetMaxHealth(lvl * 100);
			me->SetHealth(lvl * 100);

		}


		if (lvl > 40)
		{
			me->SetMaxHealth(lvl * 200);
			me->SetHealth(lvl * 200);

		}

		if (lvl > 70)
		{
			me->SetMaxHealth(lvl * 300);
			me->SetHealth(lvl * 300);

		}



		if (lvl > 80)
		{
			me->SetMaxHealth(lvl * 400);
			me->SetHealth(lvl * 400);

		}

		if (lvl > 85)
		{
			me->SetMaxHealth(lvl * 500);
			me->SetHealth(lvl * 500);

		}

		//me->UpdateMaxHealth();
		me->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			

		
		//me->SetBonusDamage(dmg);
		dmg=dmg/2;me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
/*		me->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);*/
		me->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
		me->UpdateDamagePhysical(BASE_ATTACK);
	    me->UpdateDamagePhysical(OFF_ATTACK);
	     me->UpdateAttackPowerAndDamage();
		//me->AI()->EnterEvadeMode();
 break;
}
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+1:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		int eid = int(creature->GetEntry());
		int fl=int(irand(1,8));
		std::list<Creature*> summons;
		QueryResult result = WorldDatabase.PQuery("SELECT count(id) as sl FROM pknpc where cid= %u;",player->GetGUID());if (result){Field* fields = result->Fetch();if (fields[0].GetUInt32()>=4) {player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));return false;} }if (summons.size() >= 4)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));
			return false;
}

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() >= 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有这个招募者了！"));
			return false;
}



		SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(713);
		TempSummon *summon = player->GetMap()->SummonCreature(eid, *player, properties, 0, player);
		if (!summon)
			return true;
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		((Guardian*)summon)->InitStatsForLevel(lvl);
		summon->setFaction(player->getFaction());
		((Minion*)summon)->SetFollowAngle(fl);
                summon->GetMotionMaster()->MoveFollow(player, fl, summon->GetFollowAngle());
                
                summon->SetLevel(lvl);
		summon->SetMaxHealth(lvl * 60);
		summon->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			summon->SetMaxHealth(lvl * 100);
			summon->SetHealth(lvl * 100);

		}
		if (lvl > 40)
		{
			summon->SetMaxHealth(lvl * 200);
			summon->SetHealth(lvl * 200);

		}
		if (lvl > 70)
		{
			summon->SetMaxHealth(lvl * 300);
			summon->SetHealth(lvl * 300);

		}
		if (lvl > 80)
		{
			summon->SetMaxHealth(lvl * 500);
			summon->SetHealth(lvl * 500);

		}
		if (lvl > 85)
		{
			summon->SetMaxHealth(lvl * 1000);
			summon->SetHealth(lvl * 1000);

		}
		summon->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			
		dmg=dmg/2+1;summon->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
		/*summon->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);
		*/summon->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
	     summon->UpdateAttackPowerAndDamage();WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);WorldDatabase.PExecute("INSERT INTO pknpc (cid,pknpcid) VALUES (%u, %u);" , player->GetGUID(),eid );
			//summon->AI()->EnterEvadeMode();

break;
}		
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+2:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;
		int eid = int(creature->GetEntry());
		std::list<Creature*> summons;

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() < 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("当前没有这个招募者！"));
			return false;
}


		if (summons.size() >= 1)
		{
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		player->GetSession()->SendNotification(_StringToUTF8b("解散成功！"));WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);
			return true;
}

}			


            return true;
}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_bd_onyxiaAI (creature);
    }

    struct boss_bd_onyxiaAI : public ScriptedAI
    {
        boss_bd_onyxiaAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        EventMap events;
        uint32 m_uiDistancesCheckTimer;
        uint32 m_uiPowerTimer;

        void Reset()
        {
            if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            //events.Reset();
        }

      

        
   void UpdatePower()
    {
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)
me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));

if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)
me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));

if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)
me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
    }

    void JustDied(Unit* killer)
    {
    if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
    }


        

        void EnterCombat(Unit* /*who*/)
        {
            if (irand(0,58)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            m_uiPowerTimer = 2000;
            m_uiDistancesCheckTimer = 10000;
            
            
            
        }

/*        void EnterEvadeMode()
        {
            //me->DespawnOrUnsummon(100);
        }*/

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

             uint8 lvl = me->getLevel();
             Unit* opponent = me->GetVictim();


if (!opponent)
return;
float meleedist = me->GetDistance(opponent);
	    if (_isnan(meleedist))
		return;
            if (meleedist>30) 
                return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;


    uint32 SPELLA =1776;
    uint32 SPELLB =78;
    uint32 SPELLC =100;
    uint32 SPELLD =2457;

if (lvl>=10 && irand(0,8)==1)
{
    SPELLA =1752;
    SPELLB =2098;
    SPELLC =6770;
    SPELLD =1784;	
}

if (lvl>=20 && irand(0,8)==1)
{
    SPELLA =1766;
    SPELLB =48668;
    SPELLC =1776;
    SPELLD =2983;	
}

if (lvl>=30 && irand(0,8)==1)
{
    SPELLA =1833;
    SPELLB =14183;
    SPELLC =2094;
    SPELLD =1856;	
}

if (lvl>=40 && irand(0,8)==1)
{
    SPELLA =408;
    SPELLB =1943;
    SPELLC =703;
    SPELLD =13750;	
}

if (lvl>=50 && irand(0,8)==1)
{
    SPELLA =1943;
    SPELLB =51722;
    SPELLC =5938;
    SPELLD =31224;	
}

if (lvl>=60 && irand(0,8)==1)
{
    SPELLA =36554;
    SPELLB =51723;
    SPELLC =53;
    SPELLD =14185;	
}

if (lvl>=70 && irand(0,8)==1)
{
    SPELLA =5938;
    SPELLB =8647;
    SPELLC =26679;
    SPELLD =14185;		
}

if (lvl>=76 && irand(0,8)==1)
{
    SPELLA =51690;
    SPELLB =40504;
    SPELLC =65940;
    SPELLD =36554;
}

if (lvl>=80 && irand(0,8)==1)
{
    SPELLA =65960;
    SPELLB =65957;
    SPELLC =65962;
    SPELLD =65961;	
}

            if (m_uiPowerTimer <= diff && GetHealthPCT(me) < 30)
            {
               DoCast(me, 61371);
               m_uiPowerTimer = 2000;
            }
            else m_uiPowerTimer -= diff;

            if (m_uiDistancesCheckTimer <= diff  && GetHealthPCT(me) < 10)
            {
                    me->AddAura(5016, me);

                m_uiDistancesCheckTimer = 5000;
            }
            else m_uiDistancesCheckTimer -= diff;if (irand(0,3)==0) {if (irand(0,180)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}}

           // events.Update(diff);

if (irand(0, 30) == 0) DoCastVictim(SPELLA);
else if (irand(0, 30) == 1) DoCastVictim(SPELLB);
else if (irand(0, 30) == 2) DoCastVictim(SPELLC);
else if (irand(0, 30) == 3) DoCastVictim(SPELLC);
else if (irand(0, 30) == 4) DoCast(me, SPELLD);
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
           /* while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CLASSA:
                    DoCastVictim(SPELLA);
                    
                    events.ScheduleEvent(EVENT_CLASSA, urand(2000, 4000));
                    break;                       

                    case EVENT_CLASSB:
                    DoCastVictim(SPELLB);
                    
                    break;

                    case EVENT_CLASSC:
                    DoCastVictim(SPELLC);
                    if (irand(0,6)==1) Talk(1);
                    
                    if (irand(0,12)==1) DoCast(me,SPELLD);

                    
                    break;

                    default:
                    break;
                }
            }*/

if (me->HasUnitState(UNIT_STATE_CASTING)) return;            DoMeleeAttackIfReady();
        }
    };
};


class class_npcsm : public CreatureScript
{
public:
    class_npcsm() : CreatureScript("class_npcsm") { }

 bool OnGossipHello(Player* player, Creature* creature)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("挑战"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF );
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("招募"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _StringToUTF8b("解散"), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(907, creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		me->setFaction(14);
		me->SetLevel(lvl);
		me->SetMaxHealth(lvl * 60);
		me->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			me->SetMaxHealth(lvl * 100);
			me->SetHealth(lvl * 100);

		}


		if (lvl > 40)
		{
			me->SetMaxHealth(lvl * 200);
			me->SetHealth(lvl * 200);

		}

		if (lvl > 70)
		{
			me->SetMaxHealth(lvl * 300);
			me->SetHealth(lvl * 300);

		}



		if (lvl > 80)
		{
			me->SetMaxHealth(lvl * 400);
			me->SetHealth(lvl * 400);

		}

		if (lvl > 85)
		{
			me->SetMaxHealth(lvl * 500);
			me->SetHealth(lvl * 500);

		}

		//me->UpdateMaxHealth();
		me->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			

		
		//me->SetBonusDamage(dmg);
		dmg=dmg/2;me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//me->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		//me->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		me->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
/*		me->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		me->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);*/
		me->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
		me->UpdateDamagePhysical(BASE_ATTACK);
	    me->UpdateDamagePhysical(OFF_ATTACK);
	     me->UpdateAttackPowerAndDamage();
		//me->AI()->EnterEvadeMode();
 break;
}
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+1:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;

		int lvl=int(player->getLevel());
		int eid = int(creature->GetEntry());
		int fl=int(irand(1,8));
		std::list<Creature*> summons;
		QueryResult result = WorldDatabase.PQuery("SELECT count(id) as sl FROM pknpc where cid= %u;",player->GetGUID());if (result){Field* fields = result->Fetch();if (fields[0].GetUInt32()>=4) {player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));return false;} }if (summons.size() >= 4)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有4个招募者了！"));
			return false;
}

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() >= 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("你已经有这个招募者了！"));
			return false;
}



		SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(713);
		TempSummon *summon = player->GetMap()->SummonCreature(eid, *player, properties, 0, player);
		if (!summon)
			return true;
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		((Guardian*)summon)->InitStatsForLevel(lvl);
		summon->setFaction(player->getFaction());
		((Minion*)summon)->SetFollowAngle(fl);
                summon->GetMotionMaster()->MoveFollow(player, fl, summon->GetFollowAngle());
                
                summon->SetLevel(lvl);
		summon->SetMaxHealth(lvl * 60);
		summon->SetHealth(lvl * 60);
		if (lvl > 20)
		{
			summon->SetMaxHealth(lvl * 100);
			summon->SetHealth(lvl * 100);

		}
		if (lvl > 40)
		{
			summon->SetMaxHealth(lvl * 200);
			summon->SetHealth(lvl * 200);

		}
		if (lvl > 70)
		{
			summon->SetMaxHealth(lvl * 300);
			summon->SetHealth(lvl * 300);

		}
		if (lvl > 80)
		{
			summon->SetMaxHealth(lvl * 500);
			summon->SetHealth(lvl * 500);

		}
		if (lvl > 85)
		{
			summon->SetMaxHealth(lvl * 1000);
			summon->SetHealth(lvl * 1000);

		}
		summon->SetFullHealth();
		uint32 dmg = 10;
                dmg = (lvl * 10);
		if (lvl > 20)
			dmg = (lvl * 15);

		if (lvl > 40)
			dmg =  (lvl * 25);

		if (lvl > 70)
			dmg = (lvl * 35);


		if (lvl > 80)
			dmg = (lvl * 70);

		if (lvl >= 86)
			dmg = dmg + (lvl * 80);
			
		dmg=dmg/2+1;summon->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
		//summon->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
		/*summon->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);
		*/summon->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
	     summon->UpdateAttackPowerAndDamage();WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);WorldDatabase.PExecute("INSERT INTO pknpc (cid,pknpcid) VALUES (%u, %u);" , player->GetGUID(),eid );
			//summon->AI()->EnterEvadeMode();

break;
}		
switch (action)
            {
case GOSSIP_ACTION_INFO_DEF+2:
		Creature* me=creature;
		if (!player)
		return false;
		
		if (!player->getLevel())
		return false;
		int eid = int(creature->GetEntry());
		std::list<Creature*> summons;

		player->GetAllMinionsByEntry(summons, eid);

		if (summons.size() < 1)
		{
		player->GetSession()->SendNotification(_StringToUTF8b("当前没有这个招募者！"));
			return false;
}


		if (summons.size() >= 1)
		{
        me->setDeathState(JUST_DIED);
        me->RemoveCorpse();
        me->SetHealth(0); // just for nice GM-mode view
		player->GetSession()->SendNotification(_StringToUTF8b("解散成功！"));WorldDatabase.PExecute("DELETE FROM pknpc WHERE cid = %u and pknpcid=%u;", player->GetGUID(),eid);
			return true;
}

}			


            return true;
}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_bd_onyxiaAI (creature);
    }

    struct boss_bd_onyxiaAI : public ScriptedAI
    {
        boss_bd_onyxiaAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        EventMap events;
        uint32 m_uiDistancesCheckTimer;
        uint32 m_uiPowerTimer;

        void Reset()
        {
            if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            //events.Reset();
        }

      

        
   void UpdatePower()
    {
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)
me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));

if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)
me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));

if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)
me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
    }

    void JustDied(Unit* killer)
    {
    if (irand(0,8)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
    }


        

        void EnterCombat(Unit* /*who*/)
        {
            if (irand(0,58)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}
            m_uiPowerTimer = 2000;
            m_uiDistancesCheckTimer = 10000;
            
            
            
        }

/*        void EnterEvadeMode()
        {
            //me->DespawnOrUnsummon(100);
        }*/

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

             uint8 lvl = me->getLevel();
             Unit* opponent = me->GetVictim();


if (!opponent)
return;
float meleedist = me->GetDistance(opponent);
	    if (_isnan(meleedist))
		return;
            if (meleedist>30) 
                return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;


    uint32 SPELLA =10605;
    uint32 SPELLB =78;
    uint32 SPELLC =100;
    uint32 SPELLD =2457;

if (lvl>=10 && irand(0,8)==1)
{
    SPELLA =403;
    SPELLB =8042;
    SPELLC =10414;
    SPELLD =324;	
}

if (lvl>=20 && irand(0,8)==1)
{
    SPELLA =8056;
    SPELLB =15208;
    SPELLC =421;
    SPELLD =974;	
}

if (lvl>=30 && irand(0,8)==1)
{
    SPELLA =51505;
    SPELLB =57994;
    SPELLC =3599;
    SPELLD =8177;	
}

if (lvl>=40 && irand(0,8)==1)
{
    SPELLA =1535;
    SPELLB =60103;
    SPELLC =10414;
    SPELLD =1064;	
}

if (lvl>=50 && irand(0,8)==1)
{
    SPELLA =17364;
    SPELLB =73899;
    SPELLB =15208;
    SPELLC =10414;	
}

if (lvl>=60 && irand(0,8)==1)
{
    SPELLA =61882;
    SPELLB =15208;
    SPELLC =10414;
    SPELLD =40504;	
}

if (lvl>=70 && irand(0,8)==1)
{
    SPELLA =10448;
    SPELLB =10605;
    SPELLC =10473;
    SPELLD =2825;	
}

if (lvl>=76 && irand(0,8)==1)
{
    SPELLA =10448;
    SPELLB =15208;
    SPELLC =8042;
    SPELLD =10468;	
}
if (lvl>=80 && irand(0,8)==1)
{
    SPELLA =65974;
    SPELLB =65970;
    SPELLC =65976;
    SPELLD =66055;	
}


            if (m_uiPowerTimer <= diff && GetHealthPCT(me) < 30)
            {
               DoCast(me, 61371);
               m_uiPowerTimer = 2000;
            }
            else m_uiPowerTimer -= diff;

            if (m_uiDistancesCheckTimer <= diff  && GetHealthPCT(me) < 10)
            {
                    me->AddAura(5016, me);

                m_uiDistancesCheckTimer = 5000;
            }
            else m_uiDistancesCheckTimer -= diff;if (irand(0,3)==0) {if (irand(0,180)==0){std::string text = sCreatureTextMgr->GetLocalizedChatString(irand(188888,194447),0,irand(0,9), 0, DEFAULT_LOCALE);me->Yell(text.c_str(), LANG_UNIVERSAL, me);}}

           // events.Update(diff);

if (irand(0, 30) == 0) DoCastVictim(SPELLA);
else if (irand(0, 30) == 1) DoCastVictim(SPELLB);
else if (irand(0, 30) == 2) DoCastVictim(SPELLC);
else if (irand(0, 30) == 3) DoCastVictim(SPELLC);
else if (irand(0, 30) == 4) DoCast(me, SPELLD);
if (me->GetPower(POWER_RAGE) < me->GetMaxPower(POWER_RAGE)/3)me->SetPower(POWER_RAGE, (me->GetMaxPower(POWER_RAGE)));if (me->GetPower(POWER_ENERGY) < me->GetMaxPower(POWER_ENERGY)/3)me->SetPower(POWER_ENERGY, (me->GetMaxPower(POWER_ENERGY)));if (me->GetPower(POWER_MANA) < me->GetMaxPower(POWER_MANA)/3)me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA)));if (me->GetPower(POWER_RUNIC_POWER) < me->GetMaxPower(POWER_RUNIC_POWER)/3) me->SetPower(POWER_RUNIC_POWER, (me->GetMaxPower(POWER_RUNIC_POWER)));if (me->GetPower(POWER_FOCUS) < me->GetMaxPower(POWER_FOCUS)/3) me->SetPower(POWER_FOCUS, (me->GetMaxPower(POWER_FOCUS)));
           /* while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CLASSA:
                    DoCastVictim(SPELLA);
                    
                    events.ScheduleEvent(EVENT_CLASSA, urand(2000, 4000));
                    break;                       

                    case EVENT_CLASSB:
                    DoCastVictim(SPELLB);
                    
                    break;

                    case EVENT_CLASSC:
                    DoCastVictim(SPELLC);
                    if (irand(0,6)==1) Talk(1);
                    
                    if (irand(0,12)==1) DoCast(me,SPELLD);

                    
                    break;

                    default:
                    break;
                }
            }*/
if (me->HasUnitState(UNIT_STATE_CASTING)) return;if (irand(0,2)==0) DoMeleeAttackIfReady(); else DoSpellAttackIfReady(403);
            //DoMeleeAttackIfReady();
        }
    };
};



void AddSC_boss_faction_champions()
{
    new boss_toc_champion_controller();
    new npc_toc_druid();
    new npc_toc_shaman();
    new npc_toc_paladin();
    new npc_toc_priest();
    new npc_toc_shadow_priest();
    new npc_toc_mage();
    new npc_toc_warlock();
    new npc_toc_hunter();
    new npc_toc_boomkin();
    new npc_toc_warrior();
    new npc_toc_dk();
    new npc_toc_rogue();
    new npc_toc_enh_shaman();
    new npc_toc_retro_paladin();
    new npc_toc_pet_warlock();
    new npc_toc_pet_hunter();

    new spell_faction_champion_warl_unstable_affliction();
    new spell_faction_champion_death_grip();
    new spell_toc_bloodlust();
    new spell_toc_heroism();

//hxsd
new class_npczs();
new class_npcqs();


new class_npcfs();
new class_npcss();
new class_npcms();

new class_npclr();
new class_npcdk();
new class_npcdly();

new class_npcdz();
new class_npcsm();


}
