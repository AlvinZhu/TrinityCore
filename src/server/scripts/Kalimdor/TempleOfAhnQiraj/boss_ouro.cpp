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
SDName: Boss_Ouro
SD%Complete: 85
SDComment: No model for submerging. Currently just invisible.
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "temple_of_ahnqiraj.h"

enum Spells
{
    SPELL_SWEEP                 = 26103,
    SPELL_SANDBLAST             = 26102,
    SPELL_GROUND_RUPTURE        = 26100,
    SPELL_BIRTH                 = 26262, // The Birth Animation
    SPELL_DIRTMOUND_PASSIVE     = 26092
};

class boss_ouro : public CreatureScript
{
public:
    boss_ouro() : CreatureScript("boss_ouro") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_ouroAI(creature);
    }

    struct boss_ouroAI : public ScriptedAI
    {
        boss_ouroAI(Creature* creature) : ScriptedAI(creature)
        {
			downDike = 15957;
			ouroBeetle = 15718;
			submergedSummon[0] = NULL;
			submergedSummon[1] = NULL;
			Initialize();
        }

		void UnSummonSubmerged()
		{
			for (int i = 0; i < 2; i++)
			{
				if (submergedSummon[i])
				{
					SummonBeetle(5, submergedSummon[i]->GetPosition());
					submergedSummon[i]->SetVisible(false);
					submergedSummon[i]->KillSelf();
					submergedSummon[i]->UnSummon();
					submergedSummon[i] = NULL;
				}
			}
		}

		void SummonSubmerged()
		{
			Position pos = me->GetPosition();
			for (int i = 0; i < 2; i++)
			{
				if (TempSummon* subSummon = me->SummonCreature(downDike, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ()))
				{
					subSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
					subSummon->setFaction(14);
					Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
					if (target)
						subSummon->SetTarget(target->GetGUID());
					else
						subSummon->SetTarget(me->GetTarget());
					subSummon->SetVisible(true);
					subSummon->SetObjectScale(0.8f);
					submergedSummon[i] = subSummon;
				}
			}
		}

		void SummonBeetle(uint32 count, Position basePos)
		{
			if (count == 0 || ouroBeetle == 0)
				return;
			float offset = 12;
			for (uint32 i = 0; i < count; i++)
			{
				float rndX = frand(basePos.GetPositionX() - offset, basePos.GetPositionX() + offset);
				float rndY = frand(basePos.GetPositionY() - offset, basePos.GetPositionY() + offset);
				float rndZ = basePos.GetPositionZ();
				if (TempSummon* subSummon = me->SummonCreature(ouroBeetle, rndX, rndY, rndZ))
				{
					subSummon->UpdateAllowedPositionZ(rndX, rndY, rndZ);
					subSummon->NearTeleportTo(rndX, rndY, rndZ, 0.0f);
					subSummon->setFaction(14);
					subSummon->SetTarget(me->GetTarget());
					subSummon->SetVisible(true);
					subSummon->SetObjectScale(1.1f);
					Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
					if (target)
						subSummon->SetTarget(target->GetGUID());
				}
			}
		}

		Position GetSubmergedPos()
		{
			for (int i = 0; i < 2; i++)
			{
				if (submergedSummon[i] && submergedSummon[i]->IsAlive())
					return submergedSummon[i]->GetPosition();
			}
			return me->GetPosition();
		}

        void Initialize()
        {
            Sweep_Timer = urand(15000, 30000);
            SandBlast_Timer = urand(20000, 35000);
            Submerge_Timer = urand(50000, 60000);
            Back_Timer = urand(30000, 45000);
            ChangeTarget_Timer = urand(5000, 8000);
            Spawn_Timer = urand(10000, 20000);

            Enrage = false;
            Submerged = false;

			me->SetVisible(true);
			UnSummonSubmerged();
		}

		TempSummon* submergedSummon[2];
        uint32 Sweep_Timer;
        uint32 SandBlast_Timer;
        uint32 Submerge_Timer;
        uint32 Back_Timer;
        uint32 ChangeTarget_Timer;
        uint32 Spawn_Timer;

		uint32 downDike;
		uint32 ouroBeetle;

        bool Enrage;
        bool Submerged;

        void Reset() override
        {
            Initialize();
        }

        void EnterCombat(Unit* /*who*/) override
        {
            //DoCastVictim(SPELL_BIRTH);
        }

        void UpdateAI(uint32 diff) override
        {
            //Return since we have no target
			if (!UpdateVictim())
			{
				//me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				Initialize();
				return;
			}

            //Sweep_Timer
			if (!Submerged && Sweep_Timer < 2500)
				BotCruxFleeByRange(me->GetObjectSize() + 30);
            if (!Submerged && Sweep_Timer <= diff)
            {
                DoCastVictim(SPELL_SWEEP);
                Sweep_Timer = urand(15000, 30000);
            } else Sweep_Timer -= diff;

            //SandBlast_Timer
            if (!Submerged && SandBlast_Timer <= diff)
            {
                DoCastVictim(SPELL_SANDBLAST);
				BotBlockCastingMe();
                SandBlast_Timer = urand(20000, 35000);
            } else SandBlast_Timer -= diff;

            //Submerge_Timer
            if (!Submerged && Submerge_Timer <= diff)
            {
                //Cast
                me->HandleEmoteCommand(EMOTE_ONESHOT_SUBMERGE);
                //me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                //me->setFaction(35);
                //DoCast(me, SPELL_DIRTMOUND_PASSIVE);
				me->SetVisible(false);
				ClearBotMeTarget(true);
                Submerged = true;
                Back_Timer = urand(20000, 30000);

				UnSummonSubmerged();
				SummonSubmerged();
				BotCruxFleeByRange(me->GetObjectSize() + 25);
			}
			else Submerge_Timer -= diff;

            //ChangeTarget_Timer
            if (Submerged && ChangeTarget_Timer <= diff)
            {
    //            Unit* target = NULL;
    //            target = SelectTarget(SELECT_TARGET_RANDOM, 0);

				//if (target)
				//{
				//	DoTeleportTo(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
				//}

                ChangeTarget_Timer = urand(10000, 20000);
            } else ChangeTarget_Timer -= diff;

            //Back_Timer
            if (Submerged && Back_Timer <= diff)
            {
                //me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                //me->setFaction(14);

                //DoCastVictim(SPELL_GROUND_RUPTURE);
				DoCastVictim(SPELL_BIRTH);
				DoResetThreat();
				me->SetVisible(true);
				Unit* target = SelectTarget(SELECT_TARGET_NEAREST, 0);
				if (target)
					me->SetTarget(target->GetGUID());
				Submerged = false;
                Submerge_Timer = urand(50000, 60000);
				Sweep_Timer = urand(15000, 30000);
				SandBlast_Timer = urand(20000, 35000);
				UnSummonSubmerged();
			}
			else Back_Timer -= diff;

			if (Submerged)
			{
				me->GetMotionMaster()->Clear();
				me->GetMotionMaster()->MovePoint(1, GetSubmergedPos());
			}

			DoMeleeAttackIfReady();
        }
    };

};

class npc_submerged_ouro : public CreatureScript
{
public:
	npc_submerged_ouro() : CreatureScript("boss_submerged_ouro") { }

	struct npc_submerged_ouroAI : public ScriptedAI
	{
		npc_submerged_ouroAI(Creature* creature) : ScriptedAI(creature) { }

		uint32 SwitchTarget_Timer;

		void Reset() override
		{
			SwitchTarget_Timer = 4000;
		}

		void JustDied(Unit* /*killer*/) override
		{
		}

		void UpdateAI(uint32 diff) override
		{
			if (!me->IsVisible())
			{
				return;
			}
			Unit* target = NULL;
			if (SwitchTarget_Timer <= diff)
			{
				target = SelectTarget(SELECT_TARGET_RANDOM, 0);
				if (target)
					me->SetTarget(target->GetGUID());
				SwitchTarget_Timer = urand(5000, 8000);
			}
			else SwitchTarget_Timer -= diff;

			if (target == NULL)
			{
				if (me->GetTarget() != ObjectGuid::Empty)
					target = ObjectAccessor::FindPlayer(me->GetTarget());
			}
			if (target != NULL)
			{
				me->GetMotionMaster()->Clear();
				me->GetMotionMaster()->MovePoint(1, target->GetPosition());
			}

			BotCruxFleeByRange(me->GetObjectSize() + 25);
		}
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return GetInstanceAI<npc_submerged_ouroAI>(creature);
	}
};

void AddSC_boss_ouro()
{
	new boss_ouro();
	new npc_submerged_ouro();
}
