#ifndef DEF_TRANSMOGRIFICATION_H
#define DEF_TRANSMOGRIFICATION_H

#include <vector>
#include "Define.h"
#include "ObjectGuid.h"

#define PRESETS // comment this line to disable preset feature totally
#define MAX_OPTIONS 25 // do not alter

class Item;
class Player;
class WorldSession;
struct ItemTemplate;

enum TransmogTrinityStrings // Language.h might have same entries, appears when executing SQL, change if needed
{
    LANG_ERR_TRANSMOG_OK = 11100, // change this
    LANG_ERR_TRANSMOG_INVALID_SLOT,
    LANG_ERR_TRANSMOG_INVALID_SRC_ENTRY,
    LANG_ERR_TRANSMOG_MISSING_SRC_ITEM,
    LANG_ERR_TRANSMOG_MISSING_DEST_ITEM,
    LANG_ERR_TRANSMOG_INVALID_ITEMS,
    LANG_ERR_TRANSMOG_NOT_ENOUGH_MONEY,
    LANG_ERR_TRANSMOG_NOT_ENOUGH_TOKENS,

    LANG_ERR_UNTRANSMOG_OK,
    LANG_ERR_UNTRANSMOG_NO_TRANSMOGS,

#ifdef PRESETS
    LANG_PRESET_ERR_INVALID_NAME,
#endif
	LANG_SLOT_NAME_HEAD = 11130,
	LANG_SLOT_NAME_SHOULDERS,
	LANG_SLOT_NAME_BODY,
	LANG_SLOT_NAME_CHEST,
	LANG_SLOT_NAME_WAIST,
	LANG_SLOT_NAME_LEGS,
	LANG_SLOT_NAME_FEET,
	LANG_SLOT_NAME_WRISTS,
	LANG_SLOT_NAME_HANDS,
	LANG_SLOT_NAME_BACK,
	LANG_SLOT_NAME_MAINHAND,
	LANG_SLOT_NAME_OFFHAND,
	LANG_SLOT_NAME_RANGED,
	LANG_SLOT_NAME_TABARD,
	LANG_SLOT_NAME_NECK,
	LANG_SLOT_NAME_FINGER1,
	LANG_SLOT_NAME_FINGER2,
	LANG_SLOT_NAME_TRINKET1,
	LANG_SLOT_NAME_TRINKET2,

	LANG_ITEMMOD_SPIRIT = 11170,
	LANG_ITEMMOD_DODGE,
	LANG_ITEMMOD_PARRY,
	LANG_ITEMMOD_HIT,
	LANG_ITEMMOD_CRIT,
	LANG_ITEMMOD_HASTE,
	LANG_ITEMMOD_EXPERTISE,

	LANG_MENU_ITEM_REFORGEHEAD = 11201,
	LANG_MENU_ITEM_REMOVEREFORGE,
	LANG_MENU_ITEM_UPDATEMENU,
	LANG_MENU_ITEM_DECSTAT,
	LANG_MENU_ITEM_BACK,
	LANG_MENU_ITEM_INVALIDITEM,
	LANG_MENU_ITEM_INCSTAT,
	LANG_MENU_ITEM_SLOT_REMOVEREFORGE,

	LANG_MENU_ITEM_TRANS_HAND = 11221,
	LANG_MENU_ITEM_TRANS_SETS,
	LANG_MENU_ITEM_TRANS_REMOVEALL,
	LANG_MENU_ITEM_TRANS_SETOPT,
	LANG_MENU_ITEM_TRANS_SAVESET,
	LANG_MENU_ITEM_TRANS_REMOVE,
	LANG_MENU_ITEM_TRANS_USESET,
	LANG_MENU_ITEM_TRANS_DELSET,
};

class Transmogrification
{
private:
    Transmogrification() { };
    ~Transmogrification() { };
    Transmogrification(const Transmogrification&);
    Transmogrification& operator=(const Transmogrification&);

public:
    static Transmogrification* instance()
    {
        // Thread safe in C++11 standard
        static Transmogrification instance;
        return &instance;
    }

#ifdef PRESETS

    bool EnableSetInfo;
    uint32 SetNpcText;

    bool EnableSets;
    uint8 MaxSets;
    float SetCostModifier;
    int32 SetCopperCost;

    void LoadPlayerSets(Player* player);

    void PresetTransmog(Player* player, Item* itemTransmogrified, uint32 fakeEntry, uint8 slot);
#endif

    bool EnableTransmogInfo;
    uint32 TransmogNpcText;

    // Use IsAllowed() and IsNotAllowed()
    // these are thread unsafe, but assumed to be static data so it should be safe
    std::set<uint32> Allowed;
    std::set<uint32> NotAllowed;

    float ScaledCostModifier;
    int32 CopperCost;

    bool RequireToken;
    uint32 TokenEntry;
    uint32 TokenAmount;

    bool AllowPoor;
    bool AllowCommon;
    bool AllowUncommon;
    bool AllowRare;
    bool AllowEpic;
    bool AllowLegendary;
    bool AllowArtifact;
    bool AllowHeirloom;

    bool AllowMixedArmorTypes;
    bool AllowMixedWeaponTypes;
    bool AllowFishingPoles;

    bool IgnoreReqRace;
    bool IgnoreReqClass;
    bool IgnoreReqSkill;
    bool IgnoreReqSpell;
    bool IgnoreReqLevel;
    bool IgnoreReqEvent;
    bool IgnoreReqStats;

    bool IsAllowed(uint32 entry) const;
    bool IsNotAllowed(uint32 entry) const;
    bool IsAllowedQuality(uint32 quality) const;
    bool IsRangedWeapon(uint32 Class, uint32 SubClass) const;

    void LoadConfig(bool reload); // thread unsafe

    std::string GetItemIcon(uint32 entry, uint32 width, uint32 height, int x, int y) const;
    std::string GetSlotIcon(uint8 slot, uint32 width, uint32 height, int x, int y) const;
    const char * GetSlotName(uint8 slot, WorldSession* session) const;
    std::string GetItemLink(Item* item, WorldSession* session) const;
    std::string GetItemLink(uint32 entry, WorldSession* session) const;
    uint32 GetFakeEntry(const Item* item);
    void UpdateItem(Player* player, Item* item) const;
    void DeleteFakeEntry(Player* player, Item* item);
    void SetFakeEntry(Player* player, Item* item, uint32 entry);

    TransmogTrinityStrings Transmogrify(Player* player, ObjectGuid itemGUID, uint8 slot, bool no_cost = false);
    bool CanTransmogrifyItemWithItem(Player* player, ItemTemplate const* destination, ItemTemplate const* source) const;
    bool SuitableForTransmogrification(Player* player, ItemTemplate const* proto) const;
    // bool CanBeTransmogrified(Item const* item);
    // bool CanTransmogrify(Item const* item);
    uint32 GetSpecialPrice(ItemTemplate const* proto) const;
    std::vector<ObjectGuid> GetItemList(const Player* player) const;
};
#define sTransmogrification Transmogrification::instance()

#endif
