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
SDName: Boss_Netherspite
SD%Complete: 90
SDComment: Not sure about timing and portals placing
SDCategory: Karazhan
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "karazhan.h"
#include "Player.h"
#include "BotGroupAI.h"

enum Netherspite
{
    EMOTE_PHASE_PORTAL          = 0,
    EMOTE_PHASE_BANISH          = 1,

    SPELL_NETHERBURN_AURA       = 30522,
    SPELL_VOIDZONE              = 37063,
    SPELL_NETHER_INFUSION       = 38688,
    SPELL_NETHERBREATH          = 38523,
    SPELL_BANISH_VISUAL         = 39833,
    SPELL_BANISH_ROOT           = 42716,
    SPELL_EMPOWERMENT           = 38549,
    SPELL_NETHERSPITE_ROAR      = 38684,
};


const float PortalCoord[3][3] =
{
    {-11195.353516f, -1613.237183f, 278.237258f}, // Left side
    {-11137.846680f, -1685.607422f, 278.239258f}, // Right side
    {-11094.493164f, -1591.969238f, 279.949188f}  // Back side
};

enum Netherspite_Portal{
    RED_PORTAL = 0, // Perseverence
    GREEN_PORTAL = 1, // Serenity
    BLUE_PORTAL = 2 // Dominance
};

const uint32 PortalID[3] = {17369, 17367, 17368};
const uint32 PortalVisual[3] = {30487, 30490, 30491};
const uint32 PortalBeam[3] = {30465, 30464, 30463};
const uint32 PlayerBuff[3] = {30421, 30422, 30423};
const uint32 NetherBuff[3] = {30466, 30467, 30468};
const uint32 PlayerDebuff[3] = {38637, 38638, 38639};

class boss_netherspite : public CreatureScript
{
public:
    boss_netherspite() : CreatureScript("boss_netherspite") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetInstanceAI<boss_netherspiteAI>(creature);
    }

    struct boss_netherspiteAI : public ScriptedAI
    {
        boss_netherspiteAI(Creature* creature) : ScriptedAI(creature)
        {
            Initialize();
            instance = creature->GetInstanceScript();

            PortalPhase = false;
            PhaseTimer = 0;
            EmpowermentTimer = 0;
            PortalTimer = 0;
        }

        void Initialize()
        {
            Berserk = false;
            NetherInfusionTimer = 540000;
			VoidZoneTimer = 30000;
            NetherbreathTimer = 3000;
			voidZoneFilterPos.clear();
        }

        InstanceScript* instance;

        bool PortalPhase;
        bool Berserk;
        uint32 PhaseTimer; // timer for phase switching
        uint32 VoidZoneTimer;
        uint32 NetherInfusionTimer; // berserking timer
        uint32 NetherbreathTimer;
        uint32 EmpowermentTimer;
        uint32 PortalTimer; // timer for beam checking
        ObjectGuid PortalGUID[3]; // guid's of portals
        ObjectGuid BeamerGUID[3]; // guid's of auxiliary beaming portals
        ObjectGuid BeamTarget[3]; // guid's of portals' current targets
		std::list<Position> voidZoneFilterPos;

        bool IsBetween(WorldObject* u1, WorldObject* target, WorldObject* u2) // the in-line checker
        {
            if (!u1 || !u2 || !target)
                return false;

            float xn, yn, xp, yp, xh, yh;
            xn = u1->GetPositionX();
            yn = u1->GetPositionY();
            xp = u2->GetPositionX();
            yp = u2->GetPositionY();
            xh = target->GetPositionX();
            yh = target->GetPositionY();

            // check if target is between (not checking distance from the beam yet)
			//float twoPointDist = dist(xn, yn, xp, yp);
			//float toPoint1Dist = dist(xn, yn, xh, yh);
			//float toPoint2Dist = dist(xp, yp, xh, yh);
			//if (toPoint1Dist >= twoPointDist || toPoint2Dist >= twoPointDist)
			//	return false;
			//if (toPoint1Dist + toPoint2Dist < twoPointDist + 2.0f)
			//	return true;
			//return false;
            if (dist(xn, yn, xh, yh) >= dist(xn, yn, xp, yp) || dist(xp, yp, xh, yh) >= dist(xn, yn, xp, yp))
                return false;
            // check  distance from the beam
            return (std::abs((xn-xp)*yh+(yp-yn)*xh-xn*yp+xp*yn)/dist(xn, yn, xp, yp) < 1.5f);
        }

        float dist(float xa, float ya, float xb, float yb) // auxiliary method for distance
        {
            return std::sqrt((xa-xb)*(xa-xb) + (ya-yb)*(ya-yb));
        }

        void Reset() override
        {
            Initialize();

            HandleDoors(true);
            DestroyPortals();
        }

		void ProcessBotDebuffLeave()
		{
			std::list<Player*> players;
			GetInViewBotPlayers(players, 80);
			for (std::list<Player*>::iterator itPlayer = players.begin(); itPlayer != players.end(); itPlayer++)
			{
				Player* player = *itPlayer;
				if (player->HasAura(PlayerDebuff[0]) || player->HasAura(PlayerDebuff[1]) || player->HasAura(PlayerDebuff[2]))
				{
					if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
					{
						pGroupAI->RndCruxMovement(15);
					}
				}
			}
		}

		void ProcessBotPortals()
		{
			std::list<Player*> players;
			GetInViewBotPlayers(players, 80);
			G3D::Vector3 basePosv = me->GetPosition().GetVector3();
			for (int i = 0; i < 3; ++i)
			{
				Creature* portal = ObjectAccessor::GetCreature(*me, PortalGUID[i]);
				if (!portal)
					continue;
				G3D::Vector3 dir = (portal->GetPosition().GetVector3() - basePosv).direction();
				float dist0 = me->GetDistance(portal->GetPosition());
				float dist1 = me->GetObjectSize() * 0.85f;
				float dist2 = dist1 + (dist0 - dist1) * 0.5f;
				float dist3 = dist2 + (dist0 - dist1) * 0.5f;
				Position poss[3];
				poss[0] = Position(basePosv + dir * dist1);
				poss[1] = Position(basePosv + dir * dist2);
				poss[2] = Position(basePosv + dir * dist3);
				float nearDists[3] = { 999, 999, 999 };
				BotGroupAI* nearBotAIs[3] = { NULL, NULL, NULL };
				for (std::list<Player*>::iterator itPlayer = players.begin(); itPlayer != players.end(); itPlayer++)
				{
					Player* player = *itPlayer;
					if (BotGroupAI* pBotAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
					{
						if (player->HasAura(PlayerDebuff[i]))
						{
							if (IsBetween(me, player, portal))
								pBotAI->RndCruxMovement(15);
							continue;
						}
						if (pBotAI->IsTankBotAI() || pBotAI->IsHealerBotAI())
							continue;
						if (pBotAI->IsMeleeBotAI())
						{
							float d1 = player->GetDistance(poss[0]);
							if (d1 < nearDists[0])
							{
								nearDists[0] = d1;
								nearBotAIs[0] = pBotAI;
							}
						}
						else if (pBotAI->IsRangeBotAI())
						{
							float d1 = player->GetDistance(poss[1]);
							float d2 = player->GetDistance(poss[2]);
							if (d1 < nearDists[1])
							{
								nearDists[1] = d1;
								nearBotAIs[1] = pBotAI;
							}
							if (d2 < nearDists[2])
							{
								nearDists[2] = d1;
								nearBotAIs[2] = pBotAI;
							}
						}
					}
				}
				float selectDist = 999;
				BotGroupAI* selectBotAI = NULL;
				int s = 0;
				for (int k = 0; k < 3; k++)
				{
					if (nearDists[k] < selectDist)
					{
						selectDist = nearDists[k];
						selectBotAI = nearBotAIs[k];
						s = k;
					}
				}
				if (selectBotAI)
				{
					for (std::list<Player*>::iterator itPlayer = players.begin(); itPlayer != players.end(); itPlayer++)
					{
						Player* player = *itPlayer;
						if (selectBotAI->GetAIPayer() == player)
						{
							players.erase(itPlayer);
							break;
						}
					}
					poss[s].m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), poss[s].GetPositionX(), poss[s].GetPositionY(), poss[s].GetPositionZ());
					selectBotAI->SetMovetoHaltPos(poss[s]);
				}
			}
		}

        void SummonPortals()
        {
            uint8 r = rand32() % 4;
            uint8 pos[3];
            pos[RED_PORTAL] = ((r % 2) ? (r > 1 ? 2 : 1) : 0);
            pos[GREEN_PORTAL] = ((r % 2) ? 0 : (r > 1 ? 2 : 1));
            pos[BLUE_PORTAL] = (r > 1 ? 1 : 2); // Blue Portal not on the left side (0)

            for (int i = 0; i < 3; ++i)
                if (Creature* portal = me->SummonCreature(PortalID[i], PortalCoord[pos[i]][0], PortalCoord[pos[i]][1], PortalCoord[pos[i]][2], 0, TEMPSUMMON_TIMED_DESPAWN, 60000))
                {
                    PortalGUID[i] = portal->GetGUID();
                    portal->AddAura(PortalVisual[i], portal);
                }
			ProcessBotPortals();
		}

        void DestroyPortals()
        {
            for (int i=0; i<3; ++i)
            {
                if (Creature* portal = ObjectAccessor::GetCreature(*me, PortalGUID[i]))
                    portal->DisappearAndDie();
                if (Creature* portal = ObjectAccessor::GetCreature(*me, BeamerGUID[i]))
                    portal->DisappearAndDie();
                PortalGUID[i].Clear();
                BeamTarget[i].Clear();
            }
        }

		Player* GetRealPlayerSelect()
		{
			Map::PlayerList const& players = me->GetMap()->GetPlayers();
			for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
			{
				Player* p = i->GetSource();
				if (!p->IsPlayerBot())
				{
					return p->GetSelectedPlayer();
				}
			}
			return NULL;
		}

        void UpdatePortals() // Here we handle the beams' behavior
        {
			voidZoneFilterPos.clear();
            for (int j = 0; j < 3; ++j) // j = color
                if (Creature* portal = ObjectAccessor::GetCreature(*me, PortalGUID[j]))
                {
                    // the one who's been cast upon before
                    Unit* current = ObjectAccessor::GetUnit(*portal, BeamTarget[j]);
                    // temporary store for the best suitable beam reciever
                    Unit* target = me;

                    Map::PlayerList const& players = me->GetMap()->GetPlayers();

                    // get the best suitable target
                    for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
                    {
                        Player* p = i->GetSource();
						if (!p || !p->IsAlive())
							continue;
						if (target && target != me && target->GetDistance2d(portal) < p->GetDistance2d(portal))
							continue;
						if (p->HasAura(PlayerDebuff[j]))
							continue;
						if (p->HasAura(PlayerBuff[(j + 1) % 3]) || p->HasAura(PlayerBuff[(j + 2) % 3]))
							continue;
						if (!IsBetween(me, p, portal))
							continue;
                        //if (p && p->IsAlive() // alive
                        //    && (!target || target->GetDistance2d(portal)>p->GetDistance2d(portal)) // closer than current best
                        //    && !p->HasAura(PlayerDebuff[j]) // not exhausted
                        //    && !p->HasAura(PlayerBuff[(j + 1) % 3]) // not on another beam
                        //    && !p->HasAura(PlayerBuff[(j + 2) % 3])
                        //    && IsBetween(me, p, portal)) // on the beam
                        target = p;
                    }

                    // buff the target
					if (target->GetTypeId() == TYPEID_PLAYER)
					{
						voidZoneFilterPos.push_back(target->GetPosition());
						target->AddAura(PlayerBuff[j], target);
					}
                    else
                        target->AddAura(NetherBuff[j], target);
                    // cast visual beam on the chosen target if switched
                    // simple target switching isn't working -> using BeamerGUID to cast (workaround)
                    if (!current || target != current)
                    {
                        BeamTarget[j] = target->GetGUID();
                        // remove currently beaming portal
                        if (Creature* beamer = ObjectAccessor::GetCreature(*portal, BeamerGUID[j]))
                        {
                            beamer->CastSpell(target, PortalBeam[j], false);
                            beamer->DisappearAndDie();
                            BeamerGUID[j].Clear();
                        }
                        // create new one and start beaming on the target
                        if (Creature* beamer = portal->SummonCreature(PortalID[j], portal->GetPositionX(), portal->GetPositionY(), portal->GetPositionZ(), portal->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000))
                        {
                            beamer->CastSpell(target, PortalBeam[j], false);
                            BeamerGUID[j] = beamer->GetGUID();
                        }
                    }
                    // aggro target if Red Beam
                    if (j == RED_PORTAL && me->GetVictim() != target && target->GetTypeId() == TYPEID_PLAYER)
                        me->getThreatManager().addThreat(target, 100000.0f+DoGetThreat(me->GetVictim()));
                }
        }

        void SwitchToPortalPhase()
        {
            me->RemoveAurasDueToSpell(SPELL_BANISH_ROOT);
            me->RemoveAurasDueToSpell(SPELL_BANISH_VISUAL);
            SummonPortals();
            PhaseTimer = 60000;
            PortalPhase = true;
            PortalTimer = 10000;
            EmpowermentTimer = 10000;
            Talk(EMOTE_PHASE_PORTAL);
        }

        void SwitchToBanishPhase()
        {
            me->RemoveAurasDueToSpell(SPELL_EMPOWERMENT);
            me->RemoveAurasDueToSpell(SPELL_NETHERBURN_AURA);
            DoCast(me, SPELL_BANISH_VISUAL, true);
            DoCast(me, SPELL_BANISH_ROOT, true);
            DestroyPortals();
            PhaseTimer = 30000;
            PortalPhase = false;
            Talk(EMOTE_PHASE_BANISH);

            for (uint8 i = 0; i < 3; ++i)
                me->RemoveAurasDueToSpell(NetherBuff[i]);
        }

        void HandleDoors(bool open) // Massive Door switcher
        {
            if (GameObject* Door = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(DATA_GO_MASSIVE_DOOR) ))
                Door->SetGoState(open ? GO_STATE_ACTIVE : GO_STATE_READY);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            HandleDoors(false);
            SwitchToPortalPhase();
			if (instance)
				instance->SetBossState(DATA_NETHERSPITE, IN_PROGRESS);
		}

        void JustDied(Unit* killer) override
        {
            HandleDoors(true);
            DestroyPortals();
			if (instance)
				instance->SetBossState(DATA_NETHERSPITE, DONE);
        }

		bool CanCastVoidZoneByTarget(Unit* pTarget)
		{
			if (!pTarget)
				return false;
			for (Position& pos : voidZoneFilterPos)
			{
				if (pTarget->GetDistance(pos) <= 10.0f)
					return false;
			}
			return true;
		}

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            // Void Zone
            if (VoidZoneTimer <= diff)
            {
				Unit* selPlayer = NULL;
				for (int x = 0; x < 10; x++)
				{
					selPlayer = SelectTarget(SELECT_TARGET_RANDOM, 1, 45, true);
					if (!CanCastVoidZoneByTarget(selPlayer))
						continue;
					if (selPlayer)
					{
						DoCast(selPlayer, SPELL_VOIDZONE, true);
						BotCruxFleeByArea(10.0f, 20.0f, selPlayer);
					}
					break;
				}
                VoidZoneTimer = 30000;
            } else VoidZoneTimer -= diff;

            // NetherInfusion Berserk
            //if (!Berserk && NetherInfusionTimer <= diff)
            //{
            //    me->AddAura(SPELL_NETHER_INFUSION, me);
            //    DoCast(me, SPELL_NETHERSPITE_ROAR);
            //    Berserk = true;
            //} else NetherInfusionTimer -= diff;

            if (PortalPhase) // PORTAL PHASE
            {
                // Distribute beams and buffs
				if (PortalTimer <= diff)
				{
					UpdatePortals();
					ProcessBotPortals();
					PortalTimer = 1000;
				}
				else PortalTimer -= diff;
				
				// Empowerment & Nether Burn
                //if (EmpowermentTimer <= diff)
                //{
                //    DoCast(me, SPELL_EMPOWERMENT);
                //    me->AddAura(SPELL_NETHERBURN_AURA, me);
                //    EmpowermentTimer = 90000;
                //} else EmpowermentTimer -= diff;

                if (PhaseTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCast(false))
                    {
                        SwitchToBanishPhase();
                        return;
                    }
                } else PhaseTimer -= diff;
            }
            else // BANISH PHASE
            {
                // Netherbreath
                //if (NetherbreathTimer <= diff)
                //{
                //    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40, true))
                //        DoCast(target, SPELL_NETHERBREATH);
                //    NetherbreathTimer = urand(5000, 7000);
                //} else NetherbreathTimer -= diff;

				if (PhaseTimer <= diff)
				{
					if (!me->IsNonMeleeSpellCast(false))
					{
						SwitchToPortalPhase();
						return;
					}
				}
				else PhaseTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };
};

void AddSC_boss_netherspite()
{
    new boss_netherspite();
}
