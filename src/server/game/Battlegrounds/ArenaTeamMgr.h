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

#ifndef _ARENATEAMMGR_H
#define _ARENATEAMMGR_H

#include "ArenaTeam.h"

class Player;

class TC_GAME_API ArenaTeamMgr
{
private:
    ArenaTeamMgr();
    ~ArenaTeamMgr();

public:
    static ArenaTeamMgr* instance();

    typedef std::unordered_map<uint32, ArenaTeam*> ArenaTeamContainer;

    ArenaTeam* GetArenaTeamById(uint32 arenaTeamId) const;
    ArenaTeam* GetArenaTeamByName(std::string const& arenaTeamName) const;
    ArenaTeam* GetArenaTeamByCaptain(ObjectGuid guid) const;
	ArenaTeam* GetArenaTeamByGUID(ObjectGuid playerGuid) const;
	ArenaTeam* GetArenaTypeTeamByGUID(ObjectGuid playerGuid, uint32 arenaType) const;

    void LoadArenaTeams();
    void AddArenaTeam(ArenaTeam* arenaTeam);
    void RemoveArenaTeam(uint32 Id);

    ArenaTeamContainer::iterator GetArenaTeamMapBegin() { return ArenaTeamStore.begin(); }
    ArenaTeamContainer::iterator GetArenaTeamMapEnd()   { return ArenaTeamStore.end(); }

    void DistributeArenaPoints();

    uint32 GenerateArenaTeamId();
    void SetNextArenaTeamId(uint32 Id) { NextArenaTeamId = Id; }

	void ResetPlayerBotTeamFinish() { m_AllPlayerBotTeamFinish = false; }
	uint32 SearchEnemyArenaTeam(uint32 selfArenaTeamId, Team enemy);
	bool ExistArenaTeamByType(ObjectGuid& guid, ArenaTeamTypes type);
	void BroadcastArenaTeamBuild(std::string name);
	void CheckPlayerBotArenaTeam();
	void DestoryAllArenaTeam();
	bool IsPurityArenaTeam(const ObjectGuid& guid);
	std::string GetArenaTeamVSText(uint32 arenaTeamA, uint32 arenaTeamB);
	void UpdateNextMatchOrder();
	bool BuildBotsArenaTeam(std::string name, TeamId team, uint32 arenaType, uint32 cls1, uint32 cls2, uint32 cls3, uint32 cls4, uint32 cls5, uint32 tal1, uint32 tal2, uint32 tal3, uint32 tal4, uint32 tal5);
	void SetNextPriorArenaTeam(TeamId team, uint32 arenaType, uint32 arenaTeamId);
	bool CanJoinRatedArenaQueue(Player* player, uint32 arenaType);
	void UpdateToCustomTalkMenu();
	uint32 FindArenaTeamPlayerBotTalent(ObjectGuid guid);
	void UpdateArenaTeamPlayerBotTalent(ObjectGuid guid, uint32 talent);

private:
	void VerifyArenaTeamTalentTableExist();
	uint32 GetNeedSupplementArenaTeamByType(ArenaTeamTypes type, TeamId team);
	bool AddNewPlayerBotArenaTeam(ArenaTeamTypes type, TeamId team);
	bool BuildNewPlayerBotArenaTeam(ArenaTeamTypes type, std::vector<ObjectGuid>& members, std::vector<uint32>& talents, std::string name = "");
	uint32 RandomColor32();
	bool IsValidClasses(uint32 cls);
	uint32 GetMatchCustomTalkMenu(ArenaTeam* arenaTeam);

protected:
	uint32 m_MaxPlayerBotTeam;
	bool m_AllPlayerBotTeamFinish;
    uint32 NextArenaTeamId;
    ArenaTeamContainer ArenaTeamStore;
	std::vector<uint32> m_NextMatchOrderTeams;
	std::map<ObjectGuid, uint32> m_ArenaPlayerTalents;
	uint32 m_NextMatchAssignTeams[2][3];

public:
	static bool g_AutoBuildArenaTeam;
};

#define sArenaTeamMgr ArenaTeamMgr::instance()

#endif
