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

#include "Define.h"
#include "ArenaTeamMgr.h"
#include "World.h"
#include "Log.h"
#include "DatabaseEnv.h"
#include "Language.h"
#include "CustomTalkMenu.h"
#include "Player.h"
#include "ObjectAccessor.h"
#include "PlayerBotMgr.h"
#include "Group.h"

bool ArenaTeamMgr::g_AutoBuildArenaTeam = true;

ArenaTeamMgr::ArenaTeamMgr() :
m_MaxPlayerBotTeam(9),
m_AllPlayerBotTeamFinish(false),
NextArenaTeamId(1)
{
	for (int i = 0; i < 3; i++)
	{
		m_NextMatchAssignTeams[0][i] = 0;
		m_NextMatchAssignTeams[1][i] = 0;
	}
}

ArenaTeamMgr::~ArenaTeamMgr()
{
    for (ArenaTeamContainer::iterator itr = ArenaTeamStore.begin(); itr != ArenaTeamStore.end(); ++itr)
        delete itr->second;
}

ArenaTeamMgr* ArenaTeamMgr::instance()
{
    static ArenaTeamMgr instance;
    return &instance;
}

// Arena teams collection
ArenaTeam* ArenaTeamMgr::GetArenaTeamById(uint32 arenaTeamId) const
{
    ArenaTeamContainer::const_iterator itr = ArenaTeamStore.find(arenaTeamId);
    if (itr != ArenaTeamStore.end())
        return itr->second;

    return NULL;
}

ArenaTeam* ArenaTeamMgr::GetArenaTeamByName(const std::string& arenaTeamName) const
{
    std::string search = arenaTeamName;
    std::transform(search.begin(), search.end(), search.begin(), ::toupper);
    for (ArenaTeamContainer::const_iterator itr = ArenaTeamStore.begin(); itr != ArenaTeamStore.end(); ++itr)
    {
        std::string teamName = itr->second->GetName();
        std::transform(teamName.begin(), teamName.end(), teamName.begin(), ::toupper);
        if (search == teamName)
            return itr->second;
    }
    return NULL;
}

ArenaTeam* ArenaTeamMgr::GetArenaTeamByCaptain(ObjectGuid guid) const
{
    for (ArenaTeamContainer::const_iterator itr = ArenaTeamStore.begin(); itr != ArenaTeamStore.end(); ++itr)
        if (itr->second->GetCaptain() == guid)
            return itr->second;

    return NULL;
}

ArenaTeam* ArenaTeamMgr::GetArenaTeamByGUID(ObjectGuid playerGuid) const
{
	for (ArenaTeamContainer::const_iterator itr = ArenaTeamStore.begin(); itr != ArenaTeamStore.end(); ++itr)
		if (itr->second->IsMember(playerGuid))
			return itr->second;

	return NULL;
}

ArenaTeam* ArenaTeamMgr::GetArenaTypeTeamByGUID(ObjectGuid playerGuid, uint32 arenaType) const
{
	for (ArenaTeamContainer::const_iterator itr = ArenaTeamStore.begin(); itr != ArenaTeamStore.end(); ++itr)
	{
		if (itr->second->GetType() != arenaType)
			continue;
		if (itr->second->IsMember(playerGuid))
			return itr->second;
	}

	return NULL;
}

void ArenaTeamMgr::AddArenaTeam(ArenaTeam* arenaTeam)
{
    ArenaTeamStore[arenaTeam->GetId()] = arenaTeam;
}

void ArenaTeamMgr::RemoveArenaTeam(uint32 arenaTeamId)
{
    ArenaTeamStore.erase(arenaTeamId);
}

uint32 ArenaTeamMgr::GenerateArenaTeamId()
{
    if (NextArenaTeamId >= 0xFFFFFFFE)
    {
        TC_LOG_ERROR("bg.battleground", "Arena team ids overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return NextArenaTeamId++;
}

void ArenaTeamMgr::LoadArenaTeams()
{
    uint32 oldMSTime = getMSTime();

    // Clean out the trash before loading anything
    CharacterDatabase.DirectExecute("DELETE FROM arena_team_member WHERE arenaTeamId NOT IN (SELECT arenaTeamId FROM arena_team)");       // One-time query

    //                                                        0        1         2         3          4              5            6            7           8
    QueryResult result = CharacterDatabase.Query("SELECT arenaTeamId, name, captainGuid, type, backgroundColor, emblemStyle, emblemColor, borderStyle, borderColor, "
    //      9        10        11         12           13       14
        "rating, weekGames, weekWins, seasonGames, seasonWins, rank FROM arena_team ORDER BY arenaTeamId ASC");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 arena teams. DB table `arena_team` is empty!");
		return;
    }

    QueryResult result2 = CharacterDatabase.Query(
        //              0              1           2             3              4                 5          6     7          8                  9
        "SELECT arenaTeamId, atm.guid, atm.weekGames, atm.weekWins, atm.seasonGames, atm.seasonWins, c.name, class, personalRating, matchMakerRating FROM arena_team_member atm"
        " INNER JOIN arena_team ate USING (arenaTeamId)"
        " LEFT JOIN characters AS c ON atm.guid = c.guid"
        " LEFT JOIN character_arena_stats AS cas ON c.guid = cas.guid AND (cas.slot = 0 AND ate.type = 2 OR cas.slot = 1 AND ate.type = 3 OR cas.slot = 2 AND ate.type = 5)"
        " ORDER BY atm.arenateamid ASC");

    uint32 count = 0;
    do
    {
        ArenaTeam* newArenaTeam = new ArenaTeam;

        if (!newArenaTeam->LoadArenaTeamFromDB(result) || !newArenaTeam->LoadMembersFromDB(result2))
        {
            newArenaTeam->Disband(NULL);
            delete newArenaTeam;
            continue;
        }

        AddArenaTeam(newArenaTeam);
		newArenaTeam->IsPurityPlayerBotTeam();
        ++count;
    }
    while (result->NextRow());

	VerifyArenaTeamTalentTableExist();
	UpdateToCustomTalkMenu();
    TC_LOG_INFO("server.loading", ">> Loaded %u arena teams in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ArenaTeamMgr::BroadcastArenaTeamBuild(std::string name)
{
	if (name.empty())
		return;
	std::string formatText;
	consoleToUtf8(std::string("竞技场战队 |cff00AA00<%s>|r 成立了！"), formatText);
	char outText[256] = { 0 };
	sprintf_s(outText, 255, formatText.c_str(), name.c_str());
	sWorld->SendGlobalText(outText, NULL);
}

void ArenaTeamMgr::CheckPlayerBotArenaTeam()
{
	if (!ArenaTeamMgr::g_AutoBuildArenaTeam)
		return;
	if (m_AllPlayerBotTeamFinish)
		return;
	if (uint32 count = GetNeedSupplementArenaTeamByType(ArenaTeamTypes::ARENA_TEAM_2v2, TEAM_ALLIANCE))
	{
		if (count > 0)
		{
			AddNewPlayerBotArenaTeam(ArenaTeamTypes::ARENA_TEAM_2v2, TEAM_ALLIANCE);
			return;
		}
	}
	if (uint32 count = GetNeedSupplementArenaTeamByType(ArenaTeamTypes::ARENA_TEAM_2v2, TEAM_HORDE))
	{
		if (count > 0)
		{
			AddNewPlayerBotArenaTeam(ArenaTeamTypes::ARENA_TEAM_2v2, TEAM_HORDE);
			return;
		}
	}
	if (uint32 count = GetNeedSupplementArenaTeamByType(ArenaTeamTypes::ARENA_TEAM_3v3, TEAM_ALLIANCE))
	{
		if (count > 0)
		{
			AddNewPlayerBotArenaTeam(ArenaTeamTypes::ARENA_TEAM_3v3, TEAM_ALLIANCE);
			return;
		}
	}
	if (uint32 count = GetNeedSupplementArenaTeamByType(ArenaTeamTypes::ARENA_TEAM_3v3, TEAM_HORDE))
	{
		if (count > 0)
		{
			AddNewPlayerBotArenaTeam(ArenaTeamTypes::ARENA_TEAM_3v3, TEAM_HORDE);
			return;
		}
	}
	if (uint32 count = GetNeedSupplementArenaTeamByType(ArenaTeamTypes::ARENA_TEAM_5v5, TEAM_ALLIANCE))
	{
		if (count > 0)
		{
			AddNewPlayerBotArenaTeam(ArenaTeamTypes::ARENA_TEAM_5v5, TEAM_ALLIANCE);
			return;
		}
	}
	if (uint32 count = GetNeedSupplementArenaTeamByType(ArenaTeamTypes::ARENA_TEAM_5v5, TEAM_HORDE))
	{
		if (count > 0)
		{
			AddNewPlayerBotArenaTeam(ArenaTeamTypes::ARENA_TEAM_5v5, TEAM_HORDE);
			return;
		}
	}
	m_AllPlayerBotTeamFinish = true;
}

uint32 ArenaTeamMgr::SearchEnemyArenaTeam(uint32 selfArenaTeamId, Team enemy)
{
	ArenaTeam* selfArenaTeam = GetArenaTeamById(selfArenaTeamId);
	if (!selfArenaTeam)
		return 0;

	// select prior arena team
	uint32 priorIndex = 2;
	if (enemy == ALLIANCE)
		priorIndex = 0;
	else if (enemy == HORDE)
		priorIndex = 1;
	uint32 priorType = 3;
	if (selfArenaTeam->GetType() == 2)
		priorType = 0;
	if (selfArenaTeam->GetType() == 3)
		priorType = 1;
	else if (selfArenaTeam->GetType() == 5)
		priorType = 2;
	if (priorIndex < 2 && priorType < 3 && m_NextMatchAssignTeams[priorIndex][priorType] > 0)
	{
		if (sPlayerBotMgr->CanReadyArenaByArenaTeamID(m_NextMatchAssignTeams[priorIndex][priorType]))
		{
			uint32 matchTeamId = m_NextMatchAssignTeams[priorIndex][priorType];
			m_NextMatchAssignTeams[priorIndex][priorType] = 0;
			return matchTeamId;
		}
	}

	// random select
	if (m_NextMatchOrderTeams.empty())
		UpdateNextMatchOrder();
	if (m_NextMatchOrderTeams.empty())
		return 0;
	for (uint32 arenaTeamID : m_NextMatchOrderTeams)
	{
		ArenaTeam* arenaTeam = GetArenaTeamById(arenaTeamID);
		if (!arenaTeam)
			continue;
		TeamId cmap = arenaTeam->GetArenaTeamCmap();
		if (cmap == TEAM_NEUTRAL)
			continue;
		if (enemy == ALLIANCE && cmap != TEAM_ALLIANCE)
			continue;
		if (enemy == HORDE && cmap != TEAM_HORDE)
			continue;
		if (arenaTeam == selfArenaTeam || arenaTeam->GetType() != selfArenaTeam->GetType() ||
			!arenaTeam->CanJoinArenaByBot())
			continue;
		return arenaTeam->GetId();
	}
	return 0;
}

bool ArenaTeamMgr::ExistArenaTeamByType(ObjectGuid& guid, ArenaTeamTypes type)
{
	for (ArenaTeamContainer::iterator teamItr = GetArenaTeamMapBegin(); teamItr != GetArenaTeamMapEnd(); ++teamItr)
	{
		if (ArenaTeam* arenaTeam = teamItr->second)
		{
			if (ArenaTeamTypes(arenaTeam->GetType()) != type)
				continue;
			if (arenaTeam->IsMember(guid))
				return true;
		}
	}
	return false;
}

void ArenaTeamMgr::DestoryAllArenaTeam()
{
	while (!ArenaTeamStore.empty())
	{
		ArenaTeam* arenaTeam = GetArenaTeamMapBegin()->second;
		if (arenaTeam->IsFighting())
			return;
		arenaTeam->Disband();
		delete arenaTeam;
	}
	m_ArenaPlayerTalents.clear();
	std::string sql = "TRUNCATE `arena_team_talents`;";
	CharacterDatabase.DirectExecute(sql.c_str());
}

bool ArenaTeamMgr::IsPurityArenaTeam(const ObjectGuid& guid)
{
	ArenaTeam* arenaTeam = GetArenaTeamByGUID(guid);
	if (!arenaTeam)
		return false;
	Player* player = ObjectAccessor::FindPlayer(guid);
	if (!player)
		return false;
	if (Group* pGroup = player->GetGroup())
	{
		Group::MemberSlotList const& memList = pGroup->GetMemberSlots();
		for (Group::MemberSlot const& slot : memList)
		{
			Player* groupPlayer = ObjectAccessor::FindPlayer(slot.guid);
			if (!groupPlayer || !groupPlayer->IsInWorld())
				return false;
			if (!arenaTeam->IsMember(groupPlayer->GetGUID()))
				return false;
		}
	}
	return true;
}

std::string ArenaTeamMgr::GetArenaTeamVSText(uint32 arenaTeamA, uint32 arenaTeamB)
{
	if (arenaTeamA == 0 || arenaTeamB == 0)
		return "";
	ArenaTeam* aTeam = GetArenaTeamById(arenaTeamA);
	ArenaTeam* bTeam = GetArenaTeamById(arenaTeamB);
	std::string formatText = " Ready %s VS %s ";
	//consoleToUtf8(std::string("%s VS %s 的竞技比赛准备开始！"), formatText);
	std::string aTeamText = aTeam ? aTeam->GetArenaTeamColorName() : "";
	std::string bTeamText = bTeam ? bTeam->GetArenaTeamColorName() : "";
	//char vsText[256] = { 0 };
	//sprintf_s(vsText, 255, formatText.c_str(), aTeamText, bTeamText);
	//return vsText;
	return "Ready " + aTeamText + " VS " + bTeamText;
}

void ArenaTeamMgr::UpdateNextMatchOrder()
{
	m_NextMatchOrderTeams.clear();
	for (ArenaTeamContainer::iterator teamItr = GetArenaTeamMapBegin(); teamItr != GetArenaTeamMapEnd(); ++teamItr)
		m_NextMatchOrderTeams.push_back(teamItr->first);
	if (m_NextMatchOrderTeams.size() <= 1)
		return;
	Trinity::Containers::RandomShuffle(m_NextMatchOrderTeams);
	//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	//uint32 count = urand(1, 2);
	//for (uint32 i = 0; i < count; i++)
	//	std::shuffle(m_NextMatchOrderTeams.begin(), m_NextMatchOrderTeams.end(), std::default_random_engine(seed));
}

bool ArenaTeamMgr::BuildBotsArenaTeam(std::string name, TeamId team, uint32 arenaType, uint32 cls1, uint32 cls2, uint32 cls3, uint32 cls4, uint32 cls5, uint32 tal1, uint32 tal2, uint32 tal3, uint32 tal4, uint32 tal5)
{
	if (name.size() < 3 || name.size() > 20 || sArenaTeamMgr->GetArenaTeamByName(name))
		return false;
	if (team != TEAM_ALLIANCE && team != TEAM_HORDE)
		return false;
	if (arenaType != 2 && arenaType != 3 && arenaType != 5)
		return false;
	if (tal1 > 3) tal1 = 3;
	if (tal2 > 3) tal2 = 3;
	if (tal3 > 3) tal3 = 3;
	if (tal4 > 3) tal4 = 3;
	if (tal5 > 3) tal5 = 3;
	std::vector<uint32> allClasses;
	if (IsValidClasses(cls1))
		allClasses.push_back(cls1);
	if (IsValidClasses(cls2))
		allClasses.push_back(cls2);
	if (IsValidClasses(cls3))
		allClasses.push_back(cls3);
	if (IsValidClasses(cls4))
		allClasses.push_back(cls4);
	if (IsValidClasses(cls5))
		allClasses.push_back(cls5);
	if (allClasses.size() != arenaType)
		return false;
	std::vector<ObjectGuid> members;
	std::vector<uint32> talents;
	for (uint32 cls : allClasses)
	{
		ObjectGuid guid = sPlayerBotMgr->GetNoArenaMatchCharacter(team, arenaType, Classes(cls), members);
		if (guid == ObjectGuid::Empty)
			return false;
		members.push_back(guid);
		//talents.push_back(3);
	}
	talents.push_back(tal1);
	talents.push_back(tal2);
	if (arenaType == 3 || arenaType == 5)
		talents.push_back(tal3);
	if (arenaType == 5)
	{
		talents.push_back(tal4);
		talents.push_back(tal5);
	}
	return BuildNewPlayerBotArenaTeam(ArenaTeamTypes(arenaType), members, talents, name);
}

void ArenaTeamMgr::SetNextPriorArenaTeam(TeamId team, uint32 arenaType, uint32 arenaTeamId)
{
	if (arenaTeamId > 0 && GetArenaTeamById(arenaTeamId) == NULL)
		return;
	//if (team == TEAM_NEUTRAL || arenaTeamId == 0)
	//	return;
	//if (arenaType != 2 && arenaType != 3 && arenaType != 5)
	//	return;
	if (team == TEAM_ALLIANCE)
	{
		switch (arenaType)
		{
		case 2:
			m_NextMatchAssignTeams[0][0] = arenaTeamId;
			break;
		case 3:
			m_NextMatchAssignTeams[0][1] = arenaTeamId;
			break;
		case 5:
			m_NextMatchAssignTeams[0][2] = arenaTeamId;
			break;
		}
	}
	else if (team == TEAM_HORDE)
	{
		switch (arenaType)
		{
		case 2:
			m_NextMatchAssignTeams[1][0] = arenaTeamId;
			break;
		case 3:
			m_NextMatchAssignTeams[1][1] = arenaTeamId;
			break;
		case 5:
			m_NextMatchAssignTeams[1][2] = arenaTeamId;
			break;
		}
	}
}

bool ArenaTeamMgr::CanJoinRatedArenaQueue(Player* player, uint32 arenaType)
{
	if (!player)
		return false;
	if (arenaType != 2 && arenaType != 3 && arenaType != 5)
		return false;
	//if (!player->IsInWorld() || player->InBattleground() || player->InArena() ||
	//	player->InBattlegroundQueue() || player->isUsingLfg() || player->GetMap()->IsDungeon())
	//	return false;
	//if (!player->GetArenaTeamId(ArenaTeam::GetSlotByType(arenaType)))
	//	return false;
	Group* pGroup = player->GetGroup();
	if (!pGroup)
		return false;
	uint32 memberCount = 0;
	for (GroupReference* itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
	{
		++memberCount;
		if (memberCount > arenaType)
			return false;
		Player* member = itr->GetSource();
		if (!member || member->getLevel() < 80 || member->GetTeamId() != player->GetTeamId())
			return false;
		if (!member->IsInWorld() || member->InBattleground() || member->InArena() ||
			member->InBattlegroundQueue() || member->isUsingLfg() || member->GetMap()->IsDungeon())
			return false;
		if (!member->GetArenaTeamId(ArenaTeam::GetSlotByType(arenaType)))
			return false;
	}
	if (memberCount != arenaType)
		return false;
	return true;
}

void ArenaTeamMgr::VerifyArenaTeamTalentTableExist()
{
	std::string sql = "create table IF NOT EXISTS `arena_team_talents` ("
		"`guid` INT(10) UNSIGNED NOT NULL,"
		"`talent` INT(10) UNSIGNED NOT NULL DEFAULT 3,"
		"PRIMARY KEY(`guid`),"
		"UNIQUE INDEX `guid_UNIQUE` (`guid` ASC)) "
		"ENGINE = MyISAM "
		"DEFAULT CHARACTER SET = utf8;";
	CharacterDatabase.DirectExecute(sql.c_str());
	m_ArenaPlayerTalents.clear();
	QueryResult result = CharacterDatabase.Query("SELECT guid, talent FROM `arena_team_talents`");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 id = fields[0].GetUInt32();
			uint32 talent = fields[1].GetUInt32();
			ObjectGuid guid = ObjectGuid(uint64(id));
			if (talent > 3)
				talent = 3;
			m_ArenaPlayerTalents[guid] = talent;
		} while (result->NextRow());
	}
}

uint32 ArenaTeamMgr::GetNeedSupplementArenaTeamByType(ArenaTeamTypes type, TeamId team)
{
	uint32 existPlayerBotTeam = 0;
	for (ArenaTeamContainer::iterator teamItr = GetArenaTeamMapBegin(); teamItr != GetArenaTeamMapEnd(); ++teamItr)
	{
		if (ArenaTeam* arenaTeam = teamItr->second)
		{
			if (arenaTeam->GetArenaTeamCmap() != team)
				continue;
			if (ArenaTeamTypes(arenaTeam->GetType()) != type)
				continue;
			if (!arenaTeam->IsPurityPlayerBotTeam())
				continue;
			++existPlayerBotTeam;
			if (existPlayerBotTeam >= m_MaxPlayerBotTeam)
				return 0;
		}
	}
	int32 needCount = int32(m_MaxPlayerBotTeam) - int32(existPlayerBotTeam);
	if (needCount <= 0)
		return 0;
	return uint32(needCount);
}

bool ArenaTeamMgr::AddNewPlayerBotArenaTeam(ArenaTeamTypes type, TeamId team)
{
	//TeamId team = (urand(0, 99) < 50) ? TeamId::TEAM_ALLIANCE : TeamId::TEAM_HORDE;
	int32 needCount = 5;
	if (type == ARENA_TEAM_2v2)
		needCount = 2;
	else if (type == ARENA_TEAM_3v3)
		needCount = 3;
	std::set<uint32> guids = sPlayerBotMgr->GetArenaTeamPlayerBotIDCountByTeam(team, needCount, type);
	for (int32 i = 0; i < needCount; i++)
	{
		if (guids.size() == needCount)
			break;
		guids = sPlayerBotMgr->GetArenaTeamPlayerBotIDCountByTeam(team, needCount, type);
	}
	if (guids.size() != needCount)
		return false;
	std::vector<ObjectGuid> members;
	std::vector<uint32> talents;
	for (std::set<uint32>::iterator itID = guids.begin(); itID != guids.end(); itID++)
	{
		members.push_back(ObjectGuid(uint64(*itID)));
		talents.push_back(3);
	}
	return BuildNewPlayerBotArenaTeam(type, members, talents);
}

bool ArenaTeamMgr::BuildNewPlayerBotArenaTeam(ArenaTeamTypes type, std::vector<ObjectGuid>& members, std::vector<uint32>& talents, std::string name)
{
	if (members.empty())
		return false;
	for (uint32 i = 0; i < members.size(); i++)
	{
		ObjectGuid& checkGUID = members[i];
		for (uint32 j = 0; j < members.size(); j++)
		{
			if (j == i)
				continue;
			if (checkGUID == members[j])
				return false;
		}
	}

	std::string& newArenaName = name.empty() ? sPlayerBotMgr->RandomArenaName() : name;
	if (newArenaName.size() == 0)
		return false;
	uint32 icon = urand(1, 99);
	uint32 border = urand(1, 6);
	ArenaTeam* arenaTeam = new ArenaTeam();
	if (!arenaTeam->Create(members[0], type, newArenaName, RandomColor32(), icon, RandomColor32(), border, RandomColor32()))
	{
		delete arenaTeam;
		return false;
	}
	AddArenaTeam(arenaTeam);
	for (uint32 i = 1; i < members.size(); i++)
	{
		arenaTeam->AddMember(members[i]);
	}
	if (talents.size() == members.size())
	{
		for (uint32 i = 0; i < members.size(); i++)
		{
			UpdateArenaTeamPlayerBotTalent(members[i], talents[i]);
		}
	}
	arenaTeam->SetPurityPlayerBotTeam();
	UpdateToCustomTalkMenu();
	return true;
}

uint32 ArenaTeamMgr::RandomColor32()
{
	uint32 r = urand(0, 255);
	uint32 g = urand(0, 255);
	uint32 b = urand(0, 255);
	uint32 color = 0xff000000;
	color += (r << 16);
	color += (g << 8);
	color += b;
	return color;
}

bool ArenaTeamMgr::IsValidClasses(uint32 cls)
{
	if (cls == 1 || cls == 2 || cls == 3 || cls == 4 || cls == 5 || cls == 6 || cls == 7 || cls == 8 || cls == 9 || cls == 11)
		return true;
	return false;
}

void ArenaTeamMgr::UpdateToCustomTalkMenu()
{
	std::list<CustomItem*> arenaItems;
	uint32 baseMenuEntry = 620000;
	for (ArenaTeamContainer::iterator teamItr = GetArenaTeamMapBegin(); teamItr != GetArenaTeamMapEnd(); ++teamItr)
	{
		ArenaTeam* arenaTeam = teamItr->second;
		if (!arenaTeam || !arenaTeam->IsPurityPlayerBotTeam())
			continue;
		if (arenaTeam->GetType() != arenaTeam->GetMembersSize())
			continue;
		CustomItem* pMenu = new CustomItem(++baseMenuEntry, GetMatchCustomTalkMenu(arenaTeam), 0, -1, 0, arenaTeam->GetCustomTalkMenuName(), 0, 0, 0, 0, 0, 0);
		arenaItems.push_back(pMenu);
		for (ArenaTeam::MemberList::iterator itaMem = arenaTeam->m_membersBegin(); itaMem != arenaTeam->m_membersEnd(); itaMem++)
		{
			ArenaTeamMember& mem = *itaMem;
			std::string nameText = sPlayerBotMgr->GetNameANDClassesText(mem.Guid);
			if (nameText.empty())
				continue;
			CustomItem* pMember = new CustomItem(++baseMenuEntry, pMenu->entry, MenuFunction::MENUF_ARENALIST, -1, 0, nameText, arenaTeam->GetId(), (arenaTeam->GetArenaTeamCmap() == TEAM_ALLIANCE) ? 1 : 0, arenaTeam->GetType(), 0, 0, 0);
			arenaItems.push_back(pMember);
		}
		char text[256] = { 0 };
		std::string pickText = std::string(sObjectMgr->GetTrinityStringForDBCLocale(620011)) + arenaTeam->GetArenaTeamColorName();
		//std::string pickText;
		//consoleToUtf8("点击挑战 ", pickText);
		CustomItem* pPick = new CustomItem(++baseMenuEntry, pMenu->entry, MenuFunction::MENUF_ARENALIST, -1, 0, pickText, arenaTeam->GetId(), (arenaTeam->GetArenaTeamCmap() == TEAM_ALLIANCE) ? 1 : 0, arenaTeam->GetType(), 0, 0, 0);
		arenaItems.push_back(pPick);
	}

	sCustomTalkMenu->UpdateArenaItems(arenaItems);
}

uint32 ArenaTeamMgr::FindArenaTeamPlayerBotTalent(ObjectGuid guid)
{
	std::map<ObjectGuid, uint32>::iterator itTalent = m_ArenaPlayerTalents.find(guid);
	if (itTalent == m_ArenaPlayerTalents.end())
		return 3;
	return itTalent->second;
}

void ArenaTeamMgr::UpdateArenaTeamPlayerBotTalent(ObjectGuid guid, uint32 talent)
{
	if (talent > 2)
		return;
	std::map<ObjectGuid, uint32>::iterator itTalent = m_ArenaPlayerTalents.find(guid);
	m_ArenaPlayerTalents[guid] = talent;
	char sql[256] = { 0 };
	if (itTalent == m_ArenaPlayerTalents.end())
	{
		sprintf_s(sql, 255, "INSERT INTO `arena_team_talents`(`guid`,`talent`) VALUES (%d,%d);", uint32(guid.GetCounter()), talent);
	}
	else
	{
		sprintf_s(sql, 255, "UPDATE `arena_team_talents` SET `talent` = %d WHERE `guid` = %d;", talent, uint32(guid.GetCounter()));
	}
	CharacterDatabase.DirectExecute(sql);
}

uint32 ArenaTeamMgr::GetMatchCustomTalkMenu(ArenaTeam* arenaTeam)
{
	TeamId aTeam = arenaTeam->GetArenaTeamCmap();
	switch (arenaTeam->GetType())
	{
	case 2:
		return (aTeam == TEAM_ALLIANCE) ? 84 : 87;
	case 3:
		return (aTeam == TEAM_ALLIANCE) ? 85 : 88;
	case 5:
		return (aTeam == TEAM_ALLIANCE) ? 86 : 89;
	}
	return 0;
}

void ArenaTeamMgr::DistributeArenaPoints()
{
    // Used to distribute arena points based on last week's stats
    sWorld->SendWorldText(LANG_DIST_ARENA_POINTS_START);

    sWorld->SendWorldText(LANG_DIST_ARENA_POINTS_ONLINE_START);

    // Temporary structure for storing maximum points to add values for all players
    std::map<uint32, uint32> PlayerPoints;

    // At first update all points for all team members
    for (ArenaTeamContainer::iterator teamItr = GetArenaTeamMapBegin(); teamItr != GetArenaTeamMapEnd(); ++teamItr)
        if (ArenaTeam* at = teamItr->second)
            at->UpdateArenaPointsHelper(PlayerPoints);

    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    PreparedStatement* stmt;

    // Cycle that gives points to all players
    for (std::map<uint32, uint32>::iterator playerItr = PlayerPoints.begin(); playerItr != PlayerPoints.end(); ++playerItr)
    {
        // Add points to player if online
        if (Player* player = ObjectAccessor::FindConnectedPlayer(ObjectGuid(HighGuid::Player, playerItr->first)))
            player->ModifyArenaPoints(playerItr->second, trans);
        else    // Update database
        {
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_CHAR_ARENA_POINTS);
            stmt->setUInt32(0, playerItr->second);
            stmt->setUInt32(1, playerItr->first);
            trans->Append(stmt);
        }
    }

    CharacterDatabase.CommitTransaction(trans);

    PlayerPoints.clear();

    sWorld->SendWorldText(LANG_DIST_ARENA_POINTS_ONLINE_END);

    sWorld->SendWorldText(LANG_DIST_ARENA_POINTS_TEAM_START);
    for (ArenaTeamContainer::iterator titr = GetArenaTeamMapBegin(); titr != GetArenaTeamMapEnd(); ++titr)
    {
        if (ArenaTeam* at = titr->second)
        {
            at->FinishWeek();
            at->SaveToDB();
            at->NotifyStatsChanged();
        }
    }

    sWorld->SendWorldText(LANG_DIST_ARENA_POINTS_TEAM_END);

    sWorld->SendWorldText(LANG_DIST_ARENA_POINTS_END);
}
