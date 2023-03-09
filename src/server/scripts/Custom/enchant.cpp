#include "Config.h"
#include "Pet.h"
#include "ScriptMgr.h"
#include <Player.h>
#ifndef UNORDERED_MAP
#define UNORDERED_MAP std::unordered_map
#endif

#ifdef BOOST_VERSION
#define USING_BOOST
#endif
#ifdef USING_BOOST
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#endif

static const uint32 ItemEnchants[] = { 3789, 3854, 3273, 3225, 3870, 1899, 2674, 2675, 2671, 2672, 3365, 2673, 2343, 425, 3855, 1894, 1103, 1898, 3345, 1743, 3093, 1900, 3846, 1606, 283, 1, 3265, 2, 3, 3266, 1903, 13, 26, 7, 803, 1896, 2666, 25 };
static const uint32 ItemEnchants_size = (sizeof(ItemEnchants) / sizeof(*ItemEnchants)) - 1;
#define GOSSIP_SENDER_MAIN      1000
#define MSG_TYPE                100004
#define MSG_PET                 100005
#define MAIN_MENU               "<= [Main Menu]"
namespace
{
    class RWLockable
    {
    public:
#ifdef USING_BOOST
        typedef boost::shared_mutex LockType;
        typedef boost::shared_lock<boost::shared_mutex> ReadGuard;
        typedef boost::unique_lock<boost::shared_mutex> WriteGuard;
#else
        typedef ACE_RW_Thread_Mutex LockType;
        typedef ACE_Read_Guard<LockType> ReadGuard;
        typedef ACE_Write_Guard<LockType> WriteGuard;
#endif
        LockType& GetLock() { return _lock; }
    private:
        LockType _lock;
    };

    class EnchantStore : public RWLockable
    {
    public:
        typedef UNORDERED_MAP<uint32, uint32> ItemLowToEnchant;                 // map[itemguid] = {enchant}
        typedef UNORDERED_MAP<uint32, ItemLowToEnchant> PlayerLowToItemLowMap;  // map[playerguid] = {ItemLowToEnchant}

        void LoadPlayerEnchants(uint32 playerLow)
        {
            QueryResult result = CharacterDatabase.PQuery("SELECT iguid, display FROM custom_item_enchant_visuals WHERE iguid IN(SELECT guid FROM item_instance WHERE owner_guid = %u)", playerLow);
            if (!result)
                return;

            ItemLowToEnchant temp;
            do
            {
                uint32 iguid = result->Fetch()[0].GetUInt32();
                uint32 display = result->Fetch()[1].GetUInt32();
                temp[iguid] = display;
            } while (result->NextRow());

            WriteGuard lock(GetLock());
            hashmap[playerLow] = temp;
        }

        void DeletePlayerEnchants(uint32 playerLow)
        {
            WriteGuard lock(GetLock());
            hashmap.erase(playerLow);
        }

        void AddEnchant(uint32 playerLow, uint32 itemLow, uint32 enchant)
        {
            CharacterDatabase.PExecute("REPLACE INTO custom_item_enchant_visuals (iguid, display) VALUES (%u, %u)", itemLow, enchant);

            WriteGuard lock(GetLock());
            hashmap[playerLow][itemLow] = enchant;
        }

        uint32 GetEnchant(uint32 playerLow, uint32 itemLow)
        {
            ReadGuard lock(GetLock());

            PlayerLowToItemLowMap::iterator it = hashmap.find(playerLow);
            if (it == hashmap.end())
                return 0;

            ItemLowToEnchant::iterator it2 = it->second.find(itemLow);
            if (it2 == it->second.end())
                return 0;

            return it2->second;
        }

        void RemoveEnchant(uint32 playerLow, uint32 itemLow)
        {
            {
                WriteGuard lock(GetLock());

                PlayerLowToItemLowMap::iterator it = hashmap.find(playerLow);
                if (it == hashmap.end())
                    return;

                it->second.erase(itemLow);
                if (it->second.empty())
                    hashmap.erase(playerLow);
            }

            CharacterDatabase.PExecute("DELETE FROM custom_item_enchant_visuals WHERE iguid = %u", itemLow);
        }

    private:
        PlayerLowToItemLowMap hashmap;
    };
};

static EnchantStore enchantStore;

uint32 GetItemEnchantVisual(Player* player, Item* item)
{
    if (!player || !item)
        return 0;

    uint32 visual = enchantStore.GetEnchant(player->GetGUID(), item->GetGUID());
    if (!visual)
        return 0;

    if (uint32 enchant = item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT))
    {
        enchantStore.RemoveEnchant(player->GetGUID(), item->GetGUID());
        player->SaveToDB();
        return enchant;
    }

    return visual;
}

void SetRandomEnchantVisual(Player* player, Item* item)
{
    if (!player || !item)
        return;

    const ItemTemplate* temp = item->GetTemplate();
    if (temp->Class != ITEM_CLASS_WEAPON)
        return;

    if (temp->SubClass == ITEM_SUBCLASS_WEAPON_BOW ||
        temp->SubClass == ITEM_SUBCLASS_WEAPON_GUN ||
        temp->SubClass == ITEM_SUBCLASS_WEAPON_obsolete ||
        temp->SubClass == ITEM_SUBCLASS_WEAPON_FIST ||
        temp->SubClass == ITEM_SUBCLASS_WEAPON_THROWN ||
        temp->SubClass == ITEM_SUBCLASS_WEAPON_SPEAR ||
        temp->SubClass == ITEM_SUBCLASS_WEAPON_CROSSBOW ||
        temp->SubClass == ITEM_SUBCLASS_WEAPON_WAND ||
        temp->SubClass == ITEM_SUBCLASS_WEAPON_FISHING_POLE)
        return;

    if (rand_norm() >= (sConfigMgr->GetFloatDefault("Visual.drop.chance", 0.10)))
        return;

    uint32 enchant = ItemEnchants[urand(0, ItemEnchants_size)];
    enchantStore.AddEnchant(player->GetGUID(), item->GetGUID(), enchant);

    player->SaveToDB();
    player->SetVisibleItemSlot(EQUIPMENT_SLOT_MAINHAND, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND));
    player->SetVisibleItemSlot(EQUIPMENT_SLOT_OFFHAND, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND));
}

class item_enchant_visuals : public PlayerScript
{
public:
    item_enchant_visuals() : PlayerScript("item_enchant_visuals")
    {
        // Create DB table on startup if doesnt exist
        const char* sql =
            "CREATE TABLE IF NOT EXISTS `custom_item_enchant_visuals` ("
            "    `iguid` INT(10) UNSIGNED NOT NULL COMMENT 'item DB guid',"
            "    `display` INT(10) UNSIGNED NOT NULL COMMENT 'enchantID',"
            "    PRIMARY KEY (`iguid`)"
            ")"
            "COMMENT='stores the enchant IDs for the visuals'"
            "COLLATE='latin1_swedish_ci'"
            "ENGINE=InnoDB;";
        CharacterDatabase.DirectExecute(sql);

        // Delete unused rows from DB table
        CharacterDatabase.DirectExecute("DELETE FROM custom_item_enchant_visuals WHERE NOT EXISTS(SELECT 1 FROM item_instance WHERE custom_item_enchant_visuals.iguid = item_instance.guid)");
    }

    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        enchantStore.LoadPlayerEnchants(player->GetGUID());
        player->SetVisibleItemSlot(EQUIPMENT_SLOT_MAINHAND, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND));
        player->SetVisibleItemSlot(EQUIPMENT_SLOT_OFFHAND, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND));
    }

    void OnLogout(Player* player) override
    {
        enchantStore.DeletePlayerEnchants(player->GetGUID());
    }
};

enum Enchants
{
    ENCHANT_WEP_BERSERKING              = 3789,
    ENCHANT_WEP_BLADE_WARD              = 3869,
    ENCHANT_WEP_BLOOD_DRAINING          = 3870,
    ENCHANT_WEP_ACCURACY                = 3788,
    ENCHANT_WEP_AGILITY_1H              = 1103,
    ENCHANT_WEP_SPIRIT                  = 3844,
    ENCHANT_WEP_BATTLEMASTER            = 2675,
    ENCHANT_WEP_BLACK_MAGIC             = 3790,
    ENCHANT_WEP_ICEBREAKER              = 3239,
    ENCHANT_WEP_LIFEWARD                = 3241,
    ENCHANT_WEP_MIGHTY_SPELL_POWER      = 3834, // One-hand
    ENCHANT_WEP_EXECUTIONER             = 3225,
    ENCHANT_WEP_POTENCY                 = 3833,
    ENCHANT_WEP_TITANGUARD              = 3851,
    ENCHANT_2WEP_MASSACRE               = 3827,
	ENCHANT_2WEP_SCOURGEBANE			= 3247,
	ENCHANT_2WEP_GIANT_SLAYER			= 3251,
	ENCHANT_2WEP_GREATER_SPELL_POWER    = 3854,
	ENCHANT_2WEP_AGILITY	            = 2670,
	ENCHANT_2WEP_MONGOOSE               = 2673,

    ENCHANT_SHIELD_DEFENSE              = 1952,
    ENCHANT_SHIELD_INTELLECT            = 1128,
    ENCHANT_SHIELD_RESILIENCE           = 3229,
	ENCHANT_SHIELD_BLOCK				= 2655,
	ENCHANT_SHIELD_STAMINA				= 1071,
	ENCHANT_SHIELD_TOUGHSHIELD			= 2653,
    ENCHANT_SHIELD_TITANIUM_PLATING     = 3849,

    ENCHANT_HEAD_BLISSFUL_MENDING       = 3819,
    ENCHANT_HEAD_BURNING_MYSTERIES      = 3820,
    ENCHANT_HEAD_DOMINANCE              = 3796,
    ENCHANT_HEAD_SAVAGE_GLADIATOR       = 3842,
    ENCHANT_HEAD_STALWART_PROTECTOR     = 3818,
    ENCHANT_HEAD_TORMENT                = 3817,
    ENCHANT_HEAD_TRIUMPH                = 3795,
	ENCHANT_HEAD_ECLIPSED_MOON			= 3815,
	ENCHANT_HEAD_FLAME_SOUL				= 3816,
	ENCHANT_HEAD_FLEEING_SHADOW			= 3814,
	ENCHANT_HEAD_FROSTY_SOUL			= 3812,
	ENCHANT_HEAD_TOXIC_WARDING			= 3813,
    
    ENCHANT_SHOULDER_MASTERS_AXE        = 3835,
    ENCHANT_SHOULDER_MASTERS_CRAG       = 3836,
    ENCHANT_SHOULDER_MASTERS_PINNACLE   = 3837,
    ENCHANT_SHOULDER_MASTERS_STORM      = 3838,
    ENCHANT_SHOULDER_GREATER_AXE        = 3808,
    ENCHANT_SHOULDER_GREATER_CRAG       = 3809,
    ENCHANT_SHOULDER_GREATER_GLADIATOR  = 3852,
    ENCHANT_SHOULDER_GREATER_PINNACLE   = 3811,
    ENCHANT_SHOULDER_GREATER_STORM      = 3810,
    ENCHANT_SHOULDER_DOMINANCE          = 3794,
    ENCHANT_SHOULDER_TRIUMPH            = 3793,

    ENCHANT_CLOAK_DARKGLOW_EMBROIDERY   = 3728,
    ENCHANT_CLOAK_SWORDGUARD_EMBROIDERY = 3730,
    ENCHANT_CLOAK_LIGHTWEAVE_EMBROIDERY = 3722,
    ENCHANT_CLOAK_SPRINGY_ARACHNOWEAVE  = 3859,
    ENCHANT_CLOAK_WISDOM                = 3296,
    ENCHANT_CLOAK_TITANWEAVE            = 1951,
    ENCHANT_CLOAK_SPELL_PIERCING        = 3243,
    ENCHANT_CLOAK_SHADOW_ARMOR          = 3256,
    ENCHANT_CLOAK_MIGHTY_ARMOR          = 3294,
    ENCHANT_CLOAK_MAJOR_AGILITY         = 1099,
    ENCHANT_CLOAK_GREATER_SPEED         = 3831,

	ENCHANT_LEG_EARTHEN					= 3853,
	ENCHANT_LEG_FROSTHIDE				= 3822,
	ENCHANT_LEG_ICESCALE				= 3823,
	ENCHANT_LEG_BRILLIANT_SPELLTHREAD	= 3719,
	ENCHANT_LEG_SAPPHIRE_SPELLTHREAD	= 3721,
	ENCHANT_LEG_DRAGONSCALE				= 3331,
	ENCHANT_LEG_WYRMSCALE				= 3332,

	ENCHANT_GLOVES_GREATER_BLASTING		= 3249,
	ENCHANT_GLOVES_ARMSMAN				= 3253,
	ENCHANT_GLOVES_CRUSHER				= 1603,
	ENCHANT_GLOVES_AGILITY				= 3222,
	ENCHANT_GLOVES_PRECISION			= 3234,
	ENCHANT_GLOVES_EXPERTISE			= 3231,

	ENCHANT_BRACERS_MAJOR_STAMINA		= 3850,
	ENCHANT_BRACERS_SUPERIOR_SP			= 2332,
	ENCHANT_BRACERS_GREATER_ASSUALT		= 3845,
	ENCHANT_BRACERS_MAJOR_SPIRT			= 1147,
	ENCHANT_BRACERS_EXPERTISE			= 3231,
	ENCHANT_BRACERS_GREATER_STATS		= 2661,
	ENCHANT_BRACERS_INTELLECT			= 1119,
	ENCHANT_BRACERS_FURL_ARCANE			= 3763,
	ENCHANT_BRACERS_FURL_FIRE			= 3759,
	ENCHANT_BRACERS_FURL_FROST			= 3760,
	ENCHANT_BRACERS_FURL_NATURE			= 3762,
	ENCHANT_BRACERS_FURL_SHADOW			= 3761,
	ENCHANT_BRACERS_FURL_ATTACK			= 3756,
	ENCHANT_BRACERS_FURL_STAMINA		= 3757,
	ENCHANT_BRACERS_FURL_SPELLPOWER		= 3758,

	ENCHANT_CHEST_POWERFUL_STATS		= 3832,
	ENCHANT_CHEST_SUPER_HEALTH			= 3297,
	ENCHANT_CHEST_GREATER_MAINA_REST		= 2381,
	ENCHANT_CHEST_EXCEPTIONAL_RESIL		= 3245,
	ENCHANT_CHEST_GREATER_DEFENSE		= 1953,

	ENCHANT_BOOTS_GREATER_ASSULT		= 1597,
	ENCHANT_BOOTS_TUSKARS_VITLIATY		= 3232,
	ENCHANT_BOOTS_SUPERIOR_AGILITY		= 983,
	ENCHANT_BOOTS_GREATER_SPIRIT		= 1147,
	ENCHANT_BOOTS_GREATER_VITALITY		= 3244,
	ENCHANT_BOOTS_ICEWALKER				= 3826,
	ENCHANT_BOOTS_GREATER_FORTITUDE		= 1075,
	ENCHANT_BOOTS_NITRO_BOOTS			= 3606,
	ENCHANT_BOOTS_PYRO_ROCKET			= 3603,
	ENCHANT_BOOTS_HYPERSPEED			= 3604,
	ENCHANT_BOOTS_ARMOR_WEBBING			= 3860,

	ENCHANT_RING_ASSULT					= 3839,
	ENCHANT_RING_GREATER_SP				= 3840,
	ENCHANT_RING_STAMINA				= 3791,
};

#include "ScriptPCH.h"
 
void Enchant(Player* player, Item* item, uint32 enchantid)
{
    if (!item)
    {
        player->GetSession()->SendNotification("You must first equip the item you are trying to enchant in order to enchant it!");
        return;
    }

    if (!enchantid)
    {
        player->GetSession()->SendNotification("Something went wrong in the code. It has been logged for developers and will be looked into, sorry for the inconvenience.");
        return;
    }
		
    item->ClearEnchantment(PERM_ENCHANTMENT_SLOT);
    item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchantid, 0, 0);
    player->GetSession()->SendNotification("|cff0000FF%s |cffFF0000succesfully enchanted!", item->GetTemplate()->Name1.c_str());
}
 
class npc_enchantment : public CreatureScript
{
public:
    npc_enchantment() : CreatureScript("npc_enchantment") { }
 
        bool OnGossipHello(Player* player, Creature* creature)
        {
			player->ADD_GOSSIP_ITEM(1, _StringToUTF8("欢迎来附魔!"), GOSSIP_SENDER_MAIN, 0);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("主手武器附魔"), GOSSIP_SENDER_MAIN, 1);
	    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("双手武器附魔"), GOSSIP_SENDER_MAIN, 2);
	    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("盾牌附魔"), GOSSIP_SENDER_MAIN, 3);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("头盔附魔"), GOSSIP_SENDER_MAIN, 4);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("护肩附魔"), GOSSIP_SENDER_MAIN, 5);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("披风附魔"), GOSSIP_SENDER_MAIN, 6);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("胸部附魔"), GOSSIP_SENDER_MAIN, 7);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("护腕附魔"), GOSSIP_SENDER_MAIN, 8);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("手套附魔"), GOSSIP_SENDER_MAIN, 9);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("腿部附魔"), GOSSIP_SENDER_MAIN, 10);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("脚部附魔"), GOSSIP_SENDER_MAIN, 11);

            if (player->HasSkill(SKILL_ENCHANTING) && player->GetSkillValue(SKILL_ENCHANTING) == 450)
                player->ADD_GOSSIP_ITEM(1, _StringToUTF8("[戒指附魔]"), GOSSIP_SENDER_MAIN, 12);

            player->PlayerTalkClass->SendGossipMenu(100001, creature->GetGUID());
			return true;
		}
 
        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
			Item * item;
			player->PlayerTalkClass->ClearMenus();

            switch (action)
            {
				case 0: //Welcome message on click
					player->GetSession()->SendAreaTriggerMessage("|cffFF0000Hello there, I will be enchanting your gear!");

				{
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("主手武器附魔"), GOSSIP_SENDER_MAIN, 1);
	    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("双手武器附魔"), GOSSIP_SENDER_MAIN, 2);
	    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("盾牌附魔"), GOSSIP_SENDER_MAIN, 3);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("头盔附魔"), GOSSIP_SENDER_MAIN, 4);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("护肩附魔"), GOSSIP_SENDER_MAIN, 5);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("披风附魔"), GOSSIP_SENDER_MAIN, 6);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("胸部附魔"), GOSSIP_SENDER_MAIN, 7);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("护腕附魔"), GOSSIP_SENDER_MAIN, 8);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("手套附魔"), GOSSIP_SENDER_MAIN, 9);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("腿部附魔"), GOSSIP_SENDER_MAIN, 10);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("脚部附魔"), GOSSIP_SENDER_MAIN, 11);


					if (player->HasSkill(SKILL_ENCHANTING) && player->GetSkillValue(SKILL_ENCHANTING) == 450)
						player->ADD_GOSSIP_ITEM(1, _StringToUTF8("戒指附魔"), GOSSIP_SENDER_MAIN, 12);
						
					player->PlayerTalkClass->SendGossipMenu(100001, creature->GetGUID());
					return true;
					break;
				}


                case 1: // Enchant Weapon
                    if (player->HasSkill(SKILL_ENCHANTING) && player->GetSkillValue(SKILL_ENCHANTING) == 450)
                    {
                        player->ADD_GOSSIP_ITEM(1, _StringToUTF8("利刃防护"), GOSSIP_SENDER_MAIN, 102);
                        player->ADD_GOSSIP_ITEM(1, _StringToUTF8("吸血"), GOSSIP_SENDER_MAIN, 103);
                    }
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加敏捷"), GOSSIP_SENDER_MAIN, 100);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加精神"), GOSSIP_SENDER_MAIN, 101);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("狂暴"), GOSSIP_SENDER_MAIN, 104);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("命中爆击等级"), GOSSIP_SENDER_MAIN, 105);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("黑魔法"), GOSSIP_SENDER_MAIN, 106);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("作战专家"), GOSSIP_SENDER_MAIN, 107);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("破冰武器"), GOSSIP_SENDER_MAIN, 108);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("生命护卫"), GOSSIP_SENDER_MAIN, 109);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加耐力"), GOSSIP_SENDER_MAIN, 110);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("攻击强度"), GOSSIP_SENDER_MAIN, 111);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("法术强度"), GOSSIP_SENDER_MAIN, 112);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("猫鼬"), GOSSIP_SENDER_MAIN, 113);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("斩杀"), GOSSIP_SENDER_MAIN, 114);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("<-返回"), GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(100002, creature->GetGUID());
					return true;
					break;

                case 2: // Enchant 2H Weapon
					item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
								if (!item)
													{
															creature->Whisper("This enchant needs a 2H weapon equiped.", LANG_UNIVERSAL, player);
															player->CLOSE_GOSSIP_MENU();
														return false;
													}
								if(item->GetTemplate()->InventoryType == INVTYPE_2HWEAPON)
								{
									player->ADD_GOSSIP_ITEM(1, _StringToUTF8("狂暴"), GOSSIP_SENDER_MAIN, 104);
									player->ADD_GOSSIP_ITEM(1, _StringToUTF8("猫鼬"), GOSSIP_SENDER_MAIN, 113);
									player->ADD_GOSSIP_ITEM(1, _StringToUTF8("斩杀"), GOSSIP_SENDER_MAIN, 114);
									player->ADD_GOSSIP_ITEM(1, _StringToUTF8("法术强度"), GOSSIP_SENDER_MAIN, 115);
									player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加敏捷"), GOSSIP_SENDER_MAIN, 116);
									player->ADD_GOSSIP_ITEM(1, _StringToUTF8("攻击强度"), GOSSIP_SENDER_MAIN, 117);
									player->ADD_GOSSIP_ITEM(1, _StringToUTF8("<-返回"), GOSSIP_SENDER_MAIN, 300);
								}
								else 
								{
									creature->Whisper("This enchant needs a 2H weapon equiped.", LANG_UNIVERSAL, player);
									player->CLOSE_GOSSIP_MENU();
								}
									player->PlayerTalkClass->SendGossipMenu(100003, creature->GetGUID());
					return true;
					break;

                case 3: // Enchant Shield
						item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
														if (!item)
													{
															creature->Whisper("This enchant needs a shield equiped.", LANG_UNIVERSAL, player);
															player->CLOSE_GOSSIP_MENU();
														return false;
													}
						if (item->GetTemplate()->InventoryType == INVTYPE_SHIELD)
						{
							player->ADD_GOSSIP_ITEM(1, _StringToUTF8("防御等级"), GOSSIP_SENDER_MAIN, 118);
							player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加智力"), GOSSIP_SENDER_MAIN, 119);
							player->ADD_GOSSIP_ITEM(1, _StringToUTF8("韧性等级"), GOSSIP_SENDER_MAIN, 120);
							player->ADD_GOSSIP_ITEM(1, _StringToUTF8("泰坦神铁护"), GOSSIP_SENDER_MAIN, 121);
							player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加耐力"), GOSSIP_SENDER_MAIN, 122);
							player->ADD_GOSSIP_ITEM(1, _StringToUTF8("盾牌格档"), GOSSIP_SENDER_MAIN, 123);
							player->ADD_GOSSIP_ITEM(1, _StringToUTF8("<-返回"), GOSSIP_SENDER_MAIN, 300);
						}
						else
						{
							creature->Whisper("This enchant needs a shield equiped.", LANG_UNIVERSAL, player);
							player->CLOSE_GOSSIP_MENU();
						}
							player->PlayerTalkClass->SendGossipMenu(100004, creature->GetGUID());
							return true;
							break;

                case 4: // Enchant Head
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("法术强度，法力恢复"), GOSSIP_SENDER_MAIN, 124);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("法术强度，暴击等级"), GOSSIP_SENDER_MAIN, 125);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("法术强度，韧性等级"), GOSSIP_SENDER_MAIN, 126);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加耐力，韧性等级"), GOSSIP_SENDER_MAIN, 127);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加耐力，防御等级"), GOSSIP_SENDER_MAIN, 128);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("攻击强度，爆击等级"), GOSSIP_SENDER_MAIN, 129);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("攻击强度，韧性等级"), GOSSIP_SENDER_MAIN, 130);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("奥术抗性，增加耐力"), GOSSIP_SENDER_MAIN, 131);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("火焰抗性，增加耐力"), GOSSIP_SENDER_MAIN, 132);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("暗影抗性，增加耐力"), GOSSIP_SENDER_MAIN, 133);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("冰霜抗性，增加耐力"), GOSSIP_SENDER_MAIN, 134);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("自然抗性，增加耐力"), GOSSIP_SENDER_MAIN, 135);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("<-返回"), GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(100005, creature->GetGUID());
					return true;
					break;

                case 5: // Enchant Shoulders
                    if (player->HasSkill(SKILL_INSCRIPTION) && player->GetSkillValue(SKILL_INSCRIPTION) == 450)
                    {
                        player->ADD_GOSSIP_ITEM(1, _StringToUTF8("声望: 攻击强度，暴击等级"), GOSSIP_SENDER_MAIN, 136);
                        player->ADD_GOSSIP_ITEM(1, _StringToUTF8("声望: 法术强度，法力恢复"), GOSSIP_SENDER_MAIN, 137);
                        player->ADD_GOSSIP_ITEM(1, _StringToUTF8("声望: 增加耐力，韧性等级"), GOSSIP_SENDER_MAIN, 138);
                        player->ADD_GOSSIP_ITEM(1, _StringToUTF8("声望: 法术强度，暴击等级"), GOSSIP_SENDER_MAIN, 139);
                    }
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("攻击强度，暴击等级"), GOSSIP_SENDER_MAIN, 140);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("法术强度，法力恢复"), GOSSIP_SENDER_MAIN, 141);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加耐力，韧性等级"), GOSSIP_SENDER_MAIN, 142);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("躲闪等级，防御等级"), GOSSIP_SENDER_MAIN, 143);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("法术强度，暴击等级"), GOSSIP_SENDER_MAIN, 144);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("法术强度，韧性等级"), GOSSIP_SENDER_MAIN, 145);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("攻击强度，韧性等级"), GOSSIP_SENDER_MAIN, 146);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("<-返回"), GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(100006, creature->GetGUID());
					return true;
					break;

                case 6: // Enchant Cloak
				 if (player->HasSkill(SKILL_TAILORING) && player->GetSkillValue(SKILL_TAILORING) == 450)
				  {
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("裁缝：黑光刺绣"), GOSSIP_SENDER_MAIN, 149);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("裁缝：亮纹刺绣"), GOSSIP_SENDER_MAIN, 150);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("裁缝：剑刃刺绣"), GOSSIP_SENDER_MAIN, 151); 
				  }
				  if (player->HasSkill(SKILL_ENGINEERING) && player->GetSkillValue(SKILL_ENGINEERING) == 450)
				  {
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("工程：坠落减速"), GOSSIP_SENDER_MAIN, 147);
				  }
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("强化潜行，增加敏捷"), GOSSIP_SENDER_MAIN, 148);                  
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加精神，降低威胁"), GOSSIP_SENDER_MAIN, 152);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("防御等级"), GOSSIP_SENDER_MAIN, 153);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("法术穿透"), GOSSIP_SENDER_MAIN, 154);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加护甲"), GOSSIP_SENDER_MAIN, 155);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加敏捷"), GOSSIP_SENDER_MAIN, 156);
                    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("急速等级"), GOSSIP_SENDER_MAIN, 157);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("<-返回"), GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(100007, creature->GetGUID());
					return true;
					break;

				case 7: //Enchant chest
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加全属性"), GOSSIP_SENDER_MAIN, 158);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加生命"), GOSSIP_SENDER_MAIN, 159);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("法力恢复"), GOSSIP_SENDER_MAIN, 160);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("韧性等级"), GOSSIP_SENDER_MAIN, 161);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("防御等级"), GOSSIP_SENDER_MAIN, 162);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("<-返回"), GOSSIP_SENDER_MAIN, 300);
					player->PlayerTalkClass->SendGossipMenu(100008, creature->GetGUID());
					return true;
					break;

				case 8: //Enchant Bracers
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加耐力"), GOSSIP_SENDER_MAIN, 163);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("法术强度"), GOSSIP_SENDER_MAIN, 164);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("攻击强度"), GOSSIP_SENDER_MAIN, 165);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加精神"), GOSSIP_SENDER_MAIN, 166);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("精准等级"), GOSSIP_SENDER_MAIN, 167);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加全属性"), GOSSIP_SENDER_MAIN, 168);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加智力"), GOSSIP_SENDER_MAIN, 169);
					 if (player->HasSkill(SKILL_LEATHERWORKING) && player->GetSkillValue(SKILL_LEATHERWORKING) == 450)
				   {
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("奥术抗性"), GOSSIP_SENDER_MAIN, 170);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("火焰抗性"), GOSSIP_SENDER_MAIN, 171);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("冰霜抗性"), GOSSIP_SENDER_MAIN, 172);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("自然抗性"), GOSSIP_SENDER_MAIN, 173);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("暗影抗性"), GOSSIP_SENDER_MAIN, 174);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("攻击强度"), GOSSIP_SENDER_MAIN, 175);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加耐力"), GOSSIP_SENDER_MAIN, 176);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("法术强度"), GOSSIP_SENDER_MAIN, 177);
				   }
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("<-返回"), GOSSIP_SENDER_MAIN, 300);
					player->PlayerTalkClass->SendGossipMenu(100009, creature->GetGUID());
					return true;
					break;

				case 9: //Enchant Gloves
				 if (player->HasSkill(SKILL_ENGINEERING) && player->GetSkillValue(SKILL_ENGINEERING) == 450)
				   {
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("爆击等级"), GOSSIP_SENDER_MAIN, 178);
				   }
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加威胁，招架等级"), GOSSIP_SENDER_MAIN, 179);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("攻击强度"), GOSSIP_SENDER_MAIN, 180);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加敏捷"), GOSSIP_SENDER_MAIN, 181);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("命中等级"), GOSSIP_SENDER_MAIN, 182);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("精准等级"), GOSSIP_SENDER_MAIN, 183);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("<-返回"), GOSSIP_SENDER_MAIN, 300);
					player->PlayerTalkClass->SendGossipMenu(100010, creature->GetGUID());
					return true;
					break;

				case 10: //Enchant legs
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加耐力，韧性等级"), GOSSIP_SENDER_MAIN, 184);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加耐力，增加敏捷"), GOSSIP_SENDER_MAIN, 185);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("攻击强度，暴击等级"), GOSSIP_SENDER_MAIN, 186);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("法术强度，增加精神"), GOSSIP_SENDER_MAIN, 187);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("法术强度，增加耐力"), GOSSIP_SENDER_MAIN, 188);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("强效耐力，增加敏捷"), GOSSIP_SENDER_MAIN, 189);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("强效攻强，暴击等级"), GOSSIP_SENDER_MAIN, 190);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("<-返回"), GOSSIP_SENDER_MAIN, 300);
					player->PlayerTalkClass->SendGossipMenu(100011, creature->GetGUID());
					return true;
					break;

				case 11: //Enchant feet
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("攻击强度"), GOSSIP_SENDER_MAIN, 191);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加耐力，跑速略微提升"), GOSSIP_SENDER_MAIN, 192);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加敏捷"), GOSSIP_SENDER_MAIN, 193);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加精神"), GOSSIP_SENDER_MAIN, 194);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加生命，法力恢复"), GOSSIP_SENDER_MAIN, 195);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("命中等级，暴击等级"), GOSSIP_SENDER_MAIN, 196);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加耐力"), GOSSIP_SENDER_MAIN, 197);
					 if (player->HasSkill(SKILL_ENGINEERING) && player->GetSkillValue(SKILL_ENGINEERING) == 450)
				   {
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("使用：火箭靴"), GOSSIP_SENDER_MAIN, 198);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("使用：火焰导弹"), GOSSIP_SENDER_MAIN, 199);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("使用：提高急速"), GOSSIP_SENDER_MAIN, 200);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("超级护甲"), GOSSIP_SENDER_MAIN, 201);
				   }
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("<-返回"), GOSSIP_SENDER_MAIN, 300);
					player->PlayerTalkClass->SendGossipMenu(100012, creature->GetGUID());
					return true;
					break;

				case 12: //Enchant rings
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("攻击强度"), GOSSIP_SENDER_MAIN, 202);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("法术强度"), GOSSIP_SENDER_MAIN, 203);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("增加耐力"), GOSSIP_SENDER_MAIN, 204);
					player->ADD_GOSSIP_ITEM(1, _StringToUTF8("<-返回"), GOSSIP_SENDER_MAIN, 300);
					player->PlayerTalkClass->SendGossipMenu(100013, creature->GetGUID());
					return true;
					break;

                case 100:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_AGILITY_1H);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

                case 101:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_SPIRIT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

                case 102:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_BLADE_WARD);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

                case 103:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_BLOOD_DRAINING);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

                case 104:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_BERSERKING);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 105:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_ACCURACY);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 106:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_BLACK_MAGIC);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 107:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_BATTLEMASTER);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 108:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_ICEBREAKER);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 109:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_LIFEWARD);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 110:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_TITANGUARD);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 111:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_POTENCY);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 112:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_MIGHTY_SPELL_POWER);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 113:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_2WEP_MONGOOSE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 114:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_EXECUTIONER);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 115:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_2WEP_GREATER_SPELL_POWER);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 116:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_2WEP_AGILITY);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 117:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_2WEP_MASSACRE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 118:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_DEFENSE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 119:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_INTELLECT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 120:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_RESILIENCE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 121:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_TITANIUM_PLATING);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 122:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_STAMINA);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 123:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_TOUGHSHIELD);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 124:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_BLISSFUL_MENDING);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 125:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_BURNING_MYSTERIES);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 126:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_DOMINANCE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 127:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_SAVAGE_GLADIATOR);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 128:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_STALWART_PROTECTOR);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 129:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_TORMENT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 130:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_TRIUMPH);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 131:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_ECLIPSED_MOON);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 132:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_FLAME_SOUL);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 133:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_FLEEING_SHADOW);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 134:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_FROSTY_SOUL);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 135:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_TOXIC_WARDING);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 136:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_MASTERS_AXE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 137:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_MASTERS_CRAG);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 138:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_MASTERS_PINNACLE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 139:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_MASTERS_STORM);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 140:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_GREATER_AXE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 141:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_GREATER_CRAG);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 142:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_GREATER_GLADIATOR);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 143:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_GREATER_PINNACLE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 144:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_GREATER_STORM);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 145:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_DOMINANCE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 146:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_TRIUMPH);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 147:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_SPRINGY_ARACHNOWEAVE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 148:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_SHADOW_ARMOR);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 149:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_DARKGLOW_EMBROIDERY);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 150:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_LIGHTWEAVE_EMBROIDERY);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 151:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_SWORDGUARD_EMBROIDERY);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 152:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_WISDOM);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 153:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_TITANWEAVE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 154:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_SPELL_PIERCING);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 155:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_MIGHTY_ARMOR);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 156:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_MAJOR_AGILITY);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 157:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_GREATER_SPEED);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 158:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_POWERFUL_STATS);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 159:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_SUPER_HEALTH);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 160:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_GREATER_MAINA_REST);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 161:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_EXCEPTIONAL_RESIL);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 162:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_GREATER_DEFENSE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 163:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_MAJOR_STAMINA);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 164:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_SUPERIOR_SP);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 165:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_GREATER_ASSUALT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 166:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_MAJOR_SPIRT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 167:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_EXPERTISE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 168:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_GREATER_STATS);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 169:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_INTELLECT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 170:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_ARCANE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 171:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_FIRE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 172:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_FROST);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 173:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_NATURE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 174:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_SHADOW);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 175:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_ATTACK);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 176:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_STAMINA);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 177:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_SPELLPOWER);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 178:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_GREATER_BLASTING);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 179:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_ARMSMAN);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 180:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_CRUSHER);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 181:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_AGILITY);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 182:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_PRECISION);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 183:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_EXPERTISE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 184:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_EARTHEN);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 185:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_FROSTHIDE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 186:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_ICESCALE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 187:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_BRILLIANT_SPELLTHREAD);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 188:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_SAPPHIRE_SPELLTHREAD);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 189:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_DRAGONSCALE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 190:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_WYRMSCALE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 191:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_GREATER_ASSULT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 192:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_TUSKARS_VITLIATY);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 193:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_SUPERIOR_AGILITY);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 194:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_GREATER_SPIRIT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 195:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_GREATER_VITALITY);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 196:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_ICEWALKER);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 197:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_GREATER_FORTITUDE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 198:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_NITRO_BOOTS);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 199:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_PYRO_ROCKET);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 200:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_HYPERSPEED);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 201:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_ARMOR_WEBBING);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 202:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER1), ENCHANT_RING_ASSULT);
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER2), ENCHANT_RING_ASSULT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 203:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER1), ENCHANT_RING_GREATER_SP);
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER2), ENCHANT_RING_GREATER_SP);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 204:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER1), ENCHANT_RING_STAMINA);
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER2), ENCHANT_RING_STAMINA);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 300:
					{
									player->ADD_GOSSIP_ITEM(1, _StringToUTF8("欢迎来附魔!"), GOSSIP_SENDER_MAIN, 0);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("主手武器附魔"), GOSSIP_SENDER_MAIN, 1);
	    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("双手武器附魔"), GOSSIP_SENDER_MAIN, 2);
	    player->ADD_GOSSIP_ITEM(1, _StringToUTF8("盾牌附魔"), GOSSIP_SENDER_MAIN, 3);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("头盔附魔"), GOSSIP_SENDER_MAIN, 4);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("护肩附魔"), GOSSIP_SENDER_MAIN, 5);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("披风附魔"), GOSSIP_SENDER_MAIN, 6);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("胸部附魔"), GOSSIP_SENDER_MAIN, 7);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("护腕附魔"), GOSSIP_SENDER_MAIN, 8);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("手套附魔"), GOSSIP_SENDER_MAIN, 9);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("腿部附魔"), GOSSIP_SENDER_MAIN, 10);
            player->ADD_GOSSIP_ITEM(1, _StringToUTF8("脚部附魔"), GOSSIP_SENDER_MAIN, 11);

						if (player->HasSkill(SKILL_ENCHANTING) && player->GetSkillValue(SKILL_ENCHANTING) == 450)
							player->ADD_GOSSIP_ITEM(1, _StringToUTF8("戒指附魔"), GOSSIP_SENDER_MAIN, 12);

						player->PlayerTalkClass->SendGossipMenu(100001, creature->GetGUID());
						return true;
						break;
					}
			}
			return true;
        }
};


class Npc_Beastmaster : public CreatureScript
{
public:
        Npc_Beastmaster() : CreatureScript("Npc_Beastmaster") { }

void CreatePet(Player *player, Creature * m_creature, uint32 entry) {

    if(sConfigMgr->GetBoolDefault("BeastMaster.OnlyHunter", false)) // Checks to see if Only Hunters can have pets.
     {
        if(player->getClass() != CLASS_HUNTER) {
            m_creature->Whisper("You are not a Hunter!", LANG_UNIVERSAL, player);
            return;
        }
     }

    if(player->GetPet()) {
        m_creature->Whisper("First you must abandon your Pet!", LANG_UNIVERSAL, player);
        return;
    }

    Creature *creatureTarget = m_creature->SummonCreature(entry, player->GetPositionX(), player->GetPositionY()+2, player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 500);
    if(!creatureTarget) return;

    Pet* pet = player->CreateTamedPetFrom(creatureTarget, 0);

    if(!pet) return;

        // kill original creature
    creatureTarget->setDeathState(JUST_DIED);
    creatureTarget->RemoveCorpse();
    creatureTarget->SetHealth(0);                       // just for nice GM-mode view

    pet->SetPower(POWER_HAPPINESS, 1048000);

    // prepare visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, player->getLevel() - 1);
    pet->GetMap()->AddToMap((Creature*)pet);
        // visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, player->getLevel());

    if(!pet->InitStatsForLevel(player->getLevel()))
        TC_LOG_ERROR("scripts", "Pet Create fail: no init stats for entry %u", entry);
        pet->UpdateAllStats();

    // caster have pet now
        player->SetMinion(pet, true);

        pet->SavePetToDB(PET_SAVE_AS_CURRENT);
        pet->InitTalentForLevel();
        player->PetSpellInitialize();
    //end
        player->CLOSE_GOSSIP_MENU();
        m_creature->Whisper("Pet added. You might want to feed it and name it somehow.", LANG_UNIVERSAL, player);
        return;
    }

bool OnGossipHello(Player* player, Creature* m_creature)
{
    bool EnableNormalPet = sConfigMgr->GetBoolDefault("BeastMaster.EnableNormalPet", true);
    bool EnableExoticPet = sConfigMgr->GetBoolDefault("BeastMaster.EnableExoticPet", true);

    // Main Menu

    // Check config if "Normal Pet " is enabled or not
    if(EnableNormalPet)
        player->ADD_GOSSIP_ITEM( 7, _StringToUTF8("普通的宠物 ->")              , GOSSIP_SENDER_MAIN, 1000);
    // Check if player can have an exotic pet, and check config if "Exotic" is enabled or not
    if(player->CanTameExoticPets() && EnableExoticPet)
        player->ADD_GOSSIP_ITEM( 7, _StringToUTF8("奇异的宠物 ->")              , GOSSIP_SENDER_MAIN, 2000);
    // Now to add the spells, vendor, and stable stuffs
    player->ADD_GOSSIP_ITEM(4, _StringToUTF8("教授宏物技能 ->")      , GOSSIP_SENDER_MAIN, 3000);
    player->ADD_GOSSIP_ITEM(2, _StringToUTF8("宠物管理"), GOSSIP_SENDER_MAIN, 6006);
    player->ADD_GOSSIP_ITEM(6, _StringToUTF8("宏物食物销售"), GOSSIP_SENDER_MAIN, 6007);
    player->SEND_GOSSIP_MENU(MSG_TYPE, m_creature->GetGUID());
return true;
}

bool showNormalPet(Player *player, Creature *m_creature, uint32 showFromId = 0)
{
 QueryResult result;
 result = WorldDatabase.PQuery("SELECT `entry`, `name` FROM `beastmaster` WHERE `cat_number` = 0 ORDER BY `entry` ASC");

 if (result)
 {
 uint32 entryNum = 0;
 std::string petName = "";
 uint8 tokenOrGold = 0;
 uint32 price = 0;
 uint32 nToken = 0;

  do
 {
 Field *fields = result->Fetch();
 entryNum = fields[0].GetInt32();
 petName = fields[1].GetString();

    player->ADD_GOSSIP_ITEM(9, petName, GOSSIP_SENDER_MAIN, entryNum);
}
 while (result->NextRow());

    player->SEND_GOSSIP_MENU(MSG_PET, m_creature->GetGUID());
 return true;
 }
 else
 {
 if (showFromId = 0)
 {
 //you are too poor
 m_creature->Whisper("You don't have enough money.", LANG_UNIVERSAL, player);
 player->CLOSE_GOSSIP_MENU();
 }
 else
 {

 //show Spells from beginning
 showNormalPet(player, m_creature, 0);
 }
 }

 return false;
}

bool showExoticPet(Player *player, Creature *m_creature, uint32 showFromId = 0)
{
 QueryResult result;
 result = WorldDatabase.PQuery("SELECT `entry`, `name` FROM `beastmaster` WHERE `cat_number` = 1 ORDER BY `entry` ASC");

 if (result)
 {
 uint32 entryNum = 0;
 std::string petName = "";
  do
 {
 Field *fields = result->Fetch();
 entryNum = fields[0].GetInt32();
 petName = fields[1].GetString();

    player->ADD_GOSSIP_ITEM(9, petName, GOSSIP_SENDER_MAIN, entryNum);
}
 while (result->NextRow());

    player->SEND_GOSSIP_MENU(MSG_PET, m_creature->GetGUID());
 return true;
 }
 else
 {
 if (showFromId = 0)
 {
 //you are too poor
 //m_creature->Whisper("You don't have enough money.", LANG_UNIVERSAL, player);
 //player->CLOSE_GOSSIP_MENU();
 }
 else
 {

 //show Spells from beginning
 showExoticPet(player, m_creature, 0);
 }
 }

 return false;
}

bool showPetSpells(Player *player, Creature *m_creature, uint32 showFromId = 0)
{
 QueryResult result;
 result = WorldDatabase.PQuery("SELECT `entry`, `name` FROM `beastmaster` WHERE `cat_number` = 2 ORDER BY `entry` ASC");

 if (result)
 {
 uint32 entryNum = 0;
 std::string petName = "";
 uint8 tokenOrGold = 0;
 uint32 price = 0;
 uint32 nToken = 0;

  do
 {
 Field *fields = result->Fetch();
 entryNum = fields[0].GetInt32();
 petName = fields[1].GetString();

    player->ADD_GOSSIP_ITEM(9, petName, GOSSIP_SENDER_MAIN, entryNum);
}
 while (result->NextRow());

    player->SEND_GOSSIP_MENU(MSG_PET, m_creature->GetGUID());
 return true;
 }
 else
 {
 if (showFromId = 0)
 {
 //you are too poor
 m_creature->Whisper("You don't have enough money.", LANG_UNIVERSAL, player);
 player->CLOSE_GOSSIP_MENU();
 }
 else
 {

 //show Spells from beginning
 showPetSpells(player, m_creature, 0);
 }
 }

 return false;
}

void SendDefaultMenu(Player* player, Creature* m_creature, uint32 uiAction)
{

// Not allow in combat
if (player->IsInCombat())
{
    player->CLOSE_GOSSIP_MENU();
    m_creature->Say("You are in combat!", LANG_UNIVERSAL);
    return;
}

    bool EnableNormalPet = sConfigMgr->GetBoolDefault("BeastMaster.EnableNormalPet", true);
    bool EnableExoticPet = sConfigMgr->GetBoolDefault("BeastMaster.EnableExoticPet", true);

  // send name as gossip item
        QueryResult result;
        uint32 spellId = 0;
        uint32 catNumber = 0;
        uint32 cost = 0;
        std::string spellName = "";
        uint32 token = 0;
        bool tokenOrGold = true;

        result = WorldDatabase.PQuery("SELECT * FROM `beastmaster` WHERE `entry` = %u LIMIT 1", uiAction);

if (result)
{
    do {
        Field *fields = result->Fetch();
        catNumber = fields[1].GetInt32();
        tokenOrGold = fields[2].GetBool();
        cost = fields[3].GetInt32();
        token = fields[4].GetInt32();
        spellName = fields[5].GetString();
        spellId = fields[6].GetInt32();

        if (tokenOrGold)
        {
            if (!player->HasItemCount(token, cost))
                {
                    m_creature->Whisper("You ain't gots no darn chips.", LANG_UNIVERSAL, player);
                    player->CLOSE_GOSSIP_MENU();
                    return;
                }
            else if (uiAction != 1000 && uiAction != 2000 && uiAction != 3000 && catNumber != 2)
            {
    player->CLOSE_GOSSIP_MENU();
    CreatePet(player, m_creature, spellId);
    player->DestroyItemCount(token, cost, true);
            }
            else if (catNumber = 2)
            {
            if (player->HasSpell(spellId))
            {
                m_creature->Whisper("You already know this spell.", LANG_UNIVERSAL, player);
                player->CLOSE_GOSSIP_MENU();
                return;
            } else {
    player->CLOSE_GOSSIP_MENU();
    player->LearnSpell(spellId, false);
    player->DestroyItemCount(token, cost, true);
            }
        }

        } else {
            if (player->GetMoney() < cost)
            {
                m_creature->Whisper("You dont have enough money!", LANG_UNIVERSAL, player);
                player->CLOSE_GOSSIP_MENU();
                return;
            }
        else if (uiAction != 1000 && uiAction != 2000 && uiAction != 3000 && catNumber != 2)
        {
    player->CLOSE_GOSSIP_MENU();
    CreatePet(player, m_creature, spellId);
    player->ModifyMoney(-int(cost));
        }
        else if (catNumber = 2)
        {
            if (player->HasSpell(spellId))
            {
                m_creature->Whisper("You already know this spell.", LANG_UNIVERSAL, player);
                player->CLOSE_GOSSIP_MENU();
                return;
            } else {
    player->CLOSE_GOSSIP_MENU();
    player->LearnSpell(spellId, false);
    player->ModifyMoney(-int(cost));
            }
        }
    }
} while (result->NextRow());
} else {
//player->ADD_GOSSIP_ITEM( 7, MAIN_MENU, GOSSIP_SENDER_MAIN, 5005);
}

 switch(uiAction)
{

case 1000: //Normal Pet
        showNormalPet(player, m_creature, 0);
        player->ADD_GOSSIP_ITEM( 7, "<- Main Menu"                            , GOSSIP_SENDER_MAIN, 5005);

    player->SEND_GOSSIP_MENU(MSG_PET, m_creature->GetGUID());
break;

case 2000: //Exotic Pet
        showExoticPet(player, m_creature, 0);
        player->ADD_GOSSIP_ITEM( 7, "<- Main Menu"                            , GOSSIP_SENDER_MAIN, 5005);

    player->SEND_GOSSIP_MENU(MSG_PET, m_creature->GetGUID());
break;

case 3000: //Pet Spells
        showPetSpells(player, m_creature, 0);
        player->ADD_GOSSIP_ITEM( 7, "<- Main Menu"                            , GOSSIP_SENDER_MAIN, 5005);

    player->SEND_GOSSIP_MENU(MSG_PET, m_creature->GetGUID());
break;

case 5005: //Back To Main Menu
    // Main Menu
    // Check config if "Normal Pet " is enabled or not
    if(EnableNormalPet)
        player->ADD_GOSSIP_ITEM( 7, _StringToUTF8("普通的宠物 ->")              , GOSSIP_SENDER_MAIN, 1000);
    // Check if player can have an exotic pet, and check config if "Exotic" is enabled or not
    if(player->CanTameExoticPets() && EnableExoticPet)
        player->ADD_GOSSIP_ITEM( 7, _StringToUTF8("奇异的宠物 ->")              , GOSSIP_SENDER_MAIN, 2000);
    // Now to add the spells, vendor, and stable stuffs
    player->ADD_GOSSIP_ITEM(4, _StringToUTF8("教授宏物技能 ->")      , GOSSIP_SENDER_MAIN, 3000);
    player->ADD_GOSSIP_ITEM(2, _StringToUTF8("宠物管理"), GOSSIP_SENDER_MAIN, 6006);
    player->ADD_GOSSIP_ITEM(6, _StringToUTF8("宏物食物销售"), GOSSIP_SENDER_MAIN, 6007);
    player->SEND_GOSSIP_MENU(MSG_TYPE, m_creature->GetGUID());
break;

case 6006:
    player->GetSession()->SendStablePet(m_creature->GetGUID());
    player->CLOSE_GOSSIP_MENU();
    break;

case 6007:
    player->GetSession()->SendListInventory(m_creature->GetGUID());
    player->CLOSE_GOSSIP_MENU();
    break;

 player->CLOSE_GOSSIP_MENU();
 }

} //end of function

bool OnGossipSelect(Player* player, Creature* m_creature, uint32 uiSender, uint32 uiAction)
{
    // Main menu
    player->PlayerTalkClass->ClearMenus();
    if (uiSender == GOSSIP_SENDER_MAIN)
    SendDefaultMenu(player, m_creature, uiAction);

return true;
}
};


void AddSC_npc_enchantment()
{
  new npc_enchantment();
   new Npc_Beastmaster();
   new item_enchant_visuals;
}