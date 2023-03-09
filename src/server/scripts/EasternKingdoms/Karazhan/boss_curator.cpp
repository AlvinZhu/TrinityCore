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

/* ScriptData
SDName: Boss_Curator
SD%Complete: 100
SDComment:
SDCategory: Karazhan
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

        static inline uint8 GetHealthPCT(Unit* hTarget) { if (!hTarget || !hTarget->IsInWorld() || hTarget->isDead()) return 100; return (hTarget->GetHealth()*100/hTarget->GetMaxHealth()); }
        static inline uint8 GetManaPCT(Unit* hTarget) { if (!hTarget || !hTarget->IsInWorld() || hTarget->isDead() || hTarget->getPowerType() != POWER_MANA) return 100; return (hTarget->GetPower(POWER_MANA)*100/(hTarget->GetMaxPower(POWER_MANA) + 1)); }


enum Curator
{
    SAY_AGGRO                       = 0,
    SAY_SUMMON                      = 1,
    SAY_EVOCATE                     = 2,
    SAY_ENRAGE                      = 3,
    SAY_KILL                        = 4,
    SAY_DEATH                       = 5,

    //Flare spell info
    SPELL_ASTRAL_FLARE_PASSIVE      = 30234,               //Visual effect + Flare damage

    //Curator spell info
    SPELL_HATEFUL_BOLT              = 30383,
    SPELL_EVOCATION                 = 30254,
    SPELL_ENRAGE                    = 30403,               //Arcane Infusion: Transforms Curator and adds damage.
    SPELL_BERSERK                   = 26662,
};


enum Texts
{
    SAY_AGGRO2       = 0,
    SAY_EARTHQUAKE  = 1,
    SAY_OVERRUN     = 2,
    SAY_SLAY        = 3,
    SAY_DEATH2       = 4,
};


enum Spells
{
    SPELL_EARTHQUAKE        = 37474,
    SPELL_SUNDER_ARMOR      = 40504,
    SPELL_CHAIN_LIGHTNING   = 40599,
    SPELL_OVERRUN           = 23580,
    SPELL_ENRAGE2            = 37471,
    SPELL_MARK_DEATH        = 23931,
    SPELL_AURA_DEATH        = 40653,
    
        aSPELL_EARTHQUAKE        = 37476,
    aSPELL_SUNDER_ARMOR      = 40504,
    aSPELL_CHAIN_LIGHTNING   = 40599,
    aSPELL_OVERRUN           = 23580,
    aSPELL_ENRAGE2            = 37472,
    aSPELL_MARK_DEATH        = 23931,
    aSPELL_AURA_DEATH        = 40653
    
   
};

enum Events
{
    EVENT_ENRAGE    = 1,
    EVENT_ARMOR     = 2,
    EVENT_CHAIN     = 3,
    EVENT_QUAKE     = 4,
    EVENT_OVERRUN   = 5

    
};

class boss_curator : public CreatureScript
{
public:
    boss_curator() : CreatureScript("boss_curator") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_curatorAI(creature);
    }

    struct boss_curatorAI : public ScriptedAI
    {
        boss_curatorAI(Creature* creature) : ScriptedAI(creature)
        {
            Initialize();
        }

        void Initialize()
        {
            AddTimer = 10000;
            HatefulBoltTimer = 15000;                           //This time may be wrong
            BerserkTimer = 720000;                              //12 minutes
            Enraged = false;
            Evocating = false;
        }

        uint32 AddTimer;
        uint32 HatefulBoltTimer;
        uint32 BerserkTimer;

        bool Enraged;
        bool Evocating;

        void Reset() override
        {
            Initialize();

            me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ARCANE, true);
        }

        void KilledUnit(Unit* /*victim*/) override
        {
            Talk(SAY_KILL);
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            Talk(SAY_AGGRO);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            //always decrease BerserkTimer
            if (BerserkTimer <= diff)
            {
                //if evocate, then break evocate
                if (Evocating)
                {
                    if (me->HasAura(SPELL_EVOCATION))
                        me->RemoveAurasDueToSpell(SPELL_EVOCATION);

                    Evocating = false;
                }

                //may not be correct SAY (generic hard enrage)
                Talk(SAY_ENRAGE);

                me->InterruptNonMeleeSpells(true);
                DoCast(me, SPELL_BERSERK);

                //don't know if he's supposed to do summon/evocate after hard enrage (probably not)
                Enraged = true;
            } else BerserkTimer -= diff;

            if (Evocating)
            {
                //not supposed to do anything while evocate
                if (me->HasAura(SPELL_EVOCATION))
                    return;
                else
                    Evocating = false;
            }

            if (!Enraged)
            {
                if (AddTimer <= diff)
                {
                    //Summon Astral Flare
					Position& pos = me->GetFirstCollisionPosition(24, frand(0.1f, float(M_PI) * 2.0f));
					Creature* AstralFlare = DoSpawnCreature(17096, pos.GetPositionX() - me->GetPositionX(), pos.GetPositionY() - me->GetPositionY(), 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
					//Creature* AstralFlare = DoSpawnCreature(17096, float(rand32() % 37), float(rand32() % 37), 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
					Unit* target = NULL;
                    target = SelectTarget(SELECT_TARGET_RANDOM, 0);

                    if (AstralFlare && target)
                    {
                        AstralFlare->CastSpell(AstralFlare, SPELL_ASTRAL_FLARE_PASSIVE, false);
                        AstralFlare->AI()->AttackStart(target);
                    }
					(AstralFlare, 80, false);

                    //Reduce Mana by 10% of max health
                    if (int32 mana = me->GetMaxPower(POWER_MANA))
                    {
                        mana /= 10;
                        me->ModifyPower(POWER_MANA, -mana);

                        //if this get's us below 10%, then we evocate (the 10th should be summoned now)
                        if (me->GetPower(POWER_MANA)*100 / me->GetMaxPower(POWER_MANA) < 10)
                        {
                            Talk(SAY_EVOCATE);
                            me->InterruptNonMeleeSpells(false);
                            DoCast(me, SPELL_EVOCATION);
                            Evocating = true;
                            //no AddTimer cooldown, this will make first flare appear instantly after evocate end, like expected
                            return;
                        }
                        else
                        {
                            if (urand(0, 1) == 0)
                            {
                                Talk(SAY_SUMMON);
                            }
                        }
                    }

                    AddTimer = 10000;
                } else AddTimer -= diff;

                if (!HealthAbovePct(15))
                {
                    Enraged = true;
                    DoCast(me, SPELL_ENRAGE);
                    Talk(SAY_ENRAGE);
                }
            }

            if (HatefulBoltTimer <= diff)
            {
                if (Enraged)
                    HatefulBoltTimer = 7000;
                else
                    HatefulBoltTimer = 15000;

                if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 1))
                    DoCast(target, SPELL_HATEFUL_BOLT);
            } else HatefulBoltTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};



class boss_chess1 : public CreatureScript
{
    public:
        boss_chess1() : CreatureScript("boss_chess1") { }

        struct boss_SpoilsOfPandariaAI : public ScriptedAI
        {
            boss_SpoilsOfPandariaAI(Creature* creature) : ScriptedAI(creature)
            {
                Initialize();
            }

            void Initialize()
            {
                _inEnrage = false;
            }
bool Phase1;
bool Phase2;
bool Phase3;
bool Phase4;
bool Phase5;
bool Phase6;
bool Phase7;
bool Phase8;
bool Phase9;
bool Phase10;
            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_ENRAGE, 0);
                _events.ScheduleEvent(EVENT_ARMOR, urand(5000, 13000));
                _events.ScheduleEvent(EVENT_CHAIN, urand(10000, 30000));
                _events.ScheduleEvent(EVENT_QUAKE, urand(25000, 35000));
                _events.ScheduleEvent(EVENT_OVERRUN, urand(30000, 45000));

 Phase1 = false;
                Phase2 = false;
                Phase3 = false;
                Phase4 = false;
                Phase5 = false;
                Phase6 = false;
                Phase7 = false;
                Phase8 = false;
                Phase9 = false;
                Phase10 = false;
                Initialize();
            }

            void KilledUnit(Unit* victim) override
            {
                

                if (urand(0, 4))
                    return;

                
            }

            void JustDied(Unit* killer) override
            {
                if (killer) { 
                	//if (killer->GetTypeId() == TYPEID_PLAYER)
//me->SummonGameObject(185119,  -11063.2f,-1898.3f,221.16f, 0, 0, 0, 0, 0, 1000);
me->SummonGameObject(185119,  -11063.2f,-1898.3f,221.16f, 0.f,G3D::Quat(0.f, 0.f, 0.f, 0.f), 1000);
                	
                	}
            }

            void EnterCombat(Unit* /*who*/) override
            {
                
            }

            void MoveInLineOfSight(Unit* who) override

            {
                
                    
                        
            }

            void UpdateAI(uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;


    if (!Phase1 && HealthBelowPct(90))
            {
                Phase1 = true;
  Creature* mob = me->SummonCreature(17211, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
   if (mob) mob->AI()->DoZoneInCombat(); 		
   mob = me->SummonCreature(17211, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
if (mob) mob->AI()->DoZoneInCombat(); 		
                
            }

            if (!Phase2 && HealthBelowPct(80))
            {
Phase2 = true;
 Creature* mob =me->SummonCreature(17211, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
 if (mob) mob->AI()->DoZoneInCombat(); 		

 mob = me->SummonCreature(17211, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
 if (mob) mob->AI()->DoZoneInCombat(); 		
 mob = me->SummonCreature(17211, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
if (mob) mob->AI()->DoZoneInCombat(); 		
}
            if (!Phase3 && HealthBelowPct(70))
            {
Phase3 = true;
Creature*  mob =me->SummonCreature(21664, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
  if (mob) mob->AI()->DoZoneInCombat(); 	
  mob =me->SummonCreature(21664, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
if (mob) mob->AI()->DoZoneInCombat(); 	
}

            if (!Phase4 && HealthBelowPct(60))
            {
Phase4 = true;
	Creature* mob =me->SummonCreature(21160, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
	if (mob) mob->AI()->DoZoneInCombat(); 	
	mob =me->SummonCreature(21160, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
if (mob) mob->AI()->DoZoneInCombat(); 	
}



            if (!Phase5 && HealthBelowPct(40))
            {
Phase5 = true;
Creature* mob =me->SummonCreature(21683, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
if (mob) mob->AI()->DoZoneInCombat(); 	
 mob =me->SummonCreature(21683, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
if (mob) mob->AI()->DoZoneInCombat(); 	



//me->SummonCreature(72248, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
}


            if (!Phase6 && HealthBelowPct(20))
            {
Phase6 = true;
Creature* mob =me->SummonCreature(21682, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
if (mob) mob->AI()->DoZoneInCombat(); 	
mob =me->SummonCreature(21682, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
if (mob) mob->AI()->DoZoneInCombat(); 	

}







                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ENRAGE:
                            if (!HealthAbovePct(30))
                            {
                                DoCast(me, SPELL_ENRAGE2);
                                _events.ScheduleEvent(EVENT_ENRAGE, 6000);
                                _inEnrage = true;
                            }
                            break;
                        case EVENT_OVERRUN:
                            
                            DoCastVictim(SPELL_OVERRUN);
                            _events.ScheduleEvent(EVENT_OVERRUN, urand(25000, 40000));
                            break;
                        case EVENT_QUAKE:
                            if (urand(0, 1))
                                return;

                            

                            //remove enrage before casting earthquake because enrage + earthquake = 16000dmg over 8sec and all dead
                            if (_inEnrage)
                                me->RemoveAurasDueToSpell(SPELL_ENRAGE2);

                            DoCastVictim(SPELL_EARTHQUAKE);
                            _events.ScheduleEvent(EVENT_QUAKE, urand(30000, 55000));
                            break;
                        case EVENT_CHAIN:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_CHAIN_LIGHTNING);
                            _events.ScheduleEvent(EVENT_CHAIN, urand(7000, 27000));
                            break;
                        case EVENT_ARMOR:
                            if (irand(0,5)==0) DoCastVictim(SPELL_MARK_DEATH);else if (irand(0,5)==1) DoCastVictim(SPELL_AURA_DEATH);else DoCastVictim(SPELL_SUNDER_ARMOR);
                            _events.ScheduleEvent(EVENT_ARMOR, urand(10000, 25000));
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap _events;
                bool _inEnrage;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_SpoilsOfPandariaAI(creature);
        }
};




class boss_chess2 : public CreatureScript
{
    public:
        boss_chess2() : CreatureScript("boss_chess2") { }

        struct boss_SpoilsOfPandariaAI : public ScriptedAI
        {
            boss_SpoilsOfPandariaAI(Creature* creature) : ScriptedAI(creature)
            {
                Initialize();
            }

            void Initialize()
            {
                _inEnrage = false;
            }
bool Phase1;
bool Phase2;
bool Phase3;
bool Phase4;
bool Phase5;
bool Phase6;
bool Phase7;
bool Phase8;
bool Phase9;
bool Phase10;
            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_ENRAGE, 0);
                _events.ScheduleEvent(EVENT_ARMOR, urand(5000, 13000));
                _events.ScheduleEvent(EVENT_CHAIN, urand(10000, 30000));
                _events.ScheduleEvent(EVENT_QUAKE, urand(25000, 35000));
                _events.ScheduleEvent(EVENT_OVERRUN, urand(30000, 45000));

 Phase1 = false;
                Phase2 = false;
                Phase3 = false;
                Phase4 = false;
                Phase5 = false;
                Phase6 = false;
                Phase7 = false;
                Phase8 = false;
                Phase9 = false;
                Phase10 = false;
                Initialize();
            }

            void KilledUnit(Unit* victim) override
            {
                

                if (urand(0, 4))
                    return;

                
            }

            void JustDied(Unit* killer) override
            {
                if (killer) { 
                	
                	//if (killer->GetTypeId() == TYPEID_PLAYER) 
                //me->SummonGameObject(185119,  -11063.2f,-1898.3f,221.16f, 0, 0, 0, 0, 0, 1000);
                me->SummonGameObject(185119,  -11063.2f,-1898.3f,221.16f,0.f, G3D::Quat(0.f, 0.f, 0.f, 0.f), 1000);
                 
                }
            }

            void EnterCombat(Unit* /*who*/) override
            {
                
            }

            void MoveInLineOfSight(Unit* who) override

            {
                
                    
                        
            }

            void UpdateAI(uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;


   if (!Phase1 && HealthBelowPct(90))
            {
                Phase1 = true;
 
 Creature* mob =me->SummonCreature(17469, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
if (mob) mob->AI()->DoZoneInCombat(); 
  mob=me->SummonCreature(17469, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
if (mob) mob->AI()->DoZoneInCombat();  		
                
            }

            if (!Phase2 && HealthBelowPct(80))
            {
Phase2 = true;
  Creature* mob =me->SummonCreature(17469, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
  if (mob) mob->AI()->DoZoneInCombat(); 
  mob=me->SummonCreature(17469, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
  if (mob) mob->AI()->DoZoneInCombat(); 
  mob=me->SummonCreature(17469, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
if (mob) mob->AI()->DoZoneInCombat(); 
}
            if (!Phase3 && HealthBelowPct(70))
            {
Phase3 = true;
 Creature* mob =me->SummonCreature(21748, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
 if (mob) mob->AI()->DoZoneInCombat(); 
 mob=me->SummonCreature(21748, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
 if (mob) mob->AI()->DoZoneInCombat(); 
}

            if (!Phase4 && HealthBelowPct(60))
            {
Phase4 = true;
	 Creature* mob =me->SummonCreature(21726, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
	if (mob) mob->AI()->DoZoneInCombat(); 
	mob=me->SummonCreature(21726, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
	if (mob) mob->AI()->DoZoneInCombat(); 

}



            if (!Phase5 && HealthBelowPct(40))
            {
Phase5 = true;
 Creature* mob =me->SummonCreature(21747, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
 if (mob) mob->AI()->DoZoneInCombat(); 
 mob=me->SummonCreature(21747, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
if (mob) mob->AI()->DoZoneInCombat(); 


//me->SummonCreature(72248, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
}


            if (!Phase6 && HealthBelowPct(20))
            {
Phase6 = true;
 Creature* mob =me->SummonCreature(21750, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
 if (mob) mob->AI()->DoZoneInCombat(); 
 mob=me->SummonCreature(21750, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);
 if (mob) mob->AI()->DoZoneInCombat(); 

}





                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ENRAGE:
                            if (!HealthAbovePct(30))
                            {
                                DoCast(me, aSPELL_ENRAGE2);
                                _events.ScheduleEvent(EVENT_ENRAGE, 6000);
                                _inEnrage = true;
                            }
                            break;
                        case EVENT_OVERRUN:
                            
                            DoCastVictim(aSPELL_OVERRUN);
                            _events.ScheduleEvent(EVENT_OVERRUN, urand(25000, 40000));
                            break;
                        case EVENT_QUAKE:
                            if (urand(0, 1))
                                return;

                            

                            //remove enrage before casting earthquake because enrage + earthquake = 16000dmg over 8sec and all dead
                            if (_inEnrage)
                                me->RemoveAurasDueToSpell(aSPELL_ENRAGE2);

                            DoCastVictim(aSPELL_EARTHQUAKE);
                            _events.ScheduleEvent(EVENT_QUAKE, urand(30000, 55000));
                            break;
                        case EVENT_CHAIN:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, aSPELL_CHAIN_LIGHTNING);
                            _events.ScheduleEvent(EVENT_CHAIN, urand(7000, 27000));
                            break;
                        case EVENT_ARMOR:
                            if (irand(0,5)==0) DoCastVictim(aSPELL_MARK_DEATH);else if (irand(0,5)==1) DoCastVictim(aSPELL_AURA_DEATH);else DoCastVictim(aSPELL_SUNDER_ARMOR);
                            _events.ScheduleEvent(EVENT_ARMOR, urand(10000, 25000));
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap _events;
                bool _inEnrage;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_SpoilsOfPandariaAI(creature);
        }
};







class aboss_AI : public CreatureScript
{
    public:
        aboss_AI() : CreatureScript("aboss_AI") { }

        struct boss_SpoilsOfPandariaAI : public ScriptedAI
        {
            boss_SpoilsOfPandariaAI(Creature* creature) : ScriptedAI(creature)
            {
                Initialize();
            }

            void Initialize()
            {
                _inEnrage = false;
            }

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_ENRAGE, 10);
                _events.ScheduleEvent(EVENT_ARMOR, urand(6000, 12000));
                _events.ScheduleEvent(EVENT_CHAIN, urand(12000, 20000));
                _events.ScheduleEvent(EVENT_QUAKE, urand(25000, 35000));
                _events.ScheduleEvent(EVENT_OVERRUN, urand(35000, 40000));

                Initialize();
            }

            void KilledUnit(Unit* victim) override
            {
                


                Talk(3);
            }

            void JustDied(Unit* killer) override
            {
                if (killer) { 
 				int spell15 =  me->m_spells[14]; //死亡后召唤gameobjectid
				int spell16 =  me->m_spells[15]; //死亡后召唤npcid               	
if (spell15>0)
me->SummonGameObject(spell15,  me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2, 0.f,G3D::Quat(0.f, 0.f, 0.f, 0.f), 1000);

if (spell16>0)
me->SummonCreature(spell16, me->GetPositionX()+irand(3,7), me->GetPositionY()+irand(3,7), me->GetPositionZ()+0.2);

                	
                                 Talk(4);
                }
            }

            void EnterCombat(Unit* /*who*/) override
            {
               Talk(0); 
            }


            void UpdateAI(uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
if (!me->IsInWorld())
return;

_events.Update(diff);

				int spell1 =  me->m_spells[0]; //加aura
				int spell2 =  me->m_spells[1]; //远程
				int spell3 =  me->m_spells[2]; //远程2
				int spell4 =  me->m_spells[3]; //近程1
				int spell5 =  me->m_spells[4]; //近程2
				int spell6 =  me->m_spells[5]; //小于50% 技能
				int spell7 =  me->m_spells[6]; //小于30% 技能 
				int spell8 =  me->m_spells[7]; //小于10% 技能 
				

				int spell9 =  me->m_spells2[0];  //加aura 技能2
				int spell10 =  me->m_spells2[1]; //额外技能1
				int spell11 =  me->m_spells2[2]; //额外技能2
				int spell12 =  me->m_spells2[3]; //额外技能3
				int spell13 =  me->m_spells2[4]; //召唤npc1 id
				int spell14 =  me->m_spells2[5]; //召唤npc2 id
				int spell15 =  me->m_spells2[6]; //死亡后召唤gameobjectid
				int spell16 =  me->m_spells2[7]; //死亡后召唤npcid

				int spell17 =  me->m_spells2[8]; //技能间隔1 1000为一秒 默认6000
				int spell18 =  me->m_spells2[9]; //技能间隔2 1000为一秒 默认13000
				int spell19 =  me->m_spells2[10]; //技能间隔3 1000为一秒 默认25000
				int spell20 =  me->m_spells2[11]; //技能间隔4 1000为一秒 默认35000

				

//creature_tempate说明
				//spell1 加aura  当血量小于30
				//spell2 远程  技能
				//spell3 远程2 技能
				//spell4 近程1 技能
				//spell5 近程2 技能
				//spell6 额外技能1 血量小于50% 技能 一定机率使用
				//spell7 额外技能2 血量小于30% 技能 一定机率使用 
				//spell8 额外技能3 血量小于10% 技能 一定机率使用
				//aspell1  加aura 随时
				//aspell2 额外技能1 能量小于<30% 一定机率使用
				//aspell3 额外技能2 能量小于<50% 一定机率使用
				//aspell4 额外技能3 能量小于<70% 一定机率使用
				//aspell5 战斗时 召唤npc1 id
				//aspell6 战斗时 召唤npc2 id
				//aspell7 死亡后召唤gameobjectid
				//aspell8 死亡后召唤npcid
				//aspell9 普通技能间隔1 1000为一秒 默认6000
				//aspell10 普通技能间隔2 1000为一秒 默认13000
				//aspell11 大招技能间隔3 1000为一秒 默认25000
				//aspell12 大招技能间隔4 1000为一秒 默认35000

//creature_text说明
				//Talk(0) 进入战斗
				//Talk(1) 使用技能1
				//Talk(2) 使用技能2
				//Talk(3) 杀死对方
				//Talk(4) 死亡
//creature_text 样例
//72619	0	0	开始战斗!	12	0	100	0	0	0	0	0	abossai
//72619	1	0	放绝招1！	12	7	100	0	0	0	0	0	abossai
//72619	2	0	放绝招2！	12	7	100	0	0	0	0	0	abossai
//72619	3	0	干掉一个！	12	7	100	0	0	0	0	0	abossai
//72619	4	0	我死了！	12	7	100	0	0	0	0	0	abossai
				
//默认有4个技能
					if (spell2<=0) spell2=40504;
						if (spell3<=0) spell3=40504;
							if (spell4<=0) spell4=40504;
								if (spell5<=0) spell5=40599;
if (spell17<=0) spell17=6000;
if (spell18<=0) spell18=13000;
if (spell19<=0) spell19=25000;
if (spell20<=0) spell20=35000;								

												
				

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ENRAGE:
                            if (!HealthAbovePct(30))
                            {
                                if (spell1>0)
                                me->AddAura(spell1, me);
                               if (spell9>0)
                               	me->AddAura(spell9, me);

                               // DoCast(me, spell1);
                                _events.ScheduleEvent(EVENT_ENRAGE, 5800);
                                _inEnrage = true;
                            }
                            break;
                        case EVENT_OVERRUN:
                            Talk(1);
                              if (spell9>0)
{
                               	if (!me->HasAura(spell9))
                               	me->AddAura(spell9, me);
 }                              

                              if (!HealthAbovePct(30) && !_inEnrage)
                            {
                                if (spell1>0)
                                me->AddAura(spell1, me);
 
                                _inEnrage = true;
                            }

				if (!HealthAbovePct(50) && spell6>0)
                            DoCastVictim(spell6);
else	if (!HealthAbovePct(30) && spell7>0)
		DoCastVictim(spell7);
else	if (!HealthAbovePct(10) && spell8>0)
		DoCastVictim(spell8);
else                            DoCastVictim(spell2);
//if (irand(0,3)==1 && spell13>0)
if (irand(0,3)==1 && spell13>0  && me->IsInWorld())
{
	 Creature* mob =me->SummonCreature(spell13, me->GetPositionX()+irand(1,2), me->GetPositionY()+irand(1,2), me->GetPositionZ()+0.1);
	if (mob) 
		{
	int dmg=me->getLevel();
	mob->setFaction(14);
	if (mob->getLevel() < me->getLevel())
	{
	mob->SetLevel(me->getLevel());
	mob->SetMaxHealth(dmg*200);
  mob->SetHealth(dmg*200); 
 		mob->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg*50);
		mob->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg*50);
		mob->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg*50);
		mob->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg*50);
		mob->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg*50);
	  mob->UpdateAttackPowerAndDamage();
 

 }

 
 if (Unit* target = me->GetVictim())        
  {
if (target && target->IsInWorld())
{
  mob->GetMotionMaster()->MoveChase(target);
            mob->AI()->AttackStart(target);
}
	}
	
}
}
                            _events.ScheduleEvent(EVENT_OVERRUN, urand(spell20, (spell20+6000)));
                            break;
                        case EVENT_QUAKE:
Talk(2);
                              if (spell9>0)
{
                               	if (!me->HasAura(spell9))
                               	me->AddAura(spell9, me);
 }                              

                             if (!HealthAbovePct(30) && !_inEnrage)
                            {
                                if (spell1>0)
                                me->AddAura(spell1, me);
                                _inEnrage = true;
                            }

				if (!HealthAbovePct(50) && spell6>0)
                            DoCastVictim(spell6);
else	if (!HealthAbovePct(30) && spell7>0)
		DoCastVictim(spell7);
else	if (!HealthAbovePct(10) && spell8>0)
		DoCastVictim(spell8);
		
else                            DoCastVictim(spell3);

//irand(0,3)==1 && 
if (irand(0,3)==1 && spell14>0  && me->IsInWorld())
{
    Creature* mob = me->SummonCreature(spell14, me->GetPositionX() + irand(1, 2), me->GetPositionY() + irand(1, 2), me->GetPositionZ() + 0.1);
	if (mob) 
		{
	int dmg=me->getLevel();
	mob->setFaction(14);
	if (mob->getLevel() < me->getLevel())
	{
	mob->SetLevel(me->getLevel());
	mob->SetMaxHealth(dmg*200);
  mob->SetHealth(dmg*200); 
 		mob->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg*50);
		mob->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg*50);
		mob->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg*50);
		mob->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg*50);
		mob->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg*50);
	  mob->UpdateAttackPowerAndDamage();
 

 }

 
 if (Unit* target = me->GetVictim())        
  {
  if (target && target->IsInWorld())
  {
  mob->GetMotionMaster()->MoveChase(target);
            mob->AI()->AttackStart(target);
}
	}
	
}
}

                            _events.ScheduleEvent(EVENT_QUAKE, urand(spell19, (spell19+6000)));
                            break;
                        case EVENT_CHAIN:
                                        
                           if (Unit* target = me->GetVictim())
{
  float dist = me->GetExactDist(target);

			if (_isnan(dist))
				return;

			if (dist < 0)
				return;
if ((irand(0,8)==0 && GetManaPCT(me) <30 && spell10>0) || (irand(0,88)==1 && spell10>0))
	DoCast(target, spell10);
else if ((irand(0,8)==0 && GetManaPCT(me) <50  && spell11>0)  || (irand(0,88)==1 && spell10>0))
	DoCast(target, spell11);
else if ((irand(0,8)==0 && GetManaPCT(me) <70 && spell12>0)  || (irand(0,88)==1 && spell10>0))
	DoCast(target, spell12);
			

else				if (dist<3.0f && irand(0,2)==1)
{
                                DoCast(target, spell5);
}

else 
	{
	
	DoCast(target, spell3);
}
}
                            _events.ScheduleEvent(EVENT_CHAIN, urand(spell18, (spell18+6000)));
                            break;
                        case EVENT_ARMOR:
                            if (Unit* target = me->GetVictim())

{
  float dist = me->GetExactDist(target);

			if (_isnan(dist))
				return;

			if (dist < 0)
				return;
if ((irand(0,8)==0 && GetManaPCT(me) <30 && spell10>0) || (irand(0,88)==1 && spell10>0))
	DoCast(target, spell10);
else if ((irand(0,8)==0 && GetManaPCT(me) <50  && spell11>0)  || (irand(0,88)==1 && spell10>0))
	DoCast(target, spell11);
else if ((irand(0,8)==0 && GetManaPCT(me) <70 && spell12>0)  || (irand(0,88)==1 && spell10>0))
	DoCast(target, spell12);

				if (dist<3.0f  && irand(0,2)==1)
{
                                DoCast(target, spell4);
}
else 
	{
	
	DoCast(target, spell2);
}
}



                            _events.ScheduleEvent(EVENT_ARMOR, urand(spell17, (spell17+6000)));
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap _events;
                bool _inEnrage;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_SpoilsOfPandariaAI(creature);
        }
};




void AddSC_boss_curator()
{
	new boss_chess1();
new boss_chess2();
new aboss_AI();

    new boss_curator();
}
