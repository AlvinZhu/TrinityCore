#include "bot_ai.h"
//#include "Group.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
/*
Warlock NpcBot (reworked by Graff onlysuffering@gmail.com)
Voidwalker pet AI included
Complete - 3%
TODO:
*/
class warlock_bot : public CreatureScript
{
public:
    warlock_bot() : CreatureScript("warlock_bot") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new warlock_botAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        return bot_minion_ai::OnGossipHello(player, creature);
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        if (bot_minion_ai* ai = creature->GetBotMinionAI())
            return ai->OnGossipSelect(player, creature, sender, action);
        return true;
    }

    struct warlock_botAI : public bot_minion_ai
    {
        warlock_botAI(Creature* creature) : bot_minion_ai(creature) { }

        bool doCast(Unit* victim, uint32 spellId, bool triggered = false)
        {
            if (CheckBotCast(victim, spellId, CLASS_WARRIOR) != SPELL_CAST_OK)
                return false;

            return bot_ai::doCast(victim, spellId, triggered);
        }

        void EnterCombat(Unit*) { }
        void Aggro(Unit*) { }
        void AttackStart(Unit*) { }
        void KilledUnit(Unit*) { }
        void EnterEvadeMode(EvadeReason /*why*/) { }
        void MoveInLineOfSight(Unit*) { }
        void JustDied(Unit* u) { me->SetBotsPetDied(); bot_ai::JustDied(u); }
        void DoNonCombatActions() { }

        void StartAttack(Unit* u, bool force = false)
        {
            if (GetBotCommandState() == COMMAND_ATTACK && !force) return;
            Aggro(u);
            SetBotCommandState(COMMAND_ATTACK);
            GetInPosition(force);
            feartimer = std::max<uint32>(feartimer, 1000);
        }

        void UpdateAI(uint32 diff)
        {
            ReduceCD(diff);
            if (IAmDead()) return;
            CheckAttackState();
            CheckAuras();
            if (wait == 0)
                wait = GetWait();
            else
                return;
            BreakCC(diff);
            if (CCed(me)) return;

            ////if pet is dead or unreachable
            //Creature* m_botsPet = me->GetBotsPet();
            //if (!m_botsPet || m_botsPet->FindMap() != master->GetMap() || (me->GetDistance2d(m_botsPet) > sWorld->GetMaxVisibleDistanceOnContinents() - 20.f))
            //    if (master->getLevel() >= 10 && !me->IsInCombat() && !IsCasting() && !me->IsMounted())
            //        SummonBotsPet(PET_VOIDWALKER);

            //TODO: implement healthstone
            if (GetHealthPCT(me) < 50 && Potion_cd <= diff)
            {
                temptimer = GC_Timer;
                if (doCast(me, HEALINGPOTION))
                {
                    Potion_cd = POTION_CD;
                    GC_Timer = temptimer;
                }
            }
            if (GetManaPCT(me) < 50 && Potion_cd <= diff)
            {
                temptimer = GC_Timer;
                if (doCast(me, MANAPOTION))
                {
                    Potion_cd = POTION_CD;
                    GC_Timer = temptimer;
                }
            }
            if (!me->IsInCombat())
                DoNonCombatActions();

            if (!CheckAttackTarget(CLASS_WARLOCK))
                return;

            DoNormalAttack(diff);
        }

        void DoNormalAttack(uint32 diff)
        {
 uint8 lvl = master->getLevel();
            opponent = me->GetVictim();
            
                        if (!opponent)
		return;

            float angle = me->GetDistance(opponent);

	    if (_isnan(angle))
		return;

			if (angle == 0)
				return;
            if (opponent)
            {
                if (!IsCasting())
                    StartAttack(opponent);
            }
            else
                return;

            //TODO: add more damage spells

            if (feartimer <= diff && GC_Timer <= diff)
            { CheckFear(); feartimer = 2000; }

            if (IsSpellReady(RAIN_OF_FIRE_1, diff) && !me->isMoving() && HasRole(BOT_ROLE_DPS) && Rand() < 25)
            {
                Unit* blizztarget = FindAOETarget(30, true);
                if (blizztarget && doCast(blizztarget, GetSpell(RAIN_OF_FIRE_1)))
                {
                    SetSpellCooldown(RAIN_OF_FIRE_1, 5000);
                    return;
                }
                SetSpellCooldown(RAIN_OF_FIRE_1, 2000);//fail
            }
            
            
  float dist = me->GetExactDist(opponent);

			if (_isnan(dist))
				return;

			if (dist == 0)
				return;

if (irand(0,28)==5 && lvl>=80)
            {
            doCast(opponent, 65814);            
            return;
            }
else if (irand(0,28)==1 && lvl>=80 )
            {
            doCast(opponent, 61593);            
            return;
            }
else if (irand(0,28)==1 && lvl>=80 && dist < 5)
            {
            doCast(opponent, 65816);            
            return;
            }
else if (irand(0,28)==2 && lvl>=80)
            {
            doCast(opponent, 65819);            
            return;
            }
else if (irand(0,28)==2 && lvl>=80)
            {
            doCast(opponent, 65812);            
            return;
            }
else if (irand(0,28)==2 && lvl>=80)
            {
            doCast(opponent, 65813);            
            return;
            }            
else if (irand(0,28)==2 && lvl>=80)
            {
            doCast(opponent, 65810);            
            return;
            }  

            
else if (irand(0,28)==11 && lvl>=80)
            {
            doCast(opponent, 19728);            
            return;
            }
else if (irand(0,28)==12 && lvl>=80)
            {
            doCast(opponent, 23011);            
            return;
            }            
            
else if (irand(0,18)==1 )
            {
               doCast(opponent, 116);
return;
            }
else
            if (irand(0,18)==2)
            {
               doCast(opponent, 133);
return;
            }            
else
            if (irand(0,18)==3)
            {
               doCast(opponent, 701);
return;
            }  
else
            if (irand(0,18)==4)
            {
               doCast(opponent, 707);
return;
            }  
else				
            if (irand(0,18)==5)
            {
               doCast(opponent, 709);
return;
            }  
else				
            if (irand(0,18)==6)
            {
               doCast(opponent, 711);
return;
            }  
else				
            if (irand(0,18)==7)
            {
               doCast(opponent, 714);
return;
            }  
else				
            if (irand(0,18)==8)
            {
               doCast(opponent, 1090);
return;
            }              
else				
            if (irand(0,19)==9)
            {
               doCast(opponent, 920);
return;
            }  




            
            if (irand(0,18)==1 && GetHealthPCT(me) < 10)
            {
               doCast(me, 60004);
return;
            }  

            if (irand(0,18)==1 && GetManaPCT(me) < 20)
            {
               doCast(me, 61371);
return;
}


            if ( irand(0,18)==2 && dist < 2 )
            {
                doCast(opponent, 40504);
return;
            }

            if (irand(0,18)==3 && dist < 2 )
            {
               doCast(opponent, 40599);
return;
            }

            if (irand(0,18)==6 && dist > 2 )
            {
               doCast(opponent, 403);
return;
            }
            if (irand(0,18)==7 && dist > 2 )
            {
               doCast(opponent, 690);
return;
            }
            if (irand(0,18)==1 && dist > 2 )
            {
               doCast(opponent, 705);
return;
            }
            

            if (irand(0,18)==1 && dist < 2)
            {
                doCast(me, 22857);
                return;
            }
            

//            float dist = me->GetExactDist(opponent);

            if (IsSpellReady(CURSE_OF_THE_ELEMENTS_1, diff) && dist < 30 && Rand() < 15 &&
                !HasAuraName(opponent, CURSE_OF_THE_ELEMENTS_1) &&
                doCast(opponent, GetSpell(CURSE_OF_THE_ELEMENTS_1)))
            {
                GC_Timer = 800;
                return;
            }

            if (IsSpellReady(CORRUPTION_1, diff) && HasRole(BOT_ROLE_DPS) && dist < 30 && Rand() < 25 &&
                !opponent->HasAura(GetSpell(CORRUPTION_1), me->GetGUID()) &&
                doCast(opponent, GetSpell(CORRUPTION_1)))
                return;

            if (IsSpellReady(HAUNT_1, diff) && HasRole(BOT_ROLE_DPS) && dist < 30 && Rand() < 25 &&
                !opponent->HasAura(GetSpell(HAUNT_1), me->GetGUID()) &&
                doCast(opponent, GetSpell(HAUNT_1)))
            {
                SetSpellCooldown(HAUNT_1, 8000);
                return;
            }

            if (GC_Timer <= diff && HasRole(BOT_ROLE_DPS) && dist < 30 && Rand() < 15 && !Afflicted(opponent))
            {
                if (GetSpellCooldown(CONFLAGRATE_1) <= 8000 && doCast(opponent, GetSpell(IMMOLATE_1)))
                    return;
                else if (doCast(opponent, GetSpell(UNSTABLE_AFFLICTION_1)))
                    return;
            }

            if (IsSpellReady(CONFLAGRATE_1, diff) && HasRole(BOT_ROLE_DPS) && dist < 30 && Rand() < 35 &&
                HasAuraName(opponent, IMMOLATE_1) &&
                doCast(opponent, GetSpell(CONFLAGRATE_1)))
            {
                SetSpellCooldown(CONFLAGRATE_1, 8000);
                return;
            }

            if (IsSpellReady(CHAOS_BOLT_1, diff) && HasRole(BOT_ROLE_DPS) && dist < 30 && Rand() < 50 &&
                doCast(opponent, GetSpell(CHAOS_BOLT_1)))
            {
                SetSpellCooldown(CHAOS_BOLT_1, me->getLevel() < 80 ? 10000 : 8000);
                return;
            }

            if (IsSpellReady(SHADOW_BOLT_1, diff) && HasRole(BOT_ROLE_DPS) && dist < 30 &&
                doCast(opponent, GetSpell(SHADOW_BOLT_1)))
                return;
        }

        uint8 Afflicted(Unit* target)
        {
            if (!target || target->isDead()) return 0;
            bool aff = HasAuraName(target, UNSTABLE_AFFLICTION_1, me->GetGUID());
            bool imm = HasAuraName(target, IMMOLATE_1, me->GetGUID());
            if (imm) return 1;
            if (aff) return 2;
            return 0;
        }

        void CheckFear()
        {
            uint32 FEAR = GetSpell(FEAR_1);
            if (Unit* u = FindAffectedTarget(FEAR, me->GetGUID()))
                if (Aura* aura = u->GetAura(FEAR, me->GetGUID()))
                    if (aura->GetDuration() > 3000)
                        return;
            Unit* feartarget = FindFearTarget();
            if (feartarget && doCast(feartarget, FEAR))
                return;
        }

        //void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
        //{
        //    if (summon == me->GetBotsPet())
        //        me->SetBotsPetDied();
        //}

        //void SummonedCreatureDespawn(Creature* summon)
        //{
        //    if (summon == me->GetBotsPet())
        //        me->SetBotsPet(NULL);
        //}

        void ApplyClassDamageMultiplierSpell(int32& damage, SpellNonMeleeDamage& /*damageinfo*/, SpellInfo const* /*spellInfo*/, WeaponAttackType /*attackType*/, bool& crit) const
        {
            //uint32 spellId = spellInfo->Id;
            //uint8 lvl = me->getLevel();
            float fdamage = float(damage);
            //1) apply additional crit chance. This additional chance roll will replace original (balance safe)
            if (!crit)
            {
                float aftercrit = 0.f;
                ////Shatter: frozen targets crit
                //if (lvl >= 11 && damageinfo.target && damageinfo.target->isFrozen())
                //    aftercrit *= 4.f;

                if (aftercrit > 0.f)
                    crit = roll_chance_f(aftercrit);
            }

            //2) apply bonus damage mods
            float pctbonus = 0.0f;
            if (crit)
            {
                ////!!!spell damage is not yet critical and will be multiplied by 1.5
                ////so we should put here bonus damage mult /1.5
                ////Spell Power: 50% additional crit damage bonus for All spells
                //if (lvl >= 55)
                //    pctbonus += 0.333f;
            }
            //if (lvl >= 11 && spellId == FROSTBOLT && damageinfo.target && damageinfo.target->isFrozen())
            //    pctbonus *= 0.2f;

            ////Spellpower bonus damage (temp)
            //if (m_spellpower > 0)
            //{
            //    if (spellId == SHADOW_BOLT)
            //        fdamage += m_spellpower * 1.38f;
            //    else if (spellId == IMMOLATE)
            //        fdamage += m_spellpower * 0.75f; //guessed
            //    else if (spellId == CONFLAGRATE)
            //        fdamage += m_spellpower * 2.75f; //guessed
            //    else if (spellId == CHAOS_BOLT)
            //        fdamage += m_spellpower * 2.25f * 1.24f;
            //    else if (spellId == RAIN_OF_FIRE || spellId == 42223)
            //        fdamage += m_spellpower * 0.25f * 4.f;
            //    else if (spellId == HAUNT)
            //        fdamage += m_spellpower * 1.75f;
            //}

            damage = int32(fdamage * (1.0f + pctbonus));
        }

        void ApplyClassDamageMultiplierEffect(SpellInfo const* /*spellInfo*/, uint8 /*effect_index*/, float& /*value*/) const
        {
            //uint32 spellId = spellInfo->Id;

            //float pct_mod = 1.f;

            //Spellpower bonus damage (temp)
            //if (spellInfo->Effects[effect_index].ApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE)
            //{
            //    if (spellId == CORRUPTION)
            //        value += m_spellpower * 1.35f / float(spellInfo->GetMaxDuration() / spellInfo->Effects[effect_index].Amplitude);
            //    else if (spellId == IMMOLATE)
            //        value += m_spellpower * 1.59f / float(spellInfo->GetMaxDuration() / spellInfo->Effects[effect_index].Amplitude);
            //    else if (spellId == UNSTABLE_AFFLICTION)
            //        value += m_spellpower * 1.68f / float(spellInfo->GetMaxDuration() / spellInfo->Effects[effect_index].Amplitude);
            //}

            //value *= pct_mod;
        }

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            OnSpellHit(caster, spell);
        }

        void DamageDealt(Unit* victim, uint32& /*damage*/, DamageEffectType damageType)
        {
            if (victim == me)
                return;

            if (damageType == DIRECT_DAMAGE || damageType == SPELL_DIRECT_DAMAGE)
            {
                for (uint8 i = 0; i != MAX_BOT_CTC_SPELLS; ++i)
                {
                    if (_ctc[i].first && !_ctc[i].second)
                    {
                        if (urand(1,100) <= CalcCTC(_ctc[i].first))
                            _ctc[i].second = 1000;

                        if (_ctc[i].second > 0)
                            me->CastSpell(victim, _ctc[i].first, true);
                    }
                }
            }
        }

        void DamageTaken(Unit* u, uint32& /*damage*/)
        {
            OnOwnerDamagedBy(u);
        }

        void OwnerAttackedBy(Unit* u)
        {
            OnOwnerDamagedBy(u);
        }

        void Reset()
        {
            feartimer = 0;

            if (master)
            {
                SetStats(true);
                InitRoles();
                ApplyPassives(CLASS_WARLOCK);
            }
        }

        void ReduceCD(uint32 diff)
        {
            CommonTimers(diff);
            SpellTimers(diff);

            if (feartimer > diff)                   feartimer -= diff;
        }

        bool CanRespawn()
        {return false;}

        void InitSpells()
        {
            uint8 lvl = me->getLevel();
            InitSpellMap(CURSE_OF_THE_ELEMENTS_1);
            InitSpellMap(SHADOW_BOLT_1);
            InitSpellMap(IMMOLATE_1);
            lvl >= 40 ? InitSpellMap(CONFLAGRATE_1) : RemoveSpell(CONFLAGRATE_1);
  /*Talent*/lvl >= 60 ? InitSpellMap(CHAOS_BOLT_1) : RemoveSpell(CHAOS_BOLT_1);
            InitSpellMap(RAIN_OF_FIRE_1);
  /*Talent*/lvl >= 60 ? InitSpellMap(HAUNT_1) : RemoveSpell(HAUNT_1);
            InitSpellMap(CORRUPTION_1);
  /*Talent*/lvl >= 50 ? InitSpellMap(UNSTABLE_AFFLICTION_1) : RemoveSpell(UNSTABLE_AFFLICTION_1);
            InitSpellMap(FEAR_1);
        }

        //TODO
        void ApplyClassPassives() { }

    private:
        //Timers
        uint32 feartimer;

        enum WarlockBaseSpells
        {
            CURSE_OF_THE_ELEMENTS_1             = 1490,
            SHADOW_BOLT_1                       = 686,
            IMMOLATE_1                          = 348,
            CONFLAGRATE_1                       = 17962,
            CHAOS_BOLT_1                        = 50796,
            RAIN_OF_FIRE_1                      = 5740,
            HAUNT_1                             = 59164,
            CORRUPTION_1                        = 172,
            UNSTABLE_AFFLICTION_1               = 30404,
            FEAR_1                              = 6215
        };
        enum WarlockPassives
        {
        };
    };
};

class voidwalker_bot : public CreatureScript
{
public:
    voidwalker_bot() : CreatureScript("voidwalker_bot") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new voidwalker_botAI(creature);
    }

    struct voidwalker_botAI : public bot_pet_ai
    {
        voidwalker_botAI(Creature* creature) : bot_pet_ai(creature) { }

        bool doCast(Unit* victim, uint32 spellId, bool triggered = false)
        {
            if (CheckBotCast(victim, spellId, CLASS_NONE) != SPELL_CAST_OK)
                return false;
            return bot_ai::doCast(victim, spellId, triggered);
        }

        void EnterCombat(Unit*) { }
        void Aggro(Unit*) { }
        void AttackStart(Unit*) { }
        void KilledUnit(Unit*) { }
        void EnterEvadeMode(EvadeReason /*why*/) { }
        void MoveInLineOfSight(Unit*) { }
        void JustDied(Unit*) { m_creatureOwner->SetBotsPetDied(); }
        void DoNonCombatActions() { }

        void StartAttack(Unit* u, bool force = false)
        {
            if (GetBotCommandState() == COMMAND_ATTACK && !force) return;
            Aggro(u);
            SetBotCommandState(COMMAND_ATTACK);
            GetInPosition(force);
        }

        void UpdateAI(uint32 diff)
        {
            ReduceCD(diff);
            if (IAmDead()) return;
            CheckAttackState();
            CheckAuras();
            if (wait == 0)
                wait = GetWait();
            else
                return;
            if (CCed(me)) return;

            //TODO: add checks to help owner

            if (!me->IsInCombat())
                DoNonCombatActions();

            if (!CheckAttackTarget(PET_TYPE_VOIDWALKER))
                return;

            DoNormalAttack(diff);
        }

        void DoNormalAttack(uint32 diff)
        {
            opponent = me->GetVictim();
            if (opponent)
            {
                if (!IsCasting())
                    StartAttack(opponent, true);
            }
            else
                return;
            if (MoveBehind(*opponent))
                wait = 5;

            //float dist = me->GetExactDist(opponent);
            float meleedist = me->GetDistance(opponent);

            //TORMENT
            if (IsSpellReady(TORMENT_1, diff, false) && meleedist < 5 && !IsTank(opponent->GetVictim()))
            {
                temptimer = GC_Timer;
                if (doCast(opponent, GetSpell(TORMENT_1)))
                {
                    SetSpellCooldown(TORMENT_1, 5000);
                    GC_Timer = temptimer;
                    return;
                }
            }
        }

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            OnSpellHit(caster, spell);
        }

        void DamageTaken(Unit* u, uint32& /*damage*/)
        {
            if (m_creatureOwner->IsAIEnabled)
                if (bot_minion_ai* ai = m_creatureOwner->GetBotMinionAI())
                    ai->OnOwnerDamagedBy(u);
        }

        //debug
        //void ListSpells(ChatHandler* ch) const
        //{
        //    ch->PSendSysMessage("Spells list:");
        //    ch->PSendSysMessage("Torment: %u", TORMENT);
        //    ch->PSendSysMessage("End of spells list.");
        //}

        void Reset()
        {
            if (master && m_creatureOwner)
            {
                SetStats(true);
                ApplyPassives(PET_TYPE_VOIDWALKER);
                ApplyClassPassives();
                SetBaseArmor(162 * master->getLevel());
            }
        }

        void ReduceCD(uint32 diff)
        {
            CommonTimers(diff);
            SpellTimers(diff);
        }

        bool CanRespawn()
        {return false;}

        void InitSpells()
        {
            InitSpellMap(TORMENT_1);
        }

        void ApplyClassPassives() { }

    private:
        //Timers

        enum VoidwalkerBaseSpells
        {
            TORMENT_1                           = 3716
        };
        enum VoidwalkerPassives
        {
        };
    };
};

void AddSC_warlock_bot()
{
    new warlock_bot();
//    new voidwalker_bot();
}
