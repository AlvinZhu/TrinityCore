/*
 * Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
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

/// @todo Harpoon chain from 62505 should not get removed when other chain is applied

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SpellScript.h"
#include "ulduar.h"
#include "SpellInfo.h"
#include "Player.h"
#include <limits>
enum Yells
{
    // Thorim
    SAY_AGGRO_1     = 0,
    SAY_AGGRO_2     = 1,
    SAY_SPECIAL     = 2,
    SAY_JUMPDOWN    = 3,
    SAY_SLAY        = 4,
    SAY_BERSERK     = 5,
    SAY_WIPE        = 6,
    SAY_DEATH       = 7,
    SAY_END_NORMAL  = 8,
    SAY_END_HARD    = 9,

    // Runic Colossus
    EMOTE_BARRIER   = 0,
};
enum Says
{
    // Expedition Commander
    SAY_INTRO                                    = 0,
    SAY_GROUND_PHASE                             = 1,
    SAY_AGGRO_2a                                  = 2,

    // Expedition Engineer
    SAY_AGGRO_1a                                  = 0,
    SAY_AGGRO_3                                  = 1,
    SAY_TURRETS                                  = 2, // unused

    // Razorscale Controller
    EMOTE_HARPOON                                = 0,

    // Razorscale
    EMOTE_PERMA                                  = 0,
    EMOTE_BREATH                                 = 1
};

enum Spells
{
	
	    // Thorim Spells
    SPELL_SHEAT_OF_LIGHTNING    = 62276,
    SPELL_STORMHAMMER           = 62042,
    SPELL_DEAFENING_THUNDER     = 62470,
    SPELL_CHARGE_ORB            = 62016,
    SPELL_SUMMON_LIGHTNING_ORB  = 62391,
    SPELL_TOUCH_OF_DOMINION     = 62565,
    SPELL_CHAIN_LIGHTNING       = 62131,
    SPELL_LIGHTNING_CHARGE      = 62279,
    SPELL_LIGHTNING_DESTRUCTION = 62393,
    SPELL_LIGHTNING_RELEASE     = 62466,
    SPELL_LIGHTNING_PILLAR      = 62976,
    SPELL_UNBALANCING_STRIKE    = 62130,
    SPELL_BERSERK_PHASE_1       = 62560,
    SPELL_LIGHTING_BOLT_DUMMY   = 64098,
    SPELL_BERSERK_PHASE_2       = 26662,
    SPELL_SIF_TRANSFORM         = 64778,

    SPELL_AURA_OF_CELERITY      = 62320,
    SPELL_AURA_OF_CELERITY_VIS  = 62398,
    SPELL_CHARGE                = 32323,

    SPELL_THORIM_SIFFED_CREDIT  = 64980,
    SPELL_THORIM_KILL_CREDIT    = 64985,

    // Runic Colossus (Mini Boss) Spells
    SPELL_SMASH                 = 62339,
    SPELL_RUNIC_BARRIER         = 62338,
    SPELL_RUNIC_CHARGE          = 62613,
    SPELL_RUNIC_SMASH           = 62465,
    SPELL_RUNIC_SMASH_LEFT      = 62057,
    SPELL_RUNIC_SMASH_RIGHT     = 62058,

    // Ancient Rune Giant (Mini Boss) Spells
    SPELL_RUNIC_FORTIFICATION   = 62942,
    SPELL_RUNE_DETONATION       = 62526,
    SPELL_STOMP                 = 62411,

    // Sif Spells
    SPELL_FROSTBOLT_VOLLEY      = 62580,
    SPELL_FROSTNOVA             = 62597,
    SPELL_BLIZZARD              = 62576,
    SPELL_FROSTBOLT             = 69274,
	
    SPELL_FLAMEBUFFET                            = 64016,
    SPELL_FIREBALL                               = 62796,
    SPELL_FLAME_GROUND                           = 64734,
    SPELL_WINGBUFFET                             = 62666,
    SPELL_FLAMEBREATH                            = 63317,
    SPELL_FUSEARMOR                              = 64771,
    SPELL_FLAMED                                 = 62696,
    SPELL_STUN                                   = 9032,
    SPELL_BERSERK                                = 47008,
    // Additonal Spells
    // Devouring Flame Spells
    SPELL_DEVOURING_FLAME                        = 63308,
    SPELL_DEVOURING_FLAME_DAMAGE                 = 64704,
    SPELL_DEVOURING_FLAME_TRIGGER                = 64709,
    // HarpoonSpells
    SPELL_HARPOON_TRIGGER                        = 62505,
    SPELL_HARPOON_SHOT_1                         = 63658,
    SPELL_HARPOON_SHOT_2                         = 63657,
    SPELL_HARPOON_SHOT_3                         = 63659,
    SPELL_HARPOON_SHOT_4                         = 63524,
    // MoleMachine Spells
    SPELL_SUMMON_MOLE_MACHINE                    = 62899,
    SPELL_SUMMON_IRON_DWARVES                    = 63116,
    SPELL_SUMMON_IRON_DWARVES_2                  = 63114,
    SPELL_SUMMON_IRON_DWARVE_GUARDIAN            = 62926,
    SPELL_SUMMON_IRON_DWARVE_WATCHER             = 63135,
};

enum NPC
{
    NPC_DARK_RUNE_GUARDIAN                       = 33388,
    NPC_DARK_RUNE_SENTINEL                       = 33846,
    NPC_DARK_RUNE_WATCHER                        = 33453,
    MOLE_MACHINE_TRIGGER                         = 33245,
    NPC_COMMANDER                                = 33210,
    NPC_ENGINEER                                 = 33287,
    NPC_DEFENDER                                 = 33816,
};

enum DarkRuneSpells
{
    // Dark Rune Watcher
    SPELL_CHAIN_LIGHTNINGa                        = 64758,
    SPELL_LIGHTNING_BOLT                         = 63809,
    // Dark Rune Guardian
    SPELL_STORMSTRIKE                            = 64757,
    // Dark Rune Sentinel
    SPELL_BATTLE_SHOUT                           = 46763,
    SPELL_HEROIC_STRIKE                          = 45026,
    SPELL_WHIRLWIND                              = 63807,
};

enum Actions
{
    ACTION_EVENT_START                           = 1,
    ACTION_GROUND_PHASE                          = 2,
    ACTION_HARPOON_BUILD                         = 3,
    ACTION_PLACE_BROKEN_HARPOON                  = 4,
    ACTION_COMMANDER_RESET                       = 7,
    ACTION_EVENT_STARTB                      = 8,

    ACTION_PREPHASE_ADDS_DIED       = 1,
    ACTION_DOSCHEDULE_RUNIC_SMASH   = 2,
    ACTION_BERSERK                  = 3,
    ACTION_UPDATE_PHASE             = 4,
    ACTION_ENCOUNTER_COMPLETE       = 5,

    MAX_HARD_MODE_TIME              = 3*MINUTE*IN_MILLISECONDS

};

enum Phases
{
  // Thorim
    PHASE_IDLE              = 0,
    PHASE_PRE_ARENA_ADDS    = 1,
    PHASE_ARENA_ADDS        = 2,
    PHASE_ARENA             = 3,

    // Runic Colossus
    PHASE_COLOSSUS_IDLE     = 1,
    PHASE_RUNIC_SMASH       = 2,
    PHASE_MELEE             = 3,
    PHASE_PERMAGROUND                            = 1,
    PHASE_GROUND                                 = 2,
    PHASE_FLIGHT                                 = 3,
};

enum Events
{
   // Thorim Events
    EVENT_SAY_AGGRO_2               = 1,
    EVENT_STORMHAMMER               = 2,
    EVENT_CHARGE_ORB                = 3,
    EVENT_SUMMON_WARBRINGER         = 4,
    EVENT_SUMMON_EVOKER             = 5,
    EVENT_SUMMON_COMMONER           = 6,
    EVENT_BERSERK_PHASE_1           = 7,
    EVENT_BERSERK_PHASE_2           = 8,
    EVENT_UNBALANCING_STRIKE        = 9,
    EVENT_CHAIN_LIGHTNING           = 10,
    EVENT_TRANSFER_ENERGY           = 11,
    EVENT_RELEASE_LIGHTNING_CHARGE  = 12,
    EVENT_LIGHTING_BOLT_TRIGGER     = 13,

    // Thorim controller Events
    EVENT_CHECK_PLAYER_IN_RANGE     = 1,
    EVENT_CHECK_WIPE                = 2,

    // Sif Events
    EVENT_FROSTBOLT                 = 1,
    EVENT_FROSTBOLT_VOLLEY          = 2,
    EVENT_BLIZZARD                  = 3,
    EVENT_FROST_NOVA                = 4,
    EVENT_TELEPORT                  = 5,

    // Runic Colossus
    EVENT_BARRIER                   = 1,
    EVENT_SMASH                     = 2,
    EVENT_SMASH_WAVE                = 3,
    EVENT_COLOSSUS_CHARGE           = 4,
    EVENT_RUNIC_SMASH               = 5,

    // Ancient Rune Giant
    EVENT_STOMP                     = 1,
    EVENT_DETONATION                = 2,	
	
    EVENT_BERSERK                                = 1,
    EVENT_BREATH                                 = 2,
    EVENT_BUFFET                                 = 3,
    EVENT_FIREBALL                               = 5,
    EVENT_FLIGHT                                 = 6,
    EVENT_DEVOURING                              = 7,
    EVENT_FLAME                                  = 8,
    EVENT_LAND                                   = 9,
    EVENT_GROUND                                 = 10,
    EVENT_FUSE                                   = 11,
    EVENT_SUMMON                                 = 12,
    // Razorscale Controller
    EVENT_BUILD_HARPOON_1                        = 13,
    EVENT_BUILD_HARPOON_2                        = 14,
    EVENT_BUILD_HARPOON_3                        = 15,
    EVENT_BUILD_HARPOON_4                        = 16,
};

#define GROUND_Z                                 391.517f
#define GOSSIP_ITEM_1                            "Activate Harpoons!"

enum Misc
{
    DATA_QUICK_SHAVE                             = 29192921, // 2919, 2921 are achievement IDs
    DATA_IRON_DWARF_MEDIUM_RARE                  = 29232924
};

const Position PosEngRepair[4] =
{
    { 590.442f, -130.550f, GROUND_Z, 4.789f },
    { 574.850f, -133.687f, GROUND_Z, 4.252f },
    { 606.567f, -143.369f, GROUND_Z, 4.434f },
    { 560.609f, -142.967f, GROUND_Z, 5.074f },
};

const Position PosDefSpawn[4] =
{
    { 600.75f, -104.850f, GROUND_Z, 0 },
    { 596.38f, -110.262f, GROUND_Z, 0 },
    { 566.47f, -103.633f, GROUND_Z, 0 },
    { 570.41f, -108.791f, GROUND_Z, 0 },
};

const Position PosDefCombat[4] =
{
    { 614.975f, -155.138f, GROUND_Z, 4.154f },
    { 609.814f, -204.968f, GROUND_Z, 5.385f },
    { 563.531f, -201.557f, GROUND_Z, 4.108f },
    { 560.231f, -153.677f, GROUND_Z, 5.403f },
};

const Position PosHarpoon[4] =
{
    { 571.901f, -136.554f, GROUND_Z, 0 },
    { 589.450f, -134.888f, GROUND_Z, 0 },
    { 559.119f, -140.505f, GROUND_Z, 0 },
    { 606.229f, -136.721f, GROUND_Z, 0 },
};

const Position RazorFlight = { 588.050f, -251.191f, 470.536f, 1.498f };
const Position RazorGround = { 586.966f, -175.534f, GROUND_Z, 4.682f };
const Position PosEngSpawn = { 591.951f, -95.9680f, GROUND_Z, 0.000f };

enum ThorimData
{
    DATA_LOSE_ILLUSION                  = 1,
    DATA_DO_NOT_STAND_IN_THE_LIGHTING   = 2,
};


const Position Pos[7] =
{
    {2095.53f, -279.48f, 419.84f, 0.504f},
    {2092.93f, -252.96f, 419.84f, 6.024f},
    {2097.86f, -240.97f, 419.84f, 5.643f},
    {2113.14f, -225.94f, 419.84f, 5.259f},
    {2156.87f, -226.12f, 419.84f, 4.202f},
    {2172.42f, -242.70f, 419.84f, 3.583f},
    {2171.92f, -284.59f, 419.84f, 2.691f}
};

const Position PosOrbs[7] =
{
    {2104.99f, -233.484f, 433.576f, 5.49779f},
    {2092.64f, -262.594f, 433.576f, 6.26573f},
    {2104.76f, -292.719f, 433.576f, 0.78539f},
    {2164.97f, -293.375f, 433.576f, 2.35619f},
    {2164.58f, -233.333f, 433.576f, 3.90954f},
    {2145.81f, -222.196f, 433.576f, 4.45059f},
    {2123.91f, -222.443f, 433.576f, 4.97419f}
};

const Position PosCharge[7] =
{
    {2108.95f, -289.241f, 420.149f, 5.49779f},
    {2097.93f, -262.782f, 420.149f, 6.26573f},
    {2108.66f, -237.102f, 420.149f, 0.78539f},
    {2160.56f, -289.292f, 420.149f, 2.35619f},
    {2161.02f, -237.258f, 420.149f, 3.90954f},
    {2143.87f, -227.415f, 420.149f, 4.45059f},
    {2125.84f, -227.439f, 420.149f, 4.97419f}
};

#define POS_X_ARENA  2181.19f
#define POS_Y_ARENA  -299.12f

struct SummonLocation
{
    Position pos;
    uint32 entry;
};


class HealerCheck
{
    public:
        HealerCheck(bool shouldBe): __shouldBe(shouldBe) {}
        bool operator() (const Unit* unit)
        {
            return __shouldBe ? __IsHealer(unit) : !__IsHealer(unit);
        }

    private:
        bool __shouldBe;
        bool __IsHealer(const Unit* who)
        {
           
        }
};

class ArenaAreaCheck
{
    public:
        ArenaAreaCheck(bool shouldBeIn): __shouldBeIn(shouldBeIn) {}
        bool operator() (const WorldObject* unit)
        {
            return __shouldBeIn ? __IsInArena(unit) : !__IsInArena(unit);
        }

    private:
        bool __shouldBeIn;
        bool __IsInArena(const WorldObject* who)
        {
            return (who->GetPositionX() < POS_X_ARENA && who->GetPositionY() > POS_Y_ARENA);    // TODO: Check if this is ok, end positions ?
        }
};

class boss_razorscale_controller : public CreatureScript
{
    public:
        boss_razorscale_controller() : CreatureScript("boss_razorscale_controller") { }

        struct boss_razorscale_controllerAI : public BossAI
        {
            boss_razorscale_controllerAI(Creature* creature) : BossAI(creature, DATA_RAZORSCALE_CONTROL)
            {
                me->SetDisplayId(me->GetCreatureTemplate()->Modelid2);
            }

            void Reset() override
            {
                _Reset();
                me->SetReactState(REACT_PASSIVE);
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_FLAMED:
                        if (GameObject* Harpoon1 = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_RAZOR_HARPOON_1)))
                            Harpoon1->RemoveFromWorld();
                        if (GameObject* Harpoon2 = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_RAZOR_HARPOON_2)))
                            Harpoon2->RemoveFromWorld();
                        if (GameObject* Harpoon3 = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_RAZOR_HARPOON_3)))
                            Harpoon3->RemoveFromWorld();
                        if (GameObject* Harpoon4 = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_RAZOR_HARPOON_4)))
                            Harpoon4->RemoveFromWorld();
                        DoAction(ACTION_HARPOON_BUILD);
                        DoAction(ACTION_PLACE_BROKEN_HARPOON);
                        break;
                    case SPELL_HARPOON_SHOT_1:
                    case SPELL_HARPOON_SHOT_2:
                    case SPELL_HARPOON_SHOT_3:
                    case SPELL_HARPOON_SHOT_4:
                        DoCast(SPELL_HARPOON_TRIGGER);
                        break;
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
            }

            void DoAction(int32 action) override
            {
                if (instance->GetBossState(BOSS_RAZORSCALE) != IN_PROGRESS)
                    return;

                switch (action)
                {
                    case ACTION_HARPOON_BUILD:
                        //events.ScheduleEvent(EVENT_BUILD_HARPOON_1, 20000);
                        events.ScheduleEvent(EVENT_BUILD_HARPOON_1, 20000);
                        //me->GetMotionMaster()->MoveTakeoff(0, RazorFlight);
                        if (me->GetMap()->GetSpawnMode() == RAID_DIFFICULTY_25MAN_NORMAL)
                            events.ScheduleEvent(EVENT_BUILD_HARPOON_3, 50000);
                        break;
                    case ACTION_PLACE_BROKEN_HARPOON:
                        for (uint8 n = 0; n < RAID_MODE(2, 4); n++)
                            me->SummonGameObject(GO_RAZOR_BROKEN_HARPOON, PosHarpoon[n].GetPositionX(), PosHarpoon[n].GetPositionY(), PosHarpoon[n].GetPositionZ(), 2.286f, G3D::Quat(), 180);
                        break;
                }
            }

            void UpdateAI(uint32 Diff) override
            {
                events.Update(Diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BUILD_HARPOON_1:
                            Talk(EMOTE_HARPOON);
                            if (GameObject* Harpoon = me->SummonGameObject(GO_RAZOR_HARPOON_1, PosHarpoon[0].GetPositionX(), PosHarpoon[0].GetPositionY(), PosHarpoon[0].GetPositionZ(), 4.790f, G3D::Quat(), uint32(me->GetRespawnTime())))
                            {Harpoon->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
								BotUseGOTarget(Harpoon);
								if (GameObject* BrokenHarpoon = Harpoon->FindNearestGameObject(GO_RAZOR_BROKEN_HARPOON, 5.0f)) //only nearest broken harpoon
                                    BrokenHarpoon->RemoveFromWorld();
                                events.ScheduleEvent(EVENT_BUILD_HARPOON_2, 18000);
                                events.CancelEvent(EVENT_BUILD_HARPOON_1);
                            }
                            return;
                        case EVENT_BUILD_HARPOON_2:
                            Talk(EMOTE_HARPOON);
                            if (GameObject* Harpoon = me->SummonGameObject(GO_RAZOR_HARPOON_2, PosHarpoon[1].GetPositionX(), PosHarpoon[1].GetPositionY(), PosHarpoon[1].GetPositionZ(), 4.659f, G3D::Quat(), uint32(me->GetRespawnTime())))
                            {
                            	Harpoon->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE | GO_FLAG_NODESPAWN | GO_FLAG_INTERACT_COND);
								BotUseGOTarget(Harpoon);
								if (GameObject* BrokenHarpoon = Harpoon->FindNearestGameObject(GO_RAZOR_BROKEN_HARPOON, 5.0f))
                                    BrokenHarpoon->RemoveFromWorld();
                                events.CancelEvent(EVENT_BUILD_HARPOON_2);
                            }
                            return;
                        case EVENT_BUILD_HARPOON_3:
                            Talk(EMOTE_HARPOON);
                            if (GameObject* Harpoon = me->SummonGameObject(GO_RAZOR_HARPOON_3, PosHarpoon[2].GetPositionX(), PosHarpoon[2].GetPositionY(), PosHarpoon[2].GetPositionZ(), 5.382f, G3D::Quat(), uint32(me->GetRespawnTime())))
                            {
                            	Harpoon->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE | GO_FLAG_NODESPAWN | GO_FLAG_INTERACT_COND);
								BotUseGOTarget(Harpoon);
								if (GameObject* BrokenHarpoon = Harpoon->FindNearestGameObject(GO_RAZOR_BROKEN_HARPOON, 5.0f))
                                    BrokenHarpoon->RemoveFromWorld();
                                events.ScheduleEvent(EVENT_BUILD_HARPOON_4, 18000);
                                events.CancelEvent(EVENT_BUILD_HARPOON_3);
                            }
                            return;
                        case EVENT_BUILD_HARPOON_4:
                            Talk(EMOTE_HARPOON);
                            if (GameObject* Harpoon = me->SummonGameObject(GO_RAZOR_HARPOON_4, PosHarpoon[3].GetPositionX(), PosHarpoon[3].GetPositionY(), PosHarpoon[3].GetPositionZ(), 4.266f, G3D::Quat(), uint32(me->GetRespawnTime())))
                            {
                            	Harpoon->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE | GO_FLAG_NODESPAWN | GO_FLAG_INTERACT_COND);
								BotUseGOTarget(Harpoon);
								if (GameObject* BrokenHarpoon = Harpoon->FindNearestGameObject(GO_RAZOR_BROKEN_HARPOON, 5.0f))
                                    BrokenHarpoon->RemoveFromWorld();


                            if (Creature* Razorscale = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_RAZORSCALE)))
                            {
                                Razorscale->AI()->DoAction(ACTION_EVENT_START);
                            }

                               
                                events.CancelEvent(EVENT_BUILD_HARPOON_4);
                            }
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_razorscale_controllerAI>(creature);
        }
};


class go_razorscale_harpoon : public GameObjectScript
{
    public:
        go_razorscale_harpoon() : GameObjectScript("go_razorscale_harpoon") { }

        bool OnGossipHello(Player* /*player*/, GameObject* go) override
        {
           /* InstanceScript* instance = go->GetInstanceScript();
            if (ObjectAccessor::GetCreature(*go, instance->GetGuidData(BOSS_RAZORSCALE)))
                go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                                   go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);*/
            return true;
        }
};

class boss_razorscale : public CreatureScript
{
    public:
        boss_razorscale() : CreatureScript("boss_razorscale") { }

        struct boss_razorscaleAI : public BossAI
        {
            boss_razorscaleAI(Creature* creature) : BossAI(creature, BOSS_RAZORSCALE)
            {
                Initialize();
                // Do not let Razorscale be affected by Battle Shout buff
                me->ApplySpellImmune(0, IMMUNITY_ID, (SPELL_BATTLE_SHOUT), true);
                FlyCount = 0;
                EnrageTimer = 0;
                Enraged = false;
                phase = PHASE_GROUND;
            }

            void Initialize()
            {
                PermaGround = false;
                HarpoonCounter = 0;
            }

            Phases phase;

            uint32 EnrageTimer;
            uint8 FlyCount;
            uint8 HarpoonCounter;
            bool PermaGround;
            bool Enraged;

            void Reset() override
            {
                _Reset();
                me->SetCanFly(true);
                //me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
                Initialize();
                if (Creature* commander = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_EXPEDITION_COMMANDER)))
                    commander->AI()->DoAction(ACTION_COMMANDER_RESET);
            }

            void EnterCombat(Unit* /*who*/) override
            {
                _EnterCombat();
                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_RAZORSCALE_CONTROL)))
                    controller->AI()->DoAction(ACTION_HARPOON_BUILD);
                me->SetSpeedRate(MOVE_FLIGHT, 3.0f);
                me->SetReactState(REACT_PASSIVE);
                phase = PHASE_GROUND;
                events.SetPhase(PHASE_GROUND);
                FlyCount = 0;
                EnrageTimer = 600000;
                Enraged = false;
                events.ScheduleEvent(EVENT_FLIGHT, 0, 0, PHASE_GROUND);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_RAZORSCALE_CONTROL)))
                    controller->AI()->Reset();
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_HARPOON_TRIGGER)
                    ++HarpoonCounter;
            }

            void MovementInform(uint32 type, uint32 id) override
            {
                if (type == EFFECT_MOTION_TYPE && id == 1)
                {
                    phase = PHASE_GROUND;
                    events.SetPhase(PHASE_GROUND);
                    events.ScheduleEvent(EVENT_LAND, 0, 0, PHASE_GROUND);
                }
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_QUICK_SHAVE)
                    if (FlyCount <= 2)
                        return 1;

                return 0;
            }

            void UpdateAI(uint32 Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(Diff);

                if (HealthBelowPct(70) && !PermaGround)
                    EnterPermaGround();

                if (EnrageTimer <= Diff && !Enraged)
                {
                    DoCast(me, SPELL_BERSERK);
                    Enraged = true;
                }
                else
                    EnrageTimer -= Diff;

                if (HarpoonCounter == RAID_MODE(2, 4))
                {
                    HarpoonCounter = 0;
                    me->GetMotionMaster()->MoveLand(1, RazorGround);
                }

                if (phase == PHASE_GROUND)
                {
                    while (uint32 eventId = events.ExecuteEvent())
                    {
                        switch (eventId)
                        {
                            case EVENT_FLIGHT:
                                phase = PHASE_FLIGHT;
                                events.SetPhase(PHASE_FLIGHT);
                                me->SetCanFly(true);
                                //me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                me->SetReactState(REACT_PASSIVE);
                                me->AttackStop();
                                me->GetMotionMaster()->MoveTakeoff(0, RazorFlight);
                                events.ScheduleEvent(EVENT_FIREBALL, 7000, 0, PHASE_FLIGHT);
                                events.ScheduleEvent(EVENT_DEVOURING, 10000, 0, PHASE_FLIGHT);
                                events.ScheduleEvent(EVENT_SUMMON, 5000, 0, PHASE_FLIGHT);
                                ++FlyCount;
                                return;
                            case EVENT_LAND:
                                me->SetCanFly(false);
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                //me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED | UNIT_FLAG_PACIFIED);
                                if (Creature* commander = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_EXPEDITION_COMMANDER)))
                                    commander->AI()->DoAction(ACTION_GROUND_PHASE);
                                events.ScheduleEvent(EVENT_BREATH, 30000, 0, PHASE_GROUND);
                                events.ScheduleEvent(EVENT_BUFFET, 33000, 0, PHASE_GROUND);
                                events.ScheduleEvent(EVENT_FLIGHT, 35000, 0, PHASE_GROUND);
                                return;
                            case EVENT_BREATH:
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED | UNIT_FLAG_PACIFIED);
                                me->RemoveAllAuras();
                                me->SetReactState(REACT_AGGRESSIVE);
                                Talk(EMOTE_BREATH);
                                DoCastAOE(SPELL_FLAMEBREATH);
                                events.CancelEvent(EVENT_BREATH);
                                return;
                            case EVENT_BUFFET:
                                DoCastAOE(SPELL_WINGBUFFET);
                                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_RAZORSCALE_CONTROL)))
                                    controller->CastSpell(controller, SPELL_FLAMED, true);
                                events.CancelEvent(EVENT_BUFFET);
                                return;
                        }
                    }
                }
                if (phase == PHASE_PERMAGROUND)
                {
                    while (uint32 eventId = events.ExecuteEvent())
                    {
                        switch (eventId)
                        {
                            case EVENT_FLAME:
                                DoCastAOE(SPELL_FLAMEBUFFET);
                                events.ScheduleEvent(EVENT_FLAME, 10000, 0, PHASE_PERMAGROUND);
                                return;
                            case EVENT_BREATH:
                                Talk(EMOTE_BREATH);
                                DoCastVictim(SPELL_FLAMEBREATH);
                                events.ScheduleEvent(EVENT_BREATH, 20000, 0, PHASE_PERMAGROUND);
                                return;
                            case EVENT_FIREBALL:
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                    DoCast(target, SPELL_FIREBALL);
                                events.ScheduleEvent(EVENT_FIREBALL, 3000, 0, PHASE_PERMAGROUND);
                                return;
                            case EVENT_DEVOURING:
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                    DoCast(target, SPELL_DEVOURING_FLAME);
                                events.ScheduleEvent(EVENT_DEVOURING, 10000, 0, PHASE_PERMAGROUND);
                                return;
                            case EVENT_BUFFET:
                                DoCastAOE(SPELL_WINGBUFFET);
                                events.CancelEvent(EVENT_BUFFET);
                                return;
                            case EVENT_FUSE:
                                DoCastVictim(SPELL_FUSEARMOR);
                                events.ScheduleEvent(EVENT_FUSE, 10000, 0, PHASE_PERMAGROUND);
                                return;
                        }
                    }

                    DoMeleeAttackIfReady();
                }
                else
                {
                    if (uint32 eventId = events.ExecuteEvent())
                    {
                        switch (eventId)
                        {
                            case EVENT_FIREBALL:
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                    DoCast(target, SPELL_FIREBALL);
                                events.ScheduleEvent(EVENT_FIREBALL, 3000, 0, PHASE_FLIGHT);
                                return;
                            case EVENT_DEVOURING:
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                    me->CastSpell(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), SPELL_DEVOURING_FLAME, true);
                                events.ScheduleEvent(EVENT_DEVOURING, 10000, 0, PHASE_FLIGHT);
                                return;
                            case EVENT_SUMMON:
                                SummonMoleMachines();
                                events.ScheduleEvent(EVENT_SUMMON, 45000, 0, PHASE_FLIGHT);
                                return;
                        }
                    }
                }
            }

            void EnterPermaGround()
            {
                Talk(EMOTE_PERMA);
                phase = PHASE_PERMAGROUND;
                events.SetPhase(PHASE_PERMAGROUND);
                me->SetCanFly(false);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_STUNNED | UNIT_FLAG_PACIFIED);
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveAurasDueToSpell(SPELL_HARPOON_TRIGGER);
                me->SetSpeedRate(MOVE_FLIGHT, 1.0f);
                PermaGround = true;
                DoCastAOE(SPELL_FLAMEBREATH);
                events.ScheduleEvent(EVENT_FLAME, 15000, 0, PHASE_PERMAGROUND);
                events.RescheduleEvent(EVENT_DEVOURING, 15000, 0, PHASE_PERMAGROUND);
                events.RescheduleEvent(EVENT_BREATH, 20000, 0, PHASE_PERMAGROUND);
                events.RescheduleEvent(EVENT_FIREBALL, 3000, 0, PHASE_PERMAGROUND);
                events.RescheduleEvent(EVENT_DEVOURING, 6000, 0, PHASE_PERMAGROUND);
                events.RescheduleEvent(EVENT_BUFFET, 2500, 0, PHASE_PERMAGROUND);
                events.RescheduleEvent(EVENT_FUSE, 5000, 0, PHASE_PERMAGROUND);
            }

            void SummonMoleMachines()
            {
                // Adds will come in waves from mole machines. One mole can spawn a Dark Rune Watcher
                // with 1-2 Guardians, or a lone Sentinel. Up to 4 mole machines can spawn adds at any given time.
                uint8 random = urand(2, 4);
                for (uint8 n = 0; n < random; n++)
                {
                    float x = float(irand(540, 640));       // Safe range is between 500 and 650
                    float y = float(irand(-230, -195));     // Safe range is between -235 and -145
                    float z = GROUND_Z;                     // Ground level
                    me->SummonCreature(MOLE_MACHINE_TRIGGER, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 15000);
                }
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_EVENT_START:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoZoneInCombat(me, 150.0f);
                        break;

                    case ACTION_EVENT_STARTB:
                        
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoZoneInCombat(me, 150.0f);
                        break;



                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<boss_razorscaleAI>(creature);
        }
};

class npc_expedition_commander : public CreatureScript
{
    public:
        npc_expedition_commander() : CreatureScript("npc_expedition_commander") { }

        struct npc_expedition_commanderAI : public ScriptedAI
        {
            npc_expedition_commanderAI(Creature* creature) : ScriptedAI(creature), summons(creature)
            {
                Initialize();
                instance = me->GetInstanceScript();
            }

            void Initialize()
            {
                AttackStartTimer = 0;
                Phase = 0;
                Greet = false;
            }

            InstanceScript* instance;
            SummonList summons;

            bool Greet;
            uint32 AttackStartTimer;
            uint8  Phase;
            ObjectGuid Engineer[4];
            ObjectGuid Defender[4];

            void Reset() override
            {
                Initialize();
                summons.DespawnAll();
            }

            void MoveInLineOfSight(Unit* who) override

            {
                if (!Greet && me->IsWithinDistInMap(who, 10.0f) && who->GetTypeId() == TYPEID_PLAYER)
                {
                    Talk(SAY_INTRO);
                    Greet = true;
                }
            }

            void JustSummoned(Creature* summoned) override
            {
                summons.Summon(summoned);
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_GROUND_PHASE:
                        Talk(SAY_GROUND_PHASE);
                        break;
                    case ACTION_COMMANDER_RESET:
                        summons.DespawnAll();
                        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        break;
                }
            }

            void UpdateAI(uint32 Diff) override
            {
                if (AttackStartTimer <= Diff)
                {
                    switch (Phase)
                    {
                        case 1:
                            instance->SetBossState(BOSS_RAZORSCALE, IN_PROGRESS);
                            summons.DespawnAll();
                            AttackStartTimer = 1000;
                            Phase = 2;
                            break;
                        case 2:
                            for (uint8 n = 0; n < RAID_MODE(2, 4); n++)
                            {
                                if (Creature* summonedEngineer = me->SummonCreature(NPC_ENGINEER, PosEngSpawn, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000))
                                {
                                    summonedEngineer->SetWalk(false);
                                    summonedEngineer->SetSpeedRate(MOVE_RUN, 0.5f);
                                    summonedEngineer->SetHomePosition(PosEngRepair[n]);
                                    summonedEngineer->GetMotionMaster()->MoveTargetedHome();
                                    Engineer[n] = summonedEngineer->GetGUID();
                                }
                            }
                            if (Creature* firstSummon = ObjectAccessor::GetCreature(*me, Engineer[0]))
                                firstSummon->AI()->Talk(SAY_AGGRO_3);
                            Phase = 3;
                            AttackStartTimer = 14000;
                            break;
                        case 3:
                            for (uint8 n = 0; n < 4; n++)
                            {
                                if (Creature* summonedDefender = me->SummonCreature(NPC_DEFENDER, PosDefSpawn[n], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000))
                                {
                                    summonedDefender->SetWalk(false);
                                    summonedDefender->SetHomePosition(PosDefCombat[n]);
                                    summonedDefender->GetMotionMaster()->MoveTargetedHome();
                                    Defender[n] = summonedDefender->GetGUID();
                                }
                            }
                            Phase = 4;
                            break;
                        case 4:
                            for (uint8 n = 0; n < RAID_MODE(2, 4); n++)
                                if (Creature* summonedEngineer = ObjectAccessor::GetCreature(*me, Engineer[n]))
                                    summonedEngineer->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_USE_STANDING);
                            for (uint8 n = 0; n < 4; ++n)
                                if (Creature* summonedDefender = ObjectAccessor::GetCreature(*me, Defender[n]))
                                    summonedDefender->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY2H);
                            Talk(SAY_AGGRO_2a);
                            AttackStartTimer = 16000;
                            Phase = 5;
                            break;
                        case 5:
                            if (Creature* Razorscale = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_RAZORSCALE)))
                            {
                                Razorscale->AI()->DoAction(ACTION_EVENT_START);
                                me->SetInCombatWith(Razorscale);
                            }
                            if (Creature* firstEngineer = ObjectAccessor::GetCreature(*me, Engineer[0]))
                                firstEngineer->AI()->Talk(SAY_AGGRO_1a);
                            Phase = 6;
                            break;
                    }
                }
                else
                    AttackStartTimer -= Diff;
            }
        };

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF:
                    player->CLOSE_GOSSIP_MENU();
                    ENSURE_AI(npc_expedition_commanderAI, creature->AI())->Phase = 1;
                    break;
            }
            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            InstanceScript* instance = creature->GetInstanceScript();
            if (instance && instance->GetBossState(BOSS_RAZORSCALE) == NOT_STARTED)
            {
                player->PrepareGossipMenu(creature);

                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                player->SEND_GOSSIP_MENU(13853, creature->GetGUID());
            }
            else
                player->SEND_GOSSIP_MENU(13910, creature->GetGUID());

            return true;
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_expedition_commanderAI>(creature);
        }
};

class npc_mole_machine_trigger : public CreatureScript
{
    public:
        npc_mole_machine_trigger() : CreatureScript("npc_mole_machine_trigger") { }

        struct npc_mole_machine_triggerAI : public ScriptedAI
        {
            npc_mole_machine_triggerAI(Creature* creature) : ScriptedAI(creature)
            {
                Initialize();
                SetCombatMovement(false);
                //me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
            }

            void Initialize()
            {
                SummonGobTimer = 2000;
                SummonNpcTimer = 6000;
                DissapearTimer = 10000;
                GobSummoned = false;
                NpcSummoned = false;
            }

            uint32 SummonGobTimer;
            uint32 SummonNpcTimer;
            uint32 DissapearTimer;
            bool GobSummoned;
            bool NpcSummoned;

            void Reset() override
            {
                Initialize();
            }

            void UpdateAI(uint32 Diff) override
            {
                if (!GobSummoned && SummonGobTimer <= Diff)
                {
                    DoCast(SPELL_SUMMON_MOLE_MACHINE);
                    GobSummoned = true;
                }
                else
                    SummonGobTimer -= Diff;

                if (!NpcSummoned && SummonNpcTimer <= Diff)
                {
                    switch (urand(0, 1 ))
                    {
                        case 0:
                            DoCast(SPELL_SUMMON_IRON_DWARVES);
                            break;
                        case 1:
                            DoCast(SPELL_SUMMON_IRON_DWARVES_2);
                            break;
                    }

                    DoCast(SPELL_SUMMON_IRON_DWARVE_GUARDIAN);
                    DoCast(SPELL_SUMMON_IRON_DWARVE_WATCHER);
                    NpcSummoned = true;
                }
                else
                    SummonNpcTimer -= Diff;

                if (DissapearTimer <= Diff)
                {
                    if (GameObject* molemachine = me->FindNearestGameObject(GO_MOLE_MACHINE, 1))
                        molemachine->Delete();

                    me->DisappearAndDie();
                }
                else
                    DissapearTimer -= Diff;
            }

            void JustSummoned(Creature* summoned) override
            {
                summoned->AI()->DoZoneInCombat();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_mole_machine_triggerAI(creature);
        }
};

class npc_devouring_flame : public CreatureScript
{
    public:
        npc_devouring_flame() : CreatureScript("npc_devouring_flame") { }

        struct npc_devouring_flameAI : public ScriptedAI
        {
            npc_devouring_flameAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
                //me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
            }

            void Reset() override
            {
                DoCast(SPELL_FLAME_GROUND);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_devouring_flameAI(creature);
        }
};

class npc_darkrune_watcher : public CreatureScript
{
    public:
        npc_darkrune_watcher() : CreatureScript("npc_darkrune_watcher") { }

        struct npc_darkrune_watcherAI : public ScriptedAI
        {
            npc_darkrune_watcherAI(Creature* creature) : ScriptedAI(creature)
            {
                Initialize();
            }

            void Initialize()
            {
                ChainTimer = urand(10000, 15000);
                LightTimer = urand(1000, 3000);
            }

            uint32 ChainTimer;
            uint32 LightTimer;

            void Reset() override
            {
                Initialize();
            }

            void UpdateAI(uint32 Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (ChainTimer <= Diff)
                {
                    DoCastVictim(SPELL_CHAIN_LIGHTNINGa);
                    ChainTimer = urand(10000, 15000);
                }
                else
                    ChainTimer -= Diff;

                if (LightTimer <= Diff)
                {
                    DoCastVictim(SPELL_LIGHTNING_BOLT);
                    LightTimer = urand(5000, 7000);
                }
                else
                    LightTimer -= Diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_darkrune_watcherAI(creature);
        }
};

class npc_darkrune_guardian : public CreatureScript
{
    public:
        npc_darkrune_guardian() : CreatureScript("npc_darkrune_guardian") { }

        struct npc_darkrune_guardianAI : public ScriptedAI
        {
            npc_darkrune_guardianAI(Creature* creature) : ScriptedAI(creature)
            {
                Initialize();
            }

            void Initialize()
            {
                StormTimer = urand(3000, 6000);
                killedByBreath = false;
            }

            uint32 StormTimer;

            void Reset() override
            {
                Initialize();
            }

            uint32 GetData(uint32 type) const override
            {
                return type == DATA_IRON_DWARF_MEDIUM_RARE ? killedByBreath : 0;
            }

            void SetData(uint32 type, uint32 value) override
            {
                if (type == DATA_IRON_DWARF_MEDIUM_RARE)
                    killedByBreath = value != 0;
            }


            void UpdateAI(uint32 Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (StormTimer <= Diff)
                {
                    DoCastVictim(SPELL_STORMSTRIKE);
                    StormTimer = urand(4000, 8000);
                }
                else
                    StormTimer -= Diff;

                DoMeleeAttackIfReady();
            }

        private:
            bool killedByBreath;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_darkrune_guardianAI(creature);
        }
};

class npc_darkrune_sentinel : public CreatureScript
{
    public:
        npc_darkrune_sentinel() : CreatureScript("npc_darkrune_sentinel") { }

        struct npc_darkrune_sentinelAI : public ScriptedAI
        {
            npc_darkrune_sentinelAI(Creature* creature) : ScriptedAI(creature)
            {
                Initialize();
            }

            void Initialize()
            {
                HeroicTimer = urand(4000, 8000);
                WhirlTimer = urand(20000, 25000);
                ShoutTimer = urand(15000, 30000);
            }

            uint32 HeroicTimer;
            uint32 WhirlTimer;
            uint32 ShoutTimer;

            void Reset() override
            {
                Initialize();
            }

            void UpdateAI(uint32 Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (HeroicTimer <= Diff)
                {
                    DoCastVictim(SPELL_HEROIC_STRIKE);
                    HeroicTimer = urand(4000, 6000);
                }
                else
                    HeroicTimer -= Diff;

                if (WhirlTimer <= Diff)
                {
                    DoCastVictim(SPELL_WHIRLWIND);
                    WhirlTimer = urand(20000, 25000);
                }
                else
                    WhirlTimer -= Diff;

                if (ShoutTimer <= Diff)
                {
                    DoCast(me, SPELL_BATTLE_SHOUT);
                    ShoutTimer = urand(30000, 40000);
                }
                else
                    ShoutTimer -= Diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_darkrune_sentinelAI(creature);
        }
};

class spell_razorscale_devouring_flame : public SpellScriptLoader
{
    public:
        spell_razorscale_devouring_flame() : SpellScriptLoader("spell_razorscale_devouring_flame") { }

        class spell_razorscale_devouring_flame_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_razorscale_devouring_flame_SpellScript);

            void HandleSummon(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Unit* caster = GetCaster();
                uint32 entry = uint32(GetSpellInfo()->Effects[effIndex].MiscValue);
                WorldLocation const* summonLocation = GetExplTargetDest();
                if (!caster || !summonLocation)
                    return;

                caster->SummonCreature(entry, summonLocation->GetPositionX(), summonLocation->GetPositionY(), GROUND_Z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 20000);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_razorscale_devouring_flame_SpellScript::HandleSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_razorscale_devouring_flame_SpellScript();
        }
};

class spell_razorscale_flame_breath : public SpellScriptLoader
{
    public:
        spell_razorscale_flame_breath() : SpellScriptLoader("spell_razorscale_flame_breath") { }

        class spell_razorscale_flame_breath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_razorscale_flame_breath_SpellScript);

            void CheckDamage()
            {
                Creature* target = GetHitCreature();
                if (!target || target->GetEntry() != NPC_DARK_RUNE_GUARDIAN)
                    return;

                if (GetHitDamage() >= int32(target->GetHealth()))
                    target->AI()->SetData(DATA_IRON_DWARF_MEDIUM_RARE, 1);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_razorscale_flame_breath_SpellScript::CheckDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_razorscale_flame_breath_SpellScript();
        }
};

class achievement_iron_dwarf_medium_rare : public AchievementCriteriaScript
{
    public:
        achievement_iron_dwarf_medium_rare() : AchievementCriteriaScript("achievement_iron_dwarf_medium_rare")
        {
        }

        bool OnCheck(Player* /*player*/, Unit* target) override
        {
            return target && target->IsAIEnabled && target->GetAI()->GetData(DATA_IRON_DWARF_MEDIUM_RARE);
        }
};

class achievement_quick_shave : public AchievementCriteriaScript
{
    public:
        achievement_quick_shave() : AchievementCriteriaScript("achievement_quick_shave") { }

        bool OnCheck(Player* /*source*/, Unit* target) override
        {
            if (target)
                if (Creature* razorscale = target->ToCreature())
                    if (razorscale->AI()->GetData(DATA_QUICK_SHAVE))
                        return true;

            return false;
        }
};

class boss_thorim1 : public CreatureScript
{
    public:
        boss_thorim1() : CreatureScript("boss_thorim1") { }

        struct boss_thorimAI : public BossAI
        {
            boss_thorimAI(Creature* creature) : BossAI(creature, BOSS_THORIM)
            {
                gotAddsWiped = false;
                gotEncounterFinished = false;
                homePosition = creature->GetHomePosition();
            }

            void Reset()
            {
                _Reset();

                if (gotAddsWiped)
                    Talk(SAY_WIPE);

                me->SetReactState(REACT_PASSIVE);
                me->RemoveAurasDueToSpell(SPELL_BERSERK_PHASE_1);

//                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NON_ATTACKABLE);

                phase = PHASE_IDLE;
                gotAddsWiped = false;
                HardMode = false;
                gotBerserkedAndOrbSummoned = false;
                summonChampion = false;
                doNotStandInTheLighting = true;
                checkTargetTimer = 7*IN_MILLISECONDS;

            }

            void KilledUnit(Unit* who)
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_SLAY);
            }

            void EncounterPostProgress()
            {
                gotEncounterFinished = true;
                Talk(SAY_DEATH);
                me->setFaction(35);
                me->DespawnOrUnsummon(12 * IN_MILLISECONDS);
                me->RemoveAllAuras();
                me->RemoveAllAttackers();
                me->AttackStop();
                me->CombatStop(true);


                // Kill credit
                instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_THORIM_KILL_CREDIT);

                if (HardMode)
                {
                    Talk(SAY_END_HARD);
                    me->SummonGameObject(RAID_MODE(GO_CACHE_OF_STORMS_HARDMODE_10, GO_CACHE_OF_STORMS_HARDMODE_25), 2134.58f, -286.908f, 419.495f, 1.55988f,   G3D::Quat(), WEEK);
                }
                else
                {
                    if (GameObject* go = me->SummonGameObject(RAID_MODE(GO_CACHE_OF_STORMS_10, GO_CACHE_OF_STORMS_25), 2134.58f, -286.908f, 419.495f, 1.55988f,   G3D::Quat(), WEEK))
                        go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                }


                _JustDied();
            }

            void EnterCombat(Unit* who)
            {
                //_EnterCombat();
                Talk(SAY_AGGRO_1);

                // Spawn Thunder Orbs

                EncounterTime = 0;
                phase = PHASE_ARENA_ADDS;
                events.SetPhase(phase);
                DoCast(me, SPELL_SHEAT_OF_LIGHTNING);
                events.ScheduleEvent(EVENT_STORMHAMMER, 40*IN_MILLISECONDS, 0, phase);
                events.ScheduleEvent(EVENT_CHARGE_ORB, 30*IN_MILLISECONDS, 0, phase);
                events.ScheduleEvent(EVENT_SUMMON_WARBRINGER, 25*IN_MILLISECONDS, 0, phase);
                events.ScheduleEvent(EVENT_SUMMON_EVOKER, 30*IN_MILLISECONDS, 0, phase);
                events.ScheduleEvent(EVENT_SUMMON_COMMONER, 35*IN_MILLISECONDS, 0, phase);
                events.ScheduleEvent(EVENT_BERSERK_PHASE_1, 5*MINUTE*IN_MILLISECONDS, 0, phase);
                events.ScheduleEvent(EVENT_SAY_AGGRO_2, 10*IN_MILLISECONDS, 0, phase);

 
                me->SetFacingToObject(who);


                    

                        Talk(SAY_JUMPDOWN);
                        phase = PHASE_ARENA;
                        events.SetPhase(PHASE_ARENA);
                        me->RemoveAurasDueToSpell(SPELL_SHEAT_OF_LIGHTNING);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                        me->GetMotionMaster()->MoveJump(2134.79f, -263.03f, 419.84f, 20.0f, 0);

                        events.ScheduleEvent(EVENT_UNBALANCING_STRIKE, 15*IN_MILLISECONDS, 0, PHASE_ARENA);
                        events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 20*IN_MILLISECONDS, 0, PHASE_ARENA);
                        events.ScheduleEvent(EVENT_TRANSFER_ENERGY, 20*IN_MILLISECONDS, 0, PHASE_ARENA);
                        events.ScheduleEvent(EVENT_BERSERK_PHASE_2, 5*MINUTE*IN_MILLISECONDS, 0, PHASE_ARENA);
                        // Check for Hard Mode
                        if (EncounterTime <= MAX_HARD_MODE_TIME)
                        {
                            HardMode = true;
                            // Achievement
                            instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_THORIM_SIFFED_CREDIT);
                            // Summon Sif
                            
                        }
                        else
                            me->AddAura(SPELL_TOUCH_OF_DOMINION, me);

                DoZoneInCombat();
            }

          /*  void EnterEvadeMode()
            {
                if (!_EnterEvadeMode())
                    return;

                me->SetHomePosition(homePosition);
                me->GetMotionMaster()->MoveTargetedHome();
                Reset();
            }*/

            void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                if (spell->Id == 62466)
                    if (target->GetTypeId() == TYPEID_PLAYER)
                        doNotStandInTheLighting = false;
            }

			void UpdateAI(uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (checkTargetTimer < diff)
                {
                    if (!SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                    {
                        EnterEvadeMode();
                        return;
                    }
                    checkTargetTimer = 7*IN_MILLISECONDS;
                }
                else
                    checkTargetTimer -= diff;

                // Thorim should be inside the arena during phase 3
                /*if (phase == PHASE_ARENA && ArenaAreaCheck(false)(me))
                {
                    EnterEvadeMode();
                    return;
                }*/

                EncounterTime += diff;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SAY_AGGRO_2:
                            Talk(SAY_AGGRO_2);
                            break;
                        case EVENT_STORMHAMMER:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                DoCast(target, SPELL_STORMHAMMER, true);
                            events.ScheduleEvent(EVENT_STORMHAMMER, urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS), 0, PHASE_ARENA_ADDS);
                            break;
                        case EVENT_CHARGE_ORB:
                            DoCastAOE(SPELL_CHARGE_ORB);
                            events.ScheduleEvent(EVENT_CHARGE_ORB, urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS), 0, PHASE_ARENA_ADDS);
                            break;
                        case EVENT_SUMMON_WARBRINGER:
                            events.ScheduleEvent(EVENT_SUMMON_WARBRINGER, 20*IN_MILLISECONDS, 0, PHASE_ARENA_ADDS);
                            break;
                        case EVENT_SUMMON_EVOKER:
                            
                            events.ScheduleEvent(EVENT_SUMMON_EVOKER, urand(23*IN_MILLISECONDS, 27*IN_MILLISECONDS), 0, PHASE_ARENA_ADDS);
                            break;
                        case EVENT_SUMMON_COMMONER:
                            events.ScheduleEvent(EVENT_SUMMON_COMMONER, 30*IN_MILLISECONDS, 0, PHASE_ARENA_ADDS);
                            break;
                        case EVENT_BERSERK_PHASE_1:
                            DoCast(me, SPELL_BERSERK_PHASE_1);
                            //DoCast(me, SPELL_SUMMON_LIGHTNING_ORB, true);
                            Talk(SAY_BERSERK);
                            break;
                        // Phase 3 stuff
                        case EVENT_UNBALANCING_STRIKE:
                            DoCastVictim(SPELL_UNBALANCING_STRIKE);
                            events.ScheduleEvent(EVENT_UNBALANCING_STRIKE, 26*IN_MILLISECONDS, 0, PHASE_ARENA);
                            break;
                        case EVENT_CHAIN_LIGHTNING:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(target, SPELL_CHAIN_LIGHTNING);
                            events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(7*IN_MILLISECONDS, 15*IN_MILLISECONDS), 0, PHASE_ARENA);
                            break;
                        case EVENT_TRANSFER_ENERGY:
                            events.ScheduleEvent(EVENT_RELEASE_LIGHTNING_CHARGE, 8*IN_MILLISECONDS, 0, PHASE_ARENA);
                            break;
                        case EVENT_RELEASE_LIGHTNING_CHARGE:

                            DoCast(me, SPELL_LIGHTNING_CHARGE, true);
                            events.ScheduleEvent(EVENT_TRANSFER_ENERGY, 8*IN_MILLISECONDS, 0, PHASE_ARENA);
                            break;
                        case EVENT_BERSERK_PHASE_2:
                            DoCast(me, SPELL_BERSERK_PHASE_2);
                            Talk(SAY_BERSERK);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                // EnterEvadeIfOutOfCombatArea(diff);
            }



            void DoAction(int32  action)
            {
                switch (action)
                {
                    case ACTION_BERSERK:
                        if (!gotBerserkedAndOrbSummoned)
                        {
                            if (phase == PHASE_ARENA)
                                return;

                            DoCast(me, SPELL_BERSERK_PHASE_1);
                            
                            Talk(SAY_BERSERK);
                            gotBerserkedAndOrbSummoned = true;
                        }
                        break;
                    case ACTION_UPDATE_PHASE:
                        phase = PHASE_PRE_ARENA_ADDS;
                        break;
                    default:
                        break;
                }
            }




            void DamageTaken(Unit* attacker, uint32 &damage)
            {
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    EncounterPostProgress();
                }

                if (phase == PHASE_ARENA_ADDS && attacker && instance)
                {
                    if ( me->IsWithinDistInMap(attacker, 50.0f) && attacker->ToPlayer())
                    {
                        Talk(SAY_JUMPDOWN);
                        phase = PHASE_ARENA;
                        events.SetPhase(PHASE_ARENA);
                        me->RemoveAurasDueToSpell(SPELL_SHEAT_OF_LIGHTNING);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                        me->GetMotionMaster()->MoveJump(2134.79f, -263.03f, 419.84f, 20.0f, 0);
                        
                        events.ScheduleEvent(EVENT_UNBALANCING_STRIKE, 15*IN_MILLISECONDS, 0, PHASE_ARENA);
                        events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 20*IN_MILLISECONDS, 0, PHASE_ARENA);
                        events.ScheduleEvent(EVENT_TRANSFER_ENERGY, 20*IN_MILLISECONDS, 0, PHASE_ARENA);
                        events.ScheduleEvent(EVENT_BERSERK_PHASE_2, 5*MINUTE*IN_MILLISECONDS, 0, PHASE_ARENA);
                        // Check for Hard Mode
                        if (EncounterTime <= MAX_HARD_MODE_TIME)
                        {
                            HardMode = true;
                            // Achievement
                            instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_THORIM_SIFFED_CREDIT);
                            // Summon Sif
                            
                        }
                        else
                            me->AddAura(SPELL_TOUCH_OF_DOMINION, me);
                    }
                }
            }

            private:
                Phases phase;
                uint8 PreAddsCount;
                uint32 EncounterTime;
                uint32 checkTargetTimer;
                bool gotAddsWiped;
                bool HardMode;
                bool gotBerserkedAndOrbSummoned;
                bool gotEncounterFinished;
                bool summonChampion;
                bool doNotStandInTheLighting;
                Position homePosition;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetUlduarAI<boss_thorimAI>(creature);
        }
};




void AddSC_boss_razorscale()
{
    new boss_razorscale_controller();
    new go_razorscale_harpoon();
    new boss_razorscale();
    new npc_expedition_commander();
    new npc_mole_machine_trigger();
    new npc_devouring_flame();
    new npc_darkrune_watcher();
    new npc_darkrune_guardian();
    new npc_darkrune_sentinel();
    new spell_razorscale_devouring_flame();
    new spell_razorscale_flame_breath();
    new achievement_iron_dwarf_medium_rare();
    new achievement_quick_shave();
new boss_thorim1();
}
