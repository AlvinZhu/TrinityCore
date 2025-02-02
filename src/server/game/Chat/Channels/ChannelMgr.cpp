/*
 * Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

#include "ChannelMgr.h"
#include "Player.h"
#include "World.h"
#include "PlayerBotTalkMgr.h"

ChannelMgr::~ChannelMgr()
{
    for (ChannelMap::iterator itr = channels.begin(); itr != channels.end(); ++itr)
        delete itr->second;
}

ChannelMgr* ChannelMgr::forTeam(uint32 team)
{
    static ChannelMgr allianceChannelMgr;
    static ChannelMgr hordeChannelMgr;
    if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHANNEL))
        return &allianceChannelMgr;        // cross-faction

    if (team == ALLIANCE)
        return &allianceChannelMgr;

    if (team == HORDE)
        return &hordeChannelMgr;

    return nullptr;
}

Channel* ChannelMgr::GetJoinChannel(std::string const& name, uint32 channelId)
{
	if (channelId == 0 && defaultChannel)
		return defaultChannel;
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return nullptr;

    wstrToLower(wname);

    ChannelMap::const_iterator i = channels.find(wname);

    if (i == channels.end())
    {
        Channel* nchan = new Channel(name, channelId, team);
        channels[wname] = nchan;
		if (channelId == 0)
		{
			defaultChannel = nchan;
			sPlayerBotTalkMgr->SetDefaultChannel(nchan);
		}
        return nchan;
    }

    return i->second;
}

Channel* ChannelMgr::GetChannel(std::string const& name, Player* player, bool pkt)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return nullptr;

    wstrToLower(wname);

    ChannelMap::const_iterator i = channels.find(wname);

    if (i == channels.end())
    {
        if (pkt)
        {
            WorldPacket data;
            MakeNotOnPacket(&data, name);
            player->GetSession()->SendPacket(&data);
        }

        return nullptr;
    }

    return i->second;
}

void ChannelMgr::LeftChannel(std::string const& name)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return;

    wstrToLower(wname);

    ChannelMap::const_iterator i = channels.find(wname);

    if (i == channels.end())
        return;

    Channel* channel = i->second;
	if (channel == defaultChannel)
		return;

    if (!channel->GetNumPlayers() && !channel->IsConstant())
    {
        channels.erase(wname);
        delete channel;
    }
}

void ChannelMgr::MakeNotOnPacket(WorldPacket* data, std::string const& name)
{
    data->Initialize(SMSG_CHANNEL_NOTIFY, 1 + name.size());
    (*data) << uint8(5) << name;
}
