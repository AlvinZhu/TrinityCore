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

#include "Arena.h"
#include "ArenaScore.h"
#include "ArenaTeamMgr.h"
#include "Language.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "World.h"
#include "WorldSession.h"
#include "PlayerBotMgr.h"
#include "BotAI.h"
#include "SpellAuras.h"

Arena::Arena()
{
	m_UpdateTick = 0;
	m_LastCommandTeam = TEAM_NEUTRAL;

    StartDelayTimes[BG_STARTING_EVENT_FIRST]  = BG_START_DELAY_1M;
    StartDelayTimes[BG_STARTING_EVENT_SECOND] = BG_START_DELAY_30S;
    StartDelayTimes[BG_STARTING_EVENT_THIRD]  = BG_START_DELAY_15S;
    StartDelayTimes[BG_STARTING_EVENT_FOURTH] = BG_START_DELAY_NONE;

    StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_ARENA_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_ARENA_THIRTY_SECONDS;
    StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_ARENA_FIFTEEN_SECONDS;
    StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_ARENA_HAS_BEGUN;
}

void Arena::Update(uint32 diff)
{
	m_UpdateTick += diff;
	if (m_UpdateTick < 1000)
	{
		InsureArenaAllPlayerFlag();
		Battleground::Update(diff);
		return;
	}
	m_UpdateTick = 0;

	BattlegroundStatus bgState = GetStatus();
	if (bgState == STATUS_NONE || bgState == STATUS_WAIT_QUEUE)
	{
		InsureArenaAllPlayerFlag();
		m_LastCommandTeam = TEAM_NEUTRAL;
		Battleground::Update(diff);
		return;
	}
	int32 needCommandTeam = 0;
	PLAYERS alliancePlayers;
	PLAYERS hordePlayers;
	for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
	{
		if (Player* player = ObjectAccessor::FindPlayer(itr->first))
		{
			if (!player->IsAlive())
				continue;
			if (player->GetTeamId() == TEAM_ALLIANCE)
				alliancePlayers.push_back(player);
			else if (player->GetTeamId() == TEAM_HORDE)
				hordePlayers.push_back(player);
			bool isPlayerBot = player->IsPlayerBot();
			if (needCommandTeam >= 0)
			{
				if (player->GetTeamId() == TEAM_ALLIANCE)
				{
					if (!isPlayerBot)
					{
						if (needCommandTeam == 1)
						{
							needCommandTeam = -1;
							m_LastCommandTeam = TEAM_NEUTRAL;
						}
						else
						{
							needCommandTeam = 2;
							m_LastCommandTeam = TEAM_HORDE;
						}
					}
				}
				else if (player->GetTeamId() == TEAM_HORDE)
				{
					if (!isPlayerBot)
					{
						if (needCommandTeam == 2)
						{
							needCommandTeam = -1;
							m_LastCommandTeam = TEAM_NEUTRAL;
						}
						else
						{
							needCommandTeam = 1;
							m_LastCommandTeam = TEAM_ALLIANCE;
						}
					}
				}
			}

			if (!isPlayerBot || player->GetBattleground() != this)
				continue;
			if (BotBGAI* pBotAI = dynamic_cast<BotBGAI*>(player->GetAI()))
			{
				switch (bgState)
				{
				case BattlegroundStatus::STATUS_WAIT_JOIN:
					pBotAI->ReadyBattleground();
					m_StartTryMount = false;
					break;
				case BattlegroundStatus::STATUS_IN_PROGRESS:
					pBotAI->StartBattleground();
					break;
				case BattlegroundStatus::STATUS_WAIT_LEAVE:
					pBotAI->LeaveBattleground();
					m_StartTryMount = false;
					break;
				}
			}
		}
	}
	if (bgState == BattlegroundStatus::STATUS_IN_PROGRESS)
	{
		if (needCommandTeam > 0 && needCommandTeam < 3)
			CommandCombat((needCommandTeam == 1) ? TEAM_ALLIANCE : TEAM_HORDE, alliancePlayers, hordePlayers);
		else if (m_LastCommandTeam != TEAM_NEUTRAL)
			CommandCombat(m_LastCommandTeam, alliancePlayers, hordePlayers);
	}
	InsureArenaAllPlayerFlag();
	Battleground::Update(diff);
}

void Arena::AddPlayer(Player* player)
{
    Battleground::AddPlayer(player);
#ifdef CONVERT_ARENAAI_TOBG
	if (BotBGAI* pBotAI = dynamic_cast<BotBGAI*>(player->GetAI()))
	{
		pBotAI->ResetBotAI();
	}
#endif
    PlayerScores[player->GetGUID().GetCounter()] = new ArenaScore(player->GetGUID(), player->GetBGTeam());

	InsureArenaFlag(player, true);

    UpdateArenaWorldState();
}

void Arena::RemovePlayer(Player* player, ObjectGuid /*guid*/, uint32 /*team*/)
{
	BotUtility::RemoveArenaBotSpellsByPlayer(player);
	if (GetStatus() == STATUS_WAIT_LEAVE)
        return;

	UpdateArenaWorldState();
    CheckWinConditions();
}

void Arena::FillInitialWorldStates(WorldPacket& data)
{
    data << uint32(ARENA_WORLD_STATE_ALIVE_PLAYERS_GREEN) << uint32(GetAlivePlayersCountByTeam(HORDE));
    data << uint32(ARENA_WORLD_STATE_ALIVE_PLAYERS_GOLD) << uint32(GetAlivePlayersCountByTeam(ALLIANCE));
}

void Arena::UpdateArenaWorldState()
{
    UpdateWorldState(ARENA_WORLD_STATE_ALIVE_PLAYERS_GREEN, GetAlivePlayersCountByTeam(HORDE));
    UpdateWorldState(ARENA_WORLD_STATE_ALIVE_PLAYERS_GOLD, GetAlivePlayersCountByTeam(ALLIANCE));
}

void Arena::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    Battleground::HandleKillPlayer(player, killer);

    UpdateArenaWorldState();
    CheckWinConditions();
}

void Arena::CommandCombat(TeamId commandTeam, PLAYERS& alliances, PLAYERS& hordes)
{
	if (commandTeam == TEAM_NEUTRAL)
		return;
	if (alliances.empty() || hordes.empty())
		return;
	PLAYERS& comLists = (commandTeam == TEAM_ALLIANCE) ? alliances : hordes;
	PLAYERS& enemyLists = (commandTeam == TEAM_ALLIANCE) ? hordes : alliances;
	if (!m_StartTryMount)
	{
		m_StartTryMount = true;
		if (isRated())
		{
			for (Player* self : comLists)
			{
				BotUtility::AddArenaBotSpellsByPlayer(self);
			}
		}
		for (Player* player : alliances)
		{
			if (BotBGAI* pBotAI = dynamic_cast<BotBGAI*>(player->GetAI()))
				pBotAI->TryUpMount();
		}
		for (Player* player : hordes)
		{
			if (BotBGAI* pBotAI = dynamic_cast<BotBGAI*>(player->GetAI()))
				pBotAI->TryUpMount();
		}
	}

	//PLAYERS canSelectEnemys;
	//for (Player* enemyPlayer : enemyLists)
	//{
	//	if (!CanSelectTarget(enemyPlayer))
	//		continue;
	//	canSelectEnemys.push_back(enemyPlayer);
	//}
	//if (canSelectEnemys.empty())
	//	return;
	if (AssignTactics(comLists, enemyLists))
		return;
	if (ExistMightinessHealer(enemyLists))
	{
		SuppressHealerPlayer(comLists, enemyLists);
		for (Player* enemyPlayer : enemyLists)
		{
			if (!IsMightiness(enemyPlayer))
				continue;
			if (!CanSelectTarget(enemyPlayer))
				continue;
			for (Player* comPlayer : comLists)
				comPlayer->SetSelection(enemyPlayer->GetGUID());
		}
		for (Player* enemyPlayer : enemyLists)
		{
			if (!IsRangeBot(enemyPlayer))
				continue;
			if (!CanSelectTarget(enemyPlayer))
				continue;
			for (Player* comPlayer : comLists)
				comPlayer->SetSelection(enemyPlayer->GetGUID());
			return;
		}
		NormalTactics(comLists, enemyLists);
		return;
	}
	else if (HasFullSuppressSpell(comLists))
	{
		if (ExistMightinessClasses(enemyLists))
		{
			SuppressMightinessPlayer(comLists, enemyLists);
			NormalTactics(comLists, enemyLists);
			return;
		}
		else if (CanFullHealerEnemy2(enemyLists))
		{
			SuppressRealPlayer(comLists, enemyLists);
			NormalTactics(comLists, enemyLists);
			return;
		}
		else if (ExistRealPlayerByRange(enemyLists))
		{
			SuppressHealerPlayer(comLists, enemyLists);
			for (Player* enemyPlayer : enemyLists)
			{
				if (enemyPlayer->IsPlayerBot() || enemyPlayer->IsMounted())
					continue;
				if (!CanSelectTarget(enemyPlayer))
					continue;
				for (Player* comPlayer : comLists)
					comPlayer->SetSelection(enemyPlayer->GetGUID());
				return;
			}
			NormalTactics(comLists, enemyLists);
			return;
		}
		else
		{
			SuppressRealPlayer(comLists, enemyLists);
			NormalTactics(comLists, enemyLists);
			return;
		}
	}
	else if (CanFullHealerEnemy(enemyLists))
	{
		SuppressRealPlayer(comLists, enemyLists);
		NormalTactics(comLists, enemyLists);
		return;
	}
	else if (NeedHarassHealer(comLists, enemyLists))
	{
		for (Player* self : comLists)
		{
			if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(self->GetAI()))
				pAI->ClearCruxControlCommand();
		}
		FollowEnemyHealer(comLists, enemyLists);
		for (Player* enemyPlayer : enemyLists)
		{
			if (enemyPlayer->IsPlayerBot() || enemyPlayer->IsMounted())
				continue;
			if (!CanSelectTarget(enemyPlayer))
				continue;
			for (Player* comPlayer : comLists)
				comPlayer->SetSelection(enemyPlayer->GetGUID());
			return;
		}
		NormalTactics(comLists, enemyLists);
		return;
	}
	else
	{
		SuppressHealerPlayer(comLists, enemyLists);
		for (Player* enemyPlayer : enemyLists)
		{
			if (enemyPlayer->IsPlayerBot() || enemyPlayer->IsMounted())
				continue;
			if (!CanSelectTarget(enemyPlayer))
				continue;
			for (Player* comPlayer : comLists)
				comPlayer->SetSelection(enemyPlayer->GetGUID());
			return;
		}
	}

	for (Player* enemyPlayer : enemyLists)
	{
		if (!IsHealerBot(enemyPlayer))
			continue;
		if (!CanSelectTarget(enemyPlayer))
			continue;
		for (Player* comPlayer : comLists)
			comPlayer->SetSelection(enemyPlayer->GetGUID());
		return;
	}
	for (Player* enemyPlayer : enemyLists)
	{
		if (!IsRangeBot(enemyPlayer))
			continue;
		if (!CanSelectTarget(enemyPlayer))
			continue;
		for (Player* comPlayer : comLists)
			comPlayer->SetSelection(enemyPlayer->GetGUID());
		return;
	}
	for (Player* enemyPlayer : enemyLists)
	{
		if (!IsMeleeBot(enemyPlayer))
			continue;
		if (!CanSelectTarget(enemyPlayer))
			continue;
		for (Player* comPlayer : comLists)
			comPlayer->SetSelection(enemyPlayer->GetGUID());
		return;
	}
	for (Player* enemyPlayer : enemyLists)
	{
		if (!CanSelectTarget(enemyPlayer))
			continue;
		for (Player* comPlayer : comLists)
			comPlayer->SetSelection(enemyPlayer->GetGUID());
		return;
	}
}

void Arena::NormalTactics(PLAYERS& comLists, PLAYERS& canSelectEnemys)
{
	for (Player* enemyPlayer : canSelectEnemys)
	{
		if (!IsHealerBot(enemyPlayer))
			continue;
		if (!CanSelectTarget(enemyPlayer))
			continue;
		for (Player* comPlayer : comLists)
			comPlayer->SetSelection(enemyPlayer->GetGUID());
		return;
	}
	if (Player* target = FindPriorAttackTarget(canSelectEnemys))
	{
		for (Player* comPlayer : comLists)
			comPlayer->SetSelection(target->GetGUID());
		return;
	}
	for (Player* enemyPlayer : canSelectEnemys)
	{
		if (!IsRangeBot(enemyPlayer))
			continue;
		if (!CanSelectTarget(enemyPlayer))
			continue;
		for (Player* comPlayer : comLists)
			comPlayer->SetSelection(enemyPlayer->GetGUID());
		return;
	}
	for (Player* enemyPlayer : canSelectEnemys)
	{
		if (!IsMeleeBot(enemyPlayer))
			continue;
		if (!CanSelectTarget(enemyPlayer))
			continue;
		for (Player* comPlayer : comLists)
			comPlayer->SetSelection(enemyPlayer->GetGUID());
		return;
	}
	for (Player* enemyPlayer : canSelectEnemys)
	{
		if (!CanSelectTarget(enemyPlayer))
			continue;
		for (Player* comPlayer : comLists)
			comPlayer->SetSelection(enemyPlayer->GetGUID());
		return;
	}
}

bool Arena::NeedHarassHealer(PLAYERS& selfPlayer, PLAYERS& enemyPlayer)
{
	uint32 existMelee = 0;
	for (Player* self : selfPlayer)
	{
		if (IsAllMeleeBot(self))
		{
			++existMelee;
		}
	}
	if (existMelee < 1)
		return false;
	if (CanFullSuppressPlayer(enemyPlayer))
		return false;

	return true;
}

bool Arena::HasFullSuppressSpell(PLAYERS& selfPlayer)
{
	std::set<uint32> fullCtrlers;
	std::set<uint32> onceCtrlers;
	for (Player* self : selfPlayer)
	{
		Classes cls = Classes(self->getClass());
		switch (cls)
		{
		case CLASS_WARRIOR:
		case CLASS_DEATH_KNIGHT:
		case CLASS_PRIEST:
			break;
		case CLASS_PALADIN:
			if (self->FindTalentType() == 2)
			{
				if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(self->GetAI()))
					onceCtrlers.insert(pAI->GetControlSpellDiminishingGroup());
			}
			break;
		case CLASS_ROGUE:
		case CLASS_HUNTER:
		case CLASS_SHAMAN:
			if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(self->GetAI()))
				onceCtrlers.insert(pAI->GetControlSpellDiminishingGroup());
			break;
		case CLASS_MAGE:
		case CLASS_WARLOCK:
		case CLASS_DRUID:
			if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(self->GetAI()))
				fullCtrlers.insert(pAI->GetControlSpellDiminishingGroup());
			break;
		}
	}
	if (fullCtrlers.size() >= 2)
	{
		for (Player* self : selfPlayer)
		{
			if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(self->GetAI()))
				pAI->SetReserveCtrlSpell(true);
		}
		return true;
	}
	if (fullCtrlers.size() >= 1 && onceCtrlers.size() >= 2)
	{
		for (Player* self : selfPlayer)
		{
			if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(self->GetAI()))
				pAI->SetReserveCtrlSpell(true);
		}
		return true;
	}

	for (Player* self : selfPlayer)
	{
		if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(self->GetAI()))
		{
			pAI->SetReserveCtrlSpell(false);
			pAI->ClearCruxControlCommand();
		}
	}
	return false;
}

bool Arena::CanFullSuppressPlayer(PLAYERS& enemyPlayer)
{
	for (Player* enemy : enemyPlayer)
	{
		if (enemy->IsPlayerBot())
			continue;
		Classes cls = Classes(enemy->getClass());
		switch (cls)
		{
		case CLASS_WARRIOR:
		case CLASS_PALADIN:
		case CLASS_ROGUE:
		case CLASS_DEATH_KNIGHT:
			return false;
		case CLASS_MAGE:
		case CLASS_WARLOCK:
		case CLASS_PRIEST:
		case CLASS_HUNTER:
		case CLASS_SHAMAN:
			return true;
		case CLASS_DRUID:
			return (enemy->FindTalentType() != 1);
		}
		break;
	}
	return false;
}

bool Arena::SuppressRealPlayer(PLAYERS& selfPlayer, PLAYERS& enemyPlayer)
{
	if (enemyPlayer.size() < 2 || !ExistControler(selfPlayer))
	{
		for (Player* self : selfPlayer)
		{
			if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(self->GetAI()))
				pAI->ClearCruxControlCommand();
		}
		return false;
	}
	for (PLAYERS::iterator itEnemy = enemyPlayer.begin();
		itEnemy != enemyPlayer.end();
		itEnemy++)
	{
		if ((*itEnemy)->IsPlayerBot())// || IsRangeBot(*itEnemy))
			continue;
		for (Player* sets : selfPlayer)
		{
			if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(sets->GetAI()))
			{
				pAI->SetReserveCtrlSpell(true);
				pAI->SetNonSelectTarget((*itEnemy)->GetGUID());
			}
		}
		if (MeleeTargetIsSuppress(*itEnemy))
		{
			enemyPlayer.erase(itEnemy);
			return true;
		}
		if (ExistCtrlSpellCasting(selfPlayer))
		{
			enemyPlayer.erase(itEnemy);
			return true;
		}
		bool canRemove = false;
		for (PLAYERS::iterator itSelf = selfPlayer.begin();
			itSelf != selfPlayer.end();
			itSelf++)
		{
			if (TargetIsOverSuppress(*itSelf))
				continue;
			if ((*itSelf)->HasUnitState(UNIT_STATE_CASTING))
			{
				canRemove = true;
				continue;
			}
			if (BotBGAI* pAI = dynamic_cast<BotBGAI*>((*itSelf)->GetAI()))
			{
				float dist = pAI->TryPushControlCommand((*itEnemy));
				if (dist >= 0)
				{
					//selfPlayer.erase(itSelf);
					enemyPlayer.erase(itEnemy);
					return true;
				}
			}
		}
		if (canRemove)
			enemyPlayer.erase(itEnemy);
		return false;
	}
	for (Player* self : selfPlayer)
	{
		if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(self->GetAI()))
		{
			pAI->SetReserveCtrlSpell(false);
			pAI->ClearCruxControlCommand();
		}
	}
	return false;
}

bool Arena::SuppressHealerPlayer(PLAYERS& selfPlayer, PLAYERS& enemyPlayer)
{
	if (enemyPlayer.size() < 2 || !ExistControler(selfPlayer))
	{
		for (Player* self : selfPlayer)
		{
			if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(self->GetAI()))
				pAI->ClearCruxControlCommand();
		}
		return false;
	}
	for (PLAYERS::iterator itEnemy = enemyPlayer.begin();
		itEnemy != enemyPlayer.end();
		itEnemy++)
	{
		Player* targetEnemy = *itEnemy;
		if (!IsHealerBot(targetEnemy))
			continue;
		float manaPct = uint32(((float)targetEnemy->GetPower(POWER_MANA) / (float)targetEnemy->GetMaxPower(POWER_MANA)) * 100);
		if (manaPct <= 5 || targetEnemy->GetHealthPct() < 10)
			continue;
		for (Player* sets : selfPlayer)
		{
			if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(sets->GetAI()))
			{
				pAI->SetReserveCtrlSpell(true);
				pAI->SetNonSelectTarget((*itEnemy)->GetGUID());
			}
		}
		if (TargetIsOverSuppress(targetEnemy))
		{
			enemyPlayer.erase(itEnemy);
			return true;
		}
		if (ExistCtrlSpellCasting(selfPlayer))
		{
			enemyPlayer.erase(itEnemy);
			return true;
		}
		//std::map<float, PLAYERS::iterator> canUsePlayers;
		bool canRemove = false;
		for (PLAYERS::iterator itSelf = selfPlayer.begin();
			itSelf != selfPlayer.end();
			itSelf++)
		{
			if (TargetIsOverSuppress(*itSelf))
				continue;
			if ((*itSelf)->HasUnitState(UNIT_STATE_CASTING))
			{
				canRemove = true;
				continue;
			}
			if (BotBGAI* pAI = dynamic_cast<BotBGAI*>((*itSelf)->GetAI()))
			{
				float dist = pAI->TryPushControlCommand(targetEnemy);
				if (dist >= 0)
				{
					//canUsePlayers[dist] = itSelf;
					//selfPlayer.erase(itSelf);
					enemyPlayer.erase(itEnemy);
					return true;
				}
			}
		}
		if (canRemove)
			enemyPlayer.erase(itEnemy);
		return false;
	}
	//if (canUsePlayers.size() < 2 || needSuppPlayer == enemyPlayer.end())
	for (Player* self : selfPlayer)
	{
		if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(self->GetAI()))
		{
			pAI->SetReserveCtrlSpell(false);
			pAI->ClearCruxControlCommand();
		}
	}
	return false;
}

bool Arena::SuppressMightinessPlayer(PLAYERS& selfPlayer, PLAYERS& enemyPlayer)
{
	if (enemyPlayer.size() < 2 || !ExistControler(selfPlayer))
	{
		for (Player* self : selfPlayer)
		{
			if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(self->GetAI()))
				pAI->ClearCruxControlCommand();
		}
		return false;
	}
	for (PLAYERS::iterator itEnemy = enemyPlayer.begin();
		itEnemy != enemyPlayer.end();
		itEnemy++)
	{
		Player* targetEnemy = *itEnemy;
		if (targetEnemy->IsPlayerBot() || !IsMightiness(targetEnemy))
			continue;
		for (Player* sets : selfPlayer)
		{
			if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(sets->GetAI()))
			{
				pAI->SetReserveCtrlSpell(true);
				pAI->SetNonSelectTarget(targetEnemy->GetGUID());
			}
		}
		if (TargetIsOverSuppress(targetEnemy))
		{
			enemyPlayer.erase(itEnemy);
			return true;
		}
		if (ExistCtrlSpellCasting(selfPlayer))
		{
			enemyPlayer.erase(itEnemy);
			return true;
		}
		//std::map<float, PLAYERS::iterator> canUsePlayers;
		bool canRemove = false;
		for (PLAYERS::iterator itSelf = selfPlayer.begin();
			itSelf != selfPlayer.end();
			itSelf++)
		{
			if (TargetIsOverSuppress(*itSelf))
				continue;
			if ((*itSelf)->HasUnitState(UNIT_STATE_CASTING))
			{
				canRemove = true;
				continue;
			}
			if (BotBGAI* pAI = dynamic_cast<BotBGAI*>((*itSelf)->GetAI()))
			{
				float dist = pAI->TryPushControlCommand(targetEnemy);
				if (dist >= 0)
				{
					//canUsePlayers[dist] = itSelf;
					//selfPlayer.erase(itSelf);
					enemyPlayer.erase(itEnemy);
					return true;
				}
			}
		}
		if (canRemove)
			enemyPlayer.erase(itEnemy);
		return false;
	}
	//if (canUsePlayers.size() < 2 || needSuppPlayer == enemyPlayer.end())
	for (Player* self : selfPlayer)
	{
		if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(self->GetAI()))
		{
			pAI->SetReserveCtrlSpell(false);
			pAI->ClearCruxControlCommand();
		}
	}
	return false;
}

void Arena::FollowEnemyHealer(PLAYERS& selfPlayer, PLAYERS& enemyPlayer)
{
	while (true)
	{
		PLAYERS::iterator& itHealer = GetListHealer(enemyPlayer);
		if (itHealer == enemyPlayer.end())
			return;
		PLAYERS::iterator& itMeleer = GetListMeleer(selfPlayer);
		if (itMeleer == selfPlayer.end())
		{
			itMeleer = GetListRanger(selfPlayer);
			if (itMeleer == selfPlayer.end())
				return;
		}
		(*itMeleer)->SetSelection((*itHealer)->GetGUID());
		selfPlayer.erase(itMeleer);
		enemyPlayer.erase(itHealer);
		itHealer = GetListHealer(enemyPlayer);
	}
}

Arena::PLAYERS::iterator Arena::GetListHealer(PLAYERS& players)
{
	for (PLAYERS::iterator itPlayer = players.begin();
		itPlayer != players.end();
		itPlayer++)
	{
		if (!IsHealerBot(*itPlayer))
			continue;
		return itPlayer;
	}
	return players.end();
}

Arena::PLAYERS::iterator Arena::GetListMeleer(PLAYERS& players)
{
	for (PLAYERS::iterator itPlayer = players.begin();
		itPlayer != players.end();
		itPlayer++)
	{
		if (!IsAllMeleeBot(*itPlayer))
			continue;
		return itPlayer;
	}
	return players.end();
}

Arena::PLAYERS::iterator Arena::GetListRanger(PLAYERS& players)
{
	for (PLAYERS::iterator itPlayer = players.begin();
		itPlayer != players.end();
		itPlayer++)
	{
		if (!IsRangeBot(*itPlayer))
			continue;
		return itPlayer;
	}
	return players.end();
}

bool Arena::HasAuraMechanic(Unit* pTarget, Mechanics mask)
{
	if (!pTarget)
		return false;
	return (pTarget->HasAuraWithMechanic(1 << mask));
}

bool Arena::CanSelectTarget(Player* pTarget)
{
	if (HasAuraMechanic(pTarget, Mechanics::MECHANIC_IMMUNE_SHIELD))
		return false;
	if (pTarget->HasAura(27827)) // (27827 救赎之魂 神牧死亡后)
		return false;
	if (HasAuraMechanic(pTarget, Mechanics::MECHANIC_CHARM))
		return false;
	if (HasAuraMechanic(pTarget, Mechanics::MECHANIC_SLEEP))
		return false;
	if (HasAuraMechanic(pTarget, Mechanics::MECHANIC_KNOCKOUT))
		return false;
	if (HasAuraMechanic(pTarget, Mechanics::MECHANIC_POLYMORPH))
		return false;
	if (HasAuraMechanic(pTarget, Mechanics::MECHANIC_BANISH))
		return false;
	if (HasAuraMechanic(pTarget, Mechanics::MECHANIC_HORROR))
		return false;
	if (pTarget->HasAura(1784) || pTarget->HasAura(5215) || pTarget->HasAura(66)) // (1784 盗贼潜行 || 5215 德鲁伊潜行 || 66 法师隐形)
		return false;

	return true;
}

bool Arena::MeleeTargetIsSuppress(Player* pTarget)
{
	if (!pTarget)
		return false;
	if (HasAuraMechanic(pTarget, Mechanics::MECHANIC_CHARM) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_DISORIENTED) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_DISTRACT) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_SLEEP) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_POLYMORPH) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_ROOT) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_STUN) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_BANISH) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_FEAR) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_IMMUNE_SHIELD))
		return true;
	return false;
}

bool Arena::TargetIsOverSuppress(Player* pTarget)
{
	if (!pTarget)
		return false;
	if (HasAuraMechanic(pTarget, Mechanics::MECHANIC_CHARM) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_DISORIENTED) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_DISTRACT) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_SLEEP) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_POLYMORPH) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_STUN) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_BANISH) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_FEAR) ||
		HasAuraMechanic(pTarget, Mechanics::MECHANIC_IMMUNE_SHIELD))
		return true;
	return false;
}

bool Arena::IsHealerBot(Player* pTarget)
{
	Classes cls = Classes(pTarget->getClass());
	switch (cls)
	{
	case CLASS_WARRIOR:
	case CLASS_HUNTER:
	case CLASS_ROGUE:
	case CLASS_DEATH_KNIGHT:
	case CLASS_MAGE:
	case CLASS_WARLOCK:
		return false;
	case CLASS_PALADIN:
		return (pTarget->FindTalentType() == 0);
	case CLASS_PRIEST:
		return (pTarget->FindTalentType() != 2);
	case CLASS_SHAMAN:
		return (pTarget->FindTalentType() == 2);
	case CLASS_DRUID:
		return (pTarget->FindTalentType() == 2);
	}

	return false;
}

bool Arena::IsMeleeBot(Player* pTarget)
{
	Classes cls = Classes(pTarget->getClass());
	switch (cls)
	{
	case CLASS_WARRIOR:
		return (pTarget->FindTalentType() != 2);
	case CLASS_PALADIN:
		return (pTarget->FindTalentType() != 1);
	case CLASS_ROGUE:
	case CLASS_DEATH_KNIGHT:
		return true;
	case CLASS_MAGE:
	case CLASS_WARLOCK:
	case CLASS_PRIEST:
	case CLASS_HUNTER:
		return false;
	case CLASS_SHAMAN:
		return (pTarget->FindTalentType() == 1);
	case CLASS_DRUID:
		return (pTarget->FindTalentType() == 1);
	}

	return false;
}

bool Arena::IsAllMeleeBot(Player* pTarget)
{
	Classes cls = Classes(pTarget->getClass());
	switch (cls)
	{
	case CLASS_PALADIN:
		return (pTarget->FindTalentType() == 2);
	case CLASS_WARRIOR:
	case CLASS_ROGUE:
	case CLASS_DEATH_KNIGHT:
		return true;
	case CLASS_MAGE:
	case CLASS_WARLOCK:
	case CLASS_PRIEST:
	case CLASS_HUNTER:
	case CLASS_SHAMAN:
		return false;
	case CLASS_DRUID:
		return (pTarget->FindTalentType() == 1);
	}

	return false;
}

bool Arena::IsRangeBot(Player* pTarget)
{
	Classes cls = Classes(pTarget->getClass());
	switch (cls)
	{
	case CLASS_WARRIOR:
	case CLASS_PALADIN:
	case CLASS_ROGUE:
	case CLASS_DEATH_KNIGHT:
		return false;
	case CLASS_MAGE:
	case CLASS_WARLOCK:
	case CLASS_PRIEST:
	case CLASS_HUNTER:
		return true;
	case CLASS_SHAMAN:
		return (pTarget->FindTalentType() != 1);
	case CLASS_DRUID:
		return (pTarget->FindTalentType() != 1);
	}

	return false;
}

bool Arena::IsMightiness(Player* pTarget)
{
	Classes cls = Classes(pTarget->getClass());
	switch (cls)
	{
	case CLASS_WARRIOR:
		return (pTarget->FindTalentType() != 2);
	case CLASS_PALADIN:
	case CLASS_ROGUE:
	case CLASS_DEATH_KNIGHT:
	case CLASS_MAGE:
	case CLASS_WARLOCK:
	case CLASS_PRIEST:
	case CLASS_HUNTER:
	case CLASS_SHAMAN:
	case CLASS_DRUID:
		return false;
	}

	return false;
}

bool Arena::CanFullHealerEnemy(PLAYERS& players)
{
	if (ExistMightinessClasses(players))
		return false;
	for (Player* player : players)
	{
		if (player->IsPlayerBot())
			continue;
		Classes cls = Classes(player->getClass());
		switch (cls)
		{
		case CLASS_WARRIOR:
			return (player->FindTalentType() == 2);
		case CLASS_ROGUE:
		case CLASS_DEATH_KNIGHT:
		case CLASS_HUNTER:
			return true;
		case CLASS_PALADIN:
			return (player->FindTalentType() != 0);
		case CLASS_SHAMAN:
			return (player->FindTalentType() != 2);
		case CLASS_PRIEST:
			return (player->FindTalentType() == 2);
		case CLASS_DRUID:
			return (player->FindTalentType() != 2);
		case CLASS_MAGE:
		case CLASS_WARLOCK:
			return false;
		}
	}
	return false;
}

bool Arena::CanFullHealerEnemy2(PLAYERS& players)
{
	for (Player* player : players)
	{
		if (player->IsPlayerBot())
			continue;
		Classes cls = Classes(player->getClass());
		switch (cls)
		{
		case CLASS_WARRIOR:
		case CLASS_ROGUE:
		case CLASS_DEATH_KNIGHT:
		case CLASS_HUNTER:
		case CLASS_MAGE:
		case CLASS_WARLOCK:
			return false;
		case CLASS_PALADIN:
			return (player->FindTalentType() == 0);
		case CLASS_SHAMAN:
			return (player->FindTalentType() == 2);
		case CLASS_PRIEST:
			return (player->FindTalentType() != 2);
		case CLASS_DRUID:
			return (player->FindTalentType() == 2);
		}
	}
	return false;
}

bool Arena::ExistCtrlSpellCasting(PLAYERS& players)
{
	for (Player* player : players)
	{
		if (BotBGAI* pAI = dynamic_cast<BotBGAI*>(player->GetAI()))
		{
			if (pAI->CastingControlCommandSpell())
				return true;
		}
	}
	return false;
}

bool Arena::ExistRealPlayerByRange(PLAYERS& players)
{
	for (Player* player : players)
	{
		if (player->IsPlayerBot())
			continue;
		return IsRangeBot(player);
	}
	return false;
}

bool Arena::ExistControler(PLAYERS& players)
{
	for (Player* self : players)
	{
		Classes cls = Classes(self->getClass());
		switch (cls)
		{
		case CLASS_MAGE:
		case CLASS_WARLOCK:
		case CLASS_DRUID:
			return true;
		case CLASS_WARRIOR:
		case CLASS_DEATH_KNIGHT:
		case CLASS_PRIEST:
		case CLASS_ROGUE:
		case CLASS_HUNTER:
		case CLASS_SHAMAN:
		case CLASS_PALADIN:
			break;
		}
	}
	return false;
}

bool Arena::ExistMightinessClasses(PLAYERS& players)
{
	for (Player* player : players)
	{
		if (player->IsPlayerBot())
			continue;
		Classes cls = Classes(player->getClass());
		switch (cls)
		{
		case CLASS_WARRIOR:
			return (player->FindTalentType() != 2);
		case CLASS_MAGE:
		case CLASS_WARLOCK:
		case CLASS_DRUID:
		case CLASS_DEATH_KNIGHT:
		case CLASS_PRIEST:
		case CLASS_ROGUE:
		case CLASS_HUNTER:
		case CLASS_SHAMAN:
		case CLASS_PALADIN:
			break;
		}
	}
	return false;
}

bool Arena::ExistMightinessHealer(PLAYERS& players)
{
	for (Player* player : players)
	{
		if (!player->IsPlayerBot() || !IsHealerBot(player))
			continue;
		Classes cls = Classes(player->getClass());
		switch (cls)
		{
		case CLASS_PRIEST:
			return (player->FindTalentType() != 2);
		case CLASS_WARRIOR:
		case CLASS_MAGE:
		case CLASS_WARLOCK:
		case CLASS_DRUID:
		case CLASS_DEATH_KNIGHT:
		case CLASS_ROGUE:
		case CLASS_HUNTER:
		case CLASS_SHAMAN:
		case CLASS_PALADIN:
			break;
		}
		return false;
	}
	return false;
}

Player* Arena::FindPriorAttackTarget(PLAYERS& players)
{
	for (Player* target : players)
	{
		if (!CanSelectTarget(target))
			continue;
		Classes cls = Classes(target->getClass());
		if (cls == CLASS_WARLOCK)
			return target;
	}
	for (Player* target : players)
	{
		if (!CanSelectTarget(target))
			continue;
		Classes cls = Classes(target->getClass());
		if (cls == CLASS_WARRIOR)
			return target;
	}
	for (Player* target : players)
	{
		if (!CanSelectTarget(target))
			continue;
		Classes cls = Classes(target->getClass());
		if (cls == CLASS_MAGE)
			return target;
	}
	return NULL;
}

void Arena::RemovePlayerAtLeave(ObjectGuid guid, bool transport, bool sendPacket)
{
    if (isRated() && GetStatus() == STATUS_IN_PROGRESS)
    {
        BattlegroundPlayerMap::const_iterator itr = m_Players.find(guid);
        if (itr != m_Players.end()) // check if the player was a participant of the match, or only entered through gm command (appear)
        {
            // if the player was a match participant, calculate rating
            uint32 team = itr->second.Team;

            ArenaTeam* winnerArenaTeam = sArenaTeamMgr->GetArenaTeamById(GetArenaTeamIdForTeam(GetOtherTeam(team)));
            ArenaTeam* loserArenaTeam = sArenaTeamMgr->GetArenaTeamById(GetArenaTeamIdForTeam(team));

            // left a rated match while the encounter was in progress, consider as loser
            if (winnerArenaTeam && loserArenaTeam && winnerArenaTeam != loserArenaTeam)
            {
                if (Player* player = _GetPlayer(itr->first, itr->second.OfflineRemoveTime != 0, "Arena::RemovePlayerAtLeave"))
                    loserArenaTeam->MemberLost(player, GetArenaMatchmakerRating(GetOtherTeam(team)));
                else
                    loserArenaTeam->OfflineMemberLost(guid, GetArenaMatchmakerRating(GetOtherTeam(team)));
            }
        }
    }

	if (Player* player = ObjectAccessor::FindPlayer(guid))
	{
		BotUtility::RemoveArenaBotSpellsByPlayer(player);
	}
	// remove player
    Battleground::RemovePlayerAtLeave(guid, transport, sendPacket);
}

void Arena::CheckWinConditions()
{
    if (!GetAlivePlayersCountByTeam(ALLIANCE) && GetPlayersCountByTeam(HORDE))
        EndBattleground(HORDE);
    else if (GetPlayersCountByTeam(ALLIANCE) && !GetAlivePlayersCountByTeam(HORDE))
        EndBattleground(ALLIANCE);
}

void Arena::EndBattleground(uint32 winner)
{
    // arena rating calculation
    if (isRated())
    {
        uint32 loserTeamRating        = 0;
        uint32 loserMatchmakerRating  = 0;
        int32  loserChange            = 0;
        int32  loserMatchmakerChange  = 0;
        uint32 winnerTeamRating       = 0;
        uint32 winnerMatchmakerRating = 0;
        int32  winnerChange           = 0;
        int32  winnerMatchmakerChange = 0;

        ArenaTeam* winnerArenaTeam = sArenaTeamMgr->GetArenaTeamById(GetArenaTeamIdForTeam(winner));
        ArenaTeam* loserArenaTeam = sArenaTeamMgr->GetArenaTeamById(GetArenaTeamIdForTeam(GetOtherTeam(winner)));

        if (winnerArenaTeam && loserArenaTeam && winnerArenaTeam != loserArenaTeam)
        {
            loserTeamRating = loserArenaTeam->GetRating();
            loserMatchmakerRating = GetArenaMatchmakerRating(GetOtherTeam(winner));
            winnerTeamRating = winnerArenaTeam->GetRating();
            winnerMatchmakerRating = GetArenaMatchmakerRating(winner);

            if (winner != 0)
            {
                winnerMatchmakerChange = winnerArenaTeam->WonAgainst(winnerMatchmakerRating, loserMatchmakerRating, winnerChange);
                loserMatchmakerChange = loserArenaTeam->LostAgainst(loserMatchmakerRating, winnerMatchmakerRating, loserChange);

                TC_LOG_DEBUG("bg.arena", "match Type: %u --- Winner: old rating: %u, rating gain: %d, old MMR: %u, MMR gain: %d --- Loser: old rating: %u, rating loss: %d, old MMR: %u, MMR loss: %d ---",
                    GetArenaType(), winnerTeamRating, winnerChange, winnerMatchmakerRating, winnerMatchmakerChange,
                    loserTeamRating, loserChange, loserMatchmakerRating, loserMatchmakerChange);

                SetArenaMatchmakerRating(winner, winnerMatchmakerRating + winnerMatchmakerChange);
                SetArenaMatchmakerRating(GetOtherTeam(winner), loserMatchmakerRating + loserMatchmakerChange);

                // bg team that the client expects is different to TeamId
                // alliance 1, horde 0
                uint8 winnerTeam = winner == ALLIANCE ? BG_TEAM_ALLIANCE : BG_TEAM_HORDE;
                uint8 loserTeam = winner == ALLIANCE ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE;

                _arenaTeamScores[winnerTeam].Assign(winnerChange, winnerMatchmakerRating, winnerArenaTeam->GetName());
                _arenaTeamScores[loserTeam].Assign(loserChange, loserMatchmakerRating, loserArenaTeam->GetName());

                TC_LOG_DEBUG("bg.arena", "Arena match Type: %u for Team1Id: %u - Team2Id: %u ended. WinnerTeamId: %u. Winner rating: +%d, Loser rating: %d",
                    GetArenaType(), GetArenaTeamIdByIndex(TEAM_ALLIANCE), GetArenaTeamIdByIndex(TEAM_HORDE), winnerArenaTeam->GetId(), winnerChange, loserChange);

                if (sWorld->getBoolConfig(CONFIG_ARENA_LOG_EXTENDED_INFO))
                    for (auto const& score : PlayerScores)
                        if (Player* player = ObjectAccessor::FindConnectedPlayer(ObjectGuid(HighGuid::Player, score.first)))
                        {
                            TC_LOG_DEBUG("bg.arena", "Statistics match Type: %u for %s (GUID: %u, Team: %d, IP: %s): %s",
                                GetArenaType(), player->GetName().c_str(), score.first, player->GetArenaTeamId(GetArenaType() == 5 ? 2 : GetArenaType() == 3),
                                player->GetSession()->GetRemoteAddress().c_str(), score.second->ToString().c_str());
                        }
            }
            // Deduct 16 points from each teams arena-rating if there are no winners after 45+2 minutes
            else
            {
                _arenaTeamScores[BG_TEAM_ALLIANCE].Assign(ARENA_TIMELIMIT_POINTS_LOSS, winnerMatchmakerRating, winnerArenaTeam->GetName());
                _arenaTeamScores[BG_TEAM_HORDE].Assign(ARENA_TIMELIMIT_POINTS_LOSS, loserMatchmakerRating, loserArenaTeam->GetName());

                winnerArenaTeam->FinishGame(ARENA_TIMELIMIT_POINTS_LOSS);
                loserArenaTeam->FinishGame(ARENA_TIMELIMIT_POINTS_LOSS);
            }

            uint8 aliveWinners = GetAlivePlayersCountByTeam(winner);

            for (auto const& i : GetPlayers())
            {
                uint32 team = i.second.Team;

                if (i.second.OfflineRemoveTime)
                {
                    // if rated arena match - make member lost!
                    if (team == winner)
                        winnerArenaTeam->OfflineMemberLost(i.first, loserMatchmakerRating, winnerMatchmakerChange);
                    else
                        loserArenaTeam->OfflineMemberLost(i.first, winnerMatchmakerRating, loserMatchmakerChange);
                    continue;
                }

                Player* player = _GetPlayer(i.first, i.second.OfflineRemoveTime != 0, "Arena::EndBattleground");
                if (!player)
                    continue;

                // per player calculation
                if (team == winner)
                {
                    // update achievement BEFORE personal rating update
                    uint32 rating = player->GetArenaPersonalRating(winnerArenaTeam->GetSlot());
                    player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA, rating ? rating : 1);
                    player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_ARENA, GetMapId());

                    // Last standing - Rated 5v5 arena & be solely alive player
                    if (GetArenaType() == ARENA_TYPE_5v5 && aliveWinners == 1 && player->IsAlive())
                        player->CastSpell(player, SPELL_LAST_MAN_STANDING, true);

                    winnerArenaTeam->MemberWon(player, loserMatchmakerRating, winnerMatchmakerChange);
                }
                else
                {
                    loserArenaTeam->MemberLost(player, winnerMatchmakerRating, loserMatchmakerChange);

                    // Arena lost => reset the win_rated_arena having the "no_lose" condition
                    player->ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA, ACHIEVEMENT_CRITERIA_CONDITION_NO_LOSE);
                }
            }

            // save the stat changes
            winnerArenaTeam->SaveToDB();
            loserArenaTeam->SaveToDB();
            // send updated arena team stats to players
            // this way all arena team members will get notified, not only the ones who participated in this match
            winnerArenaTeam->NotifyStatsChanged();
            loserArenaTeam->NotifyStatsChanged();
        }
    }
	sArenaTeamMgr->UpdateToCustomTalkMenu();
    // end battleground
    Battleground::EndBattleground(winner);
}

void Arena::InsureArenaAllPlayerFlag()
{
	for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
	{
		if (Player* player = ObjectAccessor::FindPlayer(itr->first))
			InsureArenaFlag(player);
	}
}

void Arena::InsureArenaFlag(Player* player, bool init)
{
	if (!player)
		return;
	if (player->GetBGTeam() == ALLIANCE)        // gold
	{
		if (player->GetTeam() == HORDE)
		{
			if (!player->HasAura(SPELL_HORDE_GOLD_FLAG))
			{
				uint32 life = player->GetHealth();
				uint32 mana = player->GetPower(player->getPowerType());
				player->CastSpell(player, SPELL_HORDE_GOLD_FLAG, true);
				if (!init)
				{
					player->SetHealth(life);
					player->SetPower(player->getPowerType(), mana);
				}
			}
		}
		else
		{
			if (!player->HasAura(SPELL_ALLIANCE_GOLD_FLAG))
			{
				uint32 life = player->GetHealth();
				uint32 mana = player->GetPower(player->getPowerType());
				player->CastSpell(player, SPELL_ALLIANCE_GOLD_FLAG, true);
				if (!init)
				{
					player->SetHealth(life);
					player->SetPower(player->getPowerType(), mana);
				}
			}
		}
	}
	else                                        // green
	{
		if (player->GetTeam() == HORDE)
		{
			if (!player->HasAura(SPELL_HORDE_GREEN_FLAG))
			{
				uint32 life = player->GetHealth();
				uint32 mana = player->GetPower(player->getPowerType());
				player->CastSpell(player, SPELL_HORDE_GREEN_FLAG, true);
				if (!init)
				{
					player->SetHealth(life);
					player->SetPower(player->getPowerType(), mana);
				}
			}
		}
		else
		{
			if (!player->HasAura(SPELL_ALLIANCE_GREEN_FLAG))
			{
				uint32 life = player->GetHealth();
				uint32 mana = player->GetPower(player->getPowerType());
				player->CastSpell(player, SPELL_ALLIANCE_GREEN_FLAG, true);
				if (!init)
				{
					player->SetHealth(life);
					player->SetPower(player->getPowerType(), mana);
				}
			}
		}
	}
}

bool Arena::AssignTactics(PLAYERS& comLists, PLAYERS& enemyLists)
{
	if (BotUtility::BotArenaTeamTactics <= 0 || BotUtility::BotArenaTeamTactics > 2)
		return false;
	if (BotUtility::BotArenaTeamTactics == 1)
	{
		SuppressHealerPlayer(comLists, enemyLists);
		for (Player* enemyPlayer : enemyLists)
		{
			if (enemyPlayer->IsPlayerBot() || enemyPlayer->IsMounted())
				continue;
			if (!CanSelectTarget(enemyPlayer))
				continue;
			for (Player* comPlayer : comLists)
				comPlayer->SetSelection(enemyPlayer->GetGUID());
			return true;
		}
		NormalTactics(comLists, enemyLists);
	}
	else if (BotUtility::BotArenaTeamTactics == 2)
	{
		SuppressRealPlayer(comLists, enemyLists);
		NormalTactics(comLists, enemyLists);
	}
	return true;
}
