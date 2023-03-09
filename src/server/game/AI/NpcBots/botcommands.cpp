/*
Name: script_bot_commands
%Complete: ???
Comment: Npcbot related commands
Category: commandscripts/custom/
*/
#include "Player.h"
#include "Chat.h"
#include "World.h"
#include "boost/date_time.hpp"
#include "Config.h"

#include "ScriptMgr.h"

#include "Spell.h"
#include "Common.h"
#include "Object.h"
#include "Language.h"
#include "AccountMgr.h"
#include "WorldSession.h"
#include "Config.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "Pet.h"
#include "CreatureTextMgr.h"
#include "ReputationMgr.h"

//UTF8ת����
inline char* _StringToUTF8a(const char *strGBK, ...)
{
	int len = MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, NULL, 0);
	WCHAR * wszUtf8 = new WCHAR[len + 1];
	//memset(wszUtf8, 0, len * 2 + 2); 
	MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, wszUtf8, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, NULL, 0, NULL, NULL);
	char *szUtf8 = new char[len + 1];
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, szUtf8, len, NULL, NULL);
	return szUtf8;
}
//UTF8ת����
#include "bot_ai.h"
#include "bothelper.h"
#include "Chat.h"
#include "Config.h"
#include "Group.h"
#include "Language.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"

class script_bot_commands : public CommandScript
{
private:
	typedef std::pair<uint32 /*id*/, std::string /*name*/> BotPair;
	static bool sortbots(BotPair p1, BotPair p2)
	{
		return p1.first < p2.first;
	}

public:
	script_bot_commands() : CommandScript("script_bot_commands") { }

	std::vector<ChatCommand> GetCommands() const
	{
		static std::vector<ChatCommand> npcbotCommandTable =
		{
			{ "add", rbac::RBAC_PERM_COMMAND_NPCBOT_ADD, false, &HandleNpcBotAddCommand, "" },
			{ "remove", rbac::RBAC_PERM_COMMAND_NPCBOT_REMOVE, false, &HandleNpcBotRemoveCommand, "" },
			{ "reset", rbac::RBAC_PERM_COMMAND_NPCBOT_RESET, false, &HandleNpcBotResetCommand, "", },
			{ "command", rbac::RBAC_PERM_COMMAND_NPCBOT_CMD, false, &HandleNpcBotCommandCommand, "" },
			{ "distance", rbac::RBAC_PERM_COMMAND_NPCBOT_DIST, false, &HandleNpcBotDistanceCommand, "" },
			{ "info", rbac::RBAC_PERM_COMMAND_NPCBOT_INFO, false, &HandleNpcBotInfoCommand, "" },
			{ "helper", rbac::RBAC_PERM_COMMAND_NPCBOT_HELPER, false, &HandleBotHelperCommand, "" },
			{ "revive", rbac::RBAC_PERM_COMMAND_NPCBOT_REVIVE, false, &HandleNpcBotReviveCommand, "" },
		};

		static std::vector<ChatCommand> commandTable =
		{
			{ "npcbot", rbac::RBAC_PERM_COMMAND_NPCBOT, false, NULL, "", npcbotCommandTable },
		};
		return commandTable;
	}

    //static bool HandleReloadEquipsCommand(ChatHandler* handler, const char* /*args*/)
    //{
    //    sLog->outInfo(LOG_FILTER_GENERAL, "Re-Loading Creature Equips...");
    //    sObjectMgr->LoadEquipmentTemplates();
    //    handler->SendGlobalGMSysMessage("DB table `creature_equip_template` (creature equipment) reloaded.");
    //    return true;
    //}

    static bool HandleBotHelperCommand(ChatHandler* handler, const char* /*args*/)
    {
        if (!handler->GetSession() && !handler->GetSession()->GetPlayer())
            return false;
        Player* player = handler->GetSession()->GetPlayer();
        handler->SetSentErrorMessage(true);
        if (player->IsInCombat() ||
            player->isDead() ||
            !player->IsAlive() ||
            player->IsInFlight() ||
            player->IsCharmed() ||
            bot_ai::CCed(player))
        {
			handler->SendSysMessage(GBKToUtf8("�������޷�ʹ�á���"));
            return false;
        }
        //close current menu
        player->PlayerTalkClass->SendCloseGossip();
        if (player->GetTrader())
            player->GetSession()->SendCancelTrade();

        BotHelper* hlpr = player->GetBotHelper();
        if (!hlpr)
        {
            hlpr = new BotHelper(player);
            player->SetBotHelper(hlpr);
        }
        return hlpr->OnGossipHello(player);
    }

    static bool HandleNpcBotInfoCommand(ChatHandler* handler, const char* /*args*/)
    {
        if (!handler->GetSession() && !handler->GetSession()->GetPlayer())
            return false;
        Player* owner = handler->GetSession()->GetPlayer();
        if (!owner->GetTarget())
        {
            handler->PSendSysMessage(".npcbot info");
			handler->PSendSysMessage(GBKToUtf8("�г��㵱ǰѡ�����С������ϸ��Ϣ������Զ����Լ����Ŷӳ�Աʹ�����ָ���"));
            handler->SetSentErrorMessage(true);
            return false;
        }
        Player* master = owner->GetSelectedPlayer();
        if (!master || (owner->GetGroup() ? !owner->GetGroup()->IsMember(master->GetGUID()) : master->GetGUID() != owner->GetGUID()))
        {
			handler->PSendSysMessage(GBKToUtf8("��Ӧ��ѡ�����Լ����Ŷӳ�Ա����"));
            handler->SetSentErrorMessage(true);
            return false;
        }
        if (!master->HaveBot())
        {
			handler->PSendSysMessage(Format(GBKToUtf8("%s û��С��飡��"), master->GetName().c_str()).c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

		handler->PSendSysMessage(Format(GBKToUtf8("%s ��С�����ϸ��Ϣ��"), master->GetName().c_str()).c_str());
		handler->PSendSysMessage(Format(GBKToUtf8("ӵ��С��飺��%u"), master->GetNpcBotsCount()).c_str());
        for (uint8 i = CLASS_WARRIOR; i != MAX_CLASSES; ++i)
        {
            uint8 count = 0;
            uint8 alivecount = 0;
            for (uint8 pos = 0; pos != master->GetMaxNpcBots(); ++pos)
            {
                if (Creature* cre = master->GetBotMap(pos)->_Cre())
                {
                    if (cre->GetBotClass() == i)
                    {
                        ++count;
                        if (cre->IsAlive())
                            ++alivecount;
                    }
                }
            }
            char const* bclass;
            switch (i)
            {
			case CLASS_WARRIOR:         bclass = GBKToUtf8("սʿ��");        break;
			case CLASS_PALADIN:         bclass = GBKToUtf8("ʥ��ʿ��");        break;
			case CLASS_MAGE:            bclass = GBKToUtf8("��ʦ��");           break;
			case CLASS_PRIEST:          bclass = GBKToUtf8("��ʦ��");         break;
			case CLASS_WARLOCK:         bclass = GBKToUtf8("��ʿ��");        break;
			case CLASS_DRUID:           bclass = GBKToUtf8("��³����");          break;
			case CLASS_DEATH_KNIGHT:    bclass = GBKToUtf8("������ʿ��");    break;
			case CLASS_ROGUE:           bclass = GBKToUtf8("Ǳ���ߡ�");          break;
			case CLASS_SHAMAN:          bclass = GBKToUtf8("������˾��");         break;
			case CLASS_HUNTER:          bclass = GBKToUtf8("���ˡ�");         break;
			default:                    bclass = GBKToUtf8("δְ֪ҵ��");   break;
            }
            if (count > 0)
				handler->PSendSysMessage(Format(GBKToUtf8("%s: %u (����%u)"), bclass, count, alivecount).c_str());
        }
        return true;
    }

    static bool HandleNpcBotDistanceCommand(ChatHandler* handler, const char* args)
    {
        if (!handler->GetSession() && !handler->GetSession()->GetPlayer())
            return false;
        Player* owner = handler->GetSession()->GetPlayer();
        if (!*args)
        {
            if (owner->HaveBot())
            {
				handler->PSendSysMessage(Format(GBKToUtf8("С����������ǡ�%u"), owner->GetBotFollowDist()).c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }
            handler->PSendSysMessage(".npcbot distance");
			handler->PSendSysMessage(GBKToUtf8("�趨С����˭��ľ��롡"));
			handler->PSendSysMessage(GBKToUtf8("������óɡ�0���������ȶԵ��˷�����������С��鲻�ṥ�����ǡ���"));
			handler->PSendSysMessage(GBKToUtf8("��С����0����󣺡�75"));
            handler->SetSentErrorMessage(true);
            return false;
        }
        char* distance = strtok((char*)args, " ");
        int8 dist = -1;

        if (distance)
            dist = (int8)atoi(distance);

        if (dist >= 0 && dist <= 75)
        {
            owner->SetBotFollowDist(dist);
            if (!owner->IsInCombat() && owner->HaveBot())
            {
                for (uint8 i = 0; i != owner->GetMaxNpcBots(); ++i)
                {
                    Creature* cre = owner->GetBotMap(i)->_Cre();
                    if (!cre || !cre->IsInWorld()) continue;
                    owner->SendBotCommandState(cre, COMMAND_FOLLOW);
                }
            }
            Group* gr = owner->GetGroup();
            if (gr && owner->GetMap()->Instanceable() && /*gr->isRaidGroup() &&*/ gr->IsLeader(owner->GetGUID()))
            {
                for (GroupReference* itr = gr->GetFirstMember(); itr != NULL; itr = itr->next())
                {
                    Player* pl = itr->GetSource();
                    if (pl && pl->IsInWorld() && pl->GetMap() == owner->GetMap())
                    {
                        pl->SetBotFollowDist(dist);
                        if (!pl->IsInCombat() && pl->HaveBot())
                        {
                            for (uint8 i = 0; i != pl->GetMaxNpcBots(); ++i)
                            {
                                Creature* cre = pl->GetBotMap(i)->_Cre();
                                if (!cre || !cre->IsInWorld()) continue;
                                pl->SendBotCommandState(cre, COMMAND_FOLLOW);
                            }
                        }
                    }
                }
            }
			handler->PSendSysMessage(Format(GBKToUtf8("С����������趨Ϊ��%u"), dist).c_str());
            return true;
        }
		handler->SendSysMessage(GBKToUtf8("�������Ӧ���ڡ�0����75֮�䡣��"));
        handler->SetSentErrorMessage(true);
        return false;
    }
	//BOT����
    static bool HandleNpcBotCommandCommand(ChatHandler* handler, const char* args)
    {
        if (!handler->GetSession() && !handler->GetSession()->GetPlayer())
            return false;
        Player* owner = handler->GetSession()->GetPlayer();
        if (!*args)
        {
            handler->PSendSysMessage(".npcbot command <command>");
			handler->PSendSysMessage(GBKToUtf8("����С���������ͣ������"));
            handler->SetSentErrorMessage(true);
            return false;
        }
        char* command = strtok((char*)args, " ");
        int8 state = -1;
        if (!strncmp(command, "s", 2) || !strncmp(command, "st", 3) || !strncmp(command, "stay", 5) || !strncmp(command, "stand", 6))
            state = COMMAND_STAY;
		else if (!strncmp(command, "f", 2) || !strncmp(command, "follow", 7) || !strncmp(command, "fol", 4) || !strncmp(command, "fo", 3))
			state = COMMAND_FOLLOW;
		else if (!strncmp(command, "sx", 3))
			state = TEXT_EMOTE_BONK;
        if (state >= 0 && owner->HaveBot())
        {
            for (uint8 i = 0; i != owner->GetMaxNpcBots(); ++i)
            {
                Creature* cre = owner->GetBotMap(i)->_Cre();
                if (!cre || !cre->IsInWorld()) 
					continue;
				if (state == TEXT_EMOTE_BONK)
					cre->GetBotAI()->ReceiveEmote(owner, state);
				else
					owner->SendBotCommandState(cre, CommandStates(state));
            }
            return true;
        }
        handler->SetSentErrorMessage(true);
        return false;
    }

    static bool HandleNpcBotRemoveCommand(ChatHandler* handler, const char* /*args*/)
    {
        if (!handler->GetSession() && !handler->GetSession()->GetPlayer())
            return false;
        Player* owner = handler->GetSession()->GetPlayer();
        ObjectGuid guid = owner->GetTarget();
        if (!guid)
        {
            handler->PSendSysMessage(".npcbot remove");
			handler->PSendSysMessage(GBKToUtf8("�Ƴ���ѡ����С��顣ѡ�����Լ����Ƴ�����С��顣��"));
            handler->SetSentErrorMessage(true);
            return false;
        }
        if (owner->isDead() || !owner->IsAlive())
        {
			owner->GetSession()->SendNotification(GBKToUtf8("���Ѿ���������"));
            handler->SetSentErrorMessage(true);
            return false;
        }
        if (guid == owner->GetGUID())
        {
            if (owner->HaveBot())
            {
                for (uint8 i = 0; i != owner->GetMaxNpcBots(); ++i)
                    owner->RemoveBot(owner->GetBotMap(i)->_Guid(), true);

                if (!owner->HaveBot())
                {
                    if (owner->GetMap()->IsNonRaidDungeon() || owner->GetMap()->IsRaid())
                    {
                        InstanceMap* map = (InstanceMap*)owner->GetMap();
                        Map::PlayerList const& plMap = map->GetPlayers();
                        if (owner->GetMap()->IsRaid())
                        {
                            for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                            {
                                if (Player* player = itr->GetSource())
                                {
                                    if (player->IsInWorld())
                                        player->RepopAtGraveyard();
                                }
                            }
                        }
                    }
					handler->PSendSysMessage(GBKToUtf8("С��鱻�ɹ��Ƴ�����"));
                    handler->SetSentErrorMessage(true);
                    return true;
                }
				handler->PSendSysMessage(GBKToUtf8("����С���û���Ƴ�����"));
                handler->SetSentErrorMessage(true);
                return false;
            }
			handler->PSendSysMessage(GBKToUtf8("�޷��ҵ�С��飡��"));
            handler->SetSentErrorMessage(true);
            return false;
        }

        Creature* cre = ObjectAccessor::GetCreature(*owner, guid);
        if (cre && cre->GetIAmABot())
        {
            Player* master = cre->GetBotOwner();
            if (!master || (master->GetGUID() != owner->GetGUID()))
            {
				handler->PSendSysMessage(GBKToUtf8("��ֻ���Ƴ����Լ���С��顣��"));
                handler->SetSentErrorMessage(true);
                return false;
            }
            uint8 pos = master->GetNpcBotSlot(guid);
            master->RemoveBot(cre->GetGUID(), true);
            if (master->GetBotMap(pos)->_Cre() == NULL)
            {
                if (!master->HaveBot())
                {
                    if (master->GetMap()->IsNonRaidDungeon() || master->GetMap()->IsRaid())
                    {
                        InstanceMap* map = (InstanceMap*)master->GetMap();
                        Map::PlayerList const& plMap = map->GetPlayers();
                        if (master->GetMap()->IsRaid())
                        {
                            for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                            {
                                if (Player* player = itr->GetSource())
                                {
                                    if (player->IsInWorld())
                                        player->RepopAtGraveyard();
                                }
                            }
                        }
                    }
                }
				handler->PSendSysMessage(GBKToUtf8("С��鱻�ɹ��Ƴ�����"));
                handler->SetSentErrorMessage(true);
                return true;
            }
			handler->PSendSysMessage(GBKToUtf8("С����޷����Ƴ�����"));
            handler->SetSentErrorMessage(true);
            return false;
        }
		handler->PSendSysMessage(GBKToUtf8("��Ӧ��ѡ���Լ������С��飡��"));
        handler->SetSentErrorMessage(true);
        return false;
    }

    static bool HandleNpcBotResetCommand(ChatHandler* handler, const char* /*args*/)
    {
        if (!handler->GetSession() && !handler->GetSession()->GetPlayer())
            return false;
        Player* owner = handler->GetSession()->GetPlayer();
        Player* master = NULL;
        bool all = false;
        ObjectGuid guid = owner->GetTarget();
        if (!guid)
        {
            handler->PSendSysMessage(".npcbot reset");
			handler->PSendSysMessage(GBKToUtf8("������ѡ����С��飬ѡ���Լ�����������С��顣��"));
            handler->SetSentErrorMessage(true);
            return false;
        }
        if (owner->isDead() || !owner->IsAlive())
        {
			owner->GetSession()->SendNotification(GBKToUtf8("���Ѿ���������"));
            handler->SetSentErrorMessage(true);
            return false;
        }
		if (guid.IsPlayer())
        {
            master = owner;
            all = true;
        }
		else if (guid.IsCreature())
        {
            if (Creature* cre = ObjectAccessor::GetCreature(*owner, guid))
                master = cre->GetBotOwner();
        }
        if (master && master->GetGUID() == owner->GetGUID())
        {
            if (!master->HaveBot())
            {
				handler->PSendSysMessage(GBKToUtf8("�޷��ҵ�С��飡��"));
                handler->SetSentErrorMessage(true);
                return false;
            }
            for (uint8 i = 0; i != master->GetMaxNpcBots(); ++i)
            {
                if (all)
				{
					master->RemoveFromGroup();
                    master->RemoveBot(master->GetBotMap(i)->_Guid());
                    if (master->GetMap()->IsNonRaidDungeon() || master->GetMap()->IsRaid())
                    {
                        InstanceMap* map = (InstanceMap*)master->GetMap();
                        Map::PlayerList const& plMap = map->GetPlayers();
                        if (master->GetMap()->IsRaid())
                        {
                            for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                            {
                                if (Player* player = itr->GetSource())
                                {
                                    if (player->IsInWorld())
                                        player->RepopAtGraveyard();
                                }
                            }
                        }
                    }
                }
                else if (master->GetBotMap(i)->_Guid() == guid)
                {
                    master->RemoveBot(guid);
                    if (master->GetNpcBotsCount() == 1)
                    {
                        if (master->GetMap()->IsNonRaidDungeon() || master->GetMap()->IsRaid())
                        {
                            InstanceMap* map = (InstanceMap*)master->GetMap();
                            Map::PlayerList const& plMap = map->GetPlayers();
                            if (master->GetMap()->IsRaid())
                            {
                                for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                                {
                                    if (Player* player = itr->GetSource())
                                    {
                                        if (player->IsInWorld())
                                            player->RepopAtGraveyard();
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
            }
            handler->SetSentErrorMessage(true);
            return true;
        }
        handler->PSendSysMessage(".npcbot reset");
		handler->PSendSysMessage(GBKToUtf8("������ѡ����С��顣�޷���ս����ʹ�á���"));
        handler->SetSentErrorMessage(true);
        return false;
    }
    //For debug purposes only
    static bool HandleNpcBotReviveCommand(ChatHandler* handler, const char* /*args*/)
    {
        if (!handler->GetSession() && !handler->GetSession()->GetPlayer())
            return false;
        Player* owner = handler->GetSession()->GetPlayer();
        if (owner)
        {
            if (owner->duel && !handler->GetSession()->GetPlayer()->IsGameMaster())
            {
				handler->PSendSysMessage(GBKToUtf8("���޷��ھ�����ʹ�����ָ���"));
                handler->SetSentErrorMessage(true);
                return false;
            }
            if (!owner->GetMap()->IsNonRaidDungeon() && !owner->GetMap()->IsRaid() && !handler->GetSession()->GetPlayer()->IsGameMaster())
            {
				handler->PSendSysMessage(GBKToUtf8("���޷���PvP����ս���;�������ʹ�����ָ���"));
                handler->SetSentErrorMessage(true);
                return false;
            }
            if ((owner->GetMap()->IsNonRaidDungeon() || owner->GetMap()->IsRaid()) && owner->GetMap()->ToInstanceMap()->GetInstanceScript() && owner->GetMap()->ToInstanceMap()->GetInstanceScript()->IsEncounterInProgress())
            {
				handler->SendSysMessage(GBKToUtf8("���޷�������ģʽ������������ս������������ʹ�����ָ���"));
                handler->SetSentErrorMessage(true);
                return false;
            }

        }
        if (owner->InBattleground())
        {
			handler->PSendSysMessage(GBKToUtf8("PvPս�����޷�����С��顣��"));
            handler->SetSentErrorMessage(true);
            return false;
        }
        if (!owner->IsInFlight() && owner->HaveBot())
        {
            for (uint8 i = 0; i != owner->GetMaxNpcBots(); ++i)
            {
                Creature* bot = owner->GetBotMap(i)->_Cre();
                if (bot && !bot->IsAlive())
                {
                    owner->SetBot(bot);
                    owner->CreateBot(0, 0, 0, true);
                }
            }
			handler->PSendSysMessage(GBKToUtf8("С����Ѹ����"));
            handler->SetSentErrorMessage(true);
            return true;
        }
        handler->PSendSysMessage(".npcbot revive");
		handler->PSendSysMessage(GBKToUtf8("��ֻ�����Ŷӳ�Աȫ������������¸���С��顣��"));
        handler->SetSentErrorMessage(true);
        return false;
    }

    static bool HandleNpcBotAddCommand(ChatHandler* handler, const char* args)
    {
        if (!handler->GetSession() && !handler->GetSession()->GetPlayer())
            return false;
        Player* owner = handler->GetSession()->GetPlayer();
        if (!owner || !*args)
        {
            handler->PSendSysMessage(".npcbot add");
			handler->PSendSysMessage(GBKToUtf8("������С���ְҵ����"));
            handler->SetSentErrorMessage(true);
            return false;
        }
        if (owner->RestrictBots())
        {
			handler->GetSession()->SendNotification(GBKToUtf8("��ǰ������ֹʹ��С��顣��"));
            handler->SetSentErrorMessage(true);
            return false;
        }
        if (owner->isDead() || !owner->IsAlive())
        {
			owner->GetSession()->SendNotification(GBKToUtf8("���Ѿ���������"));
            handler->SetSentErrorMessage(true);
            return false;
        }
        if (owner->GetGroup() && owner->GetGroup()->isRaidGroup() && owner->GetGroup()->IsFull())
        {
			handler->PSendSysMessage(GBKToUtf8("�Ŷ������������޷������ٻ�����"));
            handler->SetSentErrorMessage(true);
            return false;
        }
        if (owner->GetNpcBotsCount() >= owner->GetMaxNpcBots())
        {
			handler->PSendSysMessage(GBKToUtf8("С��������ﵽ���ޡ���"));
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* bclass = strtok((char*)args, " ");
        uint8 botclass = CLASS_NONE;

        if (!strncmp(bclass, "deathknight", 12) || !strncmp(bclass, "deathk", 7) || !strncmp(bclass, "death", 6) || !strncmp(bclass, "deat", 5) ||
            !strncmp(bclass, "dea", 4) || !strncmp(bclass, "dk", 3) || !strncmp(bclass, "de", 3))
            botclass = CLASS_DEATH_KNIGHT;
        else if (!strncmp(bclass, "druid", 6) || !strncmp(bclass, "dru", 4) || !strncmp(bclass, "dr", 3) || !strncmp(bclass, "xd", 3))
            botclass = CLASS_DRUID;
        else if (!strncmp(bclass, "hunter", 7) || !strncmp(bclass, "hunt", 5) || !strncmp(bclass, "hun", 4) || !strncmp(bclass, "hu", 3) || !strncmp(bclass, "lr", 3))
            botclass = CLASS_HUNTER;
        else if (!strncmp(bclass, "mage", 5) || !strncmp(bclass, "ma", 3) || !strncmp(bclass, "fs", 3))
            botclass = CLASS_MAGE;
        else if (!strncmp(bclass, "paladin", 8) || !strncmp(bclass, "pal", 4) || !strncmp(bclass, "pa", 3) || !strncmp(bclass, "qs", 3))
            botclass = CLASS_PALADIN;
        else if (!strncmp(bclass, "priest", 7) || !strncmp(bclass, "pri", 4) || !strncmp(bclass, "pr", 3) || !strncmp(bclass, "ms", 3))
            botclass = CLASS_PRIEST;
        else if (!strncmp(bclass, "rogue", 6) || !strncmp(bclass, "rog", 4) || !strncmp(bclass, "ro", 3) || !strncmp(bclass, "dz", 3))
            botclass = CLASS_ROGUE;
        else if (!strncmp(bclass, "shaman", 7) || !strncmp(bclass, "sham", 5) || !strncmp(bclass, "sha", 4) || !strncmp(bclass, "sh", 3) || !strncmp(bclass, "sm", 3))
            botclass = CLASS_SHAMAN;
        else if (!strncmp(bclass, "warlock", 8) || !strncmp(bclass, "warl", 5) || !strncmp(bclass, "lock", 5) || !strncmp(bclass, "ss", 3))
            botclass = CLASS_WARLOCK;
        else if (!strncmp(bclass, "warrior", 8) || !strncmp(bclass, "warr", 5) || !strncmp(bclass, "zs", 3))
            botclass = CLASS_WARRIOR;

        if (botclass == CLASS_NONE)
        {
			handler->PSendSysMessage(GBKToUtf8("С����ְҵ���󡣡�"));
            handler->SetSentErrorMessage(true);
            return false;
        }
        else if (botclass == CLASS_DEATH_KNIGHT && owner->getLevel() < 55)
        {
			handler->PSendSysMessage(GBKToUtf8("���������55�������ٻ�������ʿС��顣��"));
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint8 bots = owner->GetNpcBotsCount();
        owner->CreateNPCBot(botclass);
        owner->RefreshBot(0);
        if (owner->GetNpcBotsCount() > bots)
        {
            if (owner->IsInCombat())
				handler->PSendSysMessage(Format(GBKToUtf8("�ɹ�����С��顡(%s)������ս����������֡���"), owner->GetName().c_str()).c_str());
            else
				handler->PSendSysMessage(Format(GBKToUtf8("�ɹ�����С��顡(%s)����"), owner->GetName().c_str()).c_str());
            handler->SetSentErrorMessage(true);
            return true;
        }
		handler->PSendSysMessage(GBKToUtf8("�޷�����С��飡��"));
        handler->SetSentErrorMessage(true);
        return false;
    }
};

const uint8 GroupIcons[TARGETICONCOUNT] =
{
	/*STAR        = */0x001,
	/*CIRCLE      = */0x002,
	/*DIAMOND     = */0x004,
	/*TRIANGLE    = */0x008,
	/*MOON        = */0x010,
	/*SQUARE      = */0x020,
	/*CROSS       = */0x040,
	/*SKULL       = */0x080
};

enum HelperActions
{
	ACTION_ENABLE = 1,
	ACTION_DISABLE = 2,
	ACTION_TOGGLE = 3
};

enum BotgiverTexIDs
{
	ABANDON_MINION = 3,
	RECRUIT_MINION = 4,
	HELP_STR = 5,
	ADD_ALL = 6,
	REMOVE_ALL = 7,
	RECRUIT_WARRIOR = 8,
	RECRUIT_HUNTER = 9,
	RECRUIT_PALADIN = 10,
	RECRUIT_SHAMAN = 11,
	RECRUIT_ROGUE = 12,
	RECRUIT_DRUID = 13,
	RECRUIT_MAGE = 14,
	RECRUIT_PRIEST = 15,
	RECRUIT_WARLOCK = 16,
	RECRUIT_DEATH_KNIGHT = 17,
	ABOUT_BASIC_STR1 = 18,
	ABOUT_BASIC_STR2 = 19,
	//ABOUT_BASIC_STR3                                            = 20,
	ABOUT_ICONS_STR1 = 21,
	ABOUT_ICONS_STR2 = 22,
	ICON_STRING_STAR = 23,
	ICON_STRING_CIRCLE = 24,
	ICON_STRING_DIAMOND = 25,
	ICON_STRING_TRIANGLE = 26,
	ICON_STRING_MOON = 27,
	ICON_STRING_SQUARE = 28,
	ICON_STRING_CROSS = 29,
	ICON_STRING_SKULL = 30,
	ICON_STRING_UNKNOWN = 31,
	NO_MORE_AVAILABLE = 32,
	ONE_MORE_AVAILABLE = 33,
	SOME_MORE_AVAILABLE = 34,
	ONE_AVAILABLE = 35,
	SOME_AVAILABLE = 36,

	ENABLE_STRING,
	DISABLE_STRING,
	TOGGLE_STRING,
	BACK_STRING,
	ALL_STRING,
	MAX_STRINGS
};

enum HelperGossip
{
	SENDER_CREATE_NBOT_MENU = 11,
	SENDER_CREATE_NBOT = 12,
	SENDER_REMOVE_NBOT_MENU = 17,
	SENDER_REMOVE_NBOT = 18,

	SENDER_INFO_WHISPER = 19,

	SENDER_MAIN_PAGE,
	MAX_SENDERS
};

class item_bothelp : public ItemScript
{
public:
	item_bothelp() : ItemScript("item_bothelp"){ }

	std::string GetLocaleStringForTextID(std::string& textValue, uint32 textId, int32 localeIdx)
	{
		if (textId >= MAX_STRINGS)
		{
			TC_LOG_ERROR("misc", "botgiver:GetLocaleStringForTextID:: unknown text id: %u!", textId);
			return textValue;
		}

		if (localeIdx == DEFAULT_LOCALE)
			return textValue; //use default

		if (localeIdx < 0)
		{
			TC_LOG_ERROR("misc", "botgiver:GetLocaleStringForTextID:: unknown locale: %i! Sending default locale text...", localeIdx);
			return textValue;
		}

		uint32 idxEntry = MAKE_PAIR32(60000, textId);
		if (GossipMenuItemsLocale const* no = sObjectMgr->GetGossipMenuItemsLocale(idxEntry))
			ObjectMgr::GetLocaleString(no->OptionText, localeIdx, textValue);
		return textValue;
	}

	bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/)
	{
		if (player->IsInCombat() ||	player->isDead() ||	!player->IsAlive() ||player->IsInFlight() ||player->IsCharmed() ||bot_ai::CCed(player))
		{
			ChatHandler(player->GetSession()).PSendSysMessage(GBKToUtf8("�������޷�ʹ�á���"));
			return false;
		}
		//close current menu
		player->PlayerTalkClass->SendCloseGossip();
		if (player->GetTrader())
			player->GetSession()->SendCancelTrade();

		player->PlayerTalkClass->ClearMenus(); //in case of return;

		uint8 count = 0;

		uint8 maxNBcount = player->GetMaxNpcBots();

		bool allowNBots = sConfigMgr->GetBoolDefault("Bot.EnableNpcBots", true) && !player->RestrictBots();

		std::string tempstr;

		if (player->HaveBot())
		{
			count = player->GetNpcBotsCount();
			if (count > 0)
			{
				tempstr = GBKToUtf8("�Ƴ�һ��С���");
				player->ADD_GOSSIP_ITEM(4, GetLocaleStringForTextID(tempstr, ABANDON_MINION, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_REMOVE_NBOT_MENU, GOSSIP_ACTION_INFO_DEF + 4);
			}
			if (count < maxNBcount && allowNBots)
			{
				tempstr = GBKToUtf8("�ٻ�һ��С���");
				player->ADD_GOSSIP_ITEM(2,  GetLocaleStringForTextID(tempstr, RECRUIT_MINION, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_CREATE_NBOT_MENU, GOSSIP_ACTION_INFO_DEF + 2);
			}
		}
		else if (allowNBots && maxNBcount != 0)
		{
			tempstr = GBKToUtf8("�ٻ�һ��С���");
			player->ADD_GOSSIP_ITEM(2,  GetLocaleStringForTextID(tempstr, RECRUIT_MINION, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_CREATE_NBOT_MENU, GOSSIP_ACTION_INFO_DEF + 2);
		}

		tempstr = "Help";
		player->ADD_GOSSIP_ITEM(6, GetLocaleStringForTextID(tempstr, HELP_STR, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_INFO_WHISPER, GOSSIP_ACTION_INFO_DEF + 6);

		player->SEND_GOSSIP_MENU(8446, item->GetGUID());
		return false;
	}

	bool OnItemSelect(Player* player, Item* item, uint32 sender, uint32 action)
	{
		switch (sender)
		{
		case SENDER_MAIN_PAGE:                          OnIHello(player, item);                              break;

		case SENDER_CREATE_NBOT_MENU:                   SendCreateNPCBotMenu(player, item, action);               break;
		case SENDER_CREATE_NBOT:                        SendCreateNPCBot(player, item, action);                   break;
		case SENDER_REMOVE_NBOT_MENU:                   SendRemoveNPCBotMenu(player, item, action);               break;
		case SENDER_REMOVE_NBOT:                        SendRemoveNPCBot(player, item, action);                   break;

		case SENDER_INFO_WHISPER:                       SendBotHelpWhisper(player, item, action);                 break;

		default:
			break;
		}
		return true;
	}

	bool OnIHello(Player* player, Item* item)
	{
		player->PlayerTalkClass->ClearMenus(); //in case of return;

		uint8 count = 0;

		uint8 maxNBcount = player->GetMaxNpcBots();

		bool allowNBots = sConfigMgr->GetBoolDefault("Bot.EnableNpcBots", true) && !player->RestrictBots();

		std::string tempstr;

		if (player->HaveBot())
		{
			count = player->GetNpcBotsCount();
			if (count > 0)
			{
				tempstr = GBKToUtf8("�Ƴ�һ��С���");
				player->PlayerTalkClass->GetGossipMenu().AddMenuItem(4, 0, GetLocaleStringForTextID(tempstr, ABANDON_MINION, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_REMOVE_NBOT_MENU, GOSSIP_ACTION_INFO_DEF + 4, "", 0);
			}
			if (count < maxNBcount && allowNBots)
			{
				tempstr = GBKToUtf8("�ٻ�һ��С���");
				player->PlayerTalkClass->GetGossipMenu().AddMenuItem(2, 0, GetLocaleStringForTextID(tempstr, RECRUIT_MINION, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_CREATE_NBOT_MENU, GOSSIP_ACTION_INFO_DEF + 2, "", 0);
			}
		}
		else if (allowNBots && maxNBcount != 0)
		{
			tempstr = GBKToUtf8("�ٻ�һ��С���");
			player->PlayerTalkClass->GetGossipMenu().AddMenuItem(2, 0, GetLocaleStringForTextID(tempstr, RECRUIT_MINION, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_CREATE_NBOT_MENU, GOSSIP_ACTION_INFO_DEF + 2, "", 0);
		}

		tempstr = "Help";
		player->PlayerTalkClass->GetGossipMenu().AddMenuItem(6, 0, GetLocaleStringForTextID(tempstr, HELP_STR, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_INFO_WHISPER, GOSSIP_ACTION_INFO_DEF + 6, "", 0);

		player->SEND_GOSSIP_MENU(8446, item->GetGUID());
		return true;
	}

	void SendRemoveNPCBot(Player* player, Item* item, uint32 action)
	{
		int8 x = action - GOSSIP_ACTION_INFO_DEF;
		if (x == 1)
		{
			player->CLOSE_GOSSIP_MENU();
			for (uint8 i = 0; i != player->GetMaxNpcBots(); ++i)
				player->RemoveBot(player->GetBotMap(i)->_Guid(), true);
			return;
		}
		for (uint8 i = 0; i != player->GetMaxNpcBots(); ++i)
		{
			if (!player->GetBotMap(i)->_Cre())
				continue;
			if (x == 2)
			{
				player->RemoveBot(player->GetBotMap(i)->_Guid(), true);
				break;
			}
			--x;
		}
		player->CLOSE_GOSSIP_MENU();
	}

	void SendRemoveNPCBotMenu(Player* player, Item* item, uint32 /*action*/)
	{
		player->PlayerTalkClass->ClearMenus();
		
		if (player->GetNpcBotsCount() == 1)
		{
		uint8 y=player->GetMaxNpcBots();
		if (y>28) y=28;

			for (uint8 i = 0; i != y; ++i)
				player->RemoveBot(player->GetBotMap(i)->_Guid(), true);
			player->CLOSE_GOSSIP_MENU();
			return;
		}
		std::string tempstr = GBKToUtf8("�Ƴ�����С���");
		player->ADD_GOSSIP_ITEM(9, GetLocaleStringForTextID(tempstr, REMOVE_ALL, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_REMOVE_NBOT, GOSSIP_ACTION_INFO_DEF + 1);

		uint8 x = 2;
		for (uint8 i = 0; i != player->GetMaxNpcBots(); ++i)
		{
			Creature* bot = player->GetBotMap(i)->_Cre();
			if (!bot) continue;
			player->ADD_GOSSIP_ITEM(9, bot->GetName(), SENDER_REMOVE_NBOT, GOSSIP_ACTION_INFO_DEF + x);
			++x;
		}

		tempstr = GBKToUtf8("[����]");
		//player->ADD_GOSSIP_ITEM(0, GetLocaleStringForTextID(tempstr, BACK_STRING, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_MAIN_PAGE, GOSSIP_ACTION_INFO_DEF + 99);
player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF����|CFF009933 "), 5, GOSSIP_ACTION_INFO_DEF + 99);
		player->PlayerTalkClass->SendGossipMenu(8446, item->GetGUID());
	}

	void SendCreateNPCBot(Player* player, Item* item, uint32 action)
	{
		uint8 bot_class = 0;
		if (action == GOSSIP_ACTION_INFO_DEF + 1)//"Back"
		{
			player->CLOSE_GOSSIP_MENU();
			return;
		}
		else if (action == GOSSIP_ACTION_INFO_DEF + 2)
			bot_class = CLASS_WARRIOR;
		else if (action == GOSSIP_ACTION_INFO_DEF + 3)
			bot_class = CLASS_HUNTER;
		else if (action == GOSSIP_ACTION_INFO_DEF + 4)
			bot_class = CLASS_PALADIN;
		else if (action == GOSSIP_ACTION_INFO_DEF + 5)
			bot_class = CLASS_SHAMAN;
		else if (action == GOSSIP_ACTION_INFO_DEF + 6)
			bot_class = CLASS_ROGUE;
		else if (action == GOSSIP_ACTION_INFO_DEF + 7)
			bot_class = CLASS_DRUID;
		else if (action == GOSSIP_ACTION_INFO_DEF + 8)
			bot_class = CLASS_MAGE;
		else if (action == GOSSIP_ACTION_INFO_DEF + 9)
			bot_class = CLASS_PRIEST;
		else if (action == GOSSIP_ACTION_INFO_DEF + 10)
			bot_class = CLASS_WARLOCK;
		else if (action == GOSSIP_ACTION_INFO_DEF + 11)
			bot_class = CLASS_DEATH_KNIGHT;

		if (bot_class != 0)
			player->CreateNPCBot(bot_class);
		player->CLOSE_GOSSIP_MENU();
		return;
	}

	void SendCreateNPCBotMenu(Player* player, Item* item, uint32 /*action*/)
	{
		std::string cost = player->GetNpcBotCostStr();
		player->PlayerTalkClass->ClearMenus();

		std::string tempstr = "";
		tempstr = GBKToUtf8("[�ٻ�]սʿ");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_WARRIOR, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 2);
		tempstr = GBKToUtf8("[�ٻ�]����");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_HUNTER, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 3);
		tempstr = GBKToUtf8("[�ٻ�]��ʿ");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_PALADIN, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 4);
		tempstr = GBKToUtf8("[�ٻ�]����");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_SHAMAN, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 5);
		tempstr = GBKToUtf8("[�ٻ�]����");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_ROGUE, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 6);
		tempstr = GBKToUtf8("[�ٻ�]С��");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_DRUID, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 7);
		tempstr = GBKToUtf8("[�ٻ�]��ʦ");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_MAGE, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 8);
		tempstr = GBKToUtf8("[�ٻ�]��ʦ");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_PRIEST, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 9);
		tempstr = GBKToUtf8("[�ٻ�]��ʿ");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_WARLOCK, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 10);
		if (player->getLevel() >= 55)
		{
			tempstr = GBKToUtf8("[�ٻ�]����");
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_DEATH_KNIGHT, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 11);
		}

		std::ostringstream buff;
		uint8 bots = player->GetNpcBotsCount();
		uint8 maxNBcount = player->GetMaxNpcBots();
		uint32 freeNBSlots = maxNBcount - bots;

		if (freeNBSlots == 0)
		{
			tempstr = GBKToUtf8("û�и�����ٻ�!");
			buff << GetLocaleStringForTextID(tempstr, NO_MORE_AVAILABLE, player->GetSession()->GetSessionDbLocaleIndex());
		}
		else
		{
			buff << freeNBSlots;
			buff << ' ';
			if (freeNBSlots == 1)
			{
				if (bots == 0)
				{
					tempstr = GBKToUtf8("�ٻ�����");
					buff << GetLocaleStringForTextID(tempstr, ONE_AVAILABLE, player->GetSession()->GetSessionDbLocaleIndex());
				}
				else
				{
					tempstr = GBKToUtf8("���ٻ�����");
					buff << GetLocaleStringForTextID(tempstr, ONE_MORE_AVAILABLE, player->GetSession()->GetSessionDbLocaleIndex());
				}
			}
			else
			{
				if (bots == 0)
				{
					tempstr = GBKToUtf8("�ٻ�����");
					buff << GetLocaleStringForTextID(tempstr, SOME_AVAILABLE, player->GetSession()->GetSessionDbLocaleIndex());
				}
				else
				{
					tempstr = GBKToUtf8("���ٻ�����");
					buff << GetLocaleStringForTextID(tempstr, SOME_MORE_AVAILABLE, player->GetSession()->GetSessionDbLocaleIndex());
				}
			}
		}
		player->ADD_GOSSIP_ITEM(0, buff.str(), SENDER_CREATE_NBOT_MENU, GOSSIP_ACTION_INFO_DEF + 12);

		//tempstr = "BACK";
		//player->ADD_GOSSIP_ITEM(0, GetLocaleStringForTextID(tempstr, BACK_STRING, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_MAIN_PAGE, GOSSIP_ACTION_INFO_DEF + 13);
player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF����|CFF009933 "), 5, GOSSIP_ACTION_INFO_DEF + 99);
		player->PlayerTalkClass->SendGossipMenu(8446, item->GetGUID());
	}

	void SendBotHelpWhisper(Player* player, Item* item, uint32 /*action*/)
	{
		player->CLOSE_GOSSIP_MENU();
		ChatHandler ch(player->GetSession());
		//Basic
		std::string tempstr = GBKToUtf8("�鿴��ʹ�õ����� .npcbot or .npcb");
		std::string msg2 = GetLocaleStringForTextID(tempstr, ABOUT_BASIC_STR2, player->GetSession()->GetSessionDbLocaleIndex());
		ch.SendSysMessage(msg2.c_str());
		//Heal Icons
		uint8 mask = sConfigMgr->GetIntDefault("Bot.HealTargetIconsMask", 8);
		std::string msg4 = "";
		if (mask == 255)
		{
			tempstr = GBKToUtf8("�������ٻ�BB��ְ����Ŷ�ͼ��");
			msg4 = GetLocaleStringForTextID(tempstr, ABOUT_ICONS_STR1, player->GetSession()->GetSessionDbLocaleIndex());
			ch.SendSysMessage(msg4.c_str());
		}
		else if (mask != 0)
		{
			tempstr = GBKToUtf8("�������ٻ�BB��ְ����Ŷ�ͼ��:");
			msg4 = GetLocaleStringForTextID(tempstr, ABOUT_ICONS_STR2, player->GetSession()->GetSessionDbLocaleIndex());
			std::string iconrow = "";
			uint8 count = 0;
			for (uint8 i = 0; i != TARGETICONCOUNT; ++i)
			{
				if (mask & GroupIcons[i])
				{
					if (count != 0)
						iconrow += ", ";
					++count;
					switch (i)
					{
					case 0:
						tempstr = "star";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_STAR, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					case 1:
						tempstr = "circle";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_CIRCLE, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					case 2:
						tempstr = "diamond";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_DIAMOND, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					case 3:
						tempstr = "triangle";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_TRIANGLE, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					case 4:
						tempstr = "moon";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_MOON, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					case 5:
						tempstr = "square";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_SQUARE, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					case 6:
						tempstr = "cross";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_CROSS, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					case 7:
						tempstr = "skull";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_SKULL, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					default:
						tempstr = "unknown icon";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_UNKNOWN, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					}
				}
			}
			msg4 += iconrow;
			ch.SendSysMessage(msg4.c_str());
		}
	}

};

class item_teleport : public ItemScript
	{
	public:
		item_teleport() : ItemScript("item_teleport") { }

std::string GetLocaleStringForTextID(std::string& textValue, uint32 textId, int32 localeIdx)
	{
		if (textId >= MAX_STRINGS)
		{
			TC_LOG_ERROR("misc", "botgiver:GetLocaleStringForTextID:: unknown text id: %u!", textId);
			return textValue;
		}

		if (localeIdx == DEFAULT_LOCALE)
			return textValue; //use default

		if (localeIdx < 0)
		{
			TC_LOG_ERROR("misc", "botgiver:GetLocaleStringForTextID:: unknown locale: %i! Sending default locale text...", localeIdx);
			return textValue;
		}

		uint32 idxEntry = MAKE_PAIR32(60000, textId);
		if (GossipMenuItemsLocale const* no = sObjectMgr->GetGossipMenuItemsLocale(idxEntry))
			ObjectMgr::GetLocaleString(no->OptionText, localeIdx, textValue);
		return textValue;
	}


bool OnIHello(Player* player, Item* item)
	{
		player->PlayerTalkClass->ClearMenus(); //in case of return;

		uint8 count = 0;

		uint8 maxNBcount = player->GetMaxNpcBots();

		bool allowNBots = sConfigMgr->GetBoolDefault("Bot.EnableNpcBots", true) && !player->RestrictBots();

		std::string tempstr;

		if (player->HaveBot())
		{
			count = player->GetNpcBotsCount();
			if (count > 0)
			{
				tempstr = GBKToUtf8("�Ƴ�һ��С���");
				player->PlayerTalkClass->GetGossipMenu().AddMenuItem(4, 0, GetLocaleStringForTextID(tempstr, ABANDON_MINION, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_REMOVE_NBOT_MENU, GOSSIP_ACTION_INFO_DEF + 4, "", 0);
			}
			if (count < maxNBcount && allowNBots)
			{
				tempstr = GBKToUtf8("�ٻ�һ��С���");
				player->PlayerTalkClass->GetGossipMenu().AddMenuItem(2, 0, GetLocaleStringForTextID(tempstr, RECRUIT_MINION, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_CREATE_NBOT_MENU, GOSSIP_ACTION_INFO_DEF + 2, "", 0);
			}
		}
		else if (allowNBots && maxNBcount != 0)
		{
			tempstr = GBKToUtf8("�ٻ�һ��С���");
			player->PlayerTalkClass->GetGossipMenu().AddMenuItem(2, 0, GetLocaleStringForTextID(tempstr, RECRUIT_MINION, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_CREATE_NBOT_MENU, GOSSIP_ACTION_INFO_DEF + 2, "", 0);
		}

		tempstr = "Help";
		player->PlayerTalkClass->GetGossipMenu().AddMenuItem(6, 0, GetLocaleStringForTextID(tempstr, HELP_STR, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_INFO_WHISPER, GOSSIP_ACTION_INFO_DEF + 6, "", 0);

		player->SEND_GOSSIP_MENU(99990, item->GetGUID());
		return true;
	}

	void SendRemoveNPCBot(Player* player, Item* item, uint32 action)
	{
		int8 x = action - GOSSIP_ACTION_INFO_DEF;
		if (x == 1)
		{
			player->CLOSE_GOSSIP_MENU();
			for (uint8 i = 0; i != player->GetMaxNpcBots(); ++i)
				player->RemoveBot(player->GetBotMap(i)->_Guid(), true);
			return;
		}
		for (uint8 i = 0; i != player->GetMaxNpcBots(); ++i)
		{
			if (!player->GetBotMap(i)->_Cre())
				continue;
			if (x == 2)
			{
				player->RemoveBot(player->GetBotMap(i)->_Guid(), true);
				break;
			}
			--x;
		}
		player->CLOSE_GOSSIP_MENU();
	}

	void SendRemoveNPCBotMenu(Player* player, Item* item, uint32 /*action*/)
	{
		player->PlayerTalkClass->ClearMenus();
		uint8 y=player->GetMaxNpcBots();
		if (y>28) y=28;
		if (player->GetNpcBotsCount() == 1)
		{
			for (uint8 i = 0; i != y; ++i)
				player->RemoveBot(player->GetBotMap(i)->_Guid(), true);
			player->CLOSE_GOSSIP_MENU();
			return;
		}
		std::string tempstr = GBKToUtf8("�Ƴ�����С���");
		player->ADD_GOSSIP_ITEM(9, GetLocaleStringForTextID(tempstr, REMOVE_ALL, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_REMOVE_NBOT, GOSSIP_ACTION_INFO_DEF + 1);

		uint8 x = 2;
		for (uint8 i = 0; i != player->GetMaxNpcBots(); ++i)
		{
			Creature* bot = player->GetBotMap(i)->_Cre();
			if (!bot) continue;
			player->ADD_GOSSIP_ITEM(9, bot->GetName(), SENDER_REMOVE_NBOT, GOSSIP_ACTION_INFO_DEF + x);
			++x;
		}

		tempstr = GBKToUtf8("[����]");
		//player->ADD_GOSSIP_ITEM(0, GetLocaleStringForTextID(tempstr, BACK_STRING, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_MAIN_PAGE, GOSSIP_ACTION_INFO_DEF + 99);
		player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF����|CFF009933 "), 5, GOSSIP_ACTION_INFO_DEF + 99);

		player->PlayerTalkClass->SendGossipMenu(99990, item->GetGUID());
	}

	void SendCreateNPCBot(Player* player, Item* item, uint32 action)
	{
		uint8 bot_class = 0;
		if (action == GOSSIP_ACTION_INFO_DEF + 1)//"Back"
		{
			player->CLOSE_GOSSIP_MENU();
			return;
		}
		else if (action == GOSSIP_ACTION_INFO_DEF + 2)
			bot_class = CLASS_WARRIOR;
		else if (action == GOSSIP_ACTION_INFO_DEF + 3)
			bot_class = CLASS_HUNTER;
		else if (action == GOSSIP_ACTION_INFO_DEF + 4)
			bot_class = CLASS_PALADIN;
		else if (action == GOSSIP_ACTION_INFO_DEF + 5)
			bot_class = CLASS_SHAMAN;
		else if (action == GOSSIP_ACTION_INFO_DEF + 6)
			bot_class = CLASS_ROGUE;
		else if (action == GOSSIP_ACTION_INFO_DEF + 7)
			bot_class = CLASS_DRUID;
		else if (action == GOSSIP_ACTION_INFO_DEF + 8)
			bot_class = CLASS_MAGE;
		else if (action == GOSSIP_ACTION_INFO_DEF + 9)
			bot_class = CLASS_PRIEST;
		else if (action == GOSSIP_ACTION_INFO_DEF + 10)
			bot_class = CLASS_WARLOCK;
		else if (action == GOSSIP_ACTION_INFO_DEF + 11)
			bot_class = CLASS_DEATH_KNIGHT;

		if (bot_class != 0)
			player->CreateNPCBot(bot_class);
		player->CLOSE_GOSSIP_MENU();
		return;
	}

	void SendCreateNPCBotMenu(Player* player, Item* item, uint32 /*action*/)
	{
		std::string cost = player->GetNpcBotCostStr();
		player->PlayerTalkClass->ClearMenus();

		std::string tempstr = "";
		tempstr = GBKToUtf8("[�ٻ�]սʿ");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_WARRIOR, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 2);
		tempstr = GBKToUtf8("[�ٻ�]����");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_HUNTER, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 3);
		tempstr = GBKToUtf8("[�ٻ�]��ʿ");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_PALADIN, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 4);
		tempstr = GBKToUtf8("[�ٻ�]����");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_SHAMAN, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 5);
		tempstr = GBKToUtf8("[�ٻ�]����");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_ROGUE, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 6);
		tempstr = GBKToUtf8("[�ٻ�]С��");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_DRUID, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 7);
		tempstr = GBKToUtf8("[�ٻ�]��ʦ");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_MAGE, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 8);
		tempstr = GBKToUtf8("[�ٻ�]��ʦ");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_PRIEST, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 9);
		tempstr = GBKToUtf8("[�ٻ�]��ʿ");
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_WARLOCK, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 10);
		if (player->getLevel() >= 55)
		{
			tempstr = GBKToUtf8("[�ٻ�]����");
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetLocaleStringForTextID(tempstr, RECRUIT_DEATH_KNIGHT, player->GetSession()->GetSessionDbLocaleIndex()) + cost, SENDER_CREATE_NBOT, GOSSIP_ACTION_INFO_DEF + 11);
		}

		std::ostringstream buff;
		uint8 bots = player->GetNpcBotsCount();
		uint8 maxNBcount = player->GetMaxNpcBots();
		uint32 freeNBSlots = maxNBcount - bots;

		if (freeNBSlots == 0)
		{
			tempstr = GBKToUtf8("û�и�����ٻ�!");
			buff << GetLocaleStringForTextID(tempstr, NO_MORE_AVAILABLE, player->GetSession()->GetSessionDbLocaleIndex());
		}
		else
		{
			buff << freeNBSlots;
			buff << ' ';
			if (freeNBSlots == 1)
			{
				if (bots == 0)
				{
					tempstr = GBKToUtf8("�ٻ�����");
					buff << GetLocaleStringForTextID(tempstr, ONE_AVAILABLE, player->GetSession()->GetSessionDbLocaleIndex());
				}
				else
				{
					tempstr = GBKToUtf8("���ٻ�����");
					buff << GetLocaleStringForTextID(tempstr, ONE_MORE_AVAILABLE, player->GetSession()->GetSessionDbLocaleIndex());
				}
			}
			else
			{
				if (bots == 0)
				{
					tempstr = GBKToUtf8("�ٻ�����");
					buff << GetLocaleStringForTextID(tempstr, SOME_AVAILABLE, player->GetSession()->GetSessionDbLocaleIndex());
				}
				else
				{
					tempstr = GBKToUtf8("���ٻ�����");
					buff << GetLocaleStringForTextID(tempstr, SOME_MORE_AVAILABLE, player->GetSession()->GetSessionDbLocaleIndex());
				}
			}
		}
		player->ADD_GOSSIP_ITEM(0, buff.str(), SENDER_CREATE_NBOT_MENU, GOSSIP_ACTION_INFO_DEF + 12);

		tempstr = "BACK";
		//player->ADD_GOSSIP_ITEM(0, GetLocaleStringForTextID(tempstr, BACK_STRING, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_MAIN_PAGE, GOSSIP_ACTION_INFO_DEF + 99);
player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF����|CFF009933 "), 5, GOSSIP_ACTION_INFO_DEF + 99);
		player->PlayerTalkClass->SendGossipMenu(99990, item->GetGUID());
	}

	void SendBotHelpWhisper(Player* player, Item* item, uint32 /*action*/)
	{
		player->CLOSE_GOSSIP_MENU();
		ChatHandler ch(player->GetSession());
		//Basic
		std::string tempstr = GBKToUtf8("�鿴��ʹ�õ����� .npcbot or .npcb");
		std::string msg2 = GetLocaleStringForTextID(tempstr, ABOUT_BASIC_STR2, player->GetSession()->GetSessionDbLocaleIndex());
		ch.SendSysMessage(msg2.c_str());
		//Heal Icons
		uint8 mask = sConfigMgr->GetIntDefault("Bot.HealTargetIconsMask", 8);
		std::string msg4 = "";
		if (mask == 255)
		{
			tempstr = GBKToUtf8("�������ٻ�BB��ְ����Ŷ�ͼ��");
			msg4 = GetLocaleStringForTextID(tempstr, ABOUT_ICONS_STR1, player->GetSession()->GetSessionDbLocaleIndex());
			ch.SendSysMessage(msg4.c_str());
		}
		else if (mask != 0)
		{
			tempstr = GBKToUtf8("�������ٻ�BB��ְ����Ŷ�ͼ��:");
			msg4 = GetLocaleStringForTextID(tempstr, ABOUT_ICONS_STR2, player->GetSession()->GetSessionDbLocaleIndex());
			std::string iconrow = "";
			uint8 count = 0;
			for (uint8 i = 0; i != TARGETICONCOUNT; ++i)
			{
				if (mask & GroupIcons[i])
				{
					if (count != 0)
						iconrow += ", ";
					++count;
					switch (i)
					{
					case 0:
						tempstr = "star";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_STAR, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					case 1:
						tempstr = "circle";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_CIRCLE, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					case 2:
						tempstr = "diamond";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_DIAMOND, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					case 3:
						tempstr = "triangle";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_TRIANGLE, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					case 4:
						tempstr = "moon";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_MOON, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					case 5:
						tempstr = "square";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_SQUARE, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					case 6:
						tempstr = "cross";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_CROSS, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					case 7:
						tempstr = "skull";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_SKULL, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					default:
						tempstr = "unknown icon";
						iconrow += GetLocaleStringForTextID(tempstr, ICON_STRING_UNKNOWN, player->GetSession()->GetSessionDbLocaleIndex());
						break;
					}
				}
			}
			msg4 += iconrow;
			ch.SendSysMessage(msg4.c_str());
		}
	}

		bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/)
			{
			if (player->IsInFlight() || !player->getAttackers().empty() || player->IsInCombat())
				{
 			    player->CombatStop();
				player->GetSession()->SendAreaTriggerMessage( _StringToUTF8a("ս��״̬������ϣ����������뿪ս����"));
				player->CLOSE_GOSSIP_MENU();
				return true;
				}





			//player->Unmount();
			player->PlayerTalkClass->ClearMenus();
			player->ADD_GOSSIP_ITEM( 1, _StringToUTF8a(" |CFF0000FF|TInterface\\icons\\Spell_Shadow_Possession:30|t���������Ǵ��͡�|r"), 1011, GOSSIP_ACTION_INFO_DEF + 1);
			player->ADD_GOSSIP_ITEM( 1, _StringToUTF8a(" |CFF0000FF|TInterface\\icons\\Spell_Shadow_NetherCloak:30|t�������ͼ���͡�|r"), 1011, GOSSIP_ACTION_INFO_DEF + 2);
			//if (player->IsGameMaster())
			//{
				player->ADD_GOSSIP_ITEM(1, _StringToUTF8a(" |CFF0000FF|TInterface\\icons\\Ability_Druid_AquaticForm:30|t�������ص㴫�͡�|r"), 1011, GOSSIP_ACTION_INFO_DEF + 3);
				player->ADD_GOSSIP_ITEM(1, _StringToUTF8a(" |CFF0000FF|TInterface\\icons\\Ability_Druid_CatForm:30|t�������������͡�|r"), 1011, GOSSIP_ACTION_INFO_DEF + 4);
				player->ADD_GOSSIP_ITEM(1, _StringToUTF8a(" |CFF0000FF|TInterface\\icons\\Spell_Shadow_GrimWard:30|t���м��������͡�|r"), 1011, GOSSIP_ACTION_INFO_DEF + 5);
				player->ADD_GOSSIP_ITEM(1, _StringToUTF8a(" |CFF0000FF|TInterface\\icons\\Spell_Shadow_GrimWard:30|t���ŶӸ������͡�|r"), 1011, GOSSIP_ACTION_INFO_DEF + 6);
			//}

			player->ADD_GOSSIP_ITEM( 1, _StringToUTF8a(" |CFF0000FF|TInterface\\icons\\Spell_Shadow_Rune:30|t���������⹦�ܡ�|r"),1011, GOSSIP_ACTION_INFO_DEF + 7);


		uint8 count = 0;

		uint8 maxNBcount = player->GetMaxNpcBots();

		bool allowNBots = sConfigMgr->GetBoolDefault("Bot.EnableNpcBots", true) && !player->RestrictBots();

		std::string tempstr;

		if (player->HaveBot())
		{
			count = player->GetNpcBotsCount();
			if (count > 0)
			{
				//tempstr = GBKToUtf8("�Ƴ�һ��С���");
				tempstr =_StringToUTF8a(" |CFF0000FF|TInterface\\icons\\Spell_Shadow_Possession:30|t���Ƴ�Ӷ����顿|r");
				player->ADD_GOSSIP_ITEM(4, GetLocaleStringForTextID(tempstr, ABANDON_MINION, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_REMOVE_NBOT_MENU, GOSSIP_ACTION_INFO_DEF + 4);
			}
			if (count < maxNBcount && allowNBots)
			{
				//tempstr = GBKToUtf8("�ٻ�һ��С���");
				tempstr =_StringToUTF8a(" |CFF0000FF|TInterface\\icons\\Spell_Shadow_NetherCloak:30|t���ٻ�Ӷ����顿|r");
				player->ADD_GOSSIP_ITEM(2,  GetLocaleStringForTextID(tempstr, RECRUIT_MINION, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_CREATE_NBOT_MENU, GOSSIP_ACTION_INFO_DEF + 2);
			}
		}
		else if (allowNBots && maxNBcount != 0)
		{
			//tempstr = GBKToUtf8("�ٻ�һ��С���");
			tempstr =_StringToUTF8a(" |CFF0000FF|TInterface\\icons\\Spell_Shadow_NetherCloak:30|t���ٻ�Ӷ����顿|r");
			player->ADD_GOSSIP_ITEM(2,  GetLocaleStringForTextID(tempstr, RECRUIT_MINION, player->GetSession()->GetSessionDbLocaleIndex()), SENDER_CREATE_NBOT_MENU, GOSSIP_ACTION_INFO_DEF + 2);
		}

player->ADD_GOSSIP_ITEM( 1, _StringToUTF8a(" |CFF0000FF|TInterface\\icons\\Spell_Shadow_Rune:30|t���洢��������ӡ�|r"),1011, GOSSIP_ACTION_INFO_DEF + 8);
player->ADD_GOSSIP_ITEM( 1, _StringToUTF8a(" |CFF0000FF|TInterface\\icons\\Spell_Shadow_Rune:30|t����ȡ��ӻ����ˡ�|r"),1011, GOSSIP_ACTION_INFO_DEF + 9);
player->ADD_GOSSIP_ITEM( 1, _StringToUTF8a(" |CFF0000FF|TInterface\\icons\\Spell_Shadow_Rune:30|t��������ӻ����ˡ�|r"),1011, GOSSIP_ACTION_INFO_DEF + 10);



			
			player->SEND_GOSSIP_MENU(99990,item->GetGUID());
			return true;
			}
		//bool OnItemSelect(Player *pPlayer, Item *_Item, uint32 uiSender, uint32 uiAction , SpellCastTargets const& targets)
		//bool OnItemSelect(Player *player, Item* item , uint32 sender, uint32 action, SpellCastTargets const& targets)
		//hxsd
		bool OnItemSelect(Player* player, Item* item, uint32 sender, uint32 action)
			{
				player->PlayerTalkClass->ClearMenus();

if (player->IsInCombat() )
			{
 			    player->GetSession()->SendNotification(_StringToUTF8("ս��״̬������ϣ����������뿪ս����"));
 			    player->CombatStop();
			}

		
			//QueryResult result = WorldDatabase.PQuery("SELECT pknpcid FROM pknpc where cid= %u  limit 0,4;",player->GetGUID());
			QueryResult result = WorldDatabase.PQuery("SELECT pknpcid FROM pknpc where cid= %u  limit 0,4;",player->GetGUID());
			QueryResult resultd = CharacterDatabase.PQuery("SELECT spell FROM character_aura WHERE guid = %u;", player->GetGUID());                               
			int32 isok = sConfigMgr->GetIntDefault("pknpc_add", 1);
			if (isok<1) isok=1;
			switch(sender)
				{


		case SENDER_CREATE_NBOT_MENU:                   SendCreateNPCBotMenu(player, item, action);               break;
		case SENDER_CREATE_NBOT:                        SendCreateNPCBot(player, item, action);                   break;
		case SENDER_REMOVE_NBOT_MENU:                   SendRemoveNPCBotMenu(player, item, action);               break;
		case SENDER_REMOVE_NBOT:                        SendRemoveNPCBot(player, item, action);                   break;


				// ��ѡ��
			case GOSSIP_SENDER_MAIN :
				{
//hxsd
				//OnItemSelect(player,item,1011,action,targets);
				OnItemSelect(player,item,1011,action);
				}

				break;
				// ��������
			case 1011:
				player->PlayerTalkClass->ClearMenus();
				switch(action)
					{
					// ����

case GOSSIP_ACTION_INFO_DEF + 8 :
                    
			player->CLOSE_GOSSIP_MENU();
				player->GetSession()->SendAreaTriggerMessage( _StringToUTF8("�Ѵ洢��"));
CharacterDatabase.PExecute("update character_npcbot set isteam=0 where owner = %u and active=0;", player->GetGUID());                               
CharacterDatabase.PExecute("delete from  character_npcbot where owner = %u and active=0 and isteam=0;", player->GetGUID());                               
                    CharacterDatabase.PExecute("update character_npcbot set isteam=1 where owner = %u and active=1;", player->GetGUID());                               
                    player->ADD_GOSSIP_ITEM( 8, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933 "), 8, GOSSIP_ACTION_INFO_DEF + 99);
	             	player->SEND_GOSSIP_MENU(99987,item->GetGUID()); 
	             	break;

					// resetnpcbot
	             case GOSSIP_ACTION_INFO_DEF + 9 :
			player->CLOSE_GOSSIP_MENU();
CharacterDatabase.PExecute("update character_npcbot set active=0 where owner = %u", player->GetGUID());                               
CharacterDatabase.PExecute("update character_npcbot set active=1 where owner = %u and isteam=1;", player->GetGUID());                               
			player->GetSession()->SendAreaTriggerMessage( _StringToUTF8("�Ѷ�ȡ ��������ӻ����ˣ�"));               
           
 
     


player->ADD_GOSSIP_ITEM( 9, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933 "), 8, GOSSIP_ACTION_INFO_DEF + 99);

	             	player->SEND_GOSSIP_MENU(99987,item->GetGUID()); 
	             	break;


	             case GOSSIP_ACTION_INFO_DEF + 10 :
			player->CLOSE_GOSSIP_MENU();
			player->GetSession()->SendAreaTriggerMessage( _StringToUTF8("�������л����ˣ�"));
               
player->RemoveFromGroup();
  if (sConfigMgr->GetBoolDefault("Bot.EnableNpcBots", true))
    {
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_NPCBOTS);
        stmt->setUInt32(0, player->GetGUID());
        PreparedQueryResult result = CharacterDatabase.Query(stmt);
        if (result)
        {
            uint32 m_bot_entry = 0;
            uint8 m_bot_race = 0;
            uint8 m_bot_class = 0;
            uint32 equips[18];
            do
            {
                Field* fields = result->Fetch();
                m_bot_entry = fields[0].GetUInt32();
                m_bot_race = fields[1].GetUInt8();
                m_bot_class = fields[2].GetInt8();
                for (uint8 i = 0; i != 18; ++i)
                    equips[i] = fields[i + 4].GetUInt32();

                if (m_bot_entry && m_bot_race && m_bot_class)
                    player->SetBotMustBeCreated(m_bot_entry, m_bot_race, m_bot_class, equips);

            } while (result->NextRow());
        }
    }

               
 
     


player->ADD_GOSSIP_ITEM( 10, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933 "), 8, GOSSIP_ACTION_INFO_DEF + 99);

	             	player->SEND_GOSSIP_MENU(99987,item->GetGUID()); 
	             	break;


				case GOSSIP_ACTION_INFO_DEF + 1 :
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933  "),  2, GOSSIP_ACTION_INFO_DEF + 99);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF ¯ʯ��¼|CFF009933  "),       2, GOSSIP_ACTION_INFO_DEF + 15);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF �����ص�|CFF009933  "),       2, GOSSIP_ACTION_INFO_DEF + 16);
                // �ж������˻��ǲ���
	        	if(player->getRace() == 1 || player->getRace() == 3 || player->getRace() == 4 || player->getRace() == 7||player->getRace() ==11||player->getRace() ==22)
	          	// ���˵�
				{
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] �����|CFF009933  "),         2, GOSSIP_ACTION_INFO_DEF + 1);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ��¯��|CFF009933  "),         2, GOSSIP_ACTION_INFO_DEF + 2);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ������˹|CFF009933  "),       2, GOSSIP_ACTION_INFO_DEF + 3);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ��������|CFF009933  "),       2, GOSSIP_ACTION_INFO_DEF + 4);
				} 
		        else 
                // �����
		        { 
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] �¸�����|CFF009933  "),       2, GOSSIP_ACTION_INFO_DEF + 5);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ������|CFF009933  "),         2, GOSSIP_ACTION_INFO_DEF + 6);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] �İ���|CFF009933  "),         2, GOSSIP_ACTION_INFO_DEF + 7);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ���³�|CFF009933  "),         2, GOSSIP_ACTION_INFO_DEF + 8);
                }
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ���ݳ�|CFF009933  "),         2, GOSSIP_ACTION_INFO_DEF + 9);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] �ر�����|CFF009933  "),       2, GOSSIP_ACTION_INFO_DEF + 10);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ����Ȼ|CFF009933  "),         2, GOSSIP_ACTION_INFO_DEF + 11);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] �ӻ�ɭ|CFF009933  "),         2, GOSSIP_ACTION_INFO_DEF + 12);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ɳ˹��|CFF009933  "),         2, GOSSIP_ACTION_INFO_DEF + 13);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] �ڰ�֮��|CFF009933  "),       2, GOSSIP_ACTION_INFO_DEF + 14);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] Gm��|CFF009933  "),       2, GOSSIP_ACTION_INFO_DEF + 17);
					player->SEND_GOSSIP_MENU(99987,item->GetGUID());
					break;
                // �����ͼ���ͣ���һҳ��14
				case GOSSIP_ACTION_INFO_DEF + 2 :
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933  "),           3, GOSSIP_ACTION_INFO_DEF + 99);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(����)����ɽ|CFF009933  "),         3, GOSSIP_ACTION_INFO_DEF + 1);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(����)��շ籩|CFF009933  "),       3, GOSSIP_ACTION_INFO_DEF + 2);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(����)������뵺|CFF009933  "),     3, GOSSIP_ACTION_INFO_DEF + 3);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(����)�ɸ���|CFF009933  "),         3, GOSSIP_ACTION_INFO_DEF + 4);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(����)Ӱ�¹�|CFF009933  "),         3, GOSSIP_ACTION_INFO_DEF + 5);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(����)�޼�����|CFF009933  "),       3, GOSSIP_ACTION_INFO_DEF + 6);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(ŵɭ��)�籩�ͱ�|CFF009933  "),     3, GOSSIP_ACTION_INFO_DEF + 7);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(ŵɭ��)����|CFF009933  "),       3, GOSSIP_ACTION_INFO_DEF + 8);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(ŵɭ��)����̦ԭ|CFF009933  "),     3, GOSSIP_ACTION_INFO_DEF + 9);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(ŵɭ��)���ǻ�Ұ|CFF009933  "),     3, GOSSIP_ACTION_INFO_DEF + 10);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(ŵɭ��)����Ͽ��|CFF009933  "),     3, GOSSIP_ACTION_INFO_DEF + 11);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(ŵɭ��)��������|CFF009933  "),     3, GOSSIP_ACTION_INFO_DEF + 12);
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "),          1011, GOSSIP_ACTION_INFO_DEF + 200);
					player->SEND_GOSSIP_MENU(99993,item->GetGUID());
					break;
                // �����ͼ����(�ڶ�ҳ)
				case GOSSIP_ACTION_INFO_DEF + 200 :
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933  "),           3, GOSSIP_ACTION_INFO_DEF + 99);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(ŵɭ��)������|CFF009933  "),       3, GOSSIP_ACTION_INFO_DEF + 13);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(ŵɭ��)��ӵ��|CFF009933  "),       3, GOSSIP_ACTION_INFO_DEF + 14);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(����ķ��)���Ӷ�ɽ|CFF009933  "),   3, GOSSIP_ACTION_INFO_DEF + 15);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(����ķ��)��ƶ�֮��|CFF009933  "), 3, GOSSIP_ACTION_INFO_DEF + 16);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(����ķ��)����˹|CFF009933  "),     3, GOSSIP_ACTION_INFO_DEF + 17);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(����ķ��)�µ�ķ|CFF009933  "),     3, GOSSIP_ACTION_INFO_DEF + 18);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(����ķ��)��ƶ�֮��|CFF009933  "), 3, GOSSIP_ACTION_INFO_DEF + 19);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(����ķ��)ʯצɽ��|CFF009933  "),   3, GOSSIP_ACTION_INFO_DEF + 20);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(����ķ��)����֮��|CFF009933  "),   3, GOSSIP_ACTION_INFO_DEF + 21);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(����ķ��)��Ȫ��|CFF009933  "),     3, GOSSIP_ACTION_INFO_DEF + 22);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(��������)������֮��|CFF009933  "), 3, GOSSIP_ACTION_INFO_DEF + 23);
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "),           1011,GOSSIP_ACTION_INFO_DEF + 2);
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "),           1011,GOSSIP_ACTION_INFO_DEF + 201);
					player->SEND_GOSSIP_MENU(99993,item->GetGUID());
					break;
                // �����ͼ����(����ҳ)
				case GOSSIP_ACTION_INFO_DEF + 201 :
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933  "),           3, GOSSIP_ACTION_INFO_DEF + 99);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(��������)�����׶�|CFF009933  "),   3, GOSSIP_ACTION_INFO_DEF + 24);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(��������)ĺ��ߵ�|CFF009933  "),   3, GOSSIP_ACTION_INFO_DEF + 25);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(��������)��Ī��|CFF009933  "),     3, GOSSIP_ACTION_INFO_DEF + 26);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(��������)ȼ��ƽԭ|CFF009933  "),   3, GOSSIP_ACTION_INFO_DEF + 27);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(��������)������|CFF009933  "),     3, GOSSIP_ACTION_INFO_DEF + 28);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(��������)������Ұ|CFF009933  "),   3, GOSSIP_ACTION_INFO_DEF + 29);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(��������)��������|CFF009933  "),   3, GOSSIP_ACTION_INFO_DEF + 30);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(��������)����˹|CFF009933  "),   3, GOSSIP_ACTION_INFO_DEF + 31);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF(��������)����ϣ�ߵ�|CFF009933  "), 3, GOSSIP_ACTION_INFO_DEF + 32);
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "),           1011,GOSSIP_ACTION_INFO_DEF + 200);
					player->SEND_GOSSIP_MENU(99993,item->GetGUID());
					break;
				// �����ص㴫��-------------------------------------------------------------------------------------------------------------------
				case GOSSIP_ACTION_INFO_DEF + 3 :
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933 "),    4, GOSSIP_ACTION_INFO_DEF + 99);
                    // �ж������˻��ǲ���
	        	if(player->getRace() == 1 || player->getRace() == 3 || player->getRace() == 4 || player->getRace() == 7||player->getRace() ==11||player->getRace() ==22)
	          	// ���˵�
				{
                    player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ���������|CFF009933  "),     4, GOSSIP_ACTION_INFO_DEF + 1);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ���˳�����|CFF009933  "),     4, GOSSIP_ACTION_INFO_DEF + 2);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ��ҹ���������|CFF009933  "), 4, GOSSIP_ACTION_INFO_DEF + 3);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ٪�������|CFF009933  "), 4, GOSSIP_ACTION_INFO_DEF + 4);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] �����ݳ�����|CFF009933  "),   4, GOSSIP_ACTION_INFO_DEF + 5); 
					//player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ���˳�����|CFF009933  "),   4, GOSSIP_ACTION_INFO_DEF + 6); 
					player->ADD_GOSSIP_ITEM( 5, _StringToUTF8a(" |CFF0000FF[����] DK������|CFF009933  "), 4, GOSSIP_ACTION_INFO_DEF + 7);
				} 
		        else 
                // �����
		        { 
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ���˳�����|CFF009933  "),     4, GOSSIP_ACTION_INFO_DEF + 8);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ���������|CFF009933  "),     4, GOSSIP_ACTION_INFO_DEF + 9);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ţͷ�˳�����|CFF009933  "),   4, GOSSIP_ACTION_INFO_DEF + 10);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] ��ħ������|CFF009933  "),   4, GOSSIP_ACTION_INFO_DEF + 11);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] Ѫ���������|CFF009933  "),   4, GOSSIP_ACTION_INFO_DEF + 12);
					//player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[����] �ؾ�������|CFF009933  "),   4, GOSSIP_ACTION_INFO_DEF + 13);
					player->ADD_GOSSIP_ITEM( 5, _StringToUTF8a(" |CFF0000FF[����] DK������|CFF009933  "), 4, GOSSIP_ACTION_INFO_DEF + 14);
                }
				    //player->ADD_GOSSIP_ITEM( 5, _StringToUTF8a(" |CFF0000FF[��ȫ] ��ȫ����������|CFF009933  "), 4, GOSSIP_ACTION_INFO_DEF + 15);
                    player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FFȤζ����|CFF009933 "),    4, GOSSIP_ACTION_INFO_DEF + 99);
                    player->ADD_GOSSIP_ITEM(5, _StringToUTF8a(" |CFF0000FF[��è��] Ư���u|CFF009933  "), 4, GOSSIP_ACTION_INFO_DEF + 16);
                    player->ADD_GOSSIP_ITEM(5, _StringToUTF8a(" |CFF0000FF[��è��] ���L�{|CFF009933  "), 4, GOSSIP_ACTION_INFO_DEF + 17);
                    player->ADD_GOSSIP_ITEM(5, _StringToUTF8a(" |CFF0000FF[��è��] ���V����|CFF009933  "), 4, GOSSIP_ACTION_INFO_DEF + 18);
                    player->ADD_GOSSIP_ITEM(5, _StringToUTF8a(" |CFF0000FF[��è��] �������U��|CFF009933  "), 4, GOSSIP_ACTION_INFO_DEF + 19);
                    player->ADD_GOSSIP_ITEM(5, _StringToUTF8a(" |CFF0000FF[��è��] �Ȼ��Ұ|CFF009933  "), 4, GOSSIP_ACTION_INFO_DEF + 20);
                    player->ADD_GOSSIP_ITEM(5, _StringToUTF8a(" |CFF0000FF[��è��] ����ԭ|CFF009933  "), 4, GOSSIP_ACTION_INFO_DEF + 21);
                    player->ADD_GOSSIP_ITEM(5, _StringToUTF8a(" |CFF0000FF[��è��] ���R��|CFF009933  "), 4, GOSSIP_ACTION_INFO_DEF + 22);
                    player->ADD_GOSSIP_ITEM(5, _StringToUTF8a(" |CFF0000FF[��è��] �a����|CFF009933  "), 4, GOSSIP_ACTION_INFO_DEF + 23);
                    player->ADD_GOSSIP_ITEM(5, _StringToUTF8a(" |CFF0000FF[��è��] ������|CFF009933  "), 4, GOSSIP_ACTION_INFO_DEF + 24);
					
					player->SEND_GOSSIP_MENU(99986,item->GetGUID());
					break;
					// ������������һҳ��
				case GOSSIP_ACTION_INFO_DEF + 4 :
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933 "), 5, GOSSIP_ACTION_INFO_DEF + 99);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[14] ŭ���ѹ�|CFF009933  "),      5, GOSSIP_ACTION_INFO_DEF + 1);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[19] ������|CFF009933  "),      5, GOSSIP_ACTION_INFO_DEF + 2);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[22] ������Ѩ|CFF009933  "),      5, GOSSIP_ACTION_INFO_DEF + 3);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[24] Ӱ���Ǳ�|CFF009933  "),      5, GOSSIP_ACTION_INFO_DEF + 4);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[26] �ڰ���Ԩ|CFF009933  "),      5, GOSSIP_ACTION_INFO_DEF + 5);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[27] ����Ǽ���|CFF009933  "),    5, GOSSIP_ACTION_INFO_DEF + 6);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[31] �굶����|CFF009933  "),      5, GOSSIP_ACTION_INFO_DEF + 7);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[33] ŵĪ���|CFF009933  "),      5, GOSSIP_ACTION_INFO_DEF + 8);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[40] Ѫɫ�޵�Ժ|CFF009933  "),    5, GOSSIP_ACTION_INFO_DEF + 9);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[42] �굶�ߵ�|CFF009933  "),      5, GOSSIP_ACTION_INFO_DEF + 10);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[45] �´���|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 11);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70��] �������ǽ|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 12);
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "),          1011, GOSSIP_ACTION_INFO_DEF + 400);
					player->SEND_GOSSIP_MENU(99986,item->GetGUID());
					break;
					// �����������ڶ�ҳ��
             	case GOSSIP_ACTION_INFO_DEF + 400 :
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933 "), 5, GOSSIP_ACTION_INFO_DEF + 99);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70��] ��Ѫ��¯|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 13);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70��] ū��Χ��|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 14);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70��] �İ�����|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 15);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70��] ������Ĺ|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 16);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70��] �½����Ѩ|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 17);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70��] ����ػ�����|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 18);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70��] ��̩�˴���|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 19);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70��] �����ؽ�|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 20);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70��] ��Դ��|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 21);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70��] ��̬��|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 22);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70��] �������|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 23);
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "),          1011, GOSSIP_ACTION_INFO_DEF + 4);
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "),          1011, GOSSIP_ACTION_INFO_DEF + 401);
					player->SEND_GOSSIP_MENU(99986,item->GetGUID());
					break;
					// ��������������ҳ��
             	case GOSSIP_ACTION_INFO_DEF + 401 :
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933 "), 5, GOSSIP_ACTION_INFO_DEF + 99);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70��] ��Ӱ�Թ�|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 24);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70��] ��ħ����|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 25);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70��] �����ڰ�֮��|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 26);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70��] ħ��ʦƽ̨|CFF009933  "),        5, GOSSIP_ACTION_INFO_DEF + 27);
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "),          1011, GOSSIP_ACTION_INFO_DEF + 401);
					player->SEND_GOSSIP_MENU(99986,item->GetGUID());
					break;

					// �м���������һҳ��
				case GOSSIP_ACTION_INFO_DEF + 5 :
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933  "),   6, GOSSIP_ACTION_INFO_DEF + 99);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[46] ���������|CFF009933  "),      6, GOSSIP_ACTION_INFO_DEF + 1);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[49] ������|CFF009933  "),          6, GOSSIP_ACTION_INFO_DEF + 2);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[53] ʧ�������|CFF009933  "),      6, GOSSIP_ACTION_INFO_DEF + 3);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[57] ��ʯ��Ԩ|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 4);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[60] ͨ��ѧԺ|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 5);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[60] ����֮� (����)|CFF009933  "), 6, GOSSIP_ACTION_INFO_DEF + 6);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[60] ����֮� (����)|CFF009933  "), 6, GOSSIP_ACTION_INFO_DEF + 7);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[60] ����֮� (����)|CFF009933  "), 6, GOSSIP_ACTION_INFO_DEF + 8);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[60] ˹̹��ķ|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 9);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[60] ��ʯ����|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 10);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70] ������|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 11);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70] ��ɪ��ٵĳ�Ѩ|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 12);
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "),          1011, GOSSIP_ACTION_INFO_DEF + 500);
					player->SEND_GOSSIP_MENU(99993,item->GetGUID());
					break;
					// �м��������ڶ�ҳ��
             	case GOSSIP_ACTION_INFO_DEF + 500 :
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933  "),   6, GOSSIP_ACTION_INFO_DEF + 99);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70] ���Ӷ�ɽ|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 13);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70] �������|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 14);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70] �籩Ҫ��|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 15);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70] ̫��֮��|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 16);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70] ��³���ĳ�Ѩ|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 17);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[70] �ڰ����|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 18);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[80] ���ؼӵ�|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 19);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[80] ħ��|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 20);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[80] ��׿��³��|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 21);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[80] ��������|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 22);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[80] �����¡Ҫ��|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 23);
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "),          1011, GOSSIP_ACTION_INFO_DEF + 5);
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "),          1011, GOSSIP_ACTION_INFO_DEF + 501);
					player->SEND_GOSSIP_MENU(99993,item->GetGUID());
					break;
					// �м�����������ҳ��
             	case GOSSIP_ACTION_INFO_DEF + 501 :
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933  "),   6, GOSSIP_ACTION_INFO_DEF + 99);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[80] ����������|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 24);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[80] �Ŵ��|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 25);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[80] ��ʯ����|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 26);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[80] �������|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 27);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[80] ���ؼӵ�֮��|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 28);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[80] ħ��|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 29);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[80] �ھ�������|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 30);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[80] ����˹̹��ķ|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 31);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[80] ��¡���|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 32);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[80] ӳ�����|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 33);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[80] �����¯|CFF009933  "),        6, GOSSIP_ACTION_INFO_DEF + 34);
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "),          1011, GOSSIP_ACTION_INFO_DEF + 500);
					player->SEND_GOSSIP_MENU(99993,item->GetGUID());
					break;

					// �ŶӸ�������һҳ��
				case GOSSIP_ACTION_INFO_DEF + 6 :
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933  "),    7, GOSSIP_ACTION_INFO_DEF + 99);     
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] �ܻ�֮��|CFF009933  "),         7, GOSSIP_ACTION_INFO_DEF + 1);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] ��ʯ����|CFF009933  "),         7, GOSSIP_ACTION_INFO_DEF + 2);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] ���������|CFF009933  "),       7, GOSSIP_ACTION_INFO_DEF + 3);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] ����֮��|CFF009933  "),         7, GOSSIP_ACTION_INFO_DEF + 4);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] ������|CFF009933  "),           7, GOSSIP_ACTION_INFO_DEF + 5);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] ����������|CFF009933  "),       7, GOSSIP_ACTION_INFO_DEF + 6);
					//player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] ���ݿ�ϣ�ǵĳ�Ѩ|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 7);
					//player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] �ɿ�����˹|CFF009933  "),       7, GOSSIP_ACTION_INFO_DEF + 8);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] ������Ѩ|CFF009933  "),         7, GOSSIP_ACTION_INFO_DEF + 9);
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] �¶���|CFF009933  "),           7,GOSSIP_ACTION_INFO_DEF + 10);
                    //player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] ��������|CFF009933  "),       7,GOSSIP_ACTION_INFO_DEF + 11);
                    player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] ��ҫʥ��|CFF009933  "),         7,GOSSIP_ACTION_INFO_DEF + 12);
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "),     1011, GOSSIP_ACTION_INFO_DEF + 600);
					player->SEND_GOSSIP_MENU(99985,item->GetGUID());
					break;
					// �ŶӸ������ڶ�ҳ��
				case GOSSIP_ACTION_INFO_DEF + 600 :
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933  "),            7, GOSSIP_ACTION_INFO_DEF + 99); 
					player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] ����֮��|CFF009933  "),         7,GOSSIP_ACTION_INFO_DEF + 13);
                    player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] �ɿ�����˹|CFF009933  "),       7,GOSSIP_ACTION_INFO_DEF + 14);
                    player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] ʮ�־�������|CFF009933  "),     7,GOSSIP_ACTION_INFO_DEF + 15);
                    player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] ���ڳ���|CFF009933  "),         7,GOSSIP_ACTION_INFO_DEF + 16);
                    player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] ���ݿ�ϣ�ǵĳ�Ѩ|CFF009933  "), 7,GOSSIP_ACTION_INFO_DEF + 17);
                    player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] ����ʥ��|CFF009933  "),         7,GOSSIP_ACTION_INFO_DEF + 18);
                    player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] ��������ı���|CFF009933  "),         7,GOSSIP_ACTION_INFO_DEF + 55);
                    player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] ����ʯʥ��|CFF009933  "),         7,GOSSIP_ACTION_INFO_DEF + 56);
                    player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[��] �氢��|CFF009933  "),        7,GOSSIP_ACTION_INFO_DEF + 36);
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "),          1011, GOSSIP_ACTION_INFO_DEF + 6);
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |CFF0000FFȤζ����|CFF009933  "),          1011, GOSSIP_ACTION_INFO_DEF + 601);
					player->SEND_GOSSIP_MENU(99985,item->GetGUID());
					break;
					// �ŶӸ���������ҳ��
             	case GOSSIP_ACTION_INFO_DEF + 601 :
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933  "),                7, GOSSIP_ACTION_INFO_DEF + 99); 
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ����֮��|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 19);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ����֮Դ|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 20);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ĺ�Ɽ��|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 21);
                   player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] �������|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 22);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] �а�����|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 23);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ����֮��|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 24);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ʱ��֮Ѩ|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 25);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ��Դ����|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 26);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ���u����|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 27);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ����Ѫ��|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 28);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ��ʯ��Ѩ|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 29);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ����ķ����|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 30);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ��Դ֮��|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 31);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ʧ��֮��|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 32);
                    player->ADD_GOSSIP_ITEM(7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "), 1011, GOSSIP_ACTION_INFO_DEF + 600);
                    player->ADD_GOSSIP_ITEM(7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "), 1011, GOSSIP_ACTION_INFO_DEF + 602);

					player->SEND_GOSSIP_MENU(99985,item->GetGUID());
					break;
					// �ŶӸ���������ҳ��
             	case GOSSIP_ACTION_INFO_DEF + 602 :
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933  "),                7, GOSSIP_ACTION_INFO_DEF + 99); 
 player->ADD_GOSSIP_ITEM(7, _StringToUTF8a(" |cff8000FF�ر����˵�|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 99);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ��ϫ����|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 33);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ��ʯ֮��|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 34);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ����֮��|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 35);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] �氢��|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 36);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ĺ������|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 37);
//                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[85���ֱ�] ��������|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 40);
                    //player->ADD_GOSSIP_ITEM( 0, _StringToUTF8a(" |CFF0000FF[���ֱ�] ����������|CFF009933  "),        7,GOSSIP_ACTION_INFO_DEF + 41);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] �籩�Ҿ����쳧|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 43);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] ������|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 44);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] Ӱ����Ժ|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 45);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] ħ��ɽ����|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 46);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] Ѫɫ����|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 47);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] Χ���e����|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 48);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] ͨ��ѧԺ|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 49);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] ������|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 50);

                    player->ADD_GOSSIP_ITEM(7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "), 1011, GOSSIP_ACTION_INFO_DEF + 601);
                    player->ADD_GOSSIP_ITEM(7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "), 1011, GOSSIP_ACTION_INFO_DEF + 603);
					player->SEND_GOSSIP_MENU(99985,item->GetGUID());
					break;
					// �ŶӸ���������ҳ��
             	case GOSSIP_ACTION_INFO_DEF + 603 :
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933  "),                7, GOSSIP_ACTION_INFO_DEF + 99); 
 player->ADD_GOSSIP_ITEM(7, _StringToUTF8a(" |cff8000FF�ر����˵�|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 99);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] Ѫɫ�޵�Ժ|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 51);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] ����̨|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 52);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] ħ��ɽ����|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 53);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] �־�֮��|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 54);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] ��������|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 57);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] ��ս�¸�����|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 58);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] ����|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 59);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] ���㵺|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 60);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[90��è��] ���޵�|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 61);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[100����ŵ] Ӱ�¹�|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 62);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[100����ŵ] ˪����|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 63);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[100����ŵ] ���¡��|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 64);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[100����ŵ] ������|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 65);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[100����ŵ] �ɸ���|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 66);
                    player->ADD_GOSSIP_ITEM(0, _StringToUTF8a(" |CFF0000FF[100����ŵ] ��������|CFF009933  "), 7, GOSSIP_ACTION_INFO_DEF + 67);
					player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |CFF0000FF����һҳ|CFF009933  "),              1011, GOSSIP_ACTION_INFO_DEF + 602);
					player->SEND_GOSSIP_MENU(99985,item->GetGUID());
					break;

					// ���ӹ���
	             case GOSSIP_ACTION_INFO_DEF + 7 :
                    player->ADD_GOSSIP_ITEM( 7, _StringToUTF8a(" |cff8000FF�������˵�|CFF009933 "), 8, GOSSIP_ACTION_INFO_DEF + 99);
             		player->ADD_GOSSIP_ITEM(1, _StringToUTF8(" |CFF0000FF����״̬|CFF009933 "), 8, GOSSIP_ACTION_INFO_DEF + 9);
             		player->ADD_GOSSIP_ITEM( 1, _StringToUTF8(" |CFF0000FF�ٻ�����|CFF009933 "),       8, GOSSIP_ACTION_INFO_DEF + 8);
             		player->ADD_GOSSIP_ITEM( 1, _StringToUTF8a(" |CFF0000FF��������|CFF009933 "),       8, GOSSIP_ACTION_INFO_DEF + 1);
             		player->ADD_GOSSIP_ITEM( 1, _StringToUTF8a(" |CFF0000FFװ���޸�|CFF009933 "),       8, GOSSIP_ACTION_INFO_DEF + 2);
					//if (player->IsGameMaster())
//{
	             	player->ADD_GOSSIP_ITEM( 1, _StringToUTF8a(" |CFF0000FF�����е�|CFF009933 "),       8, GOSSIP_ACTION_INFO_DEF + 3);
	             	player->ADD_GOSSIP_ITEM( 1, _StringToUTF8a(" |CFF0000FF��ͼȫ��|CFF009933 "),       8, GOSSIP_ACTION_INFO_DEF + 4);
                    player->ADD_GOSSIP_ITEM( 1, _StringToUTF8a(" |CFF0000FF�����츳|CFF009933 "),       8, GOSSIP_ACTION_INFO_DEF + 5);
					player->ADD_GOSSIP_ITEM( 1, _StringToUTF8a(" |CFF0000FF���ø���|CFF009933 "),       8, GOSSIP_ACTION_INFO_DEF + 6);
					player->ADD_GOSSIP_ITEM( 1, _StringToUTF8a(" |CFF0000FF��Ϸ�汾|CFF009933 "),       8, GOSSIP_ACTION_INFO_DEF + 7);
//}
	             	player->SEND_GOSSIP_MENU(99987,item->GetGUID()); 
	             	break;


					// savenpcbot
	             

	             	
					}
	             	break;
			case 2 :
				switch(action)
					{
					// ���� ͳһ��ʽ˵����(MapID, X, Y, Z, 0);
				case GOSSIP_ACTION_INFO_DEF + 99 :
					{
					SpellCastTargets tmp;
					OnUse(player,item,tmp);
					}
					break;
					// �����
				case GOSSIP_ACTION_INFO_DEF + 1 :
					player->TeleportTo(0,-9065,434,94,0);
					break;
					// ��¯��
				case GOSSIP_ACTION_INFO_DEF + 2 :
					player->TeleportTo(0,-5032,-819,495,0);
					break;
					// ������˹
				case GOSSIP_ACTION_INFO_DEF + 3 :
					player->TeleportTo(1,9961,2055,1329,0);
					break;
					// ���R������
				case GOSSIP_ACTION_INFO_DEF + 4 :
					player->TeleportTo(530,-4071.7f,-12036.7f,-1.5f,0);
					break;
					// �¸�����
				case GOSSIP_ACTION_INFO_DEF + 5 :
					player->TeleportTo(1,1317,-4383,27,0);
					break;
					// ������
				case GOSSIP_ACTION_INFO_DEF + 6 :
					player->TeleportTo(1,-1391,140,23,0);
					break;
					// �İ���
				case GOSSIP_ACTION_INFO_DEF + 7 :
					player->TeleportTo(0,1909,235,53,0);
					break;
					//Ѫ��������
				case GOSSIP_ACTION_INFO_DEF + 8 :
					player->TeleportTo(530,9336.9f,-7278.4f,13.6f,0);
					break;
					// ���ݳ�
				case GOSSIP_ACTION_INFO_DEF + 9 :
					player->TeleportTo(1,-977,-3788,6,0);
					break;
					// �ر�����
				case GOSSIP_ACTION_INFO_DEF + 10 :
					player->TeleportTo(0,-14302,518,9,0);
					break;
					// ����Ȼ
				case GOSSIP_ACTION_INFO_DEF + 11 :
					player->TeleportTo(571,5818,452,660,0);
					break;
					// �ӻ�ɭ
				case GOSSIP_ACTION_INFO_DEF + 12 :
					player->TeleportTo(1,-7174,-3778,9,0);
					break;
					// ɳ˹��
				case GOSSIP_ACTION_INFO_DEF + 13 :
					player->TeleportTo(530,-1831,5297,-11,0);
					break;
					// �ڰ�֮��
				case GOSSIP_ACTION_INFO_DEF + 14 :
					player->TeleportTo(0,-11888,-3206,-14,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 15 :
player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, player->GetOrientation());
					break;

				case GOSSIP_ACTION_INFO_DEF + 16 :
player->TeleportTo(player->GetStartPosition());
					break;

					// Gm��
				case GOSSIP_ACTION_INFO_DEF + 17 :
					player->TeleportTo(1,16222.1f,16252.1f,12.5872f,0);
					break;
				

					}
					// �ڰ�֮��

				if(!(action==GOSSIP_ACTION_INFO_DEF + 99))
					{
					player->CLOSE_GOSSIP_MENU();

					}
				break;
				// �����ͼ����
			case 3:
				switch(action) 
					{
				case GOSSIP_ACTION_INFO_DEF + 99 :
					{

					SpellCastTargets tmp;
					OnUse(player,item,tmp);
					}
					break;
					// (����)����ɽ ͳһ��ʽ˵����(MapID, X, Y, Z, 0);
				case GOSSIP_ACTION_INFO_DEF + 1 :
					player->TeleportTo(530,2402.12f,5541.45f,264.74f,0);
					break;
					//(����)��շ籩
				case GOSSIP_ACTION_INFO_DEF + 2 :
					player->TeleportTo(530,2696.55f,3410.29f,143.18f,0);
					break;
					// (����)������뵺
				case GOSSIP_ACTION_INFO_DEF + 3 :
					player->TeleportTo(530,-276.23f,3115.77f,31.24f,0);
					break;
					// (����)�ɸ���
				case GOSSIP_ACTION_INFO_DEF + 4 :
					player->TeleportTo(530,-1216.67f,7566.88f,16.66f,0);
					break;
					// (����)Ӱ�¹�
				case GOSSIP_ACTION_INFO_DEF + 5 :
					player->TeleportTo(530,-3169.69f,2722.50f,70.56f,0);
					break;
					// (����)�޼�����
				case GOSSIP_ACTION_INFO_DEF + 6 :
					player->TeleportTo(530,82,7376,21,0);
					break;
					// (ŵɭ��)�籩�ͱ�
				case GOSSIP_ACTION_INFO_DEF + 7 :
					player->TeleportTo(571,6149.04f,-246.50f,507.00f,0);
					break;
					// (ŵɭ��)����
				case GOSSIP_ACTION_INFO_DEF + 8 :
					player->TeleportTo(571,5445.77f,-1356.71f,239.22f,0);
					break;
					// (ŵɭ��)����̦ԭ
				case GOSSIP_ACTION_INFO_DEF + 9 :
					player->TeleportTo(571,3074.39f,6177.04f,123.63f,0);
					break;
					// (ŵɭ��)���ǻ�Ұ
				case GOSSIP_ACTION_INFO_DEF + 10 :
					player->TeleportTo(571,3523.67f,1107.49f,136.61f,0);
					break;
					// (ŵɭ��)����Ͽ��
				case GOSSIP_ACTION_INFO_DEF + 11 :
					player->TeleportTo(571,1263,-4141,142,0);
					break;
					// (ŵɭ��)��������
				case GOSSIP_ACTION_INFO_DEF + 12 :
					player->TeleportTo(571,3036,-2926,112,0);
					break;
					// (ŵɭ��)���������
				case GOSSIP_ACTION_INFO_DEF + 13 :
					player->TeleportTo(571,5915,4404,-92,0);
					break;
					// (ŵɭ��)��ӵ��
				case GOSSIP_ACTION_INFO_DEF + 14 :
					player->TeleportTo(571,4549.78f,2261.62f,358.90f,0);
					break;
					// (����ķ��)���Ӷ�ɽ
				case GOSSIP_ACTION_INFO_DEF + 15 :
					player->TeleportTo(1,4635.06f,-3814.46f,943.80f,0);
					break;
					//(����ķ��)��ƶ�֮��
				case GOSSIP_ACTION_INFO_DEF + 16 :
					player->TeleportTo(1,877.79f,-2313.85f,91.66f,0);
					break;
					//(����ķ��)����˹
				case GOSSIP_ACTION_INFO_DEF + 17 :
					player->TeleportTo(1,-3809.13f,1367.86f,250.39f,0);
					break;
					// (����ķ��)�µ�ķ
				case GOSSIP_ACTION_INFO_DEF + 18 :
					player->TeleportTo(1,-9764.04f,-1695.18f,22.33f,0);
					break;
					// (����ķ��)��ƶ�֮��
				case GOSSIP_ACTION_INFO_DEF + 19 :
					player->TeleportTo(1,-3246,-1730,93,0);
					break;
					// (����ķ��)ʯצɽ��
				case GOSSIP_ACTION_INFO_DEF + 20 :
					player->TeleportTo(1,961,1055,106,0);
					break;
					// (����ķ��)����֮��
				case GOSSIP_ACTION_INFO_DEF + 21 :
					player->TeleportTo(1,-1037,1657,60,0);
					break;
					// (����ķ��)��Ȫ��
				case GOSSIP_ACTION_INFO_DEF + 22 :
					player->TeleportTo(1,6724,-4650,723,0);
					break;
					//(��������)������֮��
				case GOSSIP_ACTION_INFO_DEF + 23 :
					player->TeleportTo(0,1827.54f,-3018.05f,73.83f,0);
					break;
					// (��������)�����׶� 
				case GOSSIP_ACTION_INFO_DEF + 24 :
					player->TeleportTo(0,-180.48f,-860.04f,57.02f,0);
					break;
					// (��������)ĺ��ߵ�
				case GOSSIP_ACTION_INFO_DEF + 25 :
					player->TeleportTo(0,-2874.40f,-4477.25f,189.74f,0);
					break;
					// (��������)��Ī��
				case GOSSIP_ACTION_INFO_DEF + 26 :
					player->TeleportTo(0,-5679.33f,-1324.47f,393.32f,0);
					break;
					// (��������)ȼ��ƽԭ
				case GOSSIP_ACTION_INFO_DEF + 27 :
					player->TeleportTo(0,-8027.72f,-2056.66f,132.06f,0);
					break;
					// (��������)������ 
				case GOSSIP_ACTION_INFO_DEF + 28 :
					player->TeleportTo(0,-12023.00f,-122.84f,41.00f,0);
					break;
					// (��������)������Ұ 
				case GOSSIP_ACTION_INFO_DEF + 29 :
					player->TeleportTo(0,-10903,1258,37,0);
					break;
					// (��������)�������� 
				case GOSSIP_ACTION_INFO_DEF + 30 :
					player->TeleportTo(0,-10429,-3309,22,0);
					break;
					// (��������)����˹ 
				case GOSSIP_ACTION_INFO_DEF + 31 :
					player->TeleportTo(0,2259,252,34,0);
					break;
					// (��������)����ϣ�ߵ� 
				case GOSSIP_ACTION_INFO_DEF + 32 :
					player->TeleportTo(0,-1412.88f,-3240.91f,38.67f,0);
					break;
					}
				if(!(action==GOSSIP_ACTION_INFO_DEF + 99))
					{
					player->CLOSE_GOSSIP_MENU();

					}
				break;

				// ���������
			case 4:
				switch(action) 
					{
				case GOSSIP_ACTION_INFO_DEF + 99 :
					{

					SpellCastTargets tmp;
					OnUse(player,item,tmp);
					}
					break;
                 //һ�������˵ĳ�����
					// ��������� ͳһ��ʽ˵����(MapID, X, Y, Z, 0);
				case GOSSIP_ACTION_INFO_DEF + 1 :
					player->TeleportTo(0,-8945.808594f,-152.064667f,82.491745f,0);
					break;
					// ���˳�����
				case GOSSIP_ACTION_INFO_DEF + 2 :
					player->TeleportTo(0,-6240.32f,331.033f,382.758f,0);
					break;
					// ��ҹ���������
				case GOSSIP_ACTION_INFO_DEF + 3 :
					player->TeleportTo(1,10321.046875f,834.703125f,1326.373413f,0);
					break;
					// ٪�������
				case GOSSIP_ACTION_INFO_DEF + 4 :
					player->TeleportTo(0,-4983.42f,877.7f,274.31f,0);
					break;
					// �����ݳ�����
				case GOSSIP_ACTION_INFO_DEF + 5 :
					player->TeleportTo(530,-3963.237549f,-13929.471680f,100.529718f,0);
					break;
					// ���˳�����
				case GOSSIP_ACTION_INFO_DEF + 6 :
					player->TeleportTo(654,-1451.53f,1403.35f,35.5561f,0);
					break;
					// DK������
				case GOSSIP_ACTION_INFO_DEF + 7 :
					player->TeleportTo(609,2355.84f,-5664.77f,426.028f,0);
					break;
                //һ���ǲ���ĳ�����
					// ���˳�����
				case GOSSIP_ACTION_INFO_DEF + 8 :
					player->TeleportTo(1,-618.518f,-4251.67f,38.718f,0);
					break;
					// ���������
				case GOSSIP_ACTION_INFO_DEF + 9 :
					player->TeleportTo(0,1699.85f,1706.56f,135.928f,0);
					break;
					// ţͷ�˳�����
				case GOSSIP_ACTION_INFO_DEF + 10 :
					player->TeleportTo(1,-2915.55f,-257.347f,59.2693f,0);
					break;
					// ��ħ������
				case GOSSIP_ACTION_INFO_DEF + 11 :
					player->TeleportTo(1,-1171.45f,-5263.65f,0.847728f,0);
					break;
					// Ѫ���������
				case GOSSIP_ACTION_INFO_DEF + 12 :
					player->TeleportTo(530,10349.6f,-6357.29f,33.4026f,0);
					break;
					// �ؾ�������
				case GOSSIP_ACTION_INFO_DEF + 13 :
					player->TeleportTo(648,-8423.81f,1361.3f,104.671f,0);
					break;
					// DK������
				case GOSSIP_ACTION_INFO_DEF + 14 :
					player->TeleportTo(609,2358.44f,-5666.9f,426.023f,0);
					break;
					// ��ȫ������
				case GOSSIP_ACTION_INFO_DEF + 15 :
					player->TeleportTo(1,5668.307617f,-3377.860107f,1588.408936f,0);
					break;
					//  Ư���u
				case GOSSIP_ACTION_INFO_DEF + 16 :
					player->TeleportTo(860,1471.67f,3466.25f,181.675f,0);
					break;

					//  ���L�{
				case GOSSIP_ACTION_INFO_DEF + 17 :
					player->TeleportTo(870,143.938f,-296.64f,237.065f,0);
					break;

					//  ���V����
				case GOSSIP_ACTION_INFO_DEF + 18 :
					player->TeleportTo(870,722.391f,-137.208f,330.511f,0);
					break;

					//  �������U��
				case GOSSIP_ACTION_INFO_DEF + 19 :
					player->TeleportTo(870,-385.552f,-632.212f,116.541f,0);
					break;

					//  �Ȼ��Ұ
				case GOSSIP_ACTION_INFO_DEF + 20 :
					player->TeleportTo(870,-644.544f,3918.83f,0.549382f,0);
					break;

					//  ����ԭ
				case GOSSIP_ACTION_INFO_DEF + 21 :
					player->TeleportTo(870,1126.88f,4038.66f,48.0521f,0);
					break;

					//  ���R��
				case GOSSIP_ACTION_INFO_DEF + 22 :
					player->TeleportTo(870,2439.29f,2441.66f,721.878f,0);
					break;

					//  �a����
				case GOSSIP_ACTION_INFO_DEF + 23 :
					player->TeleportTo(870,1618.63f,1977.97f,459.82f,0);
					break;

					//  ������
				case GOSSIP_ACTION_INFO_DEF + 24 :
					player->TeleportTo(870,1125.89f,3-721.41f,324.471f,0);
					break;


					}
				if(!(action==GOSSIP_ACTION_INFO_DEF + 99))
					{
					player->CLOSE_GOSSIP_MENU();

					}
				break;
				// ��������
			case 5:
				switch(action)
					{
				case GOSSIP_ACTION_INFO_DEF + 99 :
					{

					SpellCastTargets tmp;
					OnUse(player,item,tmp);
					}
					break;
					// [14] ŭ���ѹ� ͳһ��ʽ˵����(MapID, X, Y, Z, 0);
				case GOSSIP_ACTION_INFO_DEF + 1 :
					player->TeleportTo(389,2.024650f,-10.021000f,-16.187500f,0);
					break;
					// [19] ������
				case GOSSIP_ACTION_INFO_DEF + 2 :
					player->TeleportTo(36,-16.4f,-383.07f,61.78f,0);
					break;
					// [22] ������Ѩ
				case GOSSIP_ACTION_INFO_DEF + 3 :
					player->TeleportTo(43,-161.841995f,133.266998f,-73.866203f,0);
					break;
					// [24] Ӱ���Ǳ�
				case GOSSIP_ACTION_INFO_DEF + 4 :
					player->TeleportTo(33,-228.19f,2110.56f,76.89f,0);
					break;
					// [26] �ڰ���Ԩ
				case GOSSIP_ACTION_INFO_DEF + 5 :
					player->TeleportTo(48,-150.367004f,102.995003f,-40.555801f,0);
					break;
					// [27] ����Ǽ���
				case GOSSIP_ACTION_INFO_DEF + 6 :
					player->TeleportTo(34,48.29f,0.45f,-16.14f,0);
					break;
					// [31] �굶����
				case GOSSIP_ACTION_INFO_DEF + 7 :
					player->TeleportTo(47,1943,1544,82,0);
					break;
					// [33] ŵĪ���
				case GOSSIP_ACTION_INFO_DEF + 8 :
					player->TeleportTo(90,-332.562988f,-3.445f,-152.845993f,0);
					break;
					// [40] Ѫɫ�޵�Ժ
				case GOSSIP_ACTION_INFO_DEF + 9 :
					player->TeleportTo(189,855.903992f,1321.939941f,18.673000f,0);
					break;
					// [42] �굶�ߵ�
				case GOSSIP_ACTION_INFO_DEF + 10 :
					player->TeleportTo(129,2593.209961f,1109.459961f,51.093300f,0);
					break;
					// [45] �´���
				case GOSSIP_ACTION_INFO_DEF + 11 :
					player->TeleportTo(70,-227.529007f,45.009800f,-46.019600f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 12 :
					player->TeleportTo(543,-1355.28f,1643.79f,68.3951f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 13 :
					player->TeleportTo(542,-4.10161f,15.5609f,-44.7921f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 14 :
					player->TeleportTo(547,122.356f,-130.206f,-1.14206f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 15 :
					player->TeleportTo(546,15.5594f,-21.5361f,-2.75561f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 16 :
					player->TeleportTo(557,0.0191f,0.9478f,-0.9543f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 17 :
					player->TeleportTo(558,-24.2657f,0.420907f,-0.1206f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 18 :
					player->TeleportTo(560,2375.55f,1178.65f,65.46f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 19 :
					player->TeleportTo(556,-4.6811f,-0.09308f,0.0062f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 20 :
					player->TeleportTo(545,-13.8423f,6.7542f,-4.2586f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 21 :
					player->TeleportTo(554,-28.906f,0.680314f,-1.81282f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 22 :
					player->TeleportTo(553,40.0395f,-28.613f,-1.1189f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 23 :
					player->TeleportTo(540,-19.642f,0.253695f,-13.1539f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 24 :
					player->TeleportTo(555,-7.79f,-0.02f,-1.04f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 25 :
					player->TeleportTo(552,-1.23165f,0.014346f,-0.20429f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 26 :
					player->TeleportTo(269,-1542.67f,7115.22f,32.9f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 27 :
					player->TeleportTo(530,12886,-7335,66,0);
					break;
					}
																																		

				if(!(action==GOSSIP_ACTION_INFO_DEF + 99))
					{
					player->CLOSE_GOSSIP_MENU();

					}
				break;

				// �м�����
			case 6:
				switch(action)
					{
				case GOSSIP_ACTION_INFO_DEF + 99 :
					{

					SpellCastTargets tmp;
					OnUse(player,item,tmp);
					}
					break;
					// [46] ��������� ͳһ��ʽ˵����(MapID, X, Y, Z, 0);
				case GOSSIP_ACTION_INFO_DEF + 1 :
					player->TeleportTo(209,1213,841,8.9f,0);
					break;
					// [49] ������
				case GOSSIP_ACTION_INFO_DEF + 2 :
					player->TeleportTo(349,1012.700012f,-459.317993f,-43.547100f,0);
					break;
					// [53] ʧ�������
				case GOSSIP_ACTION_INFO_DEF + 3 :
					player->TeleportTo(109,-335.003f,95.39f,-90.84f,0);
					break;
					// [57] ��ʯ��Ԩ
				case GOSSIP_ACTION_INFO_DEF + 4 :
					player->TeleportTo(230,456.928986f,34.927700f,-69.388100f,0);
					break;
					// [60] ͨ��ѧԺ
				case GOSSIP_ACTION_INFO_DEF + 5 :
					player->TeleportTo(289,199,126,135,0);
					break;
					// [60] ����֮� (����)
				case GOSSIP_ACTION_INFO_DEF + 6 :
					player->TeleportTo(429,255.164001f,-17.024200f,-2.560600f,0);
					break;
					// [60] ����֮� (����)
				case GOSSIP_ACTION_INFO_DEF + 7 :
					player->TeleportTo(429,46.24f,-155.53f,-2.71349f,0);
					break;
					// [60] ����֮� (����)
				case GOSSIP_ACTION_INFO_DEF + 8 :
					player->TeleportTo(429,32.722599f,159.417007f,-3.470170f,0);
					break;
					// [60] ˹̹��ķ
				case GOSSIP_ACTION_INFO_DEF + 9 :
					player->TeleportTo(329,3392,-3379,143,0);
					break;
					// [60] ��ʯ����
				case GOSSIP_ACTION_INFO_DEF + 10 :
					player->TeleportTo(229,78.19f,-227.63f,49.72f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 11 :
					player->TeleportTo(532,-11108,-2002.06f,49.89f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 12 :
					player->TeleportTo(544,188.414f,29.3279f,67.9345f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 13 :
					player->TeleportTo(534,4222.8f,-4179.65f,869.046f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 14 :
					player->TeleportTo(548,16.1122f,0.603538f,821.727f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 15 :
					player->TeleportTo(550,-10.8021f,-1.15045f,-2.42833f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 16 :
					player->TeleportTo(580,1780.58f,918.97f,15.594f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 17 :
					player->TeleportTo(565,62.7842f,35.462f,-3.9835f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 18 :
					player->TeleportTo(530,-3640.9f,316.698f,34.9995f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 19 :
					player->TeleportTo(571,1216.73f,-4867.08f,41.25f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 20 :
					player->TeleportTo(571,3891.73f,6984.91f,69.49f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 21 :
					player->TeleportTo(571,3678.06f,2166.39f,35.79f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 22 :
					player->TeleportTo(571,3644.39f,2041.29f,1.78781f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 23 :
					player->TeleportTo(571,4775.8f,-2035.5f,229.389f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 24 :
					player->TeleportTo(571,5680.7f,487.323f,652.418f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 25 :
					player->TeleportTo(571,6969.75f,-4402.35f,441.58f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 26 :
					player->TeleportTo(571,8922.09f,-991.638f,1039.4f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 27 :
					player->TeleportTo(571,9170.21f,-1373.07f,1097.66f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 28 :
					player->TeleportTo(571,1249.92f,-4856.29f,215.92f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 29 :
					player->TeleportTo(571,3877.41f,6984.64f,106.32f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 30 :
					player->TeleportTo(571,8577.66f,791.919f,558.235f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 31 :
					player->TeleportTo(1,-8756.26f,-4449.63f,-199.752f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 32 :
					player->TeleportTo(658,439.143f,209.539f,528.709f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 33 :
					player->TeleportTo(571,5629.61f,2005.34f,869.046f,0);
					break;

				case GOSSIP_ACTION_INFO_DEF + 34 :
					player->TeleportTo(571,5662.16f,2014.82f,798.042f,0);
					break;
					}

				if(!(action==GOSSIP_ACTION_INFO_DEF + 99))
					{
					player->CLOSE_GOSSIP_MENU();

					}
				break;

				// �߼�����
			case 7:
				switch(action)
					{
				case GOSSIP_ACTION_INFO_DEF + 99 :
					{

					SpellCastTargets tmp;
					OnUse(player,item,tmp);
					}
					break;
					// [��] �ܻ�֮�� ͳһ��ʽ˵����(MapID, X, Y, Z, 0);
				case GOSSIP_ACTION_INFO_DEF + 1 :
					player->TeleportTo(409,1089.601685f,-470.190247f,-106.413055f,0);
					break;
					// [��] ��ʯ����
				case GOSSIP_ACTION_INFO_DEF + 2 :
					player->TeleportTo(229,78.339836f,-227.793518f,49.7103f,0);
					break;
					// [��] ���������
				case GOSSIP_ACTION_INFO_DEF + 3 :
					player->TeleportTo(309,-11916,-1251.469971f,92.32f,0);
					break;
					// [��] ����֮��
				case GOSSIP_ACTION_INFO_DEF + 4 :
					player->TeleportTo(469,-7674.470215f,-1108.380005f,396.649994f,0);
					break;
					// [��] ������
				case GOSSIP_ACTION_INFO_DEF + 5 :
					player->TeleportTo(531,-8212.002930f,2034.474854f,129.141342f,0);
					break;
					// [��] ����������
				case GOSSIP_ACTION_INFO_DEF + 6 :
					player->TeleportTo(509,-8443.475586f,1518.648560f,31.906958f,0);
					break;
					// [��] ���ݿ�ϣ�ǵĳ�Ѩ
				case GOSSIP_ACTION_INFO_DEF + 7 :
					player->TeleportTo(249,30.010290f,-58.840508f,-5.325367f,0);
					break;
					//[��] �ɿ�����˹ 
				case GOSSIP_ACTION_INFO_DEF + 8 :
					player->TeleportTo(533,3005.87f,-3435.01f,293.882f,0);
					break;
					// [��] ������Ѩ
				case GOSSIP_ACTION_INFO_DEF + 9 :
					player->TeleportTo(533,3700.35f,-5185.92f,143.957f,4.403038f,0);
					break;
				   // �¶���
                case GOSSIP_ACTION_INFO_DEF + 10 :
                    player->TeleportTo(571,9349, -1116 ,1246 ,0);
					break;
                   // ��������
                case GOSSIP_ACTION_INFO_DEF + 11 :
                    player->TeleportTo( 571,5474 ,2839, 419 ,0);
					break;
                   // ��ҫʥ��
                case GOSSIP_ACTION_INFO_DEF + 12 :
                    player->TeleportTo(571,3476, 265, -120,0);
					break;
                   // ����֮��
                case GOSSIP_ACTION_INFO_DEF + 13 :
                    player->TeleportTo(571,3856, 6991, 153 ,0);
					break;
                   // �ɿ�����˹
                case GOSSIP_ACTION_INFO_DEF + 14 :
                    player->TeleportTo(571,3671 ,-1274 ,244 ,0);
					break;
                   // ʮ�־�
                case GOSSIP_ACTION_INFO_DEF + 15 :
                    player->TeleportTo(571,8515 ,725, 558 ,0);
					break;
                   // ���ڳ���
                case GOSSIP_ACTION_INFO_DEF + 16 :
                    player->TeleportTo(571,5793, 2073 ,637 ,0);
					break;
                   // ���ݿ�ϣ�ǵĳ�Ѩ
                case GOSSIP_ACTION_INFO_DEF + 17 :
                    player->TeleportTo( 1,-4728, -3742 ,58,0);
					break;
				   // ����ʥ��
                case GOSSIP_ACTION_INFO_DEF + 18 :
                    player->TeleportTo( 571,3580 ,224, -120,0);
					break;

                case GOSSIP_ACTION_INFO_DEF + 55 :
                    player->TeleportTo(624,-36.6743f,-277.204f,91.3079f,0);
					break;

                case GOSSIP_ACTION_INFO_DEF + 56 :
                    player->TeleportTo(571,3489.14f,267.52f,-120.14f,0);
					break;


                // ����֮��
                 case GOSSIP_ACTION_INFO_DEF + 19 :    
                    player->TeleportTo(967,-1975.54f ,-2409.64f ,68.6797f,0);
                    break;

                // ����֮Դ
                case GOSSIP_ACTION_INFO_DEF + 20 :             
                   player->TeleportTo(646,334.65f,155.76f,49.24f,0);
                   break;

                // ĺ�Ɽ��
                case GOSSIP_ACTION_INFO_DEF + 21 :            
                   player->TeleportTo(671,-1160.0f,-850.00f,840.16f,0);
                   break;

                // �������
                case GOSSIP_ACTION_INFO_DEF + 22 :             
                   player->TeleportTo(859,-11916.09f,-1265.3f,89.92f,0);
                   break;

                // �а�����
                case GOSSIP_ACTION_INFO_DEF + 23 :             
                   player->TeleportTo(732,-850,870,140,0);
                   break;

                // ����֮��
                case GOSSIP_ACTION_INFO_DEF + 24 :             
                   player->TeleportTo(939,3546.89f,-5194.92f,238.46f,0);
                   break;

                // ʱ��֮ĩ
                case GOSSIP_ACTION_INFO_DEF + 25 :             
                   player->TeleportTo(938,3546.14f,277.45f,49.58f,0);
                   break;

                // ��Դ����
                case GOSSIP_ACTION_INFO_DEF + 26 :             
                   player->TeleportTo(644,-70.62f,366.17f,89.78f,0);
                   break;

                // ���u����
                case GOSSIP_ACTION_INFO_DEF + 27 :            
                   player->TeleportTo(657,-355.88f,-2.78f,637.66f,0);
                   break;

                // ����Ѫ��
                case GOSSIP_ACTION_INFO_DEF + 28 :             
                   player->TeleportTo(669,-318.66f,-222.44f,193.43f,0);
                   break;

                // ��ʯ��Ѩ
                case GOSSIP_ACTION_INFO_DEF + 29 :             
                   player->TeleportTo(645,214.4f,718.86f,105.5f,0);
                   break;

                // ����ķ����
                case GOSSIP_ACTION_INFO_DEF + 30 :           
                   player->TeleportTo(670,-627.52f,-175.76f,272.13f,0);
                   break;

                // ��Դ֮��
                case GOSSIP_ACTION_INFO_DEF + 31 :           
                   player->TeleportTo(720,-535.91f,313.59f,115.49f,0);
                   break;
                // ��������
                case GOSSIP_ACTION_INFO_DEF + 32 :           
                   //player->TeleportTo(669,-318.66f,-222.44f,193.43f,0);
                   player->TeleportTo(755,-10700.4f,-1312.69f,17.6129f,0);
                   break;
                case GOSSIP_ACTION_INFO_DEF + 33 :           
                   player->TeleportTo(643,-589.89f,809.07f,245.33f,0);
                   break;
                case GOSSIP_ACTION_INFO_DEF + 34 :           
                   player->TeleportTo(725,1094,877,290,0);
                   break;
                case GOSSIP_ACTION_INFO_DEF + 35 :           
                  player->TeleportTo(657,-355.88f,-2.78f,637.66f,0);
                   break;
                case GOSSIP_ACTION_INFO_DEF + 36 :           
                   player->TeleportTo(568,121,1759,43,0);
                   break;
                case GOSSIP_ACTION_INFO_DEF + 37 :           
                   player->TeleportTo(940,3656.65f,284.46f,52.22f,0);
                   break;
                case GOSSIP_ACTION_INFO_DEF + 38 :           
                   player->TeleportTo(859,-11916,-1265.3f,89.92f,0);
                   break;
                case GOSSIP_ACTION_INFO_DEF + 39 :           
                   player->TeleportTo(671,-1160,-850,840,0);
                   break;
                case GOSSIP_ACTION_INFO_DEF + 40 :           
                   player->TeleportTo(1,-11355,58.15f,723.88f,0);
                   break;
                case GOSSIP_ACTION_INFO_DEF + 41 :           
                   player->TeleportTo(755,-10700.4f,-1312.69f,17.6029f,0);
                   break;
                case GOSSIP_ACTION_INFO_DEF + 42 :           
                   player->TeleportTo(0,5967.99f,613.53f,650.63f,0);
                   break;


                case GOSSIP_ACTION_INFO_DEF + 43 :           
                   player->TeleportTo(961,-733.359f,1269.48f,116.208f,0);
                   break;                   
                case GOSSIP_ACTION_INFO_DEF + 44 :           
                   player->TeleportTo(960,949.828f,-2499.11f,181.608f,0);
                   break;
                case GOSSIP_ACTION_INFO_DEF + 45 :           
                   player->TeleportTo(959,3657.92f,2551.92f,767.066f,0);
                   break;                                      
                case GOSSIP_ACTION_INFO_DEF + 46 :           
                   player->TeleportTo(994,-3969.67f,-2542.71f,26.8537f,0);
                   break;
                case GOSSIP_ACTION_INFO_DEF + 47 :           
                   player->TeleportTo(1001,832.04f,608.16f,12.81f,0);
                   break;                   
                case GOSSIP_ACTION_INFO_DEF + 48 :           
                   player->TeleportTo(1011,1463.89f,5110.86f,156.954f,0);
                   break;
                case GOSSIP_ACTION_INFO_DEF + 49 :           
                   player->TeleportTo(1007,199.69f,102.84f,131.95f,0);
                   break;
                case GOSSIP_ACTION_INFO_DEF + 50 :           
                   player->TeleportTo(962,722.09f,2108.08f,403.07f,0);
                   break;
                case GOSSIP_ACTION_INFO_DEF + 51 :           
                   player->TeleportTo(1004,1132.48f,519.38f,0.65f,0);
                   break;                   
                case GOSSIP_ACTION_INFO_DEF + 52 :           
                   player->TeleportTo(996,-1020.99f,-3145.79f,28.3346f,0);
                   break;
                case GOSSIP_ACTION_INFO_DEF + 53 :           
                   player->TeleportTo(1008,3861.55f,1045.11f,490.17f,0);
                   break;                   
                case GOSSIP_ACTION_INFO_DEF + 54 :           
                   player->TeleportTo(1009,-2378.92f,459.879f,422.441f,0);
                   break;

        case GOSSIP_ACTION_INFO_DEF + 57 :           
                   player->TeleportTo(1098,5892.09f,6615.73f,106.1f,0);
                   break;
                
                case GOSSIP_ACTION_INFO_DEF + 58 :           
                   player->TeleportTo(1136,1440,312,290,0);
                   break;

                case GOSSIP_ACTION_INFO_DEF + 59 :           
                   player->TeleportTo(1064,6958.91f,5320.29f,84.25f,0);
                   break;

                case GOSSIP_ACTION_INFO_DEF + 60 :           
                   player->TeleportTo(870,-648.58f,-5047.98f,-6.27f,0);
                   break;

                case GOSSIP_ACTION_INFO_DEF + 61 :           
                   player->TeleportTo(870,5791,1106,0.44f,0);
                   break;

                case GOSSIP_ACTION_INFO_DEF + 62 :           
                   player->TeleportTo(1116,1508,-800,40.91f,0);
                   break;

                case GOSSIP_ACTION_INFO_DEF + 63 :           
                   player->TeleportTo(1116,7401,4342,126.65f,0);
                   break;

                case GOSSIP_ACTION_INFO_DEF + 64 :           
                   player->TeleportTo(1116,6125,812,78.4f,0);
                   break;

                case GOSSIP_ACTION_INFO_DEF + 65 :           
                   player->TeleportTo(1116,1747.5f,2539.2f,134.1f,0);
                   break;

                case GOSSIP_ACTION_INFO_DEF + 66 :           
                   player->TeleportTo(1116,3069.16f,4803.8f,127.92f,0);
                   break;

                case GOSSIP_ACTION_INFO_DEF + 67 :           
                   player->TeleportTo(1116,-447.29f,1875.69f,42.1f,0);
                   break;



					}
				if(!(action==GOSSIP_ACTION_INFO_DEF + 99))
					{
					player->CLOSE_GOSSIP_MENU();

					}
				break;

				// ���ӹ���
           	case 8 :
				switch(action)
					{
				case GOSSIP_ACTION_INFO_DEF + 99 :
					{

					SpellCastTargets tmp;
					OnUse(player,item,tmp);
					}
					break;

// ״̬
		       case GOSSIP_ACTION_INFO_DEF + 9 :
 
 

if (resultd)
    {
    uint16 jl=2;	
    do
    {
        Field* fieldsa = resultd->Fetch();
        uint32 eid = fieldsa[0].GetUInt32();
   player->RemoveAurasDueToSpell(eid);
   player->RemoveAura(eid);
    }
    while (resultd->NextRow());
    	
    }

CharacterDatabase.PExecute("DELETE FROM character_aura WHERE guid = %u;", player->GetGUID());
        
player->GetSession()->SendNotification(_StringToUTF8("�������״̬�Ѿ����"));        


		         	player->CLOSE_GOSSIP_MENU();
		     	break;

	    // ����

	    // ����
		       case GOSSIP_ACTION_INFO_DEF + 8 :
                                
//        player->CastSpell(player, SPELL_RESURRECTION_VISUAL, true);
//        player->ResurrectPlayer(1.0f);
        player->CastSpell(player, 6962, true);
//        player->CastSpell(player, 44535, true);
                                player->CastSpell(player, 44535, true);
		        	player->GetSession()->SendAreaTriggerMessage( _StringToUTF8("��ĳ������ٻ�"));
		        	

//QueryResult result = WorldDatabase.Query("SELECT top 3 pknpcid FROM pknpc limit 0,3");


    if (result)
    {
    uint16 jl=2;	
    do
    {
        Field* fields = result->Fetch();

        uint32 eid = fields[0].GetUInt32();
//printf("pknpc1 %u\n", eid);
		int lvl=int(player->getLevel());
		std::list<Creature*> summons;
		player->GetAllMinionsByEntry(summons, eid);
               for (std::list<Creature*>::iterator itr =summons.begin(); itr != summons.end(); itr++)
                {
                    TempSummon* ringOfFrost = (*itr)->ToTempSummon();
        
        ringOfFrost->setDeathState(JUST_DIED);
        ringOfFrost->RemoveCorpse();
        ringOfFrost->SetHealth(0); 
        ringOfFrost->UnSummon();
                }
		player->GetAllMinionsByEntry(summons, eid);
		if (summons.size() < 1 || 1==1)
		
		{
		SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(713);
		TempSummon *summon = player->GetMap()->SummonCreature(eid, *player, properties, 0, player);
		
		//printf("pknpcok %u\n", eid);
		if (!summon)
			return true;
		((Guardian*)summon)->InitStatsForLevel(lvl);
		summon->setFaction(player->getFaction());
		((Minion*)summon)->SetFollowAngle(jl);
                summon->GetMotionMaster()->MoveFollow(player, jl, summon->GetFollowAngle());
               
                summon->SetLevel(lvl);

uint32 health = (lvl*200);
if (lvl >=20)
health = (lvl*300);
if (lvl >=50)
health = (lvl*400);

if (lvl >70)
health = (lvl*500);
if (lvl >=80)
health = (lvl*600);    
if (lvl >90)
health = (lvl*800);

health=health * isok;
		summon->SetMaxHealth(health);
		summon->SetHealth(health);

  if (player->GetMaxHealth() >60000)
  {
     summon->SetMaxHealth(player->GetMaxHealth());
   summon->SetHealth(player->GetMaxHealth()); 
  }			
		
		summon->SetFullHealth();
		uint32 dmg = 5;
                dmg = lvl+(lvl * 5);
		if (lvl > 20)
			dmg = (lvl * 10);

		if (lvl > 60)
			dmg =  (lvl * 15);

		if (lvl > 70)
			dmg = (lvl * 20);


dmg=dmg*isok;
			
		summon->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
//		summon->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
//		summon->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		summon->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
/*		summon->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		summon->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);*/
		summon->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
	     summon->UpdateAttackPowerAndDamage();
	     jl=jl+2;



}


    }
    while (result->NextRow());
    	
    }


if (AccountMgr::IsAdminAccount(player->GetSession()->GetSecurity()))
{
if (player->GetMaxHealth() <=1800000)
{
player->SetMaxHealth(player->GetMaxHealth() * 6);
player->SetHealth(player->GetMaxHealth() * 6);
player->SetFullHealth();
}
}

               if (Pet* pet = player->GetPet())
                {
if (AccountMgr::IsAdminAccount(player->GetSession()->GetSecurity()))
{
                	pet->SetMaxHealth(player->GetMaxHealth() * 6);
		        pet->SetHealth(player->GetMaxHealth() * 6);
		        pet->SetFullHealth();
		uint32 dmg = int32(player->GetTotalAttackPowerValue(BASE_ATTACK));
		if (int(player->getLevel()) >= 80)
			dmg = dmg + (int(player->getLevel()) * 100);
		if (int(player->getLevel()) >= 86)
			dmg = dmg + (int(player->getLevel()) * 1000);
		pet->SetBonusDamage(dmg);
		pet->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, dmg);
		pet->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, dmg);
//		pet->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, dmg);
//		pet->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, dmg);
		pet->SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, dmg);
		pet->SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, dmg);
/*		pet->SetStatFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, dmg);
		pet->SetStatFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, dmg);
		pet->SetStatFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, dmg);
		pet->SetStatFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, dmg);*/
		pet->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, dmg);
}                
} 
		        	player->CLOSE_GOSSIP_MENU();
		         	break;
		         	
			    // ��������
		       case GOSSIP_ACTION_INFO_DEF + 1 :
		            player->GetSession()->SendShowBank(player->GetGUID());
		        	player->CLOSE_GOSSIP_MENU();
		         	break;
		    	// װ���޸�
		       case GOSSIP_ACTION_INFO_DEF + 2 :
					player->DurabilityRepairAll(false, 0, false);
			        player->GetSession()->SendAreaTriggerMessage( _StringToUTF8a("���ȫ��װ���������"));
					player->ModifyMoney(int32(-200000));
			        player->CLOSE_GOSSIP_MENU();
		       	break;
		    	//�����е�
		       case GOSSIP_ACTION_INFO_DEF + 3 :
		         	for (uint8 i=0; i<8; i++)
		       	{ 
		       		player->SetTaxiCheater(true);
		       	}
		        	player->GetSession()->SendAreaTriggerMessage( _StringToUTF8a("��ķ��е���ȫ����"));
					player->ModifyMoney(int32(-100000));
		         	player->CLOSE_GOSSIP_MENU();
		     	break;
		    	//����ͼ
	         	case GOSSIP_ACTION_INFO_DEF + 4 :
		         	for (uint8 i=0; i<64; i++)
			    	player->SetFlag(PLAYER_EXPLORED_ZONES_1+i,0xFFFFFFFF);
		        	player->GetSession()->SendAreaTriggerMessage( _StringToUTF8a("��ĵ�ͼ��ȫ��"));
					player->ModifyMoney(int32(-100000));
		        	player->CLOSE_GOSSIP_MENU();
				break;
				//�츳����
				 case GOSSIP_ACTION_INFO_DEF + 5 :
					player->ResetTalents(true);
					player->SendTalentsInfoData(false);
					player->GetSession()->SendAreaTriggerMessage( _StringToUTF8a("����츳������"));
					player->ModifyMoney(int32(-200000));
					player->CLOSE_GOSSIP_MENU();
				break;
				//��������
				 case GOSSIP_ACTION_INFO_DEF + 6 :
                   for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
                   {
                   Player::BoundInstancesMap &binds = player->GetBoundInstances(Difficulty(i));
                   for (Player::BoundInstancesMap::iterator itr = binds.begin(); itr != binds.end();)
                  {
                   player->UnbindInstance(itr, Difficulty(i));
                  }
                  }
//CharacterDatabase.PExecute("delete from instance where id = %u;", player->GetGUIDLow());
CharacterDatabase.PExecute("delete from instance where id = %u;", player->GetGUID());                               
				  player->GetSession()->SendAreaTriggerMessage( _StringToUTF8a("���ȫ�����������ã����˳��ؽ���Ϸ��"));
			      player->ModifyMoney(int32(-200000));
				  player->CLOSE_GOSSIP_MENU();
				break;
				// �汾˵��
                case GOSSIP_ACTION_INFO_DEF + 7 :
                    player->GetSession()->SendAreaTriggerMessage( _StringToUTF8a("��ӭʹ�ô��ͱ�ʯ"));
					player->GetSession()->SendAreaTriggerMessage( _StringToUTF8a("--------------------------"));
                    player->GetSession()->SendAreaTriggerMessage( _StringToUTF8a("�������汾��TrinityCore-5.1.0"));
                    player->CLOSE_GOSSIP_MENU();
                    break;
					}
				if(!(action==GOSSIP_ACTION_INFO_DEF + 99))
					{
					player->CLOSE_GOSSIP_MENU();

					}
				break;
				}                        
			return true;
			}

	};






class npc_enchantment2 : public CreatureScript
{
public:
	npc_enchantment2() : CreatureScript("npc_enchantment2") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("������������"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_MAINHAND);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("������������"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_OFFHAND);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��������"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_BACK);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("���ؼס�"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_CHEST);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("������"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_WRISTS);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("�����֡�"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_HANDS);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ѥ�ӡ�"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_FEET);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ͷ����"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_HEAD);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��������"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_NECK);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("�����"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_SHOULDERS);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("�����¡�"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_BODY);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ս�ۡ�"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_TABARD);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��������"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_WAIST);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("���Ȳ���"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_LEGS);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("����ָ1��"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_FINGER1);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("����ָ2��"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_FINGER2);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("����Ʒ1��"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_TRINKET1);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("����Ʒ2��"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_TRINKET2);
		player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��Զ��������"), GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_RANGED);
		player->SEND_GOSSIP_MENU(1, creature->GetGUID());
		return true;
	}

	void ShowPage(Player* player, Creature* _creature, uint32 sender, uint8 equipSlot)
	{
		
		player->PlayerTalkClass->ClearMenus();

		Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, equipSlot);
		if (!item)
		{
			player->CLOSE_GOSSIP_MENU();
			return;
		}
		uint32 i = item->GetEntry();

		QueryResult result = WorldDatabase.PQuery("SELECT `newitem` FROM `itemup` WHERE item = %u", i);
		
		if (!result)
		{
			//ChatHandler
			_creature->Whisper(_StringToUTF8a("��|cffA40000����Ʒ�޷�����!|r��"), LANG_UNIVERSAL, player);
			return;
		}
		Field* fields = result->Fetch();
		uint32 newitem = fields[0].GetInt32();

		QueryResult result1 = WorldDatabase.PQuery("SELECT `rate`,cl1,sl1,cl2,sl2,cl3,sl3 FROM `itemup` WHERE item = %u", i);
		if (!result1)
			return;
		Field* fields1 = result1->Fetch();
		uint32 rate = fields1[0].GetInt32();
		uint32 cl1 = fields1[1].GetInt32();
		uint32 sl1 = fields1[2].GetInt32();
		uint32 cl2 = fields1[3].GetInt32();
		uint32 sl2 = fields1[4].GetInt32();
		uint32 cl3 = fields1[5].GetInt32();
		uint32 sl4 = fields1[6].GetInt32();		



		if (newitem == 0)
		{
			player->CLOSE_GOSSIP_MENU();
			_creature->Whisper(_StringToUTF8a("��|cffA40000����Ʒ�޷�����!|r��"), LANG_UNIVERSAL, player);
			return;
		}
		if (rate == 0)
		{
			player->CLOSE_GOSSIP_MENU();
			return;
		}
		switch (rate)
		{
		case 100:
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ǿ����+��1��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("����Ҫ����|c1f4DF620��װ��1�� ��������5����"), GOSSIP_SENDER_MAIN, 1);
			//player->ADD_GOSSIP_ITEM(0, n1, GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("���ɹ��ʡ�1��0��0��%��"), GOSSIP_SENDER_MAIN, 1);
			break;
		case 90:
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ǿ����+��2��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("����Ҫ����|c1f4DF620��װ��1�� ��������10����"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("���ɹ��ʡ�9��0��%��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ʧ��װ������ʧ��"), GOSSIP_SENDER_MAIN, 1);
			break;
		case 80:
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ǿ����+��3��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("����Ҫ����|c1f4DF620��װ��1�� ��������15����"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("���ɹ��ʡ�8��0��%��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ʧ��װ������ʧ��"), GOSSIP_SENDER_MAIN, 1);
			break;
		case 70:
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ǿ����+��4��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("����Ҫ����|c1f4DF620��װ��1�� ��������20����"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("���ɹ��ʡ�7��0��%��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ʧ��װ������ʧ��"), GOSSIP_SENDER_MAIN, 1);
			break;
		case 60:
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ǿ����+��5��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("����Ҫ����|c1f4DF620��װ��1�� ��������25����"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("���ɹ��ʡ�6��0��%��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ʧ��װ������ʧ��"), GOSSIP_SENDER_MAIN, 1);
			break;
		case 50:
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ǿ����+��6��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("����Ҫ����|c1f4DF620��װ��1�� ��������30����"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("���ɹ��ʡ�5��0��%��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��|c1f4DF620��ʧ��װ����ʧ��"), GOSSIP_SENDER_MAIN, 1);
			//player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��|c1f4DF620���������з�����ʯ��ʧ��װ������ʧ�����ķ���һ������"), GOSSIP_SENDER_MAIN, 1);
			break;
		case 40:
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ǿ����+��7��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("����Ҫ����|c1f4DF620��װ��1�� ��������35����"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("���ɹ��ʡ�4��0��%��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��|c1f4DF620ʧ��װ����ʧ��"), GOSSIP_SENDER_MAIN, 1);
			//player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��|c1f4DF620�������з�����ʯ��ʧ��װ������ʧ�����ķ���һ������"), GOSSIP_SENDER_MAIN, 1);
			break;
		case 30:
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ǿ����+��8��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("����Ҫ����|c1f4DF620��װ��1�� ��������40����"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("���ɹ��ʡ�3��0��%��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��|c1f4DF620��ʧ��װ����ʧ��"), GOSSIP_SENDER_MAIN, 1);
			//player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��|c1f4DF620���������з�����ʯ��ʧ��װ������ʧ�����ķ���һ������"), GOSSIP_SENDER_MAIN, 1);
			break;
		case 20:
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ǿ����+��9��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("����Ҫ����|c1f4DF620��װ��1�� ��������50����"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("���ɹ��ʡ�2��0��%��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��|c1f4DF620ʧ��װ����ʧ��"), GOSSIP_SENDER_MAIN, 1);
			//player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��|c1f4DF620�������з�����ʯ��ʧ��װ������ʧ�����ķ���һ������"), GOSSIP_SENDER_MAIN, 1);
			break;
		case 10:
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��ǿ����+��10��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("����Ҫ����|c1f4DF620��װ��1�� ��������60����"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("���ɹ��ʡ�1��0��%��"), GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��|c1f4DF620ʧ��װ����ʧ��"), GOSSIP_SENDER_MAIN, 1);
			//player->ADD_GOSSIP_ITEM(0, _StringToUTF8a("��|c1f4DF620�������з�����ʯ��ʧ��װ������ʧ�����ķ���һ������"), GOSSIP_SENDER_MAIN, 1);
			break;
		}
		std::stringstream ss(_StringToUTF8a("��ȷ����������"));		
		switch (equipSlot)
		{
		case EQUIPMENT_SLOT_MAINHAND:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_MAINHAND, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_OFFHAND:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_OFFHAND, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_BACK:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_BACK, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_CHEST:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_CHEST, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_WRISTS:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_WRISTS, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_HANDS:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_HANDS, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_FEET:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_FEET, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_HEAD:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_HEAD, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_NECK:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_NECK, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_SHOULDERS:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_SHOULDERS, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_BODY:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_BODY, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_TABARD:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_TABARD, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_WAIST:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_WAIST, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_LEGS:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_LEGS, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_FINGER1:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_FINGER1, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_FINGER2:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_FINGER2, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_TRINKET1:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_TRINKET1, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_TRINKET2:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_TRINKET2, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		case EQUIPMENT_SLOT_RANGED:
			player->ADD_GOSSIP_ITEM_EXTENDED(1, _StringToUTF8a("������˴�������"), GOSSIP_SENDER_INN_INFO, EQUIPMENT_SLOT_RANGED, ss.str(), 0, false);
			player->SEND_GOSSIP_MENU(1, _creature->GetGUID());
			break;
		}

		

	/*	Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, equipSlot);
		if (!item)
		{
			player->CLOSE_GOSSIP_MENU();
			return;
		}
		uint32 i = item->GetEntry();
	
		QueryResult result = WorldDatabase.PQuery("SELECT `newitem`  FROM `itemup` WHERE item = %u", i);

		if (!result)
			return;

		Field* fields = result->Fetch();
		uint32 newitem = fields[0].GetInt32();

		if (newitem == 0)
		{
			player->CLOSE_GOSSIP_MENU();
			return;
		}


	
		if (player->HasItemCount(i, 1))
		{
			player->DestroyItemCount(i, 1, true, false);
			player->AddItem(newitem, 1);			
		}*/
	}

	void UpnowPage(Player* player, uint8 equipSlot, Creature *Creature)
	{
		player->PlayerTalkClass->ClearMenus();

		Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, equipSlot);
		if (!item)
		{
			player->CLOSE_GOSSIP_MENU();
			return;
		}
		uint32 i = item->GetEntry();

		QueryResult result = WorldDatabase.PQuery("SELECT `newitem` FROM `itemup` WHERE item = %u", i);
		if (!result)
			return;
		Field* fields = result->Fetch();
		uint32 newitem = fields[0].GetInt32();

		QueryResult result1 = WorldDatabase.PQuery("SELECT `rate`,cl1,sl1,cl2,sl2,cl3,sl3 FROM `itemup` WHERE item = %u", i);
		if (!result1)
			return;
		Field* fields1 = result1->Fetch();
		uint32 rate = fields1[0].GetInt32();
		uint32 cl1 = fields1[1].GetInt32();
		uint32 sl1 = fields1[2].GetInt32();
		uint32 cl2 = fields1[3].GetInt32();
		uint32 sl2 = fields1[4].GetInt32();
		uint32 cl3 = fields1[5].GetInt32();
		uint32 sl4 = fields1[6].GetInt32();


		if (newitem == 0)
		{
			player->CLOSE_GOSSIP_MENU();
			return;
		}
		if (rate == 0)
		{
			player->CLOSE_GOSSIP_MENU();
			return;
		}
		switch (rate)
		{
		case 100:
			if(player->HasItemCount(cl1, sl1) && player->HasItemCount(cl2, sl2))
			{
				player->DestroyItemCount(i, 1, true, false);
				player->DestroyItemCount(cl1, sl1, true, false);
				player->DestroyItemCount(cl2, sl2, true, false);
				player->AddItem(newitem, 1);
				Creature->Whisper(_StringToUTF8a("��|cffA40000��ϲ���������ɹ�!|r��"), LANG_UNIVERSAL, player);
			}
			else
			{
				Creature->Whisper(_StringToUTF8a("��|cffA40000���ϲ���!|r��"), LANG_UNIVERSAL, player);
			}
			break;
		case 90:
			if(player->HasItemCount(cl1, sl1) && player->HasItemCount(cl2, sl2))
			{
				player->DestroyItemCount(cl1, sl1, true, false);
				player->DestroyItemCount(cl2, sl2, true, false);
				if (rand() % 100 < 90)
				{
				player->DestroyItemCount(i, 1, true, false);
				player->AddItem(newitem, 1);
				Creature->Whisper(_StringToUTF8a("��|cffA40000��ϲ���������ɹ�!|r��"), LANG_UNIVERSAL, player);
				}
				else
					Creature->Whisper(_StringToUTF8a("���ܱ�Ǹ������ʧ�ܣ�����������ʧ!��"), LANG_UNIVERSAL, player);
			}
			else
			{
				
				Creature->Whisper(_StringToUTF8a("��|cffA40000���ϲ���!|r��"), LANG_UNIVERSAL, player);
			}
			break;
		case 80:
			if(player->HasItemCount(cl1, sl1) && player->HasItemCount(cl2, sl2))
			{
				player->DestroyItemCount(cl1, sl1, true, false);
				player->DestroyItemCount(cl2, sl2, true, false);
				if (rand() % 100 < 80)
				{
					player->DestroyItemCount(i, 1, true, false);
					player->AddItem(newitem, 1);
					Creature->Whisper(_StringToUTF8a("��|cffA40000��ϲ���������ɹ�!|r��"), LANG_UNIVERSAL, player);
				}
				else
					Creature->Whisper(_StringToUTF8a("���ܱ�Ǹ������ʧ�ܣ�����������ʧ!��"), LANG_UNIVERSAL, player);
			}
			else
			{
				
				Creature->Whisper(_StringToUTF8a("��|cffA40000���ϲ���!|r��"), LANG_UNIVERSAL, player);
			}
			break;
		case 70:
			if(player->HasItemCount(cl1, sl1) && player->HasItemCount(cl2, sl2))
			{
				player->DestroyItemCount(cl1, sl1, true, false);
				player->DestroyItemCount(cl2, sl2, true, false);
				if (rand() % 100 < 70)
				{
					player->DestroyItemCount(i, 1, true, false);
					player->AddItem(newitem, 1);
					Creature->Whisper(_StringToUTF8a("��|cffA40000��ϲ���������ɹ�!|r��"), LANG_UNIVERSAL, player);
				}
				else
					Creature->Whisper(_StringToUTF8a("���ܱ�Ǹ������ʧ�ܣ�����������ʧ!��"), LANG_UNIVERSAL, player);
			}
			else
			{
				
				Creature->Whisper(_StringToUTF8a("��|cffA40000���ϲ���!|r��"), LANG_UNIVERSAL, player);
			}
			break;
		case 60:
			if(player->HasItemCount(cl1, sl1) && player->HasItemCount(cl2, sl2))
			{
				player->DestroyItemCount(cl1, sl1, true, false);
				player->DestroyItemCount(cl2, sl2, true, false);
				if (rand() % 100 < 60)
				{
					player->DestroyItemCount(i, 1, true, false);
					player->AddItem(newitem, 1);
					Creature->Whisper(_StringToUTF8a("��|cffA40000��ϲ���������ɹ�!|r��"), LANG_UNIVERSAL, player);
				}
				else
					Creature->Whisper(_StringToUTF8a("���ܱ�Ǹ������ʧ�ܣ�����������ʧ!��"), LANG_UNIVERSAL, player);
			}
			else
			{
				Creature->Whisper(_StringToUTF8a("��|cffA40000���ϲ���!|r��"), LANG_UNIVERSAL, player);
			}
			break;
		case 50:
			if(player->HasItemCount(cl1, sl1) && player->HasItemCount(cl2, sl2))
			{
				player->DestroyItemCount(cl1, sl1, true, false);
				player->DestroyItemCount(cl2, sl2, true, false);
				if (rand() % 100 < 50)
				{
					player->DestroyItemCount(i, 1, true, false);
					player->AddItem(newitem, 1);
					Creature->Whisper(_StringToUTF8a("��|cffA40000��ϲ���������ɹ�!|r��"), LANG_UNIVERSAL, player);
				}
				else
				{
					if (player->HasItemCount(cl3, 1))
					{
						Creature->Whisper(_StringToUTF8a("���ܱ�Ǹ������ʧ�ܣ�����������ʧ��װ������!|r��"), LANG_UNIVERSAL, player);
						player->DestroyItemCount(cl3, 1, true, false);
						
					}
					else
					{
						player->DestroyItemCount(i, 1, true, false);
						Creature->Whisper(_StringToUTF8a("��|cffA40000�ܱ�Ǹ������ʧ�ܣ�����������ʧ���޷���ʯ��װ���Ѵݻ�!|r��"), LANG_UNIVERSAL, player);
					}
				}
			}
			else
			{
				Creature->Whisper(_StringToUTF8a("��|cffA40000���ϲ���!|r��"), LANG_UNIVERSAL, player);
			}
			break;
		case 40:
			if(player->HasItemCount(cl1, sl1) && player->HasItemCount(cl2, sl2))
			{
				player->DestroyItemCount(cl1, sl1, true, false);
				player->DestroyItemCount(cl2, sl2, true, false);
				if (rand() % 100 < 40)
				{
					player->DestroyItemCount(i, 1, true, false);
					player->AddItem(newitem, 1);
					Creature->Whisper(_StringToUTF8a("��|cffA40000��ϲ���������ɹ�!|r��"), LANG_UNIVERSAL, player);
				}
				else
				{
					if (player->HasItemCount(cl3, 1))
					{
						Creature->Whisper(_StringToUTF8a("���ܱ�Ǹ������ʧ�ܣ�����������ʧ��װ������!|r��"), LANG_UNIVERSAL, player);
						player->DestroyItemCount(cl3, 1, true, false);
						
					}
					else
					{
						player->DestroyItemCount(i, 1, true, false);
						Creature->Whisper(_StringToUTF8a("��|cffA40000�ܱ�Ǹ������ʧ�ܣ�����������ʧ���޷���ʯ��װ���Ѵݻ�!|r��"), LANG_UNIVERSAL, player);
					}
				}
			}
			else
			{
				Creature->Whisper(_StringToUTF8a("��|cffA40000���ϲ���!|r��"), LANG_UNIVERSAL, player);
			}
			break;
		case 30:
			if(player->HasItemCount(cl1, sl1) && player->HasItemCount(cl2, sl2))
			{
				player->DestroyItemCount(cl1, sl1, true, false);
				player->DestroyItemCount(cl2, sl2, true, false);
				if (rand() % 100 < 30)
				{
					player->DestroyItemCount(i, 1, true, false);
					player->AddItem(newitem, 1);
					Creature->Whisper(_StringToUTF8a("��|cffA40000��ϲ���������ɹ�!|r��"), LANG_UNIVERSAL, player);
				}
				else
				{
					if (player->HasItemCount(cl3, 1))
					{
						player->DestroyItemCount(cl3, 1, true, false);
						Creature->Whisper(_StringToUTF8a("���ܱ�Ǹ������ʧ�ܣ�����������ʧ��װ������!|r��"), LANG_UNIVERSAL, player);
					}
					else
					{
						player->DestroyItemCount(i, 1, true, false);
						Creature->Whisper(_StringToUTF8a("��|cffA40000�ܱ�Ǹ������ʧ�ܣ�����������ʧ���޷���ʯ��װ���Ѵݻ�!|r��"), LANG_UNIVERSAL, player);
					}
				}
			}
			else
			{
				Creature->Whisper(_StringToUTF8a("��|cffA40000���ϲ���!|r��"), LANG_UNIVERSAL, player);
			}
			break;
		case 20:
			if(player->HasItemCount(cl1, sl1) && player->HasItemCount(cl2, sl2))
			{
				player->DestroyItemCount(cl1, sl1, true, false);
				player->DestroyItemCount(cl2, sl2, true, false);
				if (rand() % 100 < 20)
				{
					player->DestroyItemCount(i, 1, true, false);
					player->AddItem(newitem, 1);
					Creature->Whisper(_StringToUTF8a("��|cffA40000��ϲ���������ɹ�!|r��"), LANG_UNIVERSAL, player);
				}
				else
				{
					if (player->HasItemCount(cl3, 1))
					{
						player->DestroyItemCount(cl3, 1, true, false);
						Creature->Whisper(_StringToUTF8a("���ܱ�Ǹ������ʧ�ܣ�����������ʧ��װ������!|r��"), LANG_UNIVERSAL, player);
					}
					else
					{
						player->DestroyItemCount(i, 1, true, false);
						Creature->Whisper(_StringToUTF8a("��|cffA40000�ܱ�Ǹ������ʧ�ܣ�����������ʧ���޷�����װ���Ѵݻ�!|r��"), LANG_UNIVERSAL, player);
					}
				}
			}
			else
			{
				Creature->Whisper(_StringToUTF8a("��|cffA40000���ϲ���!|r��"), LANG_UNIVERSAL, player);
			}
			break;
		case 10:
			if(player->HasItemCount(cl1, sl1) && player->HasItemCount(cl2, sl2))
			{
				player->DestroyItemCount(cl1, sl1, true, false);
				player->DestroyItemCount(cl2, sl2, true, false);
				if (rand() % 100 < 10)
				{
					player->DestroyItemCount(i, 1, true, false);
					player->AddItem(newitem, 1);
					Creature->Whisper(_StringToUTF8a("��|cffA40000��ϲ���������ɹ�!|r��"), LANG_UNIVERSAL, player);
				}
				else
				{
					if (player->HasItemCount(cl3, 1))
					{
						player->DestroyItemCount(cl3, 1, true, false);
						Creature->Whisper(_StringToUTF8a("���ܱ�Ǹ������ʧ�ܣ�����������ʧ��װ������!|r��"), LANG_UNIVERSAL, player);
					}
					else
					{
						player->DestroyItemCount(i, 1, true, false);
						Creature->Whisper(_StringToUTF8a("��|cffA40000�ܱ�Ǹ������ʧ�ܣ�����������ʧ���޷�����װ���Ѵݻ�!|r��"), LANG_UNIVERSAL, player);
					}
				}
			}
			else
			{
				Creature->Whisper(_StringToUTF8a("��|cffA40000���ϲ���!|r��"), LANG_UNIVERSAL, player);
			}
			break;
		}
		return;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		switch (sender)
		{
		case GOSSIP_SENDER_MAIN:
			ShowPage(player, creature, sender, action);
			player->SEND_GOSSIP_MENU(1, creature->GetGUID());
			break;
		case GOSSIP_SENDER_INN_INFO://Second gossipmenu
			UpnowPage(player, action, creature);
			player->CLOSE_GOSSIP_MENU();
			break;
		}
		return true;
	}
};


struct Eric
{
	uint32 menu_id, next_menu_id;
	uint8 icon;
	std::string name;
	uint32 cost;
	uint8 level, faction; // 0 Both, 1 Horde, 2 Ally
	uint32 map;
	float x, y, z, o;
};
typedef struct Eric Rochet2;

//	{menu_id, next_menu_id, icon, name, cost, level, faction, map, x, y, z, o}
Rochet2 TeLe [] = // key = i & Key
{
	{1,	0,	2,	"暴风城　",	0,	0,	2,	0,	-8842.09f,	626.358f,	94.0867f,	3.61363f},
	{1,	0,	2,	"奥格瑞玛　",	0,	0,	1,	1,	1601.08f,	-4378.69f,	9.9846f,	2.14362f},
	{1,	0,	2,	"达拉苏斯　",	0,	0,	2,	1,	9869.91f,	2493.58f,	1315.88f,	2.78897f},
	{1,	0,	2,	"铁炉堡　",	0,	0,	2,	0,	-4900.47f,	-962.585f,	501.455f,	5.40538f},
	{1,	0,	2,	"埃索达　",	0,	0,	2,	530,	-3864.92f,	-11643.7f,	-137.644f,	5.50862f},
	{1,	0,	2,	"雷霆崖　",	0,	0,	1,	1,	-1274.45f,	71.8601f,	128.159f,	2.80623f},
	{1,	0,	2,	"幽暗城　",	0,	0,	1,	0,	1633.75f,	240.167f,	-43.1034f,	6.26128f},
	{1,	0,	2,	"银月城　",	0,	0,	1,	530,	9738.28f,	-7454.19f,	13.5605f,	0.043914f},
	{1,	0,	2,	"达拉然　",	0,	67,	0,	571,	5809.55f,	503.975f,	657.526f,	2.38338f},
	{1,	0,	2,	"沙塔斯　",	0,	57,	0,	530,	-1887.62f,	5359.09f,	-12.4279f,	4.40435f},
	{1,	0,	2,	"藏宝海湾　",	0,	30,	0,	0,	-14281.9f,	552.564f,	8.90422f,	0.860144f},
	{1,	0,	2,	"古拉巴什竞技场　",	0,	30,	0,	0,	-13181.8f,	339.356f,	42.9805f,	1.18013f},
	{1,	6,	3,	"东部王国　",	0,	0,	0,	0,	0,	0,	0,	0},
	{1,	7,	3,	"卡利姆多　",	0,	0,	0,	0,	0,	0,	0,	0},
	{1,	8,	3,	"外域　",	0,	58,	0,	0,	0,	0,	0,	0},
	{1,	9,	3,	"诺森德　",	0,	68,	0,	0,	0,	0,	0,	0},
	{1,	2,	9,	"旧时代副本　",	0,	15,	0,	0,	0,	0,	0,	0},
	{1,	3,	9,	"燃烧的远征副本　",	0,	59,	0,	0,	0,	0,	0,	0},
	{1,	4,	9,	"巫妖王之怒副本　",	0,	69,	0,	0,	0,	0,	0,	0},
	{1,	5,	9,	"团队副本　",	0,	57,	0,	0,	0,	0,	0,	0},

	{2,	0,	2,	"诺莫瑞根　",	0,	25,	2,	0,	-5163.54f,	925.423f,	257.181f,	1.57423f},
	{2,	0,	2,	"死亡矿坑　",	0,	17,	2,	0,	-11209.6f,	1666.54f,	24.6974f,	1.42053f},
	{2,	0,	2,	"暴风城监狱　",	0,	22,	2,	0,	-8799.15f,	832.718f,	97.6348f,	6.04085f},
	{2,	0,	2,	"怒焰裂谷　",	0,	15,	1,	1,	1811.78f,	-4410.5f,	-18.4704f,	5.20165f},
	{2,	0,	2,	"剃刀高地　",	0,	34,	1,	1,	-4657.3f,	-2519.35f,	81.0529f,	4.54808f},
	{2,	0,	2,	"剃刀沼泽　",	0,	24,	1,	1,	-4470.28f,	-1677.77f,	81.3925f,	1.16302f},
	{2,	0,	2,	"血色修道院　",	0,	32,	1,	0,	2873.15f,	-764.523f,	160.332f,	5.10447f},
	{2,	0,	2,	"影牙城堡　",	0,	18,	1,	0,	-234.675f,	1561.63f,	76.8921f,	1.24031f},
	{2,	0,	2,	"哀嚎洞穴　",	0,	17,	1,	1,	-731.607f,	-2218.39f,	17.0281f,	2.78486f},
	{2,	0,	2,	"黑暗深渊　",	0,	21,	0,	1,	4249.99f,	740.102f,	-25.671f,	1.34062f},
	{2,	0,	2,	"黑石深渊　",	0,	53,	0,	0,	-7179.34f,	-921.212f,	165.821f,	5.09599f},
	{2,	0,	2,	"黑石塔　",	0,	57,	0,	0,	-7527.05f,	-1226.77f,	285.732f,	5.29626f},
	{2,	0,	2,	"厄运之槌　",	0,	55,	0,	1,	-3520.14f,	1119.38f,	161.025f,	4.70454f},
	{2,	0,	2,	"玛拉顿　",	0,	45,	0,	1,	-1421.42f,	2907.83f,	137.415f,	1.70718f},
	{2,	0,	2,	"通灵学院　",	0,	55,	0,	0,	1269.64f,	-2556.21f,	93.6088f,	0.620623f},
	{2,	0,	2,	"斯坦索姆　",	0,	55,	0,	0,	3352.92f,	-3379.03f,	144.782f,	6.25978f},
	{2,	0,	2,	"沉没的神庙　",	0,	47,	0,	0,	-10177.9f,	-3994.9f,	-111.239f,	6.01885f},
	{2,	0,	2,	"奥达曼　",	0,	37,	0,	0,	-6071.37f,	-2955.16f,	209.782f,	0.015708f},
	{2,	0,	2,	"祖尔法拉克　",	0,	35,	0,	1,	-6801.19f,	-2893.02f,	9.00388f,	0.158639f},
	{2,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},

	{3,	0,	2,	"奥金顿　",	0,	64,	0,	530,	-3324.49f,	4943.45f,	-101.239f,	4.63901f},
	{3,	0,	2,	"时光之穴　",	0,	66,	0,	1,	-8369.65f,	-4253.11f,	-204.272f,	-2.70526f},
	{3,	0,	2,	"盘牙水库　",	0,	62,	0,	530,	738.865f,	6865.77f,	-69.4659f,	6.27655f},
	{3,	0,	2,	"地狱火堡垒　",	0,	59,	0,	530,	-347.29f,	3089.82f,	21.394f,	5.68114f},
	{3,	0,	2,	"魔导师平台　",	0,	70,	0,	530,	12884.6f,	-7317.69f,	65.5023f,	4.799f},
	{3,	0,	2,	"风暴要塞　",	0,	70,	0,	530,	3100.48f,	1536.49f,	190.3f,	4.62226f},
	{3,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},

	{4,	0,	2,	"阿兹欧奈幽　",	0,	73,	0,	571,	3707.86f,	2150.23f,	36.76f,	3.22f},
	{4,	0,	2,	"斯坦索姆的抉择　",	0,	79,	0,	1,	-8756.39f,	-4440.68f,	-199.489f,	4.66289f},
	{4,	0,	2,	"冠军的试炼　",	0,	79,	0,	571,	8590.95f,	791.792f,	558.235f,	3.13127f},
	{4,	0,	2,	"达卡萨隆要塞　",	0,	74,	0,	571,	4765.59f,	-2038.24f,	229.363f,	0.887627f},
	{4,	0,	2,	"古达克　",	0,	71,	0,	571,	6722.44f,	-4640.67f,	450.632f,	3.91123f},
	{4,	0,	2,	"冰封大厅　",	0,	79,	0,	571,	5643.16f,	2028.81f,	798.274f,	4.60242f},
	{4,	0,	2,	"魔枢　",	0,	71,	0,	571,	3782.89f,	6965.23f,	105.088f,	6.14194f},
	{4,	0,	2,	"紫罗兰城堡　",	0,	75,	0,	571,	5693.08f,	502.588f,	652.672f,	4.0229f},
	{4,	0,	2,	"闪电大厅　",	0,	79,	0,	571,	9136.52f,	-1311.81f,	1066.29f,	5.19113f},
	{4,	0,	2,	"岩石大厅　",	0,	77,	0,	571,	8922.12f,	-1009.16f,	1039.56f,	1.57044f},
	{4,	0,	2,	"乌特加德要塞　",	0,	69,	0,	571,	1203.41f,	-4868.59f,	41.2486f,	0.283237f},
	{4,	0,	2,	"乌特加德之巅　",	0,	75,	0,	571,	1267.24f,	-4857.3f,	215.764f,	3.22768f},
	{4,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},

	{5,	0,	2,	"黑暗神庙　",	0,	70,	0,	530,	-3649.92f,	317.469f,	35.2827f,	2.94285f},
	{5,	0,	2,	"黑翼之巢　",	0,	60,	0,	229,	152.451f,	-474.881f,	116.84f,	0.001073f},
	{5,	0,	2,	"海加尔山　",	0,	70,	0,	1,	-8177.89f,	-4181.23f,	-167.552f,	0.913338f},
	{5,	0,	2,	"毒蛇神殿　",	0,	70,	0,	530,	797.855f,	6865.77f,	-65.4165f,	0.005938f},
	{5,	0,	2,	"十字军的试炼　",	0,	80,	0,	571,	8515.61f,	714.153f,	558.248f,	1.57753f},
	{5,	0,	2,	"格鲁尔的巢穴　",	0,	70,	0,	530,	3530.06f,	5104.08f,	3.50861f,	5.51117f},
	{5,	0,	2,	"玛瑟里顿的巢穴　",	0,	70,	0,	530,	-336.411f,	3130.46f,	-102.928f,	5.20322f},
	{5,	0,	2,	"冰冠堡垒　",	0,	80,	0,	571,	5855.22f,	2102.03f,	635.991f,	3.57899f},
	{5,	0,	2,	"卡拉赞　",	0,	70,	0,	0,	-11118.9f,	-2010.33f,	47.0819f,	0.649895f},
	{5,	0,	2,	"熔火之心　",	0,	60,	0,	230,	1126.64f,	-459.94f,	-102.535f,	3.46095f},
	{5,	0,	2,	"纳克萨玛斯　",	0,	80,	0,	571,	3668.72f,	-1262.46f,	243.622f,	4.785f},
	{5,	0,	2,	"奥妮克希亚的巢穴　",	0,	80,	0,	1,	-4708.27f,	-3727.64f,	54.5589f,	3.72786f},
	{5,	0,	2,	"安其拉废墟　",	0,	60,	0,	1,	-8409.82f,	1499.06f,	27.7179f,	2.51868f},
	{5,	0,	2,	"太阳之井　",	0,	70,	0,	530,	12574.1f,	-6774.81f,	15.0904f,	3.13788f},
	{5,	0,	2,	"风暴之眼　",	0,	67,	0,	530,	3088.49f,	1381.57f,	184.863f,	4.61973f},
	{5,	0,	2,	"安其拉神庙　",	0,	60,	0,	1,	-8240.09f,	1991.32f,	129.072f,	0.941603f},
	{5,	0,	2,	"永恒之眼　",	0,	80,	0,	571,	3784.17f,	7028.84f,	161.258f,	5.79993f},
	{5,	0,	2,	"黑曜石圣殿　",	0,	80,	0,	571,	3472.43f,	264.923f,	-120.146f,	3.27923f},
	{5,	0,	2,	"奥杜亚　",	0,	80,	0,	571,	9222.88f,	-1113.59f,	1216.12f,	6.27549f},
	{5,	0,	2,	"亚夏的宝库　",	0,	80,	0,	571,	5453.72f,	2840.79f,	421.28f,	0.0f},
	{5,	0,	2,	"祖尔格拉布　",	0,	57,	0,	0,	-11916.7f,	-1215.72f,	92.289f,	4.72454f},
	{5,	0,	2,	"祖阿曼　",	0,	70,	1,	530,	6851.78f,	-7972.57f,	179.242f,	4.64691f},
	{5,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},

	{6,	0,	2,	"艾尔文森林　",	0,	0,	2,	0,	-9449.06f,	64.8392f,	56.3581f,	3.07047f},
	{6,	0,	2,	"永歌森林　",	0,	0,	1,	530,	9024.37f,	-6682.55f,	16.8973f,	3.14131f},
	{6,	0,	2,	"丹莫罗　",	0,	0,	2,	0,	-5603.76f,	-482.704f,	396.98f,	5.23499f},
	{6,	0,	2,	"提瑞斯法林地　",	0,	0,	1,	0,	2274.95f,	323.918f,	34.1137f,	4.24367f},
	{6,	0,	2,	"鬼魂之地　",	0,	10,	1,	530,	7595.73f,	-6819.6f,	84.3718f,	2.56561f},
	{6,	0,	2,	"洛克莫丹　",	0,	10,	2,	0,	-5405.85f,	-2894.15f,	341.972f,	5.48238f},
	{6,	0,	2,	"银松森林　",	0,	10,	1,	0,	505.126f,	1504.63f,	124.808f,	1.77987f},
	{6,	0,	2,	"西部荒野　",	0,	10,	2,	0,	-10684.9f,	1033.63f,	32.5389f,	6.07384f},
	{6,	0,	2,	"赤脊山　",	0,	15,	2,	0,	-9447.8f,	-2270.85f,	71.8224f,	0.283853f},
	{6,	0,	2,	"暮色森林　",	0,	18,	2,	0,	-10531.7f,	-1281.91f,	38.8647f,	1.56959f},
	{6,	0,	2,	"希尔斯布莱德丘陵　",	0,	20,	0,	0,	-385.805f,	-787.954f,	54.6655f,	1.03926f},
	{6,	0,	2,	"湿地　",	0,	20,	2,	0,	-3517.75f,	-913.401f,	8.86625f,	2.60705f},
	{6,	0,	2,	"奥特兰克山谷　",	0,	30,	0,	0,	275.049f,	-652.044f,	130.296f,	0.502032f},
	{6,	0,	2,	"阿拉希高地　",	0,	30,	0,	0,	-1581.45f,	-2704.06f,	35.4168f,	0.490373f},
	{6,	0,	2,	"荆棘谷　",	0,	30,	0,	0,	-11921.7f,	-59.544f,	39.7262f,	3.73574f},
	{6,	0,	2,	"荒芜之地　",	0,	35,	0,	0,	-6782.56f,	-3128.14f,	240.48f,	5.65912f},
	{6,	0,	2,	"悲伤沼泽　",	0,	35,	0,	0,	-10368.6f,	-2731.3f,	21.6537f,	5.29238f},
	{6,	0,	2,	"辛特兰　",	0,	40,	0,	0,	112.406f,	-3929.74f,	136.358f,	0.981903f},
	{6,	0,	2,	"灼热峡谷　",	0,	43,	0,	0,	-6686.33f,	-1198.55f,	240.027f,	0.916887f},
	{6,	0,	2,	"诅咒之地　",	0,	45,	0,	0,	-11184.7f,	-3019.31f,	7.29238f,	3.20542f},
	{6,	0,	2,	"燃烧平原　",	0,	50,	0,	0,	-7979.78f,	-2105.72f,	127.919f,	5.10148f},
	{6,	0,	2,	"东瘟疫之地　",	0,	51,	0,	0,	1743.69f,	-1723.86f,	59.6648f,	5.23722f},
	{6,	0,	2,	"西瘟疫之地　",	0,	53,	0,	0,	2280.64f,	-5275.05f,	82.0166f,	4.7479f},
	{6,	0,	2,	"奎尔丹纳斯岛　",	0,	70,	0,	530,	12806.5f,	-6911.11f,	41.1156f,	2.22935f},
	{6,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},

	{7,	0,	2,	"秘蓝岛　",	0,	0,	2,	530,	-4192.62f,	-12576.7f,	36.7598f,	1.62813f},
	{7,	0,	2,	"泰达希尔　",	0,	0,	2,	1,	9889.03f,	915.869f,	1307.43f,	1.9336f},
	{7,	0,	2,	"杜隆塔尔　",	0,	0,	1,	1,	228.978f,	-4741.87f,	10.1027f,	0.416883f},
	{7,	0,	2,	"莫高雷　",	0,	0,	1,	1,	-2473.87f,	-501.225f,	-9.42465f,	0.6525f},
	{7,	0,	2,	"秘血岛　",	0,	15,	2,	530,	-2095.7f,	-11841.1f,	51.1557f,	6.19288f},
	{7,	0,	2,	"黑海岸　",	0,	10,	2,	1,	6463.25f,	683.986f,	8.92792f,	4.33534f},
	{7,	0,	2,	"贫瘠之地　",	0,	10,	1,	1,	-575.772f,	-2652.45f,	95.6384f,	0.006469f},
	{7,	0,	2,	"石爪山脉　",	0,	15,	0,	1,	1574.89f,	1031.57f,	137.442f,	3.8013f},
	{7,	0,	2,	"灰谷　",	0,	18,	0,	1,	1919.77f,	-2169.68f,	94.6729f,	6.14177f},
	{7,	0,	2,	"千针石林　",	0,	25,	1,	1,	-5375.53f,	-2509.2f,	-40.432f,	2.41885f},
	{7,	0,	2,	"凄凉之地　",	0,	30,	0,	1,	-656.056f,	1510.12f,	88.3746f,	3.29553f},
	{7,	0,	2,	"沉泥沼泽　",	0,	35,	0,	1,	-3350.12f,	-3064.85f,	33.0364f,	5.12666f},
	{7,	0,	2,	"菲拉斯　",	0,	40,	0,	1,	-4808.31f,	1040.51f,	103.769f,	2.90655f},
	{7,	0,	2,	"塔纳利斯　",	0,	40,	0,	1,	-6940.91f,	-3725.7f,	48.9381f,	3.11174f},
	{7,	0,	2,	"艾萨拉　",	0,	45,	0,	1,	3117.12f,	-4387.97f,	91.9059f,	5.49897f},
	{7,	0,	2,	"费伍德森林　",	0,	48,	0,	1,	3898.8f,	-1283.33f,	220.519f,	6.24307f},
	{7,	0,	2,	"安戈洛环型山　",	0,	48,	0,	1,	-6291.55f,	-1158.62f,	-258.138f,	0.457099f},
	{7,	0,	2,	"希利苏斯　",	0,	55,	0,	1,	-6815.25f,	730.015f,	40.9483f,	2.39066f},
	{7,	0,	2,	"冬泉谷　",	0,	55,	0,	1,	6658.57f,	-4553.48f,	718.019f,	5.18088f},
	{7,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},

	{8,	0,	2,	"地狱火半岛　",	0,	58,	0,	530,	-207.335f,	2035.92f,	96.464f,	1.59676f},
	{8,	0,	2,	"赞加沼泽　",	0,	60,	0,	530,	-220.297f,	5378.58f,	23.3223f,	1.61718f},
	{8,	0,	2,	"泰罗卡森林　",	0,	62,	0,	530,	-2266.23f,	4244.73f,	1.47728f,	3.68426f},
	{8,	0,	2,	"纳格兰　",	0,	64,	0,	530,	-1610.85f,	7733.62f,	-17.2773f,	1.33522f},
	{8,	0,	2,	"刀锋山　",	0,	65,	0,	530,	2029.75f,	6232.07f,	133.495f,	1.30395f},
	{8,	0,	2,	"虚空风暴　",	0,	67,	0,	530,	3271.2f,	3811.61f,	143.153f,	3.44101f},
	{8,	0,	2,	"影月谷　",	0,	67,	0,	530,	-3681.01f,	2350.76f,	76.587f,	4.25995f},
	{8,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},

	{9,	0,	2,	"北风苔原　",	0,	68,	0,	571,	2954.24f,	5379.13f,	60.4538f,	2.55544f},
	{9,	0,	2,	"凛风海湾　",	0,	68,	0,	571,	682.848f,	-3978.3f,	230.161f,	1.54207f},
	{9,	0,	2,	"龙骨荒野　",	0,	71,	0,	571,	2678.17f,	891.826f,	4.37494f,	0.101121f},
	{9,	0,	2,	"灰熊丘陵　",	0,	73,	0,	571,	4017.35f,	-3403.85f,	290.0f,	5.35431f},
	{9,	0,	2,	"祖达克　",	0,	74,	0,	571,	5560.23f,	-3211.66f,	371.709f,	5.55055f},
	{9,	0,	2,	"索拉查盆地　",	0,	76,	0,	571,	5614.67f,	5818.86f,	-69.722f,	3.60807f},
	{9,	0,	2,	"晶歌森林　",	0,	77,	0,	571,	5411.17f,	-966.37f,	167.082f,	1.57167f},
	{9,	0,	2,	"风暴群山　",	0,	77,	0,	571,	6120.46f,	-1013.89f,	408.39f,	5.12322f},
	{9,	0,	2,	"冰冠冰川　",	0,	77,	0,	571,	8323.28f,	2763.5f,	655.093f,	2.87223f},
	{9,	0,	2,	"冬拥湖　",	0,	77,	0,	571,	4522.23f,	2828.01f,	389.975f,	0.215009f},
	{9,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},

//	{10,	0,	2,	"Teleport",	0,	0,	0,	map,	xf,	yf,	zf,	of},
//	{10,	1,	7,	"Back..",	0,	0,	0,	0,	0,	0,	0,	0},
};

// TextIDs from npc_text
enum eEnums
{
	TEXT_MAIN_H		=	300000,			//Horde main menu text
	TEXT_MAIN_A		=	300001,			//Alliance main menu text
	TEXT_DUNGEON	=	300002,			//Dungeon teleport menu texts
	TEXT_RAID		=	300003,			//Raid teleport menu text
	TEXT_AREA		=	300004,			//Area teleport location menu texts
	
	TELEPORT_COUNT	=	sizeof TeLe/sizeof(*TeLe),
};

#define ARE_YOU_SURE	"你确定要传送到　"
#define ERROR_COMBAT	"|cffff0000Вы в бою!|r"

bool Custom_FactCheck (uint32 Fact, unsigned char Key)
{
	bool Show = false;
	switch (TeLe[Key].faction)
	{
	case 0:
		Show = true;
		break;
	case 1:
		if (Fact == HORDE)
			Show = true;
		break;
	case 2:
		if (Fact == ALLIANCE)
			Show = true;
		break;
	}
	return (Show);
}

uint32 Custom_GetText (unsigned int menu, Player* pPlayer)
{
	uint32 TEXT = TEXT_AREA;
	switch (menu)
	{
	case 0:
		switch (pPlayer->GetTeam())
		{
		case ALLIANCE:
			TEXT = TEXT_MAIN_A;
			break;
		case HORDE:
			TEXT = TEXT_MAIN_H;
			break;
		}
	case 1:
	case 2:
	case 3:
		TEXT = TEXT_DUNGEON;
		break;
	case 4:
		TEXT = TEXT_RAID;
		break;
	}
	return (TEXT);
}

void Custom_GetMenu (Player* pPlayer, Creature* pCreature, uint32 Key)
{
	bool ENDMENU = false;
	for(uint32 i = 0; i < TELEPORT_COUNT; i++)
	{
		if (ENDMENU && TeLe[i].menu_id != Key)
			break;
		if (TeLe[i].menu_id == Key && pPlayer->getLevel() >= TeLe[i].level && Custom_FactCheck(pPlayer->GetTeam(), i))
		{
			if (TeLe[i].next_menu_id != 0)
				pPlayer->ADD_GOSSIP_ITEM_EXTENDED(TeLe[i].icon, TeLe[i].name, GOSSIP_SENDER_MAIN, i, "", TeLe[i].cost, false);
			else
				pPlayer->ADD_GOSSIP_ITEM_EXTENDED(TeLe[i].icon, TeLe[i].name, GOSSIP_SENDER_MAIN, i, ARE_YOU_SURE+TeLe[i].name, TeLe[i].cost, false);
			ENDMENU = true;
		}
	}
	pPlayer->PlayerTalkClass->SendGossipMenu(Custom_GetText(Key, pPlayer), pCreature->GetGUID());
}

class TeLe_gossip_codebox : public CreatureScript
{
	public:
	TeLe_gossip_codebox()
	: CreatureScript("TeLe_gossip_codebox")
	{
	}

	bool OnGossipHello(Player* pPlayer, Creature* pCreature)
	{
		Custom_GetMenu(pPlayer, pCreature, 1);
		return true;
	}

	bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
	{
		pPlayer->PlayerTalkClass->ClearMenus(); // clear the menu

		pPlayer->ModifyMoney(-1 * TeLe[uiAction].cost); // take cash
		uint32 Key = TeLe[uiAction].next_menu_id;
		if (Key == 0) // if no next_menu_id, then teleport to coords
		{
			if (!pPlayer->IsInCombat())
			{
				pPlayer->CLOSE_GOSSIP_MENU();
				pPlayer->TeleportTo(TeLe[uiAction].map, TeLe[uiAction].x, TeLe[uiAction].y, TeLe[uiAction].z, TeLe[uiAction].o);
				return true;
			}
			pPlayer->GetSession()->SendAreaTriggerMessage(ERROR_COMBAT);
			Key = TeLe[uiAction].menu_id;
		}

		Custom_GetMenu(pPlayer, pCreature, Key);
		return true;
	}
};


void AddSC_script_bot_commands()
{
    new script_bot_commands();
//	new item_bothelp();
	new TeLe_gossip_codebox();
	new item_teleport;    
        new npc_enchantment2();

}
