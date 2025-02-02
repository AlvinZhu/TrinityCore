#include "bot_ai.h"
#include "GameEventMgr.h"
#include "Group.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "WorldSession.h"
/*
Mage NpcBot (reworked by Graff onlysuffering@gmail.com)
Complete - Around 45%
TODO: Ice Lance, Deep Freeze, Mana Gems, Pet etc...
*/
class mage_bot : public CreatureScript
{
public:
    mage_bot() : CreatureScript("mage_bot") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mage_botAI(creature);
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

    struct mage_botAI : public bot_minion_ai
    {
        mage_botAI(Creature* creature) : bot_minion_ai(creature) { }

        bool doCast(Unit* victim, uint32 spellId, bool triggered = false)
        {
            if (CheckBotCast(victim, spellId, CLASS_MAGE) != SPELL_CAST_OK)
                return false;

            bool result = bot_ai::doCast(victim, spellId, triggered);

            if (result && spellId != MANAPOTION && me->HasAura(CLEARCASTBUFF))
            {
                cost = m_botSpellInfo->CalcPowerCost(me, m_botSpellInfo->GetSchoolMask());
                if (cost)
                    clearcast = true;
            }

            return result;
        }

        void EnterCombat(Unit*) { }
        void Aggro(Unit*) { }
        void AttackStart(Unit*) { }
        void KilledUnit(Unit*) { }
        void EnterEvadeMode(EvadeReason /*why*/) { }
        void MoveInLineOfSight(Unit*) { }
        void JustDied(Unit* u) { bot_ai::JustDied(u); }

        void StartAttack(Unit* u, bool force = false)
        {
            if (GetBotCommandState() == COMMAND_ATTACK && !force) return;
            Aggro(u);
            SetBotCommandState(COMMAND_ATTACK);
            GetInPosition(force);
        }

        void Counter()
        {
            Unit* u = me->GetVictim();
            bool cSpell = IsSpellReady(COUNTERSPELL_1, NULL, false, 5000);
            bool blast = IsSpellReady(FIREBLAST_1, NULL, false, 3000) && HasRole(BOT_ROLE_DPS) && !(u && u->ToCreature() && (u->ToCreature()->isWorldBoss() || u->ToCreature()->IsDungeonBoss())) && me->HasAura(IMPACT_BUFF);
            if (!cSpell && !blast) return;
            if (u && u->IsNonMeleeSpellCast(false) &&
                ((cSpell && me->GetDistance(u) < 30) || (blast && me->GetDistance(u) < 30)))
            {
                temptimer = GC_Timer;
                if (me->IsNonMeleeSpellCast(false))
                    me->InterruptNonMeleeSpells(false);
                if (cSpell && doCast(u, GetSpell(COUNTERSPELL_1)))
                    SetSpellCooldown(COUNTERSPELL_1, 15000);
                else if (blast && doCast(u, GetSpell(FIREBLAST_1)))
                    SetSpellCooldown(FIREBLAST_1, 6000);
                GC_Timer = temptimer;
            }
            else if (cSpell)
            {
                if (Unit* target = FindCastingTarget(30))
                {
                    temptimer = GC_Timer;
                    if (me->IsNonMeleeSpellCast(false))
                        me->InterruptNonMeleeSpells(false);
                    if (doCast(target, GetSpell(COUNTERSPELL_1)))
                    {
                        SetSpellCooldown(COUNTERSPELL_1, 15000);
                        GC_Timer = temptimer;
                    }
                }
            }
        }

        void CheckSpellSteal(uint32 diff)
        {
            if (!IsSpellReady(SPELLSTEAL_1, diff) || IsCasting() || Rand() > 25) return;
            Unit* target = FindHostileDispelTarget(30, true);
            if (target && doCast(target, GetSpell(SPELLSTEAL_1)))
                GC_Timer = 800;
        }

        void DoNonCombatActions(uint32 diff)
        {
            if (GC_Timer > diff || me->IsMounted() || Feasting())
                return;

            if (uint32 DAMPENMAGIC = GetSpell(DAMPENMAGIC_1))
            {
                if (!HasAuraName(me, DAMPENMAGIC) &&
                    doCast(me, DAMPENMAGIC))
                    return;
            }

            if (ICEARMOR && !HasAuraName(me, ICEARMOR) &&
                doCast(me, ICEARMOR))
                return;
        }

        bool BuffTarget(Unit* target, uint32 diff)
        {
            if (GC_Timer > diff || Rand() > 20) return false;
            if (me->IsInCombat() && !master->GetMap()->IsRaid()) return false;
            if (me->GetExactDist(target) > 30) return false;

            if (uint32 ARCANEINTELLECT = GetSpell(ARCANEINTELLECT_1))
            {
                if (target->getPowerType() == POWER_MANA && !HasAuraName(target, ARCANEINTELLECT) &&
                    doCast(target, ARCANEINTELLECT))
                    return true;
            }

            return false;
        }

        void UpdateAI(uint32 diff)
        {
            ReduceCD(diff);
            if (IAmDead()) return;
            CheckAttackState();
            if (clearcast && me->HasAura(CLEARCASTBUFF) && !me->IsNonMeleeSpellCast(false))
            {
                me->ModifyPower(POWER_MANA, cost);
                me->RemoveAurasDueToSpell(CLEARCASTBUFF,me->GetGUID(),0,AURA_REMOVE_BY_EXPIRE);
                if (me->HasAura(ARCANE_POTENCY_BUFF1))
                    me->RemoveAurasDueToSpell(ARCANE_POTENCY_BUFF1,me->GetGUID(),0,AURA_REMOVE_BY_EXPIRE);
                if (me->HasAura(ARCANE_POTENCY_BUFF2))
                    me->RemoveAurasDueToSpell(ARCANE_POTENCY_BUFF2,me->GetGUID(),0,AURA_REMOVE_BY_EXPIRE);
                clearcast = false;
            }
            CheckAuras();
            if (wait == 0)
                wait = GetWait();
            else
                return;
            BreakCC(diff);
            if (CCed(me) && (!GetSpell(ICEBLOCK_1) || !me->HasAura(GetSpell(ICEBLOCK_1)))) return; //TODO

            CheckBlink(diff);
            CheckPoly(diff);
            CheckPots(diff);
            CureGroup(master, GetSpell(REMOVE_CURSE_1), diff);

            FocusMagic(diff);
            BuffAndHealGroup(master, diff);

            if (!me->IsInCombat())
                DoNonCombatActions(diff);

            if (!CheckAttackTarget(CLASS_MAGE))
                return;

            CheckPoly2();//this should go AFTER getting opponent

            Counter();
            CheckSpellSteal(diff);
            DoNormalAttack(diff);
        }

        void DoNormalAttack(uint32 diff)
        {
            opponent = me->GetVictim();
uint8 lvl = master->getLevel();
            opponent = me->GetVictim();


            if (!opponent)
		return;

            float angle = me->GetDistance(opponent);

	    if (_isnan(angle))
		return;
            
            if (opponent)
            {
                if (!IsCasting())
                    StartAttack(opponent);
            }
            else
                return;

            AttackerSet m_attackers = master->getAttackers();
            AttackerSet b_attackers = me->getAttackers();

            Unit* u = me->SelectNearestTarget(20);

            float dist = me->GetExactDist(opponent);

	    if (_isnan(dist))
		return;

	    if (dist==0)
		return;

if (IsCasting()) return;

               if (dist < 40 && irand(0,28)==1 && lvl>=80)
                    {
                    doCast(opponent, 40536);    
                    return;    
return;
                }
                   else if (dist < 40 && irand(0,28)==1 && lvl>=80)
                    {
                    doCast(opponent, 65799);        
                    return;
return;
                }
                   else if (dist < 40 && irand(0,28)==1 && lvl>=80)
                    {
                    doCast(opponent, 40598);        
return;
                }


                   else if (dist < 40 && irand(0,28)==1 && lvl>=80)
                    {
                    doCast(opponent, 65807);        
return;
                }
                   else if (dist < 40 && irand(0,28)==1 && lvl>=80)
                    {
                    doCast(opponent, 65792);        
return;
                }



                  if (dist < 10 && irand(0,28)==1 && lvl>=80)
                    {
                    doCast(opponent, 65800);        
return;
                }
                   else if (dist < 40 && irand(0,28)==1 && lvl>=80)
                    {
                    doCast(opponent, 65791);        
return;
                }


            if (irand(0,18)==1 && GetHealthPCT(me) <= 30  && lvl>=80)
            {
               doCast(me, 69923);
return;
            } 

            if ( irand(0,18)==1 && GetHealthPCT(me) <= 10  && lvl>=80 )
            {
                doCast(me, 71736);
                return;
            } 

            if (irand(0,18)==1 && GetHealthPCT(me) < 10)
            {
               doCast(me, 60004);
return;
            }             

            if (irand(0,18)==1 && GetManaPCT(me) < 5)
            {
               doCast(me, 61371);
return;
}



            //ICE_BARRIER
            uint32 ICE_BARRIER = GetSpell(ICE_BARRIER_1);
            if (IsSpellReady(ICE_BARRIER_1, diff, false) && u && u->GetVictim() == me && u->GetDistance(me) < 8 &&
                !me->HasAura(ICE_BARRIER))
            {
                if (me->IsNonMeleeSpellCast(true))
                    me->InterruptNonMeleeSpells(true);
                if (doCast(me, ICE_BARRIER))
                {
                    SetSpellCooldown(ICE_BARRIER_1, 25000);
                    GC_Timer = 800;
                    return;
                }
            }
            if (!IsSpellReady(ICE_BARRIER_1, diff, false) &&
                IsSpellReady(BLINK_1, diff, false, 3000) && u && u->GetVictim() == me &&
                u->GetDistance(me) < 6 && !me->HasAura(ICE_BARRIER))
            {
                if (me->IsNonMeleeSpellCast(true))
                    me->InterruptNonMeleeSpells(true);
                if (doCast(me, GetSpell(BLINK_1)))
                {
                    SetSpellCooldown(BLINK_1, 13000);
                    GC_Timer = 800;
                    return;
                }
            }

            //ICEBLOCK
            if (uint32 ICEBLOCK = GetSpell(ICEBLOCK_1))
            {
                if (IsSpellReady(ICEBLOCK_1, diff, false, 57000))
                {
                    if (((GetManaPCT(me) > 45 && GetHealthPCT(me) > 80) || b_attackers.empty()) &&
                        me->HasAura(ICEBLOCK))
                        me->RemoveAurasDueToSpell(ICEBLOCK);
                }

                if (IsSpellReady(ICEBLOCK_1, diff, false) && !b_attackers.empty() && Rand() < 50 &&
                    (GetManaPCT(me) < 15 || GetHealthPCT(me) < 45 || b_attackers.size() > 4) &&
                    !me->HasAura(ICEBLOCK))
                {
                    if (me->IsNonMeleeSpellCast(true))
                        me->InterruptNonMeleeSpells(true);
                    if (doCast(me, ICEBLOCK))
                    {
                        SetSpellCooldown(ICEBLOCK_1, 60000);
                        Nova_cd = 0; //Glyph of Iceblock
                        return;
                    }
                }
            }

            

            //float dist = me->GetExactDist(opponent);

            uint32 FROSTBOLT = GetSpell(FROSTBOLT_1);
            uint32 FIREBALL = GetSpell(FIREBALL_1);
            uint32 BLASTWAVE = GetSpell(BLASTWAVE_1);
            uint32 FROSTNOVA = GetSpell(FROSTNOVA_1);
            BOLT = (CCed(opponent, true) || !FROSTBOLT) ? FIREBALL : FROSTBOLT;
            NOVA = BOLT == FROSTBOLT && BLASTWAVE && dist > 5 ? BLASTWAVE : FROSTNOVA ? FROSTNOVA : 0;

            if (IsSpellReady(COMBUSTION_1, diff, false) && HasRole(BOT_ROLE_DPS) &&
                (opponent->GetMaxHealth() > master->GetMaxHealth()*6 ||
                m_attackers.size() > 1 || b_attackers.size() > 2) &&
                Rand() < 15 && !HasAuraName(me, COMBUSTION_1))
            {
                temptimer = GC_Timer;
                if (doCast(me, GetSpell(COMBUSTION_1)))
                {
                    SetSpellCooldown(COMBUSTION_1, 60000);
                    GC_Timer = temptimer;
                    //Reset timers for fun
                    ResetSpellCooldown(FIREBLAST_1);
                    ResetSpellCooldown(DRAGONBREATH_1);
                    Nova_cd = 0;
                }
            }
            //DAMAGE
            //PYROBLAST
            if (IsSpellReady(PYROBLAST_1, diff) && opponent->IsPolymorphed() && HasRole(BOT_ROLE_DPS) &&
                (b_attackers.size() < 2 || (*b_attackers.begin()) == opponent) &&
                dist < 35 && Rand() < 75 &&
                doCast(opponent, GetSpell(PYROBLAST_1)))
            {
                SetSpellCooldown(PYROBLAST_1, 7500);
                //debug
                SetSpellCooldown(DRAGONBREATH_1, std::max<uint32>(GetSpellCooldown(DRAGONBREATH_1), uint32(float(sSpellMgr->GetSpellInfo(GetSpell(PYROBLAST_1))->CalcCastTime()/100) * me->GetFloatValue(UNIT_MOD_CAST_SPEED) + 400)));
                Nova_cd = std::max<uint32>(Nova_cd, 450);
                return;
            }
            //nova //TODO: SEPARATE
            u = me->SelectNearestTarget(6.3f);
            if (NOVA && Nova_cd <= diff && HasRole(BOT_ROLE_DPS) && u && Rand() < 75 && !CCed(u, true) && IsInBotParty(u->GetVictim()))
            {
                if (doCast(me, NOVA))
                {
                    Nova_cd = 15000;
                    GetInPosition(true);
                    return;
                }
            }
            //living bomb
            if (IsSpellReady(LIVINGBOMB_1, diff) && HasRole(BOT_ROLE_DPS) && dist < 35 && opponent->GetHealth() > me->GetHealth()/2 &&
                Rand() < 45 && !HasAuraName(opponent, LIVINGBOMB_1, me->GetGUID()) &&
                doCast(opponent, GetSpell(LIVINGBOMB_1)))
            {
                SetSpellCooldown(LIVINGBOMB_1, 6000);
                GC_Timer = 500;
                return;
            }
            //cone of cold
            if (IsSpellReady(CONEOFCOLD_1, diff) && HasRole(BOT_ROLE_DPS) && dist < 7 && Rand() < 50 &&
                me->HasInArc(M_PI*0.75f, opponent) &&
                doCast(opponent, GetSpell(CONEOFCOLD_1)))
            {
                SetSpellCooldown(CONEOFCOLD_1, 8000);
                GC_Timer = 500;
                GetInPosition(true);
                return;
            }
            //dragon's breath
            if (IsSpellReady(DRAGONBREATH_1, diff) && HasRole(BOT_ROLE_DPS) && !CCed(opponent, true) &&
                ((me->HasInArc(M_PI*0.75f, opponent) && dist < 7) ||
                (u && u != opponent && me->HasInArc(M_PI*0.75f, u) && IsInBotParty(u->GetVictim()))) &&
                doCast(me, GetSpell(DRAGONBREATH_1)))
            {
                SetSpellCooldown(DRAGONBREATH_1, 25000);
                GC_Timer = 800;
                return;
            }
            /*//blast wave //TODO Separate again
            u = me->SelectNearestTarget(8);
            if (BLASTWAVE != 0 && u && isTimerReady(BlastWave_cd) &&
                !HasAuraName(u, FROSTNOVA) && !HasAuraName(u, DRAGONBREATH) &&
                doCast(me, BLASTWAVE))
            {
                BlastWave_cd = BLASTWAVE_CD;
                GC_Timer = 800;
            }*/
            //fire blast
            if (IsSpellReady(FIREBLAST_1, diff) && HasRole(BOT_ROLE_DPS) && dist < 25 &&
                Rand() < 20 + 80*(!opponent->isFrozen() && !opponent->HasAuraType(SPELL_AURA_MOD_STUN) && me->HasAura(IMPACT_BUFF)))
            {
                if (doCast(opponent, GetSpell(FIREBLAST_1)))
                {
                    SetSpellCooldown(FIREBLAST_1, 6000);
                    GC_Timer = 500;
                    return;
                }
            }
            //flamestrike - use Improved Flamestrike for instant cast
            if (IsSpellReady(FLAMESTRIKE_1, diff) && HasRole(BOT_ROLE_DPS) && me->HasAura(FIRESTARTERBUFF) && Rand() < 25)
            {
                Unit* FStarget = FindAOETarget(30, true, false);
                if (FStarget && doCast(FStarget, GetSpell(FLAMESTRIKE_1), true))
                {
                    me->RemoveAurasDueToSpell(FIRESTARTERBUFF);
                    GC_Timer = 300;
                    return;
                }
            }
            //blizzard
            if (IsSpellReady(BLIZZARD_1, diff, false) && HasRole(BOT_ROLE_DPS) && !me->isMoving() && Rand() < 40)
            {
                Unit* blizztarget = FindAOETarget(30, true);
                if (blizztarget && doCast(blizztarget, GetSpell(BLIZZARD_1)))
                {
                    SetSpellCooldown(BLIZZARD_1, 5000);
                    return;
                }
                SetSpellCooldown(BLIZZARD_1, 1500); //fail
            }
            //Frost or Fire Bolt
            if (BOLT && Bolt_cd <= diff && HasRole(BOT_ROLE_DPS) && dist < 30 && Rand() < 75 &&
                doCast(opponent, BOLT))
            {
                Bolt_cd = uint32(float(sSpellMgr->GetSpellInfo(BOLT)->CalcCastTime()/100) * me->GetFloatValue(UNIT_MOD_CAST_SPEED) + 200);
                //debug
                SetSpellCooldown(DRAGONBREATH_1, std::max<uint32>(GetSpellCooldown(DRAGONBREATH_1), Bolt_cd + 200));
                Nova_cd = std::max<uint32>(Nova_cd, 450);
                return;
            }
            //Arcane Missiles
            if (IsSpellReady(ARCANEMISSILES_1, diff) && !me->isMoving() && HasRole(BOT_ROLE_DPS) && dist < 20 && Rand() < 15 &&
                doCast(opponent, GetSpell(ARCANEMISSILES_1)))
                return;
        }

        void CheckPoly(uint32 diff)
        {
            if (polyCheckTimer <= diff)
            {
                Polymorph = FindAffectedTarget(GetSpell(POLYMORPH_1), me->GetGUID());
                polyCheckTimer = 2000;
            }
        }

        void CheckPoly2()
        {
            if (Polymorph == false && GC_Timer < 500 && GetSpell(POLYMORPH_1))
            {
                if (Unit* target = FindPolyTarget(30, me->GetVictim()))
                {
                    if (doCast(target, GetSpell(POLYMORPH_1)))
                    {
                        Polymorph = true;
                        polyCheckTimer += 2000;
                    }
                }
            }
        }

        void CheckPots(uint32 diff)
        {
            if (me->IsMounted() || IsCasting())
                return;

            if (GetHealthPCT(me) < 67 && Potion_cd <= diff)
            {
                temptimer = GC_Timer;
                if (doCast(me, HEALINGPOTION))
                {
                    Potion_cd = POTION_CD;
                    GC_Timer = temptimer;
                }
            }
            if (GetManaPCT(me) < 35 && Rand() < 35)
            {
                if (IsSpellReady(EVOCATION_1, diff, false) && !me->isMoving() && me->getAttackers().empty() &&
                    doCast(me, GetSpell(EVOCATION_1)))
                {
                    SetSpellCooldown(EVOCATION_1, 60000);
                    return;
                }
                if (Potion_cd <= diff)
                {
                    temptimer = GC_Timer;
                    if (doCast(me, MANAPOTION))
                    {
                        Potion_cd = POTION_CD;
                        GC_Timer = temptimer;
                    }
                }
            }
        }

        void CheckBlink(uint32 diff)
        {
            if (GetBotCommandState() == COMMAND_STAY || me->IsMounted()) return;
            if (!IsSpellReady(BLINK_1, diff, false) || me->getLevel() < 20 || IsCasting()) return;

            if (!me->IsInCombat() && me->GetExactDist(master) > std::max<uint8>(master->GetBotFollowDist(), 35) &&
                me->HasInArc(M_PI*0.75f, master))
            {
                temptimer = GC_Timer;
                if (doCast(me, GetSpell(BLINK_1)))
                {
                    SetSpellCooldown(BLINK_1, 13000);
                    GC_Timer = temptimer;
                    return;
                }
            }
            if (!me->getAttackers().empty() && me->GetExactDist(master) > 15)
            {
                if (Unit* op = me->SelectNearestTarget(7))
                {
                    if (op->GetVictim() == me)
                    {
                        me->SetFacingTo(me->GetAngle(master));
                        temptimer = GC_Timer;
                        if (doCast(me, GetSpell(BLINK_1)))
                        {
                            SetSpellCooldown(BLINK_1, 13000);
                            GC_Timer = temptimer;
                        }
                    }
                }
            }
        }

        void FocusMagic(uint32 diff)
        {
            if (fmCheckTimer > diff || GC_Timer > diff || me->getLevel() < 20 || IsCasting() || Rand() < 50)
                return;

            uint32 FOCUSMAGIC = GetSpell(FOCUSMAGIC_1);
            if (!FOCUSMAGIC)
                return;

            if (Unit* target = FindAffectedTarget(FOCUSMAGIC, me->GetGUID(), 70, 2))
            {
                fmCheckTimer = 15000;
                return;
            }
            else
            {
                Group* pGroup = master->GetGroup();
                if (!pGroup)
                {
                    if (master->getPowerType() == POWER_MANA && me->GetExactDist(master) < 30 &&
                        !master->HasAura(FOCUSMAGIC))
                    target = master;
                }
                else
                {
                    for (GroupReference* itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
                    {
                        Player* pPlayer = itr->GetSource();
                        if (!pPlayer || pPlayer->IsInWorld() || pPlayer->isDead()) continue;
                        if (me->GetMapId() != pPlayer->GetMapId()) continue;
                        if (pPlayer->getPowerType() == POWER_MANA && me->GetExactDist(pPlayer) < 30 &&
                            !pPlayer->HasAura(FOCUSMAGIC))
                        {
                            target = pPlayer;
                            break;
                        }
                    }
                    if (!target)
                    {
                        for (GroupReference* itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
                        {
                            Player* pPlayer = itr->GetSource();
                            if (!pPlayer || !pPlayer->IsInWorld() || !pPlayer->HaveBot()) continue;
                            if (me->GetMapId() != pPlayer->GetMapId()) continue;
                            for (uint8 i = 0; i != pPlayer->GetMaxNpcBots(); ++i)
                            {
                                Creature* cre = pPlayer->GetBotMap(i)->_Cre();
                                if (!cre || !cre->IsInWorld() || cre == me || cre->isDead()) continue;
                                if (cre->getPowerType() == POWER_MANA && me->GetExactDist(cre) < 30 &&
                                    !cre->HasAura(FOCUSMAGIC))
                                {
                                    target = cre;
                                    break;
                                }
                            }
                        }
                    }
                }

                if (target && doCast(target, FOCUSMAGIC))
                {
                    GC_Timer = 500;
                    fmCheckTimer = 30000;
                    return;
                }
            }

            fmCheckTimer = 5000;
        }

        void ApplyClassDamageMultiplierSpell(int32& damage, SpellNonMeleeDamage& /*damageinfo*/, SpellInfo const* spellInfo, WeaponAttackType /*attackType*/, bool& crit) const
        {
            uint32 spellId = spellInfo->Id;
            uint8 lvl = me->getLevel();
            float fdamage = float(damage);
            //1) apply additional crit chance. This additional chance roll will replace original (balance safe)
            if (!crit)
            {
                float aftercrit = 0.f;
                //Combustion: 10% per stack
                if (SPELL_SCHOOL_MASK_FIRE & spellInfo->GetSchoolMask())
                    if (Aura* combustion = me->GetAura(COMBUSTION_BUFF))
                        aftercrit += float(combustion->GetStackAmount()*10);
                //Incineration: 6% additional critical chance for Fire Blast, Scorch, Arcane Blast and Cone of Cold
                if (lvl >= 10 &&
                    (spellId == GetSpell(FIREBLAST_1) ||
                    spellId == GetSpell(CONEOFCOLD_1)/* ||
                    spellId == ARCANEBLAST ||
                    spellId == SCORCH*/))
                    aftercrit += 6.f;
                //World In Flames: 6% additional critical chance for
                //Flamestrike, Pyroblast, Blast Wave, Dragon's Breath, Living Bomb, Blizzard and Arcane Explosion
                if (lvl >= 15 &&
                    (spellId == GetSpell(FLAMESTRIKE_1) ||
                    spellId == GetSpell(PYROBLAST_1) ||
                    spellId == GetSpell(BLASTWAVE_1) ||
                    spellId == GetSpell(DRAGONBREATH_1)/* ||
                    spellId == ARCANEXPLOSION ||
                    spellId == LIVINGBOMB ||    //cannot be handled here
                    spellId == BLIZZARD*/))    //cannot be handled here
                    aftercrit += 6.f;

                if (aftercrit > 0.f)
                    crit = roll_chance_f(aftercrit);
            }

            //2) apply bonus damage mods
            float pctbonus = 0.0f;
            if (crit)
            {
                //!!!spell damage is not yet critical and will be multiplied by 1.5
                //so we should put here bonus damage mult /1.5
                //Spell Power: 50% additional crit damage bonus for All spells
                if (lvl >= 55)
                    pctbonus += 0.333f;
                //Ice Shards: 50% additional crit damage bonus for Frost spells
                else if (lvl >= 15 && (SPELL_SCHOOL_MASK_FROST & spellInfo->GetSchoolMask()))
                    pctbonus += 0.333f;
            }
            //Improved Cone of Cold: 35% bonus damage for Cone of Cold
            if (lvl >= 30 && spellId == GetSpell(CONEOFCOLD_1))
                pctbonus += 0.35f;
            //Fire Power: 10% bonus damage for Fire spells
            if (lvl >= 35 && (SPELL_SCHOOL_MASK_FIRE & spellInfo->GetSchoolMask()))
                pctbonus += 0.1f;

            damage = int32(fdamage * (1.0f + pctbonus));
        }

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            OnSpellHit(caster, spell);
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (aftercastTargetGuid != 0)
            {
                //only players for now
				if (!aftercastTargetGuid.IsPlayer())
                {
                    aftercastTargetGuid.Clear();
                    return;
                }

				Player* pTarget = ObjectAccessor::FindPlayer(aftercastTargetGuid);
                aftercastTargetGuid .Clear();

                if (!pTarget/* || me->GetDistance(pTarget) > 15*/)
                    return;

                //handle effects
                for (uint8 i = 0; i != MAX_SPELL_EFFECTS; ++i)
                {
                    switch (spell->Effects[i].Effect)
                    {
                        case SPELL_EFFECT_CREATE_ITEM:
                        case SPELL_EFFECT_CREATE_ITEM_2:
                        {
                            uint32 newitemid = spell->Effects[i].ItemType;
                            if (newitemid)
                            {
                                ItemPosCountVec dest;
                                ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(newitemid);
                                if (!pProto)
                                    return;
                                uint32 count = pProto->GetMaxStackSize();
                                uint32 no_space = 0;
                                InventoryResult msg = pTarget->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, newitemid, count, &no_space);
                                if (msg != EQUIP_ERR_OK)
                                {
                                    if (msg == EQUIP_ERR_INVENTORY_FULL || msg == EQUIP_ERR_CANT_CARRY_MORE_OF_THIS)
                                        count -= no_space;
                                    else
                                    {
                                        // if not created by another reason from full inventory or unique items amount limitation
                                        pTarget->SendEquipError(msg, NULL, NULL, newitemid);
                                        continue;
                                    }
                                }
                                if (count)
                                {
                                    Item* pItem = pTarget->StoreNewItem(dest, newitemid, true, Item::GenerateItemRandomPropertyId(newitemid));
                                    if (!pItem)
                                    {
                                        pTarget->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
                                        continue;
                                    }
                                    //unsafe possible
                                    pItem->SetUInt32Value(ITEM_FIELD_CREATOR, me->GetGUIDLow());

                                    pTarget->SendNewItem(pItem, count, true, false, true);
                                }
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }

                return;
            }

            //Winter Veil addition
            uint32 spellId = spell->Id;
            if (sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
            {
                if (SPELL_SCHOOL_MASK_FROST & spell->GetSchoolMask())
                    me->AddAura(44755, target); //snowflakes

                if (spellId == GetSpell(FROSTBOLT_1) && urand(1,100) <= 10)
                    me->CastSpell(target, 25686, true); //10% super snowball
            }
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
            Bolt_cd = 0;
            Nova_cd = 0;
            polyCheckTimer = 0;
            fmCheckTimer = 0;
            Polymorph = false;
            clearcast = false;
            BOLT = 0;
            NOVA = 0;

            if (master)
            {
                SetStats(true);
                InitRoles();
                ApplyPassives(CLASS_MAGE);
           }
        }

        void ReduceCD(uint32 diff)
        {
            CommonTimers(diff);
            SpellTimers(diff);

            if (Bolt_cd > diff)                     Bolt_cd -= diff;
            if (Nova_cd > diff)                     Nova_cd -= diff;

            if (polyCheckTimer > diff)              polyCheckTimer -= diff;
            if (fmCheckTimer > diff)                fmCheckTimer -= diff;
        }

        bool CanRespawn()
        {return false;}

        void InitSpells()
        {
            uint8 lvl = me->getLevel();
            InitSpellMap(DAMPENMAGIC_1);
            InitSpellMap(ARCANEINTELLECT_1);
            InitSpellMap(ARCANEMISSILES_1);
            InitSpellMap(POLYMORPH_1);
            InitSpellMap(COUNTERSPELL_1);
            InitSpellMap(SPELLSTEAL_1);
            InitSpellMap(EVOCATION_1);
            InitSpellMap(BLINK_1);
            InitSpellMap(REMOVE_CURSE_1);
            //InitSpellMap(INVISIBILITY_1);
            InitSpellMap(FIREBALL_1);
  /*Talent*/lvl >= 30 ? InitSpellMap(BLASTWAVE_1) : RemoveSpell(BLASTWAVE_1);
  /*Talent*/lvl >= 40 ? InitSpellMap(DRAGONBREATH_1) : RemoveSpell(DRAGONBREATH_1);
            InitSpellMap(FIREBLAST_1);
  /*Talent*/lvl >= 20 ? InitSpellMap(PYROBLAST_1) : RemoveSpell(PYROBLAST_1);
  /*Talent*/lvl >= 60 ? InitSpellMap(LIVINGBOMB_1) : RemoveSpell(LIVINGBOMB_1);
            InitSpellMap(DAMPENMAGIC_1);
  /*Talent*/lvl >= 50 ? InitSpellMap(COMBUSTION_1) : RemoveSpell(COMBUSTION_1);
            InitSpellMap(FROSTBOLT_1);
            InitSpellMap(FROSTNOVA_1);
            InitSpellMap(CONEOFCOLD_1);
            InitSpellMap(BLIZZARD_1);
 /*Special*/ICEARMOR = lvl >= 20 ? InitSpell(me, ICEARMOR_1) : InitSpell(me, FROSTARMOR_1);
  /*Talent*/lvl >= 40 ? InitSpellMap(ICE_BARRIER_1) : RemoveSpell(ICE_BARRIER_1);
            InitSpellMap(ICEBLOCK_1);
  /*Talent*/lvl >= 20 ? InitSpellMap(FOCUSMAGIC_1) : RemoveSpell(FOCUSMAGIC_1);
        }

        void ApplyClassPassives()
        {
            uint8 level = master->getLevel();
            //Dam+(-Hit)
            if (level >= 50)
                RefreshAura(ARCTIC_WINDS,3); //+15%/-15%
            else if (level >= 25)
                RefreshAura(ARCTIC_WINDS,2); //+10%/-10%
            else if (level >= 10)
                RefreshAura(ARCTIC_WINDS); //+5%/-5%
            //CHILL
            if (level >= 30)
                RefreshAura(WINTERS_CHILL3); //100%
            else if (level >= 25)
                RefreshAura(WINTERS_CHILL2); //66%
            else if (level >= 20)
                RefreshAura(WINTERS_CHILL1); //33%
            //Imp Blizzard
            if (level >= 20)
                RefreshAura(IMPROVED_BLIZZARD); //50% slow
            //Frostbite
            if (level >= 80)
                FROSTBITE = FROSTBITE3;
            else if (level >= 50)
                FROSTBITE = FROSTBITE2;
            else if (level >= 10)
                FROSTBITE = FROSTBITE1;
            if (level >= 60)
                RefreshAura(FROSTBITE,3); //3x
            else if (level >= 30)
                RefreshAura(FROSTBITE,2); //2x
            else if (level >= 10)
                RefreshAura(FROSTBITE); //1x
            //Shattered Barrier
            if (level >= 45)
                RefreshAura(SHATTERED_BARRIER);
            //Bonus
            if (level >= 65)
                RefreshAura(ARCANE_INSTABILITY,4); //+12%dmg crit
            else if (level >= 55)
                RefreshAura(ARCANE_INSTABILITY,3); //+9%dmg crit
            else if (level >= 45)
                RefreshAura(ARCANE_INSTABILITY,2); //+6%dmg crit
            else if (level >= 35)
                RefreshAura(ARCANE_INSTABILITY); //+3%dmg crit
            //Absorb
            if (level >= 50)
                RefreshAura(INCANTERS_ABSORPTION3);
            else if (level >= 45)
                RefreshAura(INCANTERS_ABSORPTION2);
            else if (level >= 40)
                RefreshAura(INCANTERS_ABSORPTION1);
            //Shatter
            if (level >= 35)
                RefreshAura(SHATTER3);
            else if (level >= 30)
                RefreshAura(SHATTER2);
            else if (level >= 25)
                RefreshAura(SHATTER1);
            //ClearCasting
            if (level >= 75)
                RefreshAura(CLEARCAST,3); //30%
            else if (level >= 40)
                RefreshAura(CLEARCAST,2); //20%
            else if (level >= 15)
                RefreshAura(CLEARCAST); //10%
            //Fingers
            if (level >= 45)
                RefreshAura(FINGERS_OF_FROST); //15%
            //Potency
            if (level >= 40)
                RefreshAura(ARCANE_POTENCY2); //30% bonus
            else if (level >= 35)
                RefreshAura(ARCANE_POTENCY1); //15% bonus
            //Ignite
            if (level >= 15)
                RefreshAura(IGNITE);
            //Impact
            if (level >= 60)
                RefreshAura(IMPACT,2);
            else if (level >= 20)
                RefreshAura(IMPACT);
            //Imp. Counterspell
            if (level >= 35)
                RefreshAura(IMPROVED_COUNTERSPELL2); //4 sec
            else if (level >= 25)
                RefreshAura(IMPROVED_COUNTERSPELL1); //2 sec
            //Firestarter
            if (level >= 55)
                RefreshAura(FIRESTARTER2); //100% chance
            else if (level >= 45)
                RefreshAura(FIRESTARTER1); //50% chance
            //Spells
            if (GetSpell(LIVINGBOMB_1))
                RefreshAura(GLYPH_LIVING_BOMB);
            if (GetSpell(POLYMORPH_1))
                RefreshAura(GLYPH_POLYMORPH);
        }

        bool CanUseManually(uint32 basespell) const
        {
            switch (basespell)
            {
                case DAMPENMAGIC_1:
                case ARCANEINTELLECT_1:
                case EVOCATION_1:
                case REMOVE_CURSE_1:
                case FOCUSMAGIC_1:
                case FROSTARMOR_1:
                case ICEARMOR_1:
                    return true;
                default:
                    return false;
            }
        }

    private:
        //Spells
/*frst*/uint32 ICEARMOR;
/*exc.*/uint32 BOLT, NOVA;
/*exc.*/uint32 FROSTBITE;
        //Timers
/*exc.*/uint32 Bolt_cd, Nova_cd;
/*exc.*/uint32 polyCheckTimer, fmCheckTimer;
        //Check
/*exc.*/bool Polymorph, clearcast;

        enum MageBaseSpells
        {
            DAMPENMAGIC_1                       = 604,
            ARCANEINTELLECT_1                   = 1459,
            ARCANEMISSILES_1                    = 5143,
            POLYMORPH_1                         = 118,
            COUNTERSPELL_1                      = 2139,
            SPELLSTEAL_1                        = 30449,
            EVOCATION_1                         = 12051,
            BLINK_1                             = 1953,
            REMOVE_CURSE_1                      = 475,
            //INVISIBILITY_1                      = 0,
            FIREBALL_1                          = 133,
            BLASTWAVE_1                         = 11113,
            DRAGONBREATH_1                      = 31661,
            FIREBLAST_1                         = 2136,
            PYROBLAST_1                         = 11366,
            LIVINGBOMB_1                        = 44457,
            FLAMESTRIKE_1                       = 2120,
            COMBUSTION_1                        = 11129,
            FROSTBOLT_1                         = 116,
            FROSTNOVA_1                         = 122,
            CONEOFCOLD_1                        = 120,
            BLIZZARD_1                          = 10,
            FROSTARMOR_1                        = 168,
            ICEARMOR_1                          = 7302,
            ICE_BARRIER_1                       = 11426,
            ICEBLOCK_1                          = 45438,
            FOCUSMAGIC_1                        = 54646
        };

        enum MagePassives
        {
        //Talents
            SHATTERED_BARRIER                   = 54787,//rank 2
            ARCTIC_WINDS                        = 31678,//rank 5
            WINTERS_CHILL1                      = 11180,
            WINTERS_CHILL2                      = 28592,
            WINTERS_CHILL3                      = 28593,
            FROSTBITE1                          = 11071,
            FROSTBITE2                          = 12496,
            FROSTBITE3                          = 12497,
            IMPROVED_BLIZZARD                   = 12488,//rank 3
            CLEARCAST /*Arcane Concentration*/  = 12577,//rank 5
            ARCANE_POTENCY1                     = 31571,
            ARCANE_POTENCY2                     = 31572,
            SHATTER1                            = 11170,
            SHATTER2                            = 12982,
            SHATTER3                            = 12983,
            INCANTERS_ABSORPTION1               = 44394,
            INCANTERS_ABSORPTION2               = 44395,
            INCANTERS_ABSORPTION3               = 44396,
            FINGERS_OF_FROST                    = 44545,//rank 2
            ARCANE_INSTABILITY                  = 15060,//rank 3
            IMPROVED_COUNTERSPELL1              = 11255,
            IMPROVED_COUNTERSPELL2              = 12598,
            IGNITE                              = 12848,
            FIRESTARTER1                        = 44442,
            FIRESTARTER2                        = 44443,
            IMPACT                              = 12358,
            GLYPH_LIVING_BOMB                   = 63091,
        //Special
            GLYPH_POLYMORPH                     = 56375
        };
        enum MageSpecial
        {
            CLEARCASTBUFF                       = 12536,
            IMPACT_BUFF                         = 64343,
            FIRESTARTERBUFF                     = 54741,
            ARCANE_POTENCY_BUFF1                = 57529,
            ARCANE_POTENCY_BUFF2                = 57531,
            COMBUSTION_BUFF                     = 28682
        };
    };
};

void AddSC_mage_bot()
{
    new mage_bot();
}
