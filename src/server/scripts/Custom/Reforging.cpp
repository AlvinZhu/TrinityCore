#include "GameEventMgr.h"
#include "Language.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellAuraEffects.h"
#include "SpellScript.h"
#include "Vehicle.h"

enum DataTypes
{
    DATA_WORLDBOSS5             = 1,
    DATA_WORLDBOSS6             = 2,
    DATA_WORLDBOSS7             = 3,
    DATA_WORLDBOSS12            = 4,
    DATA_WORLDBOSS14            = 5,
    DATA_WORLDBOSS15            = 6,
    DATA_WORLDBOSS16            = 7
};

enum Misc
{
    DATA_INTENSE_COLD           = 1,
};

enum Worldboss18Timers
{
    TIMER_FLAME_BREATH     = 0,
    TIMER_DARKNESS         = 1,
    TIMER_ARMAGEDDON       = 2,
    TIMER_BIG_BANG         = 3,
    TIMER_DOOM             = 4,
    TIMER_FIERY_COMBUSTION = 5,
    TIMER_GROWTH           = 6,
    TIMER_ENRAGE           = 7,
    TIMER_CHECK            = 8,
};

enum Spells
{
    SPELL_CLEAVE                                = 19983,
    SPELL_GROWTH                                = 36300,
    SPELL_CAPTURE_SOUL                          = 32966,
    SPELL_DARKNESS_OF_A_THOUSAND_SOULS          = 46605,
    SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE   = 45657,
    SPELL_ANTI_AOE                              = 68595,
    SPELL_PVP_TRINKET                           = 65547,
    SPELL_SUMMON                                = 20477,
    SPELL_ENRAGE                                = 46587,
    SPELL_ENRAGE_LARGE                          = 47008,
    SPELL_SHADOW_PRISON                         = 40647,
    SPELL_FURY_OF_FROSTMOURNE                   = 72350,
    SPELL_BIG_BANG                              = 64584,
    SPELL_IMMUNE_TREAT                          = 90041,
    SPELL_STEALTH                               = 30991,
    SPELL_DISAPPEAR                             = 26889,
    SPELL_CHANNEL_SPIRIT_HEAL                   = 90121,

    //Worldboss1
    SPELL_SHADOW_VOLLEY                         = 32963,
    SPELL_THUNDERCLAP                           = 36706,
    SPELL_SHADOW_TRAP                           = 73539,
    SPELL_ARCANE_ORB                            = 34172,

    //Worldboss2
    SPELL_FROST_AURA                            = 28531,
    SPELL_FROSTBOLT_VOLLEY                      = 72905,
    SPELL_TAIL_SWEEP                            = 55697,
    SPELL_FROSTBOLT                             = 71420,
    SPELL_MANA_DETONATION                       = 27819,
    SPELL_ARCTIC_BREATH                         = 66689,

    //Worldboss3
    SPELL_WAR_STOMP                             = 33707,
    SPELL_INCITE_CHAOS                          = 33676,
    SPELL_INCITE_CHAOS_B                        = 33684,
    SPELL_WHIRLWIND                             = 33238,
    SPELL_LIGHT_TWIN_SPIKE                      = 66075,
    SPELL_LIGHT_SHIELD                          = 65858,
    SPELL_LIGHT_TWIN_PACT                       = 65876,
    SPELL_LIGHT_VORTEX                          = 66046,
    SPELL_LIGHT_VORTEX_DAMAGE                   = 66048,

    //Worldboss4
    SPELL_FIRE_BLOOM                            = 45641,
    SPELL_FLAME_BREATH                          = 18435,
    SPELL_WING_BUFFET                           = 18500,
    SPELL_FLAMEBUFFET                           = 23341,
    SPELL_FLAME_STRIKE                          = 36735,

    //Worldboss5
    SPELL_BLADESTORM                            = 65947,
    SPELL_INTIMIDATING_SHOUT                    = 65930,
    SPELL_MORTAL_STRIKE                         = 65926,
    SPELL_CHARGE                                = 68764,
    SPELL_DISARM                                = 65935,
    SPELL_OVERPOWER                             = 65924,
    SPELL_SUNDER_ARMOR                          = 65936,
    SPELL_SHATTERING_THROW                      = 65940,
    SPELL_RETALIATION                           = 65932,

    //Worldboss6
    SPELL_CYCLONE                               = 65859,
    SPELL_ENTANGLING_ROOTS                      = 65857,
    SPELL_FAERIE_FIRE                           = 65863,
    SPELL_FORCE_OF_NATURE                       = 65861,
    SPELL_INSECT_SWARM                          = 65855,
    SPELL_MOONFIRE                              = 65856,
    SPELL_STARFIRE                              = 65854,
    SPELL_WRATH                                 = 65862,
    SPELL_LIFEBLOOM                             = 66093,
    SPELL_NOURISH                               = 66066,
    SPELL_REGROWTH                              = 66067,
    SPELL_REJUVENATION                          = 66065,
    SPELL_TRANQUILITY                           = 66086,
    SPELL_BARKSKIN                              = 65860,
    SPELL_THORNS                                = 66068,
    SPELL_NATURE_GRASP                          = 66071,

    //Worldboss7
    SPELL_FAN_OF_KNIVES                         = 65955,
    SPELL_BLIND                                 = 65960,
    SPELL_CLOAK                                 = 65961,
    SPELL_BLADE_FLURRY                          = 65956,
    SPELL_SHADOWSTEP                            = 66178,
    SPELL_HEMORRHAGE                            = 65954,
    SPELL_EVISCERATE                            = 65957,
    SPELL_WOUND_POISON                          = 65962,

    //Worldboss8
    SPELL_AVENGING_WRATH                        = 66011,
    SPELL_CRUSADER_STRIKE                       = 66003,
    SPELL_DIVINE_STORM                          = 66006,
    SPELL_HAMMER_OF_JUSTICE_RET                 = 66007,
    SPELL_JUDGEMENT_OF_COMMAND                  = 66005,
    SPELL_REPENTANCE                            = 66008,
    SPELL_SEAL_OF_COMMAND                       = 66004,
    SPELL_HAND_OF_FREEDOM                       = 68757,
    SPELL_DIVINE_SHIELD                         = 66010,
    SPELL_CLEANSE                               = 66116,
    SPELL_FLASH_OF_LIGHT                        = 66113,
    SPELL_HOLY_LIGHT                            = 66112,
    SPELL_HOLY_SHOCK                            = 66114,
    SPELL_HAND_OF_PROTECTION                    = 66009,
    SPELL_HAMMER_OF_JUSTICE                     = 66613,
    SPELL_FORBEARANCE                           = 25771,

    //Worldboss9
    SPELL_LEGION_FLAME                          = 66197,
    SPELL_LEGION_FLAME_EFFECT                   = 66201,
    SPELL_NETHER_POWER                          = 66228,
    SPELL_FEL_LIGHTING                          = 66528,
    SPELL_FEL_FIREBALL                          = 66532,
    SPELL_INCINERATE_FLESH                      = 66237,
    SPELL_BURNING_INFERNO                       = 66242,
    SPELL_INFERNAL_ERUPTION                     = 66258,
    SPELL_INFERNAL_ERUPTION_EFFECT              = 66252,
    SPELL_NETHER_PORTAL                         = 66269,
    SPELL_NETHER_PORTAL_EFFECT                  = 66263,
    SPELL_LORD_HITTIN                           = 66326,
    SPELL_HELLFIRE                              = 65816,
    SPELL_CORRUPTION                            = 65810,
    SPELL_CURSE_OF_AGONY                        = 65814,
    SPELL_CURSE_OF_EXHAUSTION                   = 65815,
    SPELL_FEAR                                  = 65809,
    SPELL_SEARING_PAIN                          = 65819,
    SPELL_SHADOW_BOLT                           = 65821,
    SPELL_UNSTABLE_AFFLICTION                   = 65812,
    SPELL_UNSTABLE_AFFLICTION_DISPEL            = 65813,
    SPELL_SUMMON_FELHUNTER                      = 67514,

    //Worldboss10
    SPELL_ARCANE_BARRAGE                        = 65799,
    SPELL_ARCANE_BLAST                          = 65791,
    SPELL_ARCANE_EXPLOSION                      = 65800,
    SPELL_BLINK                                 = 65793,
    SPELL_COUNTERSPELL                          = 65790,
    SPELL_FROST_NOVA                            = 65792,
    SPELL_FROSTBOLT_WORLDBOSS10                 = 65807,
    SPELL_ICE_BLOCK                             = 65802,
    SPELL_POLYMORPH                             = 65801,
    SPELL_SILENCE                               = 65542,
    SPELL_VAMPIRIC_TOUCH                        = 65490,
    SPELL_SW_PAIN                               = 65541,
    SPELL_MIND_FLAY                             = 65488,
    SPELL_MIND_BLAST                            = 65492,
    SPELL_HORROR                                = 65545,
    SPELL_DISPERSION                            = 65544,
    SPELL_SHADOWFORM                            = 22917,
    SPELL_RENEW                                 = 66177,
    SPELL_SHIELD                                = 66099,
    SPELL_FLASH_HEAL                            = 66104,
    SPELL_DISPEL                                = 65546,
    SPELL_PSYCHIC_SCREAM                        = 65543,
    SPELL_MANA_BURN                             = 66100,
    SPELL_PENANCE                               = 66097,
    SPELL_FREEZING_GROUND                       = 72090,
    SPELL_FROZEN_ORB                            = 72091,
    SPELL_WHITEOUT                              = 72034,
    SPELL_FROZEN_MALLET                         = 71993,
    SPELL_SUMMON_BLIZZARD                       = 28560,
    SPELL_SEDUCTION                             = 29490,

    //Worldboss11
    SPELL_FIRE_BOMB_CHANNEL                     = 42621,
    SPELL_FIRE_BOMB_THROW                       = 42628,
    SPELL_FIRE_BOMB_DUMMY                       = 42629,
    SPELL_FIRE_BOMB_DAMAGE                      = 42630,
    SPELL_FLAME_WREATH                          = 29946,
    SPELL_BURNING_BREATH                        = 66665,
    SPELL_BURNING_FURY                          = 66721,
    SPELL_FLAME_CINDER_A                        = 66684,
    SPELL_FLAME_CINDER_B                        = 66681,
    SPELL_METEOR_FISTS                          = 66725,
    SPELL_METEOR_FISTS_DAMAGE                   = 66765,

    //Worldboss12
    SPELL_HEALING_WAVE                          = 66055,
    SPELL_RIPTIDE                               = 66053,
    SPELL_SPIRIT_CLEANSE                        = 66056,
    SPELL_HEROISM                               = 65983,
    SPELL_BLOODLUST                             = 65980,
    SPELL_HEX                                   = 66054,
    SPELL_EARTH_SHIELD                          = 66063,
    SPELL_EARTH_SHOCK                           = 65973,
    AURA_EXHAUSTION                             = 57723,
    AURA_SATED                                  = 57724,
    SPELL_LAVA_LASH                             = 65974,
    SPELL_STORMSTRIKE                           = 65970,
    SPELL_WINDFURY                              = 65976,

    //Worldboss13
    SPELL_CHAINS_OF_ICE                         = 66020,
    SPELL_DEATH_COIL                            = 66019,
    SPELL_DEATH_GRIP                            = 66017,
    SPELL_FROST_STRIKE                          = 66047,
    SPELL_ICEBOUND_FORTITUDE                    = 66023,
    SPELL_ICY_TOUCH                             = 66021,
    SPELL_STRANGULATE                           = 66018,
    SPELL_DEATH_GRIP_PULL                       = 64431,
    SPELL_AIMED_SHOT                            = 65883,
    SPELL_DETERRENCE                            = 65871,
    SPELL_DISENGAGE                             = 65869,
    SPELL_EXPLOSIVE_SHOT                        = 65866,
    SPELL_FROST_TRAP                            = 65880,
    SPELL_SHOOT                                 = 65868,
    SPELL_STEADY_SHOT                           = 65867,
    SPELL_WING_CLIP                             = 66207,
    SPELL_WYVERN_STING                          = 65877,
    SPELL_CALL_PET                              = 67777,

    //Worldboss14
    SPELL_FIREBOLT_VOLLEY                       = 43240,
    SPELL_PYROGENICS                            = 45230,
    SPELL_FLAME_TOUCHED                         = 45348,
    SPELL_CONFLAGRATION                         = 45342,
    SPELL_BLAZE                                 = 45235,
    SPELL_FLAME_SEAR                            = 46771,
    SPELL_BLAZE_SUMMON                          = 45236,
    SPELL_BLAZE_BURN                            = 45246,

    //Worldboss15
    SPELL_DARK_TOUCHED                          = 45347,
    SPELL_SHADOW_BLADES                         = 45248,
    SPELL_DARK_STRIKE                           = 45271,
    SPELL_SHADOW_NOVA                           = 45329,
    SPELL_CONFOUNDING_BLOW                      = 45256,
    SPELL_SHADOW_FURY                           = 45270,
    SPELL_IMAGE_VISUAL                          = 45263,
    SPELL_EMPOWER                               = 45366,
    SPELL_DARK_FLAME                            = 45345,

    //Worldboss16
    SPELL_FROZEN_PRISON                         = 47854,
    SPELL_TAIL_SWEEP_WORLDBOSS16                = 50155,
    SPELL_CRYSTAL_CHAINS                        = 50997,
    SPELL_CRYSTALFIRE_BREATH                    = 48096,
    H_SPELL_CRYSTALFIRE_BREATH                  = 57091,
    SPELL_CRYSTALIZE                            = 48179,
    SPELL_INTENSE_COLD                          = 48094,
    SPELL_INTENSE_COLD_TRIGGERED                = 48095,

    //Worldboss17
    SPELL_STATIC_DISRUPTION                     = 43622,
    SPELL_CALL_LIGHTNING                        = 43661,
    SPELL_GUST_OF_WIND                          = 43621,
    SPELL_ELECTRICAL_STORM                      = 43648,
    SPELL_ZAP                                   = 43137,
    SPELL_SAND_STORM                            = 25160,
    SPELL_SHOCK_BLAST                           = 38509,
    SPELL_STATIC_CHARGE_TRIGGER                 = 38280,
    SPELL_FORKED_LIGHTNING                      = 40088,
    SPELL_BALL_LIGHTNING                        = 52780,
    SPELL_STATIC_OVERLOAD                       = 52658,

    //Worldboss18
    SPELL_DOOM                                  = 31347,
    SPELL_ENFEEBLE                              = 30843,
    SPELL_FIERY_COMBUSTION                      = 74562,

    //Worldboss18_3
    SPELL_CHAIN_LIGHTNING                       = 16033,
    SPELL_SHOCK                                 = 16034,

    //Worldboss18_4
    SPELL_BLACK_ARROW                           = 59712,
    SPELL_FADE                                  = 20672,
    SPELL_FADE_BLINK                            = 29211,
    SPELL_MULTI_SHOT                            = 59713,
    SPELL_SHOT                                  = 59710,
    SPELL_SUMMON_SKELETON                       = 59711,

    //Allianceguard1
    SPELL_ALLIANCE_GUARD1_1                     = 60964,
    SPELL_ALLIANCE_GUARD1_2                     = 53642,

    //Allianceguard2
    SPELL_ALLIANCE_GUARD2_1                     = 61011,
    SPELL_ALLIANCE_GUARD2_2                     = 60509,

    //Hordeguard1
    SPELL_HORDE_GUARD1_1                        = 64670,
    SPELL_HORDE_GUARD1_2                        = 53642,

    //Hordeguard2
    SPELL_HORDE_GUARD2_1                        = 59756,
    SPELL_HORDE_GUARD2_2                        = 61212,
};

enum Events
{
    EVENT_1                         = 1,
    EVENT_2                         = 2,
    EVENT_3                         = 3,
    EVENT_4                         = 4,
    EVENT_5                         = 5,
    EVENT_6                         = 6,
    EVENT_7                         = 7,
    EVENT_8                         = 8,
    EVENT_9                         = 9,

    //Worldboss5
    EVENT_BLADESTORM                = 1,
    EVENT_INTIMIDATING_SHOUT        = 2,
    EVENT_MORTAL_STRIKE             = 3,
    EVENT_WARR_CHARGE               = 4,
    EVENT_DISARM                    = 5,
    EVENT_OVERPOWER                 = 6,
    EVENT_SUNDER_ARMOR              = 7,
    EVENT_SHATTERING_THROW          = 8,
    EVENT_RETALIATION               = 9,
    EVENT_THREAT_WARRIOR            = 10,
    EVENT_GROWTH_WARRIOR            = 11,
    EVENT_CAPTURE_SOUL_WARRIOR      = 12,

    //Worldboss6
    EVENT_CYCLONE                   = 1,
    EVENT_ENTANGLING_ROOTS          = 2,
    EVENT_FAERIE_FIRE               = 3,
    EVENT_FORCE_OF_NATURE           = 4,
    EVENT_INSECT_SWARM              = 5,
    EVENT_MOONFIRE                  = 6,
    EVENT_STARFIRE                  = 7,
    EVENT_LIFEBLOOM                 = 8,
    EVENT_NOURISH                   = 9,
    EVENT_REGROWTH                  = 10,
    EVENT_REJUVENATION              = 11,
    EVENT_TRANQUILITY               = 12,
    EVENT_BARKSKIN                  = 13,
    EVENT_THORNS                    = 14,
    EVENT_NATURE_GRASP              = 15,
    EVENT_THREAT_DRUID              = 16,
    EVENT_GROWTH_DRUID              = 17,
    EVENT_CAPTURE_SOUL_DRUID        = 18,

    //Worldboss7
    EVENT_FAN_OF_KNIVES             = 1,
    EVENT_BLIND                     = 2,
    EVENT_CLOAK                     = 3,
    EVENT_BLADE_FLURRY              = 4,
    EVENT_SHADOWSTEP                = 5,
    EVENT_HEMORRHAGE                = 6,
    EVENT_EVISCERATE                = 7,
    EVENT_WOUND_POISON              = 8,
    EVENT_THREAT_ROGUE              = 9,
    EVENT_GROWTH_ROGUE              = 10,
    EVENT_CAPTURE_SOUL_ROGUE        = 11,

    //Worldboss8
    EVENT_AVENGING_WRATH            = 1,
    EVENT_CRUSADER_STRIKE           = 2,
    EVENT_DIVINE_STORM              = 3,
    EVENT_HAMMER_OF_JUSTICE_RET     = 4,
    EVENT_JUDGEMENT_OF_COMMAND      = 5,
    EVENT_REPENTANCE                = 6,
    EVENT_DPS_HAND_OF_PROTECTION    = 7,
    EVENT_DPS_DIVINE_SHIELD         = 8,
    EVENT_HAND_OF_FREEDOM           = 9,
    EVENT_HEAL_DIVINE_SHIELD        = 10,
    EVENT_CLEANSE                   = 11,
    EVENT_FLASH_OF_LIGHT            = 12,
    EVENT_HOLY_LIGHT                = 13,
    EVENT_HOLY_SHOCK                = 14,
    EVENT_HEAL_HAND_OF_PROTECTION   = 15,
    EVENT_HAMMER_OF_JUSTICE         = 16,
    EVENT_THREAT_PALADIN            = 17,
    EVENT_GROWTH_PALADIN            = 18,

    //Worldboss9
    EVENT_FEL_FIREBALL              = 1,
    EVENT_FEL_LIGHTNING             = 2,
    EVENT_INCINERATE_FLESH          = 3,
    EVENT_NETHER_POWER              = 4,
    EVENT_LEGION_FLAME              = 5,
    EVENT_SUMMONO_NETHER_PORTAL     = 6,
    EVENT_SUMMON_INFERNAL_ERUPTION  = 7,
    EVENT_HELLFIRE                  = 8,
    EVENT_CORRUPTION                = 9,
    EVENT_CURSE_OF_AGONY            = 10,
    EVENT_CURSE_OF_EXHAUSTION       = 11,
    EVENT_FEAR                      = 12,
    EVENT_SEARING_PAIN              = 13,
    EVENT_UNSTABLE_AFFLICTION       = 14,
    EVENT_THREAT_WORLDBOSS9         = 15,
    EVENT_GROWTH_WORLDBOSS9         = 16,
    EVENT_CAPTURE_SOUL_WORLDBOSS9   = 17,

    //Worldboss10
    EVENT_ARCANE_BARRAGE            = 1,
    EVENT_ARCANE_BLAST              = 2,
    EVENT_ARCANE_EXPLOSION          = 3,
    EVENT_BLINK                     = 4,
    EVENT_COUNTERSPELL              = 5,
    EVENT_FROST_NOVA                = 6,
    EVENT_ICE_BLOCK                 = 7,
    EVENT_POLYMORPH                 = 8,
    EVENT_SILENCE                   = 9,
    EVENT_VAMPIRIC_TOUCH            = 10,
    EVENT_SW_PAIN                   = 11,
    EVENT_MIND_BLAST                = 12,
    EVENT_HORROR                    = 13,
    EVENT_DISPERSION                = 14,
    EVENT_DPS_DISPEL                = 15,
    EVENT_DPS_PSYCHIC_SCREAM        = 16,
    EVENT_RENEW                     = 17,
    EVENT_SHIELD                    = 18,
    EVENT_FLASH_HEAL                = 19,
    EVENT_HEAL_DISPEL               = 20,
    EVENT_HEAL_PSYCHIC_SCREAM       = 21,
    EVENT_MANA_BURN                 = 22,
    EVENT_PENANCE                   = 23,
    EVENT_FREEZING_GROUND           = 24,
    EVENT_FROZEN_ORB                = 25,
    EVENT_WHITEOUT                  = 26,
    EVENT_BLIZZARD                  = 27,
    EVENT_SEDUCTION                 = 28,
    EVENT_THREAT_WORLDBOSS10        = 29,
    EVENT_GROWTH_WORLDBOSS10        = 30,
    EVENT_CAPTURE_SOUL_WORLDBOSS10  = 31,

    //Worldboss11
    EVENT_FLAME_WREATH              = 1,
    EVENT_FLAME_BREATH              = 2,
    EVENT_FLAMEBUFFET               = 3,
    EVENT_BURNING_BREATH            = 4,
    EVENT_BURNING_FURY              = 5,
    EVENT_FLAME_CINDER              = 6,
    EVENT_METEOR_FISTS              = 7,
    EVENT_THREAT_WORLDBOSS11        = 8,
    EVENT_GROWTH_WORLDBOSS11        = 9,
    EVENT_CAPTURE_SOUL_WORLDBOSS11  = 10,

    //Worldboss12
    EVENT_HEALING_WAVE              = 1,
    EVENT_RIPTIDE                   = 2,
    EVENT_SPIRIT_CLEANSE            = 3,
    EVENT_BLOODLUST_HEROISM         = 4,
    EVENT_HEX                       = 5,
    EVENT_EARTH_SHIELD              = 6,
    EVENT_EARTH_SHOCK               = 7,
    EVENT_LAVA_LASH                 = 8,
    EVENT_STORMSTRIKE               = 9,
    EVENT_DEPLOY_TOTEM              = 10,
    EVENT_WINDFURY                  = 11,
    EVENT_THREAT_SHAMAN             = 12,
    EVENT_GROWTH_SHAMAN             = 13,
    EVENT_CAPTURE_SOUL_SHAMAN       = 14,

    //Worldboss13
    EVENT_CHAINS_OF_ICE             = 1,
    EVENT_DEATH_COIL                = 2,
    EVENT_DEATH_GRIP                = 3,
    EVENT_FROST_STRIKE              = 4,
    EVENT_ICEBOUND_FORTITUDE        = 5,
    EVENT_ICY_TOUCH                 = 6,
    EVENT_STRANGULATE               = 7,
    EVENT_AIMED_SHOT                = 8,
    EVENT_DETERRENCE                = 9,
    EVENT_DISENGAGE                 = 10,
    EVENT_EXPLOSIVE_SHOT            = 11,
    EVENT_FROST_TRAP                = 12,
    EVENT_STEADY_SHOT               = 13,
    EVENT_WING_CLIP                 = 14,
    EVENT_WYVERN_STING              = 15,
    EVENT_THREAT_DEATHKNIGHT        = 16,
    EVENT_GROWTH_DEATHKNIGHT        = 17,

    //Worldboss14
    EVENT_THREAT_WORLDBOSS14        = 1,
    EVENT_GROWTH_WORLDBOSS14        = 2,
    EVENT_CAPTURE_SOUL_WORLDBOSS14  = 3,

    //Worldboss15
    EVENT_THREAT_WORLDBOSS15        = 1,
    EVENT_GROWTH_WORLDBOSS15        = 2,
    EVENT_CAPTURE_SOUL_WORLDBOSS15  = 3,

    //Worldboss16
    EVENT_THREAT_WORLDBOSS16        = 1,
    EVENT_GROWTH_WORLDBOSS16        = 2,
    EVENT_CAPTURE_SOUL_WORLDBOSS16  = 3,

    //Worldboss17
    EVENT_STATIC_DISRUPTION         = 1,
    EVENT_GUST_OF_WIND              = 2,
    EVENT_CALL_LIGHTNING            = 3,
    EVENT_ELECTRICAL_STORM          = 4,
    EVENT_RAIN                      = 5,
    EVENT_STORM_SEQUENCE            = 6,
    EVENT_LAVA_LASH_WORLDBOSS17     = 7,
    EVENT_STORMSTRIKE_WORLDBOSS17   = 8,
    EVENT_WINDFURY_WORLDBOSS17      = 9,
    EVENT_SHOCK_BLAST               = 10,
    EVENT_STATIC_CHARGE_TRIGGER     = 11,
    EVENT_FORKED_LIGHTNING          = 12,
    EVENT_BALL_LIGHTNING            = 13,
    EVENT_STATIC_OVERLOAD           = 14,
    EVENT_GROWTH_WORLDBOSS17        = 15,
    EVENT_CAPTURE_SOUL_WORLDBOSS17  = 16,

    //Guard
    EVENT_GUARD_1                   = 1,
    EVENT_GUARD_2                   = 2,

    //Mine
    EVENT_PROXIMITY_MINE_ARM        = 1,
    EVENT_PROXIMITY_MINE_DETONATION = 2,
};




/*
Reforging by Rochet2
https://rochet2.github.io/?page=Transmogrification

Rules of thumb:
Item can be reforged once.
Item reforge wont show to anyone but you in tooltips. Stats will be there nevertheless.
You will see the increased stats on all tooltips of the same item you reforged.
You can disable the stat changes to tooltips by setting send_cache_packets to false.
Reforges are stripped when you mail, ah, guildbank the item etc. Only YOU can have the reforge.
Only item base stats are reforgable. Enchants and random stats are not.

This script is made blizzlike. This means that the reforgable stats etc are from CATACLYSM!
I have been informed that some stats were removed etc that would be important to be reforgable.
However I do not know what those stats are currently. Do look through the statTypes to add whatever you want.
Edit IsReforgable is you want to tweak requirements

*/

static const bool send_cache_packets = true;    // change player cache?

// Remember to add to GetStatName too
static const ItemModType statTypes[] = { ITEM_MOD_SPIRIT, ITEM_MOD_DODGE_RATING, ITEM_MOD_PARRY_RATING, ITEM_MOD_HIT_RATING, ITEM_MOD_CRIT_RATING, ITEM_MOD_HASTE_RATING, ITEM_MOD_EXPERTISE_RATING };
static const uint8 stat_type_max = sizeof(statTypes) / sizeof(*statTypes);

static const char* GetStatName(uint32 ItemStatType, WorldSession* session)
{
	switch (ItemStatType)
	{
	case ITEM_MOD_SPIRIT: return session->GetTrinityString(TransmogTrinityStrings::LANG_ITEMMOD_SPIRIT); break;
	case ITEM_MOD_DODGE_RATING: return session->GetTrinityString(TransmogTrinityStrings::LANG_ITEMMOD_DODGE); break;
	case ITEM_MOD_PARRY_RATING: return session->GetTrinityString(TransmogTrinityStrings::LANG_ITEMMOD_PARRY); break;
	case ITEM_MOD_HIT_RATING: return session->GetTrinityString(TransmogTrinityStrings::LANG_ITEMMOD_HIT); break;
	case ITEM_MOD_CRIT_RATING: return session->GetTrinityString(TransmogTrinityStrings::LANG_ITEMMOD_CRIT); break;
	case ITEM_MOD_HASTE_RATING: return session->GetTrinityString(TransmogTrinityStrings::LANG_ITEMMOD_HASTE); break;
	case ITEM_MOD_EXPERTISE_RATING: return session->GetTrinityString(TransmogTrinityStrings::LANG_ITEMMOD_EXPERTISE); break;
	default: return NULL;
	}
	//switch (ItemStatType)
 //   {
 //   case ITEM_MOD_SPIRIT                   	: return "Spirit"; break;
 //   case ITEM_MOD_DODGE_RATING             	: return "Dodge rating"; break;
 //   case ITEM_MOD_PARRY_RATING             	: return "Parry rating"; break;
 //   case ITEM_MOD_HIT_RATING               	: return "Hit rating"; break;
 //   case ITEM_MOD_CRIT_RATING              	: return "Crit rating"; break;
 //   case ITEM_MOD_HASTE_RATING             	: return "Haste rating"; break;
 //   case ITEM_MOD_EXPERTISE_RATING         	: return "Expertise rating"; break;
 //   default: return NULL;
 //   }
}

static const char* GetSlotName(uint8 slot, WorldSession* session)
{
	switch (slot)
	{
	case EQUIPMENT_SLOT_HEAD: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_HEAD);
	case EQUIPMENT_SLOT_SHOULDERS: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_SHOULDERS);
	case EQUIPMENT_SLOT_BODY: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_BODY);
	case EQUIPMENT_SLOT_CHEST: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_CHEST);
	case EQUIPMENT_SLOT_WAIST: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_WAIST);
	case EQUIPMENT_SLOT_LEGS: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_LEGS);
	case EQUIPMENT_SLOT_FEET: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_FEET);
	case EQUIPMENT_SLOT_WRISTS: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_WRISTS);
	case EQUIPMENT_SLOT_HANDS: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_HANDS);
	case EQUIPMENT_SLOT_BACK: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_BACK);
	case EQUIPMENT_SLOT_MAINHAND: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_MAINHAND);
	case EQUIPMENT_SLOT_OFFHAND: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_OFFHAND);
	case EQUIPMENT_SLOT_RANGED: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_RANGED);
	case EQUIPMENT_SLOT_TABARD: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_TABARD);
	case EQUIPMENT_SLOT_NECK: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_NECK);
	case EQUIPMENT_SLOT_FINGER1: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_FINGER1);
	case EQUIPMENT_SLOT_FINGER2: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_FINGER2);
	case EQUIPMENT_SLOT_TRINKET1: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_TRINKET1);
	case EQUIPMENT_SLOT_TRINKET2: return session->GetTrinityString(TransmogTrinityStrings::LANG_SLOT_NAME_TRINKET2);
	default: return NULL;
	}
	//switch (slot)
 //   {
 //   case EQUIPMENT_SLOT_HEAD      : return "Head";
 //   case EQUIPMENT_SLOT_NECK      : return "Neck";
 //   case EQUIPMENT_SLOT_SHOULDERS : return "Shoulders";
 //   case EQUIPMENT_SLOT_BODY      : return "Shirt";
 //   case EQUIPMENT_SLOT_CHEST     : return "Chest";
 //   case EQUIPMENT_SLOT_WAIST     : return "Waist";
 //   case EQUIPMENT_SLOT_LEGS      : return "Legs";
 //   case EQUIPMENT_SLOT_FEET      : return "Feet";
 //   case EQUIPMENT_SLOT_WRISTS    : return "Wrists";
 //   case EQUIPMENT_SLOT_HANDS     : return "Hands";
 //   case EQUIPMENT_SLOT_FINGER1   : return "Right finger";
 //   case EQUIPMENT_SLOT_FINGER2   : return "Left finger";
 //   case EQUIPMENT_SLOT_TRINKET1  : return "Right trinket";
 //   case EQUIPMENT_SLOT_TRINKET2  : return "Left trinket";
 //   case EQUIPMENT_SLOT_BACK      : return "Back";
 //   case EQUIPMENT_SLOT_MAINHAND  : return "Main hand";
 //   case EQUIPMENT_SLOT_OFFHAND   : return "Off hand";
 //   case EQUIPMENT_SLOT_TABARD    : return "Tabard";
 //   case EQUIPMENT_SLOT_RANGED    : return "Ranged";
 //   default: return NULL;
 //   }
}

static uint32 Melt(uint8 i, uint8 j)
{
    return (i << 8) + j;
}

static void Unmelt(uint32 melt, uint8& i, uint8& j)
{
    i = melt >> 8;
    j = melt & 0xFF;
}

static std::vector<Item*> GetItemList(const Player* player)
{
    std::vector<Item*> itemlist;

    // Copy paste from Player::GetItemByGuid(guid)

    for (uint8 i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
        if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            itemlist.push_back(pItem);

    for (uint8 i = KEYRING_SLOT_START; i < CURRENCYTOKEN_SLOT_END; ++i)
        if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            itemlist.push_back(pItem);

    for (int i = BANK_SLOT_ITEM_START; i < BANK_SLOT_BAG_END; ++i)
        if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            itemlist.push_back(pItem);

    for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
        if (Bag* pBag = player->GetBagByPos(i))
            for (uint32 j = 0; j < pBag->GetBagSize(); ++j)
                if (Item* pItem = pBag->GetItemByPos(j))
                    itemlist.push_back(pItem);

    for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
        if (Bag* pBag = player->GetBagByPos(i))
            for (uint32 j = 0; j < pBag->GetBagSize(); ++j)
                if (Item* pItem = pBag->GetItemByPos(j))
                    itemlist.push_back(pItem);

    return itemlist;
}

static Item* GetEquippedItem(Player* player, uint32 guidlow)
{
    for (uint8 i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
        if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            if (pItem->GetGUID().GetCounter() == guidlow)
                return pItem;
    return NULL;
}

void RemoveReforge(Player* player, uint32 itemguid, bool update);

// Supply lowguid or reforge! (or both)
// Warning, this function may modify player->reforgeMap when lowguid is supplied
static void SendReforgePacket(Player* player, uint32 entry, uint32 lowguid = 0, const ReforgeData* reforge = NULL)
{
    ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(entry);
    if (!pProto)
        return;

    if (lowguid)
    {
        if (!player->reforgeMap.empty() && player->reforgeMap.find(lowguid) != player->reforgeMap.end())
            reforge = &player->reforgeMap[lowguid];
        else
            RemoveReforge(player, lowguid, true);
    }

    // Update player cache (self only) pure visual.
    // HandleItemQuerySingleOpcode copy paste
    std::string Name        = pProto->Name1;
    std::string Description = pProto->Description;
    int loc_idx = player->GetSession()->GetSessionDbLocaleIndex();
    if (loc_idx >= 0)
    {
        if (ItemLocale const* il = sObjectMgr->GetItemLocale(pProto->ItemId))
        {
            ObjectMgr::GetLocaleString(il->Name, loc_idx, Name);
            ObjectMgr::GetLocaleString(il->Description, loc_idx, Description);
        }
    }
    WorldPacket data(SMSG_ITEM_QUERY_SINGLE_RESPONSE, 600);
    data << pProto->ItemId;
    data << pProto->Class;
    data << pProto->SubClass;
    data << pProto->SoundOverrideSubclass;
    data << Name;
    data << uint8(0x00);                                //pProto->Name2; // blizz not send name there, just uint8(0x00); <-- \0 = empty string = empty name...
    data << uint8(0x00);                                //pProto->Name3; // blizz not send name there, just uint8(0x00);
    data << uint8(0x00);                                //pProto->Name4; // blizz not send name there, just uint8(0x00);
    data << pProto->DisplayInfoID;
    data << pProto->Quality;
    data << pProto->Flags;
    data << pProto->Flags2;
    data << pProto->BuyPrice;
    data << pProto->SellPrice;
    data << pProto->InventoryType;
    data << pProto->AllowableClass;
    data << pProto->AllowableRace;
    data << pProto->ItemLevel;
    data << pProto->RequiredLevel;
    data << pProto->RequiredSkill;
    data << pProto->RequiredSkillRank;
    data << pProto->RequiredSpell;
    data << pProto->RequiredHonorRank;
    data << pProto->RequiredCityRank;
    data << pProto->RequiredReputationFaction;
    data << pProto->RequiredReputationRank;
    data << int32(pProto->MaxCount);
    data << int32(pProto->Stackable);
    data << pProto->ContainerSlots;
    data << pProto->StatsCount + (reforge ? 1 : 0); // increase stat count by 1
    bool decreased = false;
    for (uint32 i = 0; i < pProto->StatsCount; ++i)
    {
        data << pProto->ItemStat[i].ItemStatType;
        if (reforge && !decreased && pProto->ItemStat[i].ItemStatType == reforge->decrease)
        {
            data << pProto->ItemStat[i].ItemStatValue-reforge->stat_value;
            decreased = true;
        }
        else
            data << pProto->ItemStat[i].ItemStatValue;
    }
    if (reforge)
    {
        data << reforge->increase;      // set new stat type
        data << reforge->stat_value;    // and value
    }
    data << pProto->ScalingStatDistribution;            // scaling stats distribution
    data << pProto->ScalingStatValue;                   // some kind of flags used to determine stat values column
    for (int i = 0; i < MAX_ITEM_PROTO_DAMAGES; ++i)
    {
        data << pProto->Damage[i].DamageMin;
        data << pProto->Damage[i].DamageMax;
        data << pProto->Damage[i].DamageType;
    }

    // resistances (7)
    data << pProto->Armor;
    data << pProto->HolyRes;
    data << pProto->FireRes;
    data << pProto->NatureRes;
    data << pProto->FrostRes;
    data << pProto->ShadowRes;
    data << pProto->ArcaneRes;

    data << pProto->Delay;
    data << pProto->AmmoType;
    data << pProto->RangedModRange;

    for (int s = 0; s < MAX_ITEM_PROTO_SPELLS; ++s)
    {
        // send DBC data for cooldowns in same way as it used in Spell::SendSpellCooldown
        // use `item_template` or if not set then only use spell cooldowns
        SpellInfo const* spell = sSpellMgr->GetSpellInfo(pProto->Spells[s].SpellId);
        if (spell)
        {
            bool db_data = pProto->Spells[s].SpellCooldown >= 0 || pProto->Spells[s].SpellCategoryCooldown >= 0;

            data << pProto->Spells[s].SpellId;
            data << pProto->Spells[s].SpellTrigger;
            data << uint32(-abs(pProto->Spells[s].SpellCharges));

            if (db_data)
            {
                data << uint32(pProto->Spells[s].SpellCooldown);
                data << uint32(pProto->Spells[s].SpellCategory);
                data << uint32(pProto->Spells[s].SpellCategoryCooldown);
            }
            else
            {
                data << uint32(spell->RecoveryTime);
                data << uint32(spell->GetCategory());
                data << uint32(spell->CategoryRecoveryTime);
            }
        }
        else
        {
            data << uint32(0);
            data << uint32(0);
            data << uint32(0);
            data << uint32(-1);
            data << uint32(0);
            data << uint32(-1);
        }
    }
    data << pProto->Bonding;
    data << Description;
    data << pProto->PageText;
    data << pProto->LanguageID;
    data << pProto->PageMaterial;
    data << pProto->StartQuest;
    data << pProto->LockID;
    data << int32(pProto->Material);
    data << pProto->Sheath;
    data << pProto->RandomProperty;
    data << pProto->RandomSuffix;
    data << pProto->Block;
    data << pProto->ItemSet;
    data << pProto->MaxDurability;
    data << pProto->Area;
    data << pProto->Map;                                // Added in 1.12.x & 2.0.1 client branch
    data << pProto->BagFamily;
    data << pProto->TotemCategory;
    for (int s = 0; s < MAX_ITEM_PROTO_SOCKETS; ++s)
    {
        data << pProto->Socket[s].Color;
        data << pProto->Socket[s].Content;
    }
    data << pProto->socketBonus;
    data << pProto->GemProperties;
    data << pProto->RequiredDisenchantSkill;
    data << pProto->ArmorDamageModifier;
    data << pProto->Duration;                           // added in 2.4.2.8209, duration (seconds)
    data << pProto->ItemLimitCategory;                  // WotLK, ItemLimitCategory
    data << pProto->HolidayId;                          // Holiday.dbc?
    player->GetSession()->SendPacket(&data);
}

static void SendReforgePackets(Player* player)
{
    if (!send_cache_packets)
        return;

    std::vector<Item*> items = GetItemList(player);
    for (std::vector<Item*>::const_iterator it = items.begin(); it != items.end(); ++it)
        SendReforgePacket(player, (*it)->GetEntry(), (*it)->GetGUID().GetCounter());
}

void RemoveReforge(Player* player, uint32 itemguid, bool update)
{
    uint32 lowguid = player->GetGUID().GetCounter();
    if (!itemguid || player->reforgeMap.empty() ||
        player->reforgeMap.find(itemguid) == player->reforgeMap.end())
        return;

    player->reforgeMap.erase(itemguid);
    Item* invItem = update ? player->GetItemByGuid(ObjectGuid(HighGuid::Item, itemguid)) : NULL;
    if (invItem)
        player->_ApplyItemMods(invItem, invItem->GetSlot(), false);
    player->reforgeMap.erase(itemguid);
    if (invItem)
        player->_ApplyItemMods(invItem, invItem->GetSlot(), true);

    //if (!database)
    //    return;
    //CharacterDatabase.PExecute("DELETE FROM `custom_reforging` WHERE `GUID` = %u", itemguid);
    if (invItem)
        SendReforgePacket(player, invItem->GetEntry());
    //player->SaveToDB();
}

static bool IsReforgable(Item* invItem, Player* player)
{
     //hxsd
     //return true;
    //if (!invItem->IsEquipped())
    //    return false;
    if (invItem->GetOwnerGUID() != player->GetGUID())
        return false;
    const ItemTemplate* pProto = invItem->GetTemplate();
    //if (pProto->ItemLevel < 200)
    //    return false;
    //if (pProto->Quality == ITEM_QUALITY_HEIRLOOM) // block heirlooms necessary?
    //    return false;
    if (!pProto->StatsCount || pProto->StatsCount >= MAX_ITEM_PROTO_STATS) // Mandatory! Do NOT remove or edit
        return false;
    if (!player->reforgeMap.empty() && player->reforgeMap.find(invItem->GetGUID().GetCounter()) != player->reforgeMap.end()) // Mandatory! Do NOT remove or edit
        return false;
    for (uint32 i = 0; i < pProto->StatsCount; ++i)
    {
		if (!GetStatName(pProto->ItemStat[i].ItemStatType, player->GetSession()))
            continue;
        if (((int32)floorf((float)pProto->ItemStat[i].ItemStatValue * 0.4f)) > 1)
            return true;
    }
    return false;
}

static void UpdatePlayerReforgeStats(Item* invItem, Player* player, uint32 decrease, uint32 increase) // stat types
{
    const ItemTemplate* pProto = invItem->GetTemplate();

    int32 stat_diff = 0;
    for (uint32 i = 0; i < pProto->StatsCount; ++i)
    {
        if (pProto->ItemStat[i].ItemStatType == increase)
            return; // Should not have the increased stat already
        if (pProto->ItemStat[i].ItemStatType == decrease)
            stat_diff = (int32)floorf((float)pProto->ItemStat[i].ItemStatValue * 0.4f);
    }
    if (stat_diff <= 0)
        return; // Should have some kind of diff

    // Update player stats
    player->_ApplyItemMods(invItem, invItem->GetSlot(), false);
    uint32 guidlow = invItem->GetGUID().GetCounter();
    ReforgeData& data = player->reforgeMap[guidlow];
    data.increase = increase;
    data.decrease = decrease;
    data.stat_value = stat_diff;
    player->_ApplyItemMods(invItem, invItem->GetSlot(), true);
    // CharacterDatabase.PExecute("REPLACE INTO `custom_reforging` (`GUID`, `increase`, `decrease`, `stat_value`) VALUES (%u, %u, %u, %i)", guidlow, increase, decrease, stat_diff);
    player->ModifyMoney(pProto->SellPrice < (10*GOLD) ? (-10*GOLD) : -(int32)pProto->SellPrice);
    SendReforgePacket(player, invItem->GetEntry(), 0, &data);
    // player->SaveToDB();
}

class REFORGE_PLAYER : public PlayerScript
{
public:
    REFORGE_PLAYER() : PlayerScript("REFORGE_PLAYER")
    {
        CharacterDatabase.DirectExecute("DELETE FROM `custom_reforging` WHERE NOT EXISTS (SELECT 1 FROM `item_instance` WHERE `item_instance`.`guid` = `custom_reforging`.`GUID`)");
    }

    class SendRefPackLogin : public BasicEvent
    {
    public:
        SendRefPackLogin(Player* _player) : player(_player)
        {
            _player->m_Events.AddEvent(this, _player->m_Events.CalculateTime(1000));
        }

        bool Execute(uint64, uint32) override
        {
            SendReforgePackets(player);
            return true;
        }
        Player* player;
    };

    void OnLogin(Player* player, bool firstLogin) override
    {
        uint32 playerGUID = player->GetGUID().GetCounter();
        QueryResult result = CharacterDatabase.PQuery("SELECT `GUID`, `increase`, `decrease`, `stat_value` FROM `custom_reforging` WHERE `Owner` = %u", playerGUID);
        if (result)
        {
            do
            {
                uint32 lowGUID = (*result)[0].GetUInt32();
                Item* invItem = player->GetItemByGuid(ObjectGuid(HighGuid::Item, lowGUID));
                if (invItem)
                    player->_ApplyItemMods(invItem, invItem->GetSlot(), false);
                ReforgeData& data = player->reforgeMap[lowGUID];
                data.increase = (*result)[1].GetUInt32();
                data.decrease = (*result)[2].GetUInt32();
                data.stat_value = (*result)[3].GetInt32();
                if (invItem)
                    player->_ApplyItemMods(invItem, invItem->GetSlot(), true);
                // SendReforgePacket(player, entry, lowGUID);
            } while (result->NextRow());

            // SendReforgePackets(player);
            new SendRefPackLogin(player);
        }
    }

    //void OnLogout(Player* player) override
    //{
    //    if (player->reforgeMap.empty())
    //        return;
    //    for (ReforgeMapType::const_iterator it = player->reforgeMap.begin(); it != player->reforgeMap.end();)
    //    {
    //        ReforgeMapType::const_iterator old_it = it++;
    //        RemoveReforge(player, old_it->first, false);
    //    }
    //}

    void OnSave(Player* player) override
    {
        uint32 lowguid = player->GetGUID().GetCounter();
        SQLTransaction trans = CharacterDatabase.BeginTransaction();
        trans->PAppend("DELETE FROM `custom_reforging` WHERE `Owner` = %u", lowguid);
        if (!player->reforgeMap.empty())
        {
            // Only save items that are in inventory / bank / etc
            std::vector<Item*> items = GetItemList(player);
            for (std::vector<Item*>::const_iterator it = items.begin(); it != items.end(); ++it)
            {
                ReforgeMapType::const_iterator it2 = player->reforgeMap.find((*it)->GetGUID().GetCounter());
                if (it2 == player->reforgeMap.end())
                    continue;

                trans->PAppend("REPLACE INTO `custom_reforging` (`GUID`, `increase`, `decrease`, `stat_value`, `Owner`) VALUES (%u, %u, %u, %i, %u)", it2->first, it2->second.increase, it2->second.decrease, it2->second.stat_value, lowguid);
            }
        }

        if (trans->GetSize()) // basically never false
            CharacterDatabase.CommitTransaction(trans);
    }
};

class REFORGER_NPC : public CreatureScript
{
public:
    REFORGER_NPC() : CreatureScript("REFORGER_NPC") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
		WorldSession* session = player->GetSession();
		//player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Select slot of the item to reforge:", 0, Melt(MAIN_MENU, 0));
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, session->GetTrinityString(TransmogTrinityStrings::LANG_MENU_ITEM_REFORGEHEAD), 0, Melt(MAIN_MENU, 0));
		for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
        {
            if (Item* invItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                if (IsReforgable(invItem, player))
                    if (const char* slotname = GetSlotName(slot, player->GetSession()))
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, slotname, 0, Melt(SELECT_STAT_REDUCE, slot));
        }
		//player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Remove reforges", 0, Melt(SELECT_RESTORE, 0));
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, session->GetTrinityString(TransmogTrinityStrings::LANG_MENU_ITEM_REMOVEREFORGE), 0, Melt(SELECT_RESTORE, 0));
		//player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Update menu", 0, Melt(MAIN_MENU, 0));
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, session->GetTrinityString(TransmogTrinityStrings::LANG_MENU_ITEM_UPDATEMENU), 0, Melt(MAIN_MENU, 0));
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 melt) override
    {
		WorldSession* session = player->GetSession();
		player->PlayerTalkClass->ClearMenus();

        uint8 menu, action;
        Unmelt(melt, menu, action);

        switch (menu)
        {
        case MAIN_MENU: OnGossipHello(player, creature); break;
        case SELECT_STAT_REDUCE:
            // action = slot
            if (Item* invItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, action))
            {
                if (IsReforgable(invItem, player))
                {
                    uint32 guidlow = invItem->GetGUID().GetCounter();
                    const ItemTemplate* pProto = invItem->GetTemplate();
					//player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Stat to decrease:", sender, melt);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, session->GetTrinityString(TransmogTrinityStrings::LANG_MENU_ITEM_DECSTAT), sender, melt);
					for (uint32 i = 0; i < pProto->StatsCount; ++i)
                    {
                        int32 stat_diff = ((int32)floorf((float)pProto->ItemStat[i].ItemStatValue * 0.4f));
                        if (stat_diff > 1)
							if (const char* stat_name = GetStatName(pProto->ItemStat[i].ItemStatType, player->GetSession()))
                            {
                                std::ostringstream oss;
                                oss << stat_name << " (" << pProto->ItemStat[i].ItemStatValue << " |cFFDB2222-" << stat_diff << "|r)";
                                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, oss.str(), guidlow, Melt(SELECT_STAT_INCREASE, i));
                            }
                    }
					//player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back..", 0, Melt(MAIN_MENU, 0));
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, session->GetTrinityString(TransmogTrinityStrings::LANG_MENU_ITEM_BACK), 0, Melt(MAIN_MENU, 0));
					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                }
                else
                {
					//player->GetSession()->SendNotification("Invalid item selected");
					player->GetSession()->SendNotification(session->GetTrinityString(TransmogTrinityStrings::LANG_MENU_ITEM_INVALIDITEM));
					OnGossipHello(player, creature);
                }
            }
            else
            {
                //player->GetSession()->SendNotification("Invalid item selected");
				player->GetSession()->SendNotification(session->GetTrinityString(TransmogTrinityStrings::LANG_MENU_ITEM_INVALIDITEM));
				OnGossipHello(player, creature);
            }
            break;
        case SELECT_STAT_INCREASE:
            // sender = item guidlow
            // action = StatsCount id
            {
                Item* invItem = GetEquippedItem(player, sender);
                if (invItem)
                {
                    const ItemTemplate* pProto = invItem->GetTemplate();
                    int32 stat_diff = ((int32)floorf((float)pProto->ItemStat[action].ItemStatValue * 0.4f));

                    //player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Stat to increase:", sender, melt);
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, session->GetTrinityString(TransmogTrinityStrings::LANG_MENU_ITEM_INCSTAT), sender, melt);
					for (uint8 i = 0; i < stat_type_max; ++i)
                    {
                        bool cont = false;
                        for (uint32 j = 0; j < pProto->StatsCount; ++j)
                        {
                            if (statTypes[i] == pProto->ItemStat[j].ItemStatType) // skip existing stats on item
                            {
                                cont = true;
                                break;
                            }
                        }
                        if (cont)
                            continue;
						if (const char* stat_name = GetStatName(statTypes[i], player->GetSession()))
                        {
                            std::ostringstream oss;
                            oss << stat_name << " |cFF2222DB+" << stat_diff << "|r";
                            player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, oss.str(), sender, Melt(i, (uint8)pProto->ItemStat[action].ItemStatType), "Are you sure you want to reforge\n\n" + pProto->Name1, (pProto->SellPrice < (10 * GOLD) ? (10 * GOLD) : pProto->SellPrice), false);
                        }
                    }
					//player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back..", 0, Melt(SELECT_STAT_REDUCE, invItem->GetSlot()));
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, session->GetTrinityString(TransmogTrinityStrings::LANG_MENU_ITEM_BACK), 0, Melt(SELECT_STAT_REDUCE, invItem->GetSlot()));
					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                }
                else
                {
                    //player->GetSession()->SendNotification("Invalid item selected");
					player->GetSession()->SendNotification(session->GetTrinityString(TransmogTrinityStrings::LANG_MENU_ITEM_INVALIDITEM));
					OnGossipHello(player, creature);
                }
            }
            break;
        case SELECT_RESTORE:
            {
				//player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Select slot to remove reforge from:", sender, melt);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, session->GetTrinityString(TransmogTrinityStrings::LANG_MENU_ITEM_SLOT_REMOVEREFORGE), sender, melt);
				if (!player->reforgeMap.empty())
                {
                    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
                    {
                        if (Item* invItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                            if (player->reforgeMap.find(invItem->GetGUID().GetCounter()) != player->reforgeMap.end())
                                if (const char* slotname = GetSlotName(slot, player->GetSession()))
                                    player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, slotname, invItem->GetGUID().GetCounter(), Melt(RESTORE, 0), "Remove reforge from\n\n" + invItem->GetTemplate()->Name1, 0, false);
                    }
                }
				//player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Update menu", sender, melt);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, session->GetTrinityString(TransmogTrinityStrings::LANG_MENU_ITEM_UPDATEMENU), sender, melt);
				//player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Back..", 0, Melt(MAIN_MENU, 0));
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, session->GetTrinityString(TransmogTrinityStrings::LANG_MENU_ITEM_BACK), 0, Melt(MAIN_MENU, 0));
				player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            }
            break;
        case RESTORE:
            // sender = item guidlow
            {
                if (Item* invItem = player->GetItemByGuid(ObjectGuid(HighGuid::Item, sender)))
                {
                    if (!player->reforgeMap.empty() && player->reforgeMap.find(sender) != player->reforgeMap.end())
                        RemoveReforge(player, sender, true);
                }
                OnGossipHello(player, creature);
            }
            break;
        default: // Reforge
            // sender = item guidlow
            // menu = stat type to increase index to statTypes[]
            // action = stat type to decrease
            {
                if (menu < stat_type_max)
                {
                    Item* invItem = GetEquippedItem(player, sender);
                    if (invItem && IsReforgable(invItem, player))
                    {
                        if (player->HasEnoughMoney(invItem->GetTemplate()->SellPrice < (10 * GOLD) ? (10 * GOLD) : invItem->GetTemplate()->SellPrice))
                        {
                            // int32 stat_diff = ((int32)floorf((float)invItem->GetTemplate()->ItemStat[action].ItemStatValue * 0.4f));
                            UpdatePlayerReforgeStats(invItem, player, action, statTypes[menu]); // rewrite this function
                        }
						else
						{
							//player->GetSession()->SendNotification("Not enough money");
							player->GetSession()->SendNotification(session->GetTrinityString(TransmogTrinityStrings::LANG_ERR_TRANSMOG_NOT_ENOUGH_MONEY));
						}
                    }
					else
					{
						//player->GetSession()->SendNotification("Invalid item selected");
						player->GetSession()->SendNotification(session->GetTrinityString(TransmogTrinityStrings::LANG_MENU_ITEM_INVALIDITEM));
					}
                }
                OnGossipHello(player, creature);
            }
        }
        return true;
    }

    enum Menus
    {
        MAIN_MENU = 200, // stat_type_max
        SELECT_ITEM,
        SELECT_STAT_REDUCE,
        SELECT_STAT_INCREASE,
        SELECT_RESTORE,
        RESTORE,
        REFORGE,
    };
};


class boss_worldboss1 : public CreatureScript
{
    public:
        boss_worldboss1() : CreatureScript("boss_worldboss1") { }

        struct boss_worldboss1AI : public ScriptedAI
        {
            boss_worldboss1AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_1, urand(6000, 10000));
                _events.ScheduleEvent(EVENT_2, 7000);
                _events.ScheduleEvent(EVENT_3, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_4, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_5, 3000);
                _events.ScheduleEvent(EVENT_6, 30000);
                _events.ScheduleEvent(EVENT_7, 900000);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
     
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth() )
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {

                    char message[1024];
                    
                    snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的伊拉贡出现了！　|r");
                    sWorld->SendGlobalText(message, NULL);
                
            }

            void EnterCombat(Unit* who) override
            {
                if (!Announced)
                {
                    char message[1024];
                    
                    if (who->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (who->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的伊拉贡正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的伊拉贡正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->GetTypeId() == TYPEID_UNIT)
                    {
                        if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                        {
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的伊拉贡正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的伊拉贡正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        if (who->IsPet())
                        {
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的伊拉贡正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的伊拉贡正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的伊拉贡正在遭到攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的伊拉贡正在遭到攻击！　|r");
                        
                    }
                    sWorld->SendGlobalText(message, NULL);
                    Announced = true;
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;


                _events.Update(diff);

                if ((me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f) || me->GetDistance(me->GetVictim()) < 0.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_1:
                            DoCastVictim(SPELL_SHADOW_VOLLEY);
                            _events.ScheduleEvent(EVENT_1, urand(4000, 6000));
                            break;
                        case EVENT_2:
                            DoCastVictim(SPELL_CLEAVE);
                            _events.ScheduleEvent(EVENT_2, urand(8000, 12000));
                            break;
                        case EVENT_3:
                            DoCastVictim(SPELL_THUNDERCLAP);
                            _events.ScheduleEvent(EVENT_3, urand(10000, 14000));
                            break;
                        case EVENT_4:

                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                    DoCast(target, SPELL_SHADOW_TRAP);

                            _events.ScheduleEvent(EVENT_4, urand(10000, 14000));
                            break;
                        case EVENT_5:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                            {
                                DoCast(target, SPELL_ARCANE_ORB);
                                if ( target->GetTypeId() == TYPEID_PLAYER)
                                {
                                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                                    uint32 ThreatSize = 0;
                                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                                    {
                                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                                            if (target->GetTypeId() == TYPEID_PLAYER)
                                                ThreatSize = ThreatSize + 1;
                                    }
                                    if (ThreatSize < 2)
                                        target->CastSpell(target, 90020, true);
                                }
                            }
                            _events.ScheduleEvent(EVENT_5, 3000);
                            break;
                        case EVENT_6:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_6, 30000);
                            break;
                        case EVENT_7:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_7, 10000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss1AI(creature);
        }
};

class boss_worldboss2 : public CreatureScript
{
    public:
        boss_worldboss2() : CreatureScript("boss_worldboss2") { }

        struct boss_worldboss2AI : public ScriptedAI
        {
            boss_worldboss2AI(Creature* creature) : ScriptedAI(creature)
            {

                    SetCombatMovement(false);
               
            }

            bool Announced;

            void Reset() override
            {

                    me->SetHover(true);
                    me->SetSwim(true);
                    me->SetDisableGravity(true);
                
                _events.Reset();
                _events.ScheduleEvent(EVENT_1, urand(6000, 10000));
                _events.ScheduleEvent(EVENT_2, 7000);
                _events.ScheduleEvent(EVENT_3, 7000);
                _events.ScheduleEvent(EVENT_4, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_5, 10000);
                _events.ScheduleEvent(EVENT_6, 20000);
                _events.ScheduleEvent(EVENT_7, 30000);
                _events.ScheduleEvent(EVENT_8, 900000);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {

                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth() )
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {

                    me->SetHover(true);
                    me->SetSwim(true);
                    me->SetDisableGravity(true);
                    char message[1024];
                    
                    snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的吞天出现了！　|r");
                    
                    sWorld->SendGlobalText(message, NULL);
                
            }

            void EnterCombat(Unit* who) override
            {
                if (!Announced)
                {
                    char message[1024];
                    
                    if (who->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (who->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的吞天正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的吞天正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->GetTypeId() == TYPEID_UNIT)
                    {
                        if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                        {
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的吞天正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的吞天正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        if (who->IsPet())
                        {
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的吞天正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的吞天正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的吞天正在遭到攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的吞天正在遭到攻击！　|r");
                        
                    }
                    sWorld->SendGlobalText(message, NULL);
                    Announced = true;
                    me->SetHover(false);
                    me->SetSwim(true);
                    me->SetDisableGravity(true);
                }
                me->CastSpell(me, SPELL_FROST_AURA, true);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {

                    me->SetHover(true);
                    me->SetSwim(true);
                    me->SetDisableGravity(true);



                if (!UpdateVictim())
                    return;


                _events.Update(diff);

                if (me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_1:
                            if (!me->IsNonMeleeSpellCast(false))
                                DoCastAOE(SPELL_FROSTBOLT_VOLLEY, false);
                            _events.ScheduleEvent(EVENT_1, urand(4000, 6000));
                            break;
                        case EVENT_2:
                            DoCastVictim(SPELL_CLEAVE);
                            _events.ScheduleEvent(EVENT_2, urand(8000, 12000));
                            break;
                        case EVENT_3:
                           
                                DoCastAOE(SPELL_TAIL_SWEEP);
                            _events.ScheduleEvent(EVENT_3, urand(8000, 12000));
                            break;
                        case EVENT_4:

                                if (!me->IsNonMeleeSpellCast(false))
                                {
                                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                        DoCast(target, SPELL_FROSTBOLT);
                                }
                            
                            _events.ScheduleEvent(EVENT_4, urand(10000, 14000));
                            break;
                        case EVENT_5:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_MANA_DETONATION);
                            _events.ScheduleEvent(EVENT_5, 10000);
                            break;
                        case EVENT_6:
                            if (!me->IsNonMeleeSpellCast(false))
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                {
                                    DoCast(target, SPELL_ARCTIC_BREATH);
                                    if (target->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                                        uint32 ThreatSize = 0;
                                        for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                                        {
                                            if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                                                if (target->GetTypeId() == TYPEID_PLAYER)
                                                    ThreatSize = ThreatSize + 1;
                                        }
                                        if (ThreatSize < 2)
                                            target->CastSpell(target, 90020, true);
                                    }
                                }
                            }
                            _events.ScheduleEvent(EVENT_6, 20000);
                            break;
                        case EVENT_7:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_7, 30000);
                            break;
                        case EVENT_8:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_8, 10000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss2AI(creature);
        }
};

class boss_worldboss3 : public CreatureScript
{
    public:
        boss_worldboss3() : CreatureScript("boss_worldboss3") { }

        struct boss_worldboss3AI : public ScriptedAI
        {
            boss_worldboss3AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_1, 5000);
                _events.ScheduleEvent(EVENT_2, 15000);
                _events.ScheduleEvent(EVENT_3, 20000);
                _events.ScheduleEvent(EVENT_4, 30000);
                _events.ScheduleEvent(EVENT_5, 10000);
                _events.ScheduleEvent(EVENT_6, 60000);
                _events.ScheduleEvent(EVENT_7, 50000);
                _events.ScheduleEvent(EVENT_8, 30000);
                _events.ScheduleEvent(EVENT_9, 900000);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
  
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {

                    char message[1024];
                    
                    snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的陈·风暴烈酒出现了！　|r");
                    sWorld->SendGlobalText(message, NULL);
               
            }

            void EnterCombat(Unit* who) override
            {
                if (!Announced)
                {
                    char message[1024];
                    
                    if (who->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (who->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的陈·风暴烈酒正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的陈·风暴烈酒正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->GetTypeId() == TYPEID_UNIT)
                    {
                        if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                        {
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的陈·风暴烈酒正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的陈·风暴烈酒正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        if (who->IsPet())
                        {
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的陈·风暴烈酒正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的陈·风暴烈酒正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的陈·风暴烈酒正在遭到攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的陈·风暴烈酒正在遭到攻击！　|r");
                        
                    }
                    sWorld->SendGlobalText(message, NULL);
                    Announced = true;
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

             

                _events.Update(diff);

                if (me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_1:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_CHARGE);
                            _events.ScheduleEvent(EVENT_1, urand(15000, 25000));
                            break;
                        case EVENT_2:
                            DoCast(me, SPELL_WAR_STOMP);
                            _events.ScheduleEvent(EVENT_2, urand(18000, 24000));
                            break;
                        case EVENT_3:
                        {
                            DoCast(me, SPELL_INCITE_CHAOS);

                            std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                            {
                                if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                                {
                                    if (target->GetTypeId() == TYPEID_PLAYER)
                                    {

                                            me->CastSpell(target, SPELL_INCITE_CHAOS_B, true);
                                        if (target->GetTypeId() == TYPEID_PLAYER)
                                        {
                                            std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                                            uint32 ThreatSize = 0;
                                            for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                                            {
                                                if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                                                    if (target->GetTypeId() == TYPEID_PLAYER)
                                                        ThreatSize = ThreatSize + 1;
                                            }
                                            if (ThreatSize < 2)
                                                target->CastSpell(target, 90020, true);
                                        }
                                    }
                                }
                            }
                            DoResetThreat();
                            _events.ScheduleEvent(EVENT_3, 40000);
                            break;
                        }
                        case EVENT_4:
                            DoCastVictim(SPELL_WHIRLWIND);
                            _events.ScheduleEvent(EVENT_4, 30000);
                            break;
                        case EVENT_5:
                            DoCastVictim(SPELL_LIGHT_TWIN_SPIKE);
                            _events.ScheduleEvent(EVENT_5, 10000);
                            break;
                        case EVENT_6:
                            DoCast(me, SPELL_LIGHT_SHIELD);
                            DoCast(me, SPELL_LIGHT_TWIN_PACT);
                            _events.ScheduleEvent(EVENT_6, 60000);
                            break;
                        case EVENT_7:
                            DoCastAOE(SPELL_LIGHT_VORTEX);
                            _events.ScheduleEvent(EVENT_7, 50000);
                            break;
                        case EVENT_8:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_8, 30000);
                            break;
                        case EVENT_9:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_9, 10000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss3AI(creature);
        }
};

class boss_worldboss4 : public CreatureScript
{
    public:
        boss_worldboss4() : CreatureScript("boss_worldboss4") { }

        struct boss_worldboss4AI : public ScriptedAI
        {
            boss_worldboss4AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_1, urand(6000, 10000));
                _events.ScheduleEvent(EVENT_2, 7000);
                _events.ScheduleEvent(EVENT_3, urand (10000, 20000));
                _events.ScheduleEvent(EVENT_4, 20000);
                _events.ScheduleEvent(EVENT_5, 5000);
                _events.ScheduleEvent(EVENT_6, urand(10000, 15000));
                _events.ScheduleEvent(EVENT_7, 30000);
                _events.ScheduleEvent(EVENT_8, 900000);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
   
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth() )
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {

                    char message[1024];
                    
                    snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的堕落的阿莱克丝塔萨，生命的焚化者出现了！　|r");
                    
                    sWorld->SendGlobalText(message, NULL);
                
            }

            void EnterCombat(Unit* who) override
            {
                if (!Announced)
                {
                    char message[1024];
                    
                    if (who->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (who->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的堕落的阿莱克丝塔萨，生命的焚化者正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的堕落的阿莱克丝塔萨，生命的焚化者正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->GetTypeId() == TYPEID_UNIT)
                    {
                        if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                        {
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的堕落的阿莱克丝塔萨，生命的焚化者正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的堕落的阿莱克丝塔萨，生命的焚化者正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        if (who->IsPet())
                        {
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的堕落的阿莱克丝塔萨，生命的焚化者正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的堕落的阿莱克丝塔萨，生命的焚化者正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的堕落的阿莱克丝塔萨，生命的焚化者正在遭到攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领海加尔山的堕落的阿莱克丝塔萨，生命的焚化者正在遭到攻击！　|r");
                        
                    }
                    sWorld->SendGlobalText(message, NULL);
                    Announced = true;
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

             
                _events.Update(diff);

                if (me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_1:
                            DoCastAOE(SPELL_FIRE_BLOOM, false);
                            _events.ScheduleEvent(EVENT_1, urand(6000, 10000));
                            break;
                        case EVENT_2:
                            DoCastVictim(SPELL_CLEAVE);
                            _events.ScheduleEvent(EVENT_2, urand(8000, 12000));
                            break;
                        case EVENT_3:
                            DoCastVictim(SPELL_FLAME_BREATH);
                            _events.ScheduleEvent(EVENT_3, urand (10000, 20000));
                            break;
                        case EVENT_4:


                                DoCastVictim(SPELL_WING_BUFFET);
                                if (DoGetThreat(me->GetVictim()))
                                    DoModifyThreatPercent(me->GetVictim(), -75);
                            
                            _events.ScheduleEvent(EVENT_4, 20000);
                            break;
                        case EVENT_5:
                            DoCastVictim(SPELL_FLAMEBUFFET);
                            _events.ScheduleEvent(EVENT_5, 5000);
                            break;
                        case EVENT_6:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            {
                                DoCast(target, SPELL_FLAME_STRIKE);
                                if (target->GetTypeId() == TYPEID_PLAYER)
                                {
                                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                                    uint32 ThreatSize = 0;
                                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                                    {
                                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                                            if (target->GetTypeId() == TYPEID_PLAYER)
                                                ThreatSize = ThreatSize + 1;
                                    }
                                    if (ThreatSize < 2)
                                        target->CastSpell(target, 90020, true);
                                }
                            }
                            _events.ScheduleEvent(EVENT_6, urand(10000, 15000));
                            break;
                        case EVENT_7:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_7, 30000);
                            break;
                        case EVENT_8:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_8, 10000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss4AI(creature);
        }
};

class boss_worldboss5 : public CreatureScript
{
    public:
        boss_worldboss5() : CreatureScript("boss_worldboss5") { }

        struct boss_worldboss5AI : public ScriptedAI
        {
            boss_worldboss5AI(Creature* creature) : ScriptedAI(creature) { }

            bool WorldBoss5Dead;
            bool Announced;

            void Reset() override
            {
                
                if (!me->IsInCombat())
                {
                    _events.Reset();
                    _events.ScheduleEvent(EVENT_BLADESTORM, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_INTIMIDATING_SHOUT, urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_WARR_CHARGE, 1*IN_MILLISECONDS);
                    _events.ScheduleEvent(EVENT_DISARM, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_OVERPOWER, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_SUNDER_ARMOR, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_SHATTERING_THROW, urand(20*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_RETALIATION, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_THREAT_WARRIOR, 5*IN_MILLISECONDS);
                    _events.ScheduleEvent(EVENT_GROWTH_WARRIOR, 30000);
                    _events.ScheduleEvent(EVENT_CAPTURE_SOUL_WARRIOR, 900000);
                    SetEquipmentSlots(false, 45899, 45899, EQUIP_NO_CHANGE);
                    WorldBoss5Dead = false;
                    Announced = false;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_WORLDBOSS5:
                        if (WorldBoss5Dead)
                            return 1;
                        break;
                }
                return 0;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
            } 

            void JustRespawned() override
            {
                float x, y, z, o;
                me->GetHomePosition(x, y, z, o);
                DoTeleportTo(x, y, z);
        
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领厄运之槌竞技场的拉格什、布罗尔·熊皮、瓦莉拉·萨古纳尔和雷加·大地之怒出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                if (!Announced)
                {
                    char message[1024];
                    
                    if (who->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (who->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领厄运之槌竞技场的拉格什、布罗尔·熊皮、瓦莉拉·萨古纳尔和雷加·大地之怒正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领厄运之槌竞技场的拉格什、布罗尔·熊皮、瓦莉拉·萨古纳尔和雷加·大地之怒正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->GetTypeId() == TYPEID_UNIT)
                    {
                        if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                        {
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领厄运之槌竞技场的拉格什、布罗尔·熊皮、瓦莉拉·萨古纳尔和雷加·大地之怒正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领厄运之槌竞技场的拉格什、布罗尔·熊皮、瓦莉拉·萨古纳尔和雷加·大地之怒正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        if (who->IsPet())
                        {
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领厄运之槌竞技场的拉格什、布罗尔·熊皮、瓦莉拉·萨古纳尔和雷加·大地之怒正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领厄运之槌竞技场的拉格什、布罗尔·熊皮、瓦莉拉·萨古纳尔和雷加·大地之怒正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领厄运之槌竞技场的拉格什、布罗尔·熊皮、瓦莉拉·萨古纳尔和雷加·大地之怒正在遭到攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领厄运之槌竞技场的拉格什、布罗尔·熊皮、瓦莉拉·萨古纳尔和雷加·大地之怒正在遭到攻击！　|r");
                        
                    }
                    sWorld->SendGlobalText(message, NULL);
                    Announced = true;
                }
                DoCast(me, SPELL_ANTI_AOE, true);
             
                WorldBoss5Dead = false;
            }

            void JustDied(Unit* /*killer*/) override
            {

                WorldBoss5Dead = true;
            }

            void JustReachedHome()
            {

            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

           

                _events.Update(diff);

                if (me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BLADESTORM:
                            DoCastVictim(SPELL_BLADESTORM);
                            _events.ScheduleEvent(EVENT_BLADESTORM, 150*IN_MILLISECONDS);
                            return;
                        case EVENT_INTIMIDATING_SHOUT:
                            DoCastAOE(SPELL_INTIMIDATING_SHOUT);
                            _events.ScheduleEvent(EVENT_INTIMIDATING_SHOUT, 120*IN_MILLISECONDS);
                            return;
                        case EVENT_MORTAL_STRIKE:
                            DoCastVictim(SPELL_MORTAL_STRIKE);
                            _events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_WARR_CHARGE:
                            DoCastVictim(SPELL_CHARGE);
                            _events.ScheduleEvent(EVENT_WARR_CHARGE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_DISARM:
                            DoCastVictim(SPELL_DISARM);
                            _events.ScheduleEvent(EVENT_DISARM, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_OVERPOWER:
                            DoCastVictim(SPELL_OVERPOWER);
                            _events.ScheduleEvent(EVENT_OVERPOWER, urand(20*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_SUNDER_ARMOR:
                            DoCastVictim(SPELL_SUNDER_ARMOR);
                            _events.ScheduleEvent(EVENT_SUNDER_ARMOR, urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            return;
                        case EVENT_SHATTERING_THROW:
                            if (Unit* target = me->GetVictim())
                            {
                                if (target->HasAuraWithMechanic(1 << MECHANIC_IMMUNE_SHIELD))
                                {
                                    DoCast(target, SPELL_SHATTERING_THROW);
                                    _events.RescheduleEvent(EVENT_SHATTERING_THROW, 5*MINUTE*IN_MILLISECONDS);
                                    return;
                                }
                            }
                            _events.RescheduleEvent(EVENT_SHATTERING_THROW, 3*IN_MILLISECONDS);
                            return;
                        case EVENT_RETALIATION:
                            if (HealthBelowPct(50))
                            {
                                DoCast(SPELL_RETALIATION);
                                _events.RescheduleEvent(EVENT_RETALIATION, 5*MINUTE*IN_MILLISECONDS);
                            }
                            else
                                _events.RescheduleEvent(EVENT_RETALIATION, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_THREAT_WARRIOR:
                        {
                            std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                            {
                                Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
                                if (unit && me->getThreatManager().getThreat(unit))
                                {
                                    if (unit->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        float dist_mod = 15.0f / (15.0f + me->GetDistance2d(unit));
                                        float armor_mod = (float)unit->GetArmor() / 16635.0f;
                                        float eh = (unit->GetHealth() + 1) * (1.0f + armor_mod);
                                        float threat = dist_mod * 30000.0f / eh;
                                        me->getThreatManager().modifyThreatPercent(unit, -100);
                                        me->AddThreat(unit, 1000000.0f * threat);
                                    }
                                }
                            }
                            _events.ScheduleEvent(EVENT_THREAT_WARRIOR, 4*IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_GROWTH_WARRIOR:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_GROWTH_WARRIOR, 30000);
                            break;
                        case EVENT_CAPTURE_SOUL_WARRIOR:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_CAPTURE_SOUL_WARRIOR, 10000);
                            break;
                        default:
                            return;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss5AI(creature);
        }
};

class boss_worldboss6 : public CreatureScript
{
    public:
        boss_worldboss6() : CreatureScript("boss_worldboss6") { }

        struct boss_worldboss6AI : public ScriptedAI
        {
            boss_worldboss6AI(Creature* creature) : ScriptedAI(creature) { }

            bool WorldBoss6Dead;

            void Reset() override
            {


                if (!me->IsInCombat())
                {
                    _events.Reset();
                    _events.ScheduleEvent(EVENT_CYCLONE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_ENTANGLING_ROOTS, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_FAERIE_FIRE, urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_FORCE_OF_NATURE, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_INSECT_SWARM, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_MOONFIRE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_STARFIRE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_LIFEBLOOM, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_NOURISH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_REGROWTH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_REJUVENATION, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_TRANQUILITY, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_BARKSKIN, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_THORNS, 2*IN_MILLISECONDS);
                    _events.ScheduleEvent(EVENT_NATURE_GRASP, urand(3*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_THREAT_DRUID, 5*IN_MILLISECONDS);
                    _events.ScheduleEvent(EVENT_GROWTH_DRUID, 30000);
                    _events.ScheduleEvent(EVENT_CAPTURE_SOUL_DRUID, 900000);
                    SetEquipmentSlots(false, 5600, 11505, EQUIP_NO_CHANGE);
                    WorldBoss6Dead = false;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_WORLDBOSS6:
                        if (WorldBoss6Dead)
                            return 1;
                        break;
                }
                return 0;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
            } 

            void JustRespawned() override
            {
                float x, y, z, o;
                me->GetHomePosition(x, y, z, o);
                DoTeleportTo(x, y, z);
               
            }

            void EnterCombat(Unit* who) override
            {
                DoCast(me, SPELL_ANTI_AOE, true);
               
                WorldBoss6Dead = false;
            }

            void JustDied(Unit* /*killer*/) override
            {
                
                WorldBoss6Dead = true;
            }

            void JustReachedHome()
            {
               
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

              
                _events.Update(diff);

                if (me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CYCLONE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_CYCLONE);
                            _events.ScheduleEvent(EVENT_CYCLONE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_ENTANGLING_ROOTS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true))
                                DoCast(target, SPELL_ENTANGLING_ROOTS);
                            _events.ScheduleEvent(EVENT_ENTANGLING_ROOTS, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_FAERIE_FIRE:
                            DoCastVictim(SPELL_FAERIE_FIRE);
                            _events.ScheduleEvent(EVENT_FAERIE_FIRE, urand(30*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_FORCE_OF_NATURE:
                            DoCastVictim(SPELL_FORCE_OF_NATURE);
                            _events.ScheduleEvent(EVENT_FORCE_OF_NATURE, 180*IN_MILLISECONDS);
                            return;
                        case EVENT_INSECT_SWARM:
                            DoCastVictim(SPELL_INSECT_SWARM);
                            _events.ScheduleEvent(EVENT_INSECT_SWARM, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_MOONFIRE:
                            DoCastVictim(SPELL_MOONFIRE);
                            _events.ScheduleEvent(EVENT_MOONFIRE, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_STARFIRE:
                            DoCastVictim(SPELL_STARFIRE);
                            _events.ScheduleEvent(EVENT_STARFIRE, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_LIFEBLOOM:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_LIFEBLOOM);
                            _events.ScheduleEvent(EVENT_LIFEBLOOM, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_NOURISH:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_NOURISH);
                            _events.ScheduleEvent(EVENT_NOURISH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_REGROWTH:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_REGROWTH);
                            _events.ScheduleEvent(EVENT_REGROWTH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_REJUVENATION:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_REJUVENATION);
                            _events.ScheduleEvent(EVENT_REJUVENATION, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_TRANQUILITY:
                            DoCastAOE(SPELL_TRANQUILITY);
                            _events.ScheduleEvent(EVENT_TRANQUILITY, urand(15*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_BARKSKIN:
                            if (HealthBelowPct(30))
                            {
                                DoCast(me, SPELL_BARKSKIN);
                                _events.RescheduleEvent(EVENT_BARKSKIN, 60*IN_MILLISECONDS);
                            }
                            else
                                _events.RescheduleEvent(EVENT_BARKSKIN, 3*IN_MILLISECONDS);
                            return;
                        case EVENT_THORNS:
                            DoCast(me, SPELL_THORNS);
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_THORNS);
                            _events.ScheduleEvent(EVENT_THORNS, urand(25*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_NATURE_GRASP:
                            DoCast(me, SPELL_NATURE_GRASP);
                            _events.ScheduleEvent(EVENT_NATURE_GRASP, 60*IN_MILLISECONDS);
                            return;
                        case EVENT_THREAT_DRUID:
                        {
                            me->ModifyPower(POWER_MANA, me->GetMaxPower(POWER_MANA) / 3);
                            std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                            {
                                Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
                                if (unit && me->getThreatManager().getThreat(unit))
                                {
                                    if (unit->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        float dist_mod = 1.0f;
                                        float armor_mod = 0.0f;
                                        float eh = (unit->GetHealth() + 1) * (1.0f + armor_mod);
                                        float threat = dist_mod * 30000.0f / eh;
                                        me->getThreatManager().modifyThreatPercent(unit, -100);
                                        me->AddThreat(unit, 1000000.0f * threat);
                                    }
                                }
                            }
                            _events.ScheduleEvent(EVENT_THREAT_DRUID, 4*IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_GROWTH_DRUID:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_GROWTH_DRUID, 30000);
                            break;
                        case EVENT_CAPTURE_SOUL_DRUID:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_CAPTURE_SOUL_DRUID, 10000);
                            break;
                        default:
                            return;
                    }
                }
                DoSpellAttackIfReady(SPELL_WRATH);
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss6AI(creature);
        }
};

class boss_worldboss7 : public CreatureScript
{
    public:
        boss_worldboss7() : CreatureScript("boss_worldboss7") { }

        struct boss_worldboss7AI : public ScriptedAI
        {
            boss_worldboss7AI(Creature* creature) : ScriptedAI(creature) { }

            bool WorldBoss7Dead;

            void Reset() override
            {
                
                if (!me->IsInCombat())
                {
                    _events.Reset();
                    _events.ScheduleEvent(EVENT_FAN_OF_KNIVES, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_BLIND, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_CLOAK, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_BLADE_FLURRY, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_SHADOWSTEP, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_HEMORRHAGE, urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_EVISCERATE, urand(20*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_WOUND_POISON, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_THREAT_ROGUE, 5*IN_MILLISECONDS);
                    _events.ScheduleEvent(EVENT_GROWTH_ROGUE, 30000);
                    _events.ScheduleEvent(EVENT_CAPTURE_SOUL_ROGUE, 900000);
                    SetEquipmentSlots(false, 32471, 32471, EQUIP_NO_CHANGE);
                    me->setPowerType(POWER_ENERGY);
                    me->SetMaxPower(POWER_ENERGY, 100);
                    WorldBoss7Dead = false;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_WORLDBOSS7:
                        if (WorldBoss7Dead)
                            return 1;
                        break;
                }
                return 0;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
            } 

            void JustRespawned() override
            {
                float x, y, z, o;
                me->GetHomePosition(x, y, z, o);
                DoTeleportTo(x, y, z);
                
            }

            void EnterCombat(Unit* who) override
            {
                DoCast(me, SPELL_ANTI_AOE, true);
               
                WorldBoss7Dead = false;
            }

            void JustDied(Unit* /*killer*/) override
            {
                
                WorldBoss7Dead = true;
            }

            void JustReachedHome()
            {
                
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

               

                _events.Update(diff);

                if (me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FAN_OF_KNIVES:
                            DoCastAOE(SPELL_FAN_OF_KNIVES);
                            _events.ScheduleEvent(EVENT_FAN_OF_KNIVES, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_BLIND:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_BLIND);
                            _events.ScheduleEvent(EVENT_BLIND, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_CLOAK:
                            if (HealthBelowPct(50))
                            {
                                DoCast(SPELL_CLOAK);
                                _events.RescheduleEvent(EVENT_CLOAK, 90*IN_MILLISECONDS);
                            }
                            else
                                _events.RescheduleEvent(EVENT_CLOAK, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_BLADE_FLURRY:
                            DoCast(SPELL_BLADE_FLURRY);
                            _events.RescheduleEvent(EVENT_BLADE_FLURRY, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_SHADOWSTEP:
                            if (Unit* target = me->GetVictim())
                            {
                                if (me->IsInRange(target, 10.0f, 40.0f, false))
                                {
                                    DoCast(target, SPELL_SHADOWSTEP);
                                    _events.RescheduleEvent(EVENT_SHADOWSTEP, 30*IN_MILLISECONDS);
                                    return;
                                }
                            }
                            _events.RescheduleEvent(EVENT_SHADOWSTEP, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_HEMORRHAGE:
                            DoCastVictim(SPELL_HEMORRHAGE);
                            _events.ScheduleEvent(EVENT_HEMORRHAGE, urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_EVISCERATE:
                            DoCastVictim(SPELL_EVISCERATE);
                            _events.ScheduleEvent(EVENT_EVISCERATE, urand(30*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_WOUND_POISON:
                            DoCastVictim(SPELL_WOUND_POISON);
                            _events.ScheduleEvent(EVENT_WOUND_POISON, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_THREAT_ROGUE:
                        {
                            std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                            {
                                Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
                                if (unit && me->getThreatManager().getThreat(unit))
                                {
                                    if (unit->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        float dist_mod = 15.0f / (15.0f + me->GetDistance2d(unit));
                                        float armor_mod = (float)unit->GetArmor() / 16635.0f;
                                        float eh = (unit->GetHealth() + 1) * (1.0f + armor_mod);
                                        float threat = dist_mod * 30000.0f / eh;
                                        me->getThreatManager().modifyThreatPercent(unit, -100);
                                        me->AddThreat(unit, 1000000.0f * threat);
                                    }
                                }
                            }
                            _events.ScheduleEvent(EVENT_THREAT_ROGUE, 4*IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_GROWTH_ROGUE:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_GROWTH_ROGUE, 30000);
                            break;
                        case EVENT_CAPTURE_SOUL_ROGUE:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_CAPTURE_SOUL_ROGUE, 10000);
                            break;
                        default:
                            return;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss7AI(creature);
        }
};

class boss_worldboss8 : public CreatureScript
{
    public:
        boss_worldboss8() : CreatureScript("boss_worldboss8") { }

        struct boss_worldboss8AI : public ScriptedAI
        {
            boss_worldboss8AI(Creature* creature) : ScriptedAI(creature) { }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_AVENGING_WRATH, urand(25*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_CRUSADER_STRIKE, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_DIVINE_STORM, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_HAMMER_OF_JUSTICE_RET, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_JUDGEMENT_OF_COMMAND, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_REPENTANCE, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_DPS_HAND_OF_PROTECTION, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_DPS_DIVINE_SHIELD, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_HAND_OF_FREEDOM, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_HEAL_DIVINE_SHIELD, 20*IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_CLEANSE, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_FLASH_OF_LIGHT, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_HOLY_LIGHT, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_HOLY_SHOCK, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_HEAL_HAND_OF_PROTECTION, urand(30*IN_MILLISECONDS, 60*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_HAMMER_OF_JUSTICE, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_THREAT_PALADIN, 5*IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_GROWTH_PALADIN, 30000);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领古拉巴什竞技场的土豪·金出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                if (!Announced)
                {
                    char message[1024];
                    
                    if (who->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (who->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领古拉巴什竞技场的土豪·金正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领古拉巴什竞技场的土豪·金正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->GetTypeId() == TYPEID_UNIT)
                    {
                        if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                        {
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领古拉巴什竞技场的土豪·金正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领古拉巴什竞技场的土豪·金正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        if (who->IsPet())
                        {
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领古拉巴什竞技场的土豪·金正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领古拉巴什竞技场的土豪·金正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领古拉巴什竞技场的土豪·金正在遭到攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领古拉巴什竞技场的土豪·金正在遭到攻击！　|r");
                        
                    }
                    sWorld->SendGlobalText(message, NULL);
                    Announced = true;
                }
                DoCast(me, SPELL_ANTI_AOE, true);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void JustReachedHome()
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

              

                _events.Update(diff);

                if (me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_AVENGING_WRATH:
                            DoCast(SPELL_AVENGING_WRATH);
                            _events.ScheduleEvent(EVENT_AVENGING_WRATH, 180*IN_MILLISECONDS);
                            return;
                        case EVENT_CRUSADER_STRIKE:
                            DoCastVictim(SPELL_CRUSADER_STRIKE);
                            _events.ScheduleEvent(EVENT_CRUSADER_STRIKE, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_DIVINE_STORM:
                            DoCast(SPELL_DIVINE_STORM);
                            _events.ScheduleEvent(EVENT_DIVINE_STORM, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_HAMMER_OF_JUSTICE_RET:
                            DoCastVictim(SPELL_HAMMER_OF_JUSTICE_RET);
                            _events.ScheduleEvent(EVENT_HAMMER_OF_JUSTICE_RET, 40*IN_MILLISECONDS);
                            return;
                        case EVENT_JUDGEMENT_OF_COMMAND:
                            DoCastVictim(SPELL_JUDGEMENT_OF_COMMAND);
                            _events.ScheduleEvent(EVENT_JUDGEMENT_OF_COMMAND, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_REPENTANCE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                            {
                                DoCast(target, SPELL_REPENTANCE);
                                if (target->GetTypeId() == TYPEID_PLAYER)
                                    target->CastSpell(target, 90015, true);
                            }
                            _events.ScheduleEvent(EVENT_REPENTANCE, 60*IN_MILLISECONDS);
                            return;
                        case EVENT_DPS_HAND_OF_PROTECTION:
                            if (Unit* target = DoSelectLowestHpFriendly(30.0f))
                            {
                                if (!target->HasAura(SPELL_FORBEARANCE))
                                {
                                    DoCast(target, SPELL_HAND_OF_PROTECTION);
                                    _events.RescheduleEvent(EVENT_DPS_HAND_OF_PROTECTION, 5*MINUTE*IN_MILLISECONDS);
                                }
                                else
                                    _events.RescheduleEvent(EVENT_DPS_HAND_OF_PROTECTION, 5*IN_MILLISECONDS);
                            }
                            else
                                _events.RescheduleEvent(EVENT_DPS_HAND_OF_PROTECTION, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_DPS_DIVINE_SHIELD:
                            if (HealthBelowPct(30) && !me->HasAura(SPELL_FORBEARANCE))
                            {
                                DoCast(me, SPELL_DIVINE_SHIELD);
                                _events.RescheduleEvent(EVENT_DPS_DIVINE_SHIELD, 5*MINUTE*IN_MILLISECONDS);
                            }
                            else
                                _events.RescheduleEvent(EVENT_DPS_DIVINE_SHIELD, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_HAND_OF_FREEDOM:
                            DoCast(me, SPELL_HAND_OF_FREEDOM);
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_HAND_OF_FREEDOM);
                            _events.ScheduleEvent(EVENT_HAND_OF_FREEDOM, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_DIVINE_SHIELD:
                            if (HealthBelowPct(30) && !me->HasAura(SPELL_FORBEARANCE))
                            {
                                DoCast(me, SPELL_DIVINE_SHIELD);
                                _events.RescheduleEvent(EVENT_HEAL_DIVINE_SHIELD, 5*MINUTE*IN_MILLISECONDS);
                            }
                            else
                                _events.RescheduleEvent(EVENT_HEAL_DIVINE_SHIELD, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_CLEANSE:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_CLEANSE);
                            _events.ScheduleEvent(EVENT_CLEANSE, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_FLASH_OF_LIGHT:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_FLASH_OF_LIGHT);
                            _events.ScheduleEvent(EVENT_FLASH_OF_LIGHT, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            return;
                        case EVENT_HOLY_LIGHT:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_HOLY_LIGHT);
                            _events.ScheduleEvent(EVENT_HOLY_LIGHT, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_HOLY_SHOCK:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_HOLY_SHOCK);
                            _events.ScheduleEvent(EVENT_HOLY_SHOCK, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_HAND_OF_PROTECTION:
                            if (Unit* target = DoSelectLowestHpFriendly(30.0f))
                            {
                                if (!target->HasAura(SPELL_FORBEARANCE))
                                {
                                    DoCast(target, SPELL_HAND_OF_PROTECTION);
                                    _events.RescheduleEvent(EVENT_HEAL_HAND_OF_PROTECTION, 5*MINUTE*IN_MILLISECONDS);
                                }
                                else
                                    _events.RescheduleEvent(EVENT_HEAL_HAND_OF_PROTECTION, 3*IN_MILLISECONDS);
                            }
                            else
                                _events.RescheduleEvent(EVENT_HEAL_HAND_OF_PROTECTION, 10*IN_MILLISECONDS);
                            return;
                        case EVENT_HAMMER_OF_JUSTICE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 15.0f, true))
                            {
                                DoCast(target, SPELL_HAMMER_OF_JUSTICE);
                                if (target->GetTypeId() == TYPEID_PLAYER)
                                    target->CastSpell(target, 90015, true);
                            }
                            _events.ScheduleEvent(EVENT_HAMMER_OF_JUSTICE, 40*IN_MILLISECONDS);
                            return;
                        case EVENT_THREAT_PALADIN:
                        {
                            me->ModifyPower(POWER_MANA, me->GetMaxPower(POWER_MANA) / 3);
                            std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                            {
                                Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
                                if (unit && me->getThreatManager().getThreat(unit))
                                {
                                    if (unit->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        float dist_mod = 15.0f / (15.0f + me->GetDistance2d(unit));
                                        float armor_mod = (float)unit->GetArmor() / 16635.0f;
                                        float eh = (unit->GetHealth() + 1) * (1.0f + armor_mod);
                                        float threat = dist_mod * 30000.0f / eh;
                                        me->getThreatManager().modifyThreatPercent(unit, -100);
                                        me->AddThreat(unit, 1000000.0f * threat);
                                    }
                                }
                            }
                            _events.ScheduleEvent(EVENT_THREAT_PALADIN, 4*IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_GROWTH_PALADIN:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_GROWTH_PALADIN, 30000);
                            break;
                        default:
                            return;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss8AI(creature);
        }
};

class boss_worldboss9 : public CreatureScript
{
    public:
        boss_worldboss9() : CreatureScript("boss_worldboss9") { }

        struct boss_worldboss9AI : public ScriptedAI
        {
            boss_worldboss9AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_FEL_FIREBALL, 5*IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_FEL_LIGHTNING, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_INCINERATE_FLESH, urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_NETHER_POWER, 40*IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_LEGION_FLAME, 30*IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_SUMMONO_NETHER_PORTAL, 20*IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_SUMMON_INFERNAL_ERUPTION, 80*IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_HELLFIRE, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_CORRUPTION, urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_CURSE_OF_AGONY, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_CURSE_OF_EXHAUSTION, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_FEAR, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_SEARING_PAIN, urand(5*IN_MILLISECONDS, 12*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_UNSTABLE_AFFLICTION, urand(7*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_THREAT_WORLDBOSS9, 5*IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_GROWTH_WORLDBOSS9, 30000);
                _events.ScheduleEvent(EVENT_CAPTURE_SOUL_WORLDBOSS9, 900000);
                SetEquipmentSlots(false, 45176, 45177, EQUIP_NO_CHANGE);
                me->setFaction(7);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的看你妹出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                if (!Announced)
                {
                    char message[1024];
                    
                    if (who->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (who->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的看你妹正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的看你妹正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->GetTypeId() == TYPEID_UNIT)
                    {
                        if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                        {
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的看你妹正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的看你妹正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        if (who->IsPet())
                        {
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的看你妹正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的看你妹正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的看你妹正在遭到攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的看你妹正在遭到攻击！　|r");
                        
                    }
                    sWorld->SendGlobalText(message, NULL);
                    Announced = true;
                }
                SetEquipmentSlots(false, 22691, 13262, EQUIP_NO_CHANGE);
                me->setFaction(14);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if ((me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f) || me->GetDistance(me->GetVictim()) < 0.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FEL_FIREBALL:
                            DoCastVictim(SPELL_FEL_FIREBALL);
                            _events.ScheduleEvent(EVENT_FEL_FIREBALL, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            break;
                        case EVENT_FEL_LIGHTNING:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true, -SPELL_LORD_HITTIN))
                                DoCast(target, SPELL_FEL_LIGHTING);
                            _events.ScheduleEvent(EVENT_FEL_LIGHTNING, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            break;
                        case EVENT_INCINERATE_FLESH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true, -SPELL_LORD_HITTIN))
                                DoCast(target, SPELL_INCINERATE_FLESH);
                            _events.ScheduleEvent(EVENT_INCINERATE_FLESH, urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            break;
                        case EVENT_NETHER_POWER:
                            me->CastCustomSpell(SPELL_NETHER_POWER, SPELLVALUE_AURA_STACK, 10, me, true);
                            _events.ScheduleEvent(EVENT_NETHER_POWER, 40*IN_MILLISECONDS);
                            break;
                        case EVENT_LEGION_FLAME:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true, -SPELL_LORD_HITTIN))
                                DoCast(target, SPELL_LEGION_FLAME);
                            _events.ScheduleEvent(EVENT_LEGION_FLAME, 30*IN_MILLISECONDS);
                            break;
                        case EVENT_SUMMONO_NETHER_PORTAL:
                            DoCast(SPELL_NETHER_PORTAL);
                            _events.ScheduleEvent(EVENT_SUMMONO_NETHER_PORTAL, 2*MINUTE*IN_MILLISECONDS);
                            break;
                        case EVENT_SUMMON_INFERNAL_ERUPTION:
                            DoCast(SPELL_INFERNAL_ERUPTION);
                            _events.ScheduleEvent(EVENT_SUMMON_INFERNAL_ERUPTION, 2*MINUTE*IN_MILLISECONDS);
                            break;
                        case EVENT_HELLFIRE:
                            DoCastAOE(SPELL_HELLFIRE);
                            _events.ScheduleEvent(EVENT_HELLFIRE, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_CORRUPTION:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f))
                                DoCast(target, SPELL_CORRUPTION);
                            _events.ScheduleEvent(EVENT_CORRUPTION, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_CURSE_OF_AGONY:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f))
                                DoCast(target, SPELL_CURSE_OF_AGONY);
                            _events.ScheduleEvent(EVENT_CURSE_OF_AGONY, urand(20*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_CURSE_OF_EXHAUSTION:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f))
                                DoCast(target, SPELL_CURSE_OF_EXHAUSTION);
                            _events.ScheduleEvent(EVENT_CURSE_OF_EXHAUSTION, urand(20*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_FEAR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 20.0f, true))
                                DoCast(target, SPELL_FEAR);
                            _events.ScheduleEvent(EVENT_FEAR, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_SEARING_PAIN:
                            DoCastVictim(SPELL_SEARING_PAIN);
                            _events.ScheduleEvent(EVENT_SEARING_PAIN, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_UNSTABLE_AFFLICTION:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true))
                                DoCast(target, SPELL_UNSTABLE_AFFLICTION);
                            _events.ScheduleEvent(EVENT_UNSTABLE_AFFLICTION, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_THREAT_WORLDBOSS9:
                        {
                            std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                            {
                                Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
                                if (unit && me->getThreatManager().getThreat(unit))
                                {
                                    if (unit->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        float dist_mod = 1.0f;
                                        float armor_mod = 0.0f;
                                        float eh = (unit->GetHealth() + 1) * (1.0f + armor_mod);
                                        float threat = dist_mod * 30000.0f / eh;
                                        me->getThreatManager().modifyThreatPercent(unit, -100);
                                        me->AddThreat(unit, 1000000.0f * threat);
                                    }
                                }
                            }
                            _events.ScheduleEvent(EVENT_THREAT_WORLDBOSS9, 4*IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_GROWTH_WORLDBOSS9:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_GROWTH_WORLDBOSS9, 30000);
                            break;
                        case EVENT_CAPTURE_SOUL_WORLDBOSS9:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_CAPTURE_SOUL_WORLDBOSS9, 10000);
                            break;
                    }
                }

                DoSpellAttackIfReady(SPELL_SHADOW_BOLT);
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss9AI(creature);
        }
};

class boss_worldboss10 : public CreatureScript
{
    public:
        boss_worldboss10() : CreatureScript("boss_worldboss10") { }

        struct boss_worldboss10AI : public ScriptedAI
        {
            boss_worldboss10AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_ARCANE_BARRAGE, urand(1*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_ARCANE_BLAST, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_ARCANE_EXPLOSION, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_BLINK, urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_COUNTERSPELL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_FROST_NOVA, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_ICE_BLOCK, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_POLYMORPH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_SILENCE, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_VAMPIRIC_TOUCH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_SW_PAIN, urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_MIND_BLAST, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_HORROR, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_DISPERSION, urand(20*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_DPS_DISPEL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_DPS_PSYCHIC_SCREAM, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_RENEW, urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_SHIELD, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_FLASH_HEAL, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_HEAL_DISPEL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_HEAL_PSYCHIC_SCREAM, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_MANA_BURN, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_PENANCE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_FROZEN_ORB, 11000);
                _events.ScheduleEvent(EVENT_WHITEOUT, 13000);
                _events.ScheduleEvent(EVENT_FREEZING_GROUND, 15000);
                _events.ScheduleEvent(EVENT_BLIZZARD, urand(5, 10) * IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_SEDUCTION, urand(5, 10) * IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_THREAT_WORLDBOSS10, 5*IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_GROWTH_WORLDBOSS10, 30000);
                _events.ScheduleEvent(EVENT_CAPTURE_SOUL_WORLDBOSS10, 900000);
                SetEquipmentSlots(false, 7713, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
                me->setFaction(7);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的黑寡妇出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                if (!Announced)
                {
                    char message[1024];
                    
                    if (who->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (who->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的黑寡妇正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的黑寡妇正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->GetTypeId() == TYPEID_UNIT)
                    {
                        if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                        {
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的黑寡妇正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的黑寡妇正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        if (who->IsPet())
                        {
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的黑寡妇正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的黑寡妇正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的黑寡妇正在遭到攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领卡拉赞墓穴的黑寡妇正在遭到攻击！　|r");
                        
                    }
                    sWorld->SendGlobalText(message, NULL);
                    Announced = true;
                }
                DoCast(me, SPELL_SHADOWFORM);
                DoCast(me, SPELL_FROZEN_MALLET);
                SetEquipmentSlots(false, 18873, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
                me->setFaction(14);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if ((me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f) || me->GetDistance(me->GetVictim()) < 0.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARCANE_BARRAGE:
                            DoCastVictim(SPELL_ARCANE_BARRAGE);
                            _events.ScheduleEvent(EVENT_ARCANE_BARRAGE, urand(5*IN_MILLISECONDS, 7*IN_MILLISECONDS));
                            return;
                        case EVENT_ARCANE_BLAST:
                            DoCastVictim(SPELL_ARCANE_BLAST);
                            _events.ScheduleEvent(EVENT_ARCANE_BLAST, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_ARCANE_EXPLOSION:
                            DoCastAOE(SPELL_ARCANE_EXPLOSION);
                            _events.ScheduleEvent(EVENT_ARCANE_EXPLOSION, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_BLINK:
                            DoCast(SPELL_BLINK);
                            _events.ScheduleEvent(EVENT_BLINK, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_COUNTERSPELL:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_COUNTERSPELL);
                            _events.ScheduleEvent(EVENT_COUNTERSPELL, 24*IN_MILLISECONDS);
                            return;
                        case EVENT_FROST_NOVA:
                            DoCastAOE(SPELL_FROST_NOVA);
                            _events.ScheduleEvent(EVENT_FROST_NOVA, 25*IN_MILLISECONDS);
                            return;
                        case EVENT_ICE_BLOCK:
                            if (HealthBelowPct(30))
                            {
                                DoCast(SPELL_ICE_BLOCK);
                                _events.RescheduleEvent(EVENT_ICE_BLOCK, 5*MINUTE*IN_MILLISECONDS);
                            }
                            else
                                _events.RescheduleEvent(EVENT_ICE_BLOCK, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_POLYMORPH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_POLYMORPH);
                            _events.ScheduleEvent(EVENT_POLYMORPH, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_SILENCE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_SILENCE);
                            _events.ScheduleEvent(EVENT_SILENCE, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_VAMPIRIC_TOUCH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true))
                                DoCast(target, SPELL_VAMPIRIC_TOUCH);
                            _events.ScheduleEvent(EVENT_VAMPIRIC_TOUCH, urand(10*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_SW_PAIN:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                                DoCast(target, SPELL_SW_PAIN);
                            _events.ScheduleEvent(EVENT_SW_PAIN, urand(10*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_MIND_BLAST:
                            DoCastVictim(SPELL_MIND_BLAST);
                            _events.ScheduleEvent(EVENT_MIND_BLAST, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_HORROR:
                            DoCastVictim(SPELL_HORROR);
                            _events.ScheduleEvent(EVENT_HORROR, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_DISPERSION:
                            if (HealthBelowPct(40))
                            {
                                DoCast(me, SPELL_DISPERSION);
                                _events.RescheduleEvent(EVENT_DISPERSION, 180*IN_MILLISECONDS);
                            }
                            else
                                _events.RescheduleEvent(EVENT_DISPERSION, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_DPS_DISPEL:
                            if (Unit* target = urand(0, 1) ? SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true) : DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_DISPEL);
                            _events.ScheduleEvent(EVENT_DPS_DISPEL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_DPS_PSYCHIC_SCREAM:
                            DoCastAOE(SPELL_PSYCHIC_SCREAM);
                            _events.ScheduleEvent(EVENT_DPS_PSYCHIC_SCREAM, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_RENEW:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_RENEW);
                            _events.ScheduleEvent(EVENT_RENEW, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            return;
                        case EVENT_SHIELD:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_SHIELD);
                            _events.ScheduleEvent(EVENT_SHIELD, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_FLASH_HEAL:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_FLASH_HEAL);
                            _events.ScheduleEvent(EVENT_FLASH_HEAL, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_DISPEL:
                            if (Unit* target = urand(0, 1) ? SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true) : DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_DISPEL);
                            _events.ScheduleEvent(EVENT_HEAL_DISPEL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_PSYCHIC_SCREAM:
                            DoCastAOE(SPELL_PSYCHIC_SCREAM);
                            _events.ScheduleEvent(EVENT_HEAL_PSYCHIC_SCREAM, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_MANA_BURN:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_MANA_BURN);
                            _events.ScheduleEvent(EVENT_MANA_BURN, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_PENANCE:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_PENANCE);
                            _events.ScheduleEvent(EVENT_PENANCE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_FROZEN_ORB:
                            me->CastCustomSpell(SPELL_FROZEN_ORB, SPELLVALUE_MAX_TARGETS, 1, me);
                            _events.ScheduleEvent(EVENT_FROZEN_ORB, 38000);
                            break;
                        case EVENT_WHITEOUT:
                            DoCast(me, SPELL_WHITEOUT);
                            _events.ScheduleEvent(EVENT_WHITEOUT, 38000);
                            break;
                        case EVENT_FREEZING_GROUND:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                                DoCast(target, SPELL_FREEZING_GROUND);
                            _events.ScheduleEvent(EVENT_FREEZING_GROUND, 20000);
                            break;
                        case EVENT_BLIZZARD:
                        {
                            //DoCastAOE(SPELL_SUMMON_BLIZZARD);
                            if (Creature* summon = DoSummon(16474, me, 0.0f, urand(25, 30) * IN_MILLISECONDS, TEMPSUMMON_TIMED_DESPAWN))
                                summon->GetMotionMaster()->MoveRandom(40);
                            _events.ScheduleEvent(EVENT_BLIZZARD, 7 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_SEDUCTION:
                            DoCastAOE(SPELL_SEDUCTION);
                            _events.ScheduleEvent(EVENT_SEDUCTION, 7 * IN_MILLISECONDS);
                            break;
                        case EVENT_THREAT_WORLDBOSS10:
                        {
                            me->ModifyPower(POWER_MANA, me->GetMaxPower(POWER_MANA) / 3);
                            std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                            {
                                Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
                                if (unit && me->getThreatManager().getThreat(unit))
                                {
                                    if (unit->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        float dist_mod = 1.0f;
                                        float armor_mod = 0.0f;
                                        float eh = (unit->GetHealth() + 1) * (1.0f + armor_mod);
                                        float threat = dist_mod * 30000.0f / eh;
                                        me->getThreatManager().modifyThreatPercent(unit, -100);
                                        me->AddThreat(unit, 1000000.0f * threat);
                                    }
                                }
                            }
                            _events.ScheduleEvent(EVENT_THREAT_WORLDBOSS10, 4*IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_GROWTH_WORLDBOSS10:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_GROWTH_WORLDBOSS10, 30000);
                            break;
                        case EVENT_CAPTURE_SOUL_WORLDBOSS10:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_CAPTURE_SOUL_WORLDBOSS10, 10000);
                            break;
                        default:
                            break;
                    }
                }

                DoSpellAttackIfReady(SPELL_FROSTBOLT_WORLDBOSS10);
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss10AI(creature);
        }
};

class boss_worldboss11 : public CreatureScript
{
    public:
        boss_worldboss11() : CreatureScript("boss_worldboss11") { }

        struct boss_worldboss11AI : public ScriptedAI
        {
            boss_worldboss11AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            uint32 BombTimer;
            uint32 BombSequenceTimer;
            uint32 BombCount;
            uint32 FlameWreathTimer;
            uint32 FlameWreathCheckTime;
            bool isBombing;
            bool Announced;
            ObjectGuid FireBombGUIDs[50];
            ObjectGuid FlameWreathTarget[3];
            float FWTargPosX[3];
            float FWTargPosY[3];

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_FLAME_WREATH, 25000);
                _events.ScheduleEvent(EVENT_FLAME_BREATH, 8000);
                _events.ScheduleEvent(EVENT_FLAMEBUFFET, 5000);
                _events.ScheduleEvent(EVENT_BURNING_FURY, 20000);
                _events.ScheduleEvent(EVENT_BURNING_BREATH, 15000);
                _events.ScheduleEvent(EVENT_METEOR_FISTS, 75000);
                _events.ScheduleEvent(EVENT_FLAME_CINDER, 30000);
                _events.ScheduleEvent(EVENT_THREAT_WORLDBOSS11, 5*IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_GROWTH_WORLDBOSS11, 30000);
                _events.ScheduleEvent(EVENT_CAPTURE_SOUL_WORLDBOSS11, 900000);
                BombTimer = 30000;
                BombSequenceTimer = 1000;
                BombCount = 0;
                isBombing = false;
                Announced = false;
                FlameWreathTimer = 0;
                FlameWreathCheckTime = 0;
                for (uint32 i = 0; i < 50; ++i)
                    FireBombGUIDs[i] = ObjectGuid::Empty;
            }

            void SpawnBombs()
            {
                float dx, dy;
                for (uint32 i = 0; i < 50; ++i)
                {
                    dx = float(irand(-60/2, 60/2));
                    dy = float(irand(-60/2, 60/2));

                    Creature* bomb = DoSpawnCreature(23920, dx, dy, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 30000);
                    if (bomb)
                        FireBombGUIDs[i] = bomb->GetGUID();
                }
                BombCount = 0;
            }

            void Boom()
            {
                std::list<Creature*> templist;
                float x, y, z;
                me->GetPosition(x, y, z);

                {
                    CellCoord pair(Trinity::ComputeCellCoord(x, y));
                    Cell cell(pair);
                    cell.SetNoCreate();

                    Trinity::AllCreaturesOfEntryInRange check(me, 23920, 100);
                    Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(me, templist, check);

                    TypeContainerVisitor<Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange>, GridTypeMapContainer> cSearcher(searcher);

                    cell.Visit(pair, cSearcher, *me->GetMap(), *me, me->GetGridActivationRange());
                }
                for (std::list<Creature*>::const_iterator i = templist.begin(); i != templist.end(); ++i)
                {
                   (*i)->CastSpell(*i, SPELL_FIRE_BOMB_DAMAGE, true);
                   (*i)->RemoveAllAuras();
                }
            }

            void HandleBombSequence()
            {
                if (BombCount < 50)
                {
                    if (Unit* FireBomb = ObjectAccessor::GetUnit(*me, FireBombGUIDs[BombCount]))
                    {
                        FireBomb->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        DoCast(FireBomb, SPELL_FIRE_BOMB_THROW, true);
                        FireBomb->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                    ++BombCount;
                    if (BombCount == 50)
                    {
                        BombSequenceTimer = 5000;
                    } else BombSequenceTimer = 100;
                }
                else
                {
                    Boom();
                    isBombing = false;
                    BombTimer = urand(10000, 20000);
                    me->RemoveAurasDueToSpell(SPELL_FIRE_BOMB_CHANNEL);
                }
            }

            void FlameWreathEffect()
            {
                std::vector<Unit*> targets;
                ThreatContainer::StorageType const &t_list = me->getThreatManager().getThreatList();

                if (t_list.empty())
                    return;

                //store the threat list in a different container
                for (ThreatContainer::StorageType::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                {
                    Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
                    //only on alive players
                    if (target && target->IsAlive() && target->GetTypeId() == TYPEID_PLAYER)
                        targets.push_back(target);
                }

                //cut down to size ifwe have more than 3 targets
                while (targets.size() > 3)
                    targets.erase(targets.begin()+rand32()%targets.size());

                uint32 i = 0;
                for (std::vector<Unit*>::const_iterator itr = targets.begin(); itr!= targets.end(); ++itr)
                {
                    if (*itr)
                    {
                        FlameWreathTarget[i] = (*itr)->GetGUID();
                        FWTargPosX[i] = (*itr)->GetPositionX();
                        FWTargPosY[i] = (*itr)->GetPositionY();
                        DoCast((*itr), SPELL_FLAME_WREATH, true);
                        ++i;
                    }
                }
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领纳格兰竞技场的鸡儿加蛋出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                if (!Announced)
                {
                    char message[1024];
                    
                    if (who->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (who->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领纳格兰竞技场的鸡儿加蛋正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领纳格兰竞技场的鸡儿加蛋正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->GetTypeId() == TYPEID_UNIT)
                    {
                        if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                        {
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领纳格兰竞技场的鸡儿加蛋正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领纳格兰竞技场的鸡儿加蛋正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        if (who->IsPet())
                        {
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领纳格兰竞技场的鸡儿加蛋正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领纳格兰竞技场的鸡儿加蛋正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领纳格兰竞技场的鸡儿加蛋正在遭到攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领纳格兰竞技场的鸡儿加蛋正在遭到攻击！　|r");
                        
                    }
                    sWorld->SendGlobalText(message, NULL);
                    Announced = true;
                }
                DoCast(me, SPELL_ANTI_AOE, true);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (isBombing)
                {
                    if (BombSequenceTimer <= diff)
                        HandleBombSequence();
                    else
                        BombSequenceTimer -= diff;
                    return;
                }

                if (!UpdateVictim())
                    return;

               

                if (BombTimer <= diff)
                {
                    float x, y, z, o;
                    me->AttackStop();
                    me->GetMotionMaster()->Clear();
                    me->GetHomePosition(x, y, z, o);
                    DoTeleportTo(x, y, z);
                    me->StopMoving();
                    DoCast(me, SPELL_FIRE_BOMB_CHANNEL, false);

                    SpawnBombs();
                    isBombing = true;
                    BombSequenceTimer = 100;

                    ThreatContainer::StorageType threatlist = me->getThreatManager().getThreatList();
                    ThreatContainer::StorageType::const_iterator i = threatlist.begin();
                    for (i = threatlist.begin(); i != threatlist.end(); ++i)
                    {
                        Unit* unit = ObjectAccessor::GetUnit(*me, (*i)->getUnitGuid());
                        if (unit && (unit->GetTypeId() == TYPEID_PLAYER))
                        {
                            DoTeleportPlayer(unit, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()+3, unit->GetOrientation());
                        }
                    }
                    return;
                } else BombTimer -= diff;

                _events.Update(diff);

                if ((me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f) || me->GetDistance(me->GetVictim()) < 0.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FLAME_WREATH:
                            FlameWreathTimer = 20000;
                            FlameWreathCheckTime = 500;
                            FlameWreathTarget[0] = ObjectGuid::Empty;
                            FlameWreathTarget[1] = ObjectGuid::Empty;
                            FlameWreathTarget[2] = ObjectGuid::Empty;
                            FlameWreathEffect();
                            _events.ScheduleEvent(EVENT_FLAME_WREATH, 25000);
                            break;
                        case EVENT_FLAME_BREATH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_FLAME_BREATH, false);
                            _events.ScheduleEvent(EVENT_FLAME_BREATH, 8000);
                            break;
                        case EVENT_FLAMEBUFFET:
                            DoCastVictim(SPELL_FLAMEBUFFET);
                            _events.ScheduleEvent(EVENT_FLAMEBUFFET, 5000);
                            break;
                        case EVENT_BURNING_FURY:
                            DoCast(me, SPELL_BURNING_FURY);
                            _events.ScheduleEvent(EVENT_BURNING_FURY, 20000);
                            break;
                        case EVENT_BURNING_BREATH:
                            DoCast(me, SPELL_BURNING_BREATH);
                            _events.ScheduleEvent(EVENT_BURNING_BREATH, 45000);
                            break;
                        case EVENT_METEOR_FISTS:
                            DoCast(me, SPELL_METEOR_FISTS);
                            _events.ScheduleEvent(EVENT_METEOR_FISTS, 45000);
                            break;
                        case EVENT_FLAME_CINDER:
                            DoCast(me, SPELL_FLAME_CINDER_A);
                            _events.ScheduleEvent(EVENT_FLAME_CINDER, 30000);
                            break;
                        case EVENT_THREAT_WORLDBOSS11:
                        {
                            std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                            {
                                Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
                                if (unit && me->getThreatManager().getThreat(unit))
                                {
                                    if (unit->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        float dist_mod = 1.0f;
                                        float armor_mod = 0.0f;
                                        float eh = (unit->GetHealth() + 1) * (1.0f + armor_mod);
                                        float threat = dist_mod * 30000.0f / eh;
                                        me->getThreatManager().modifyThreatPercent(unit, -100);
                                        me->AddThreat(unit, 1000000.0f * threat);
                                    }
                                }
                            }
                            _events.ScheduleEvent(EVENT_THREAT_WORLDBOSS11, 4*IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_GROWTH_WORLDBOSS11:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_GROWTH_WORLDBOSS11, 30000);
                            break;
                        case EVENT_CAPTURE_SOUL_WORLDBOSS11:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_CAPTURE_SOUL_WORLDBOSS11, 10000);
                            break;
                    }
                }

                //Flame Wreath check
                if (FlameWreathTimer)
                {
                    if (FlameWreathTimer >= diff)
                        FlameWreathTimer -= diff;
                    else FlameWreathTimer = 0;

                    if (FlameWreathCheckTime <= diff)
                    {
                        for (uint8 i = 0; i < 3; ++i)
                        {
                            if (!FlameWreathTarget[i])
                                continue;

                            Unit* unit = ObjectAccessor::GetUnit(*me, FlameWreathTarget[i]);
                            if (unit && !unit->IsWithinDist2d(FWTargPosX[i], FWTargPosY[i], 3))
                            {
                                unit->CastSpell(unit, 20476, true, 0, 0, me->GetGUID());
                                unit->CastSpell(unit, 11027, true);
                                FlameWreathTarget[i] = ObjectGuid::Empty;
                            }
                        }
                        FlameWreathCheckTime = 500;
                    } else FlameWreathCheckTime -= diff;
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss11AI(creature);
        }
};

class boss_worldboss12 : public CreatureScript
{
    public:
        boss_worldboss12() : CreatureScript("boss_worldboss12") { }

        struct boss_worldboss12AI : public ScriptedAI
        {
            boss_worldboss12AI(Creature* creature) : ScriptedAI(creature), _summons(me) { }

            bool WorldBoss12Dead;

            void Reset() override
            {
               
                if (!me->IsInCombat())
                {
                    _events.Reset();
                    _events.ScheduleEvent(EVENT_HEALING_WAVE, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_RIPTIDE, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_SPIRIT_CLEANSE, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_BLOODLUST_HEROISM, 20*IN_MILLISECONDS);
                    _events.ScheduleEvent(EVENT_HEX, urand(5*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_EARTH_SHIELD, 1*IN_MILLISECONDS);
                    _events.ScheduleEvent(EVENT_EARTH_SHOCK, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_LAVA_LASH, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_STORMSTRIKE, urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_DEPLOY_TOTEM, 1*IN_MILLISECONDS);
                    _events.ScheduleEvent(EVENT_WINDFURY, urand(20*IN_MILLISECONDS, 50*IN_MILLISECONDS));
                    _events.ScheduleEvent(EVENT_THREAT_SHAMAN, 5*IN_MILLISECONDS);
                    _events.ScheduleEvent(EVENT_GROWTH_SHAMAN, 30000);
                    _events.ScheduleEvent(EVENT_CAPTURE_SOUL_SHAMAN, 900000);
                    _totemCount = 0;
                    _totemOldCenterX = me->GetPositionX();
                    _totemOldCenterY = me->GetPositionY();
                    SetEquipmentSlots(false, 32946, 32946, EQUIP_NO_CHANGE);
                    WorldBoss12Dead = false;
                    _summons.DespawnAll();
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_WORLDBOSS12:
                        if (WorldBoss12Dead)
                            return 1;
                        break;
                }
                return 0;
            }

            void JustSummoned(Creature* summoned) override
            {
                _summons.Summon(summoned);
            }

            void SummonedCreatureDespawn(Creature* /*pSummoned*/) override
            {
                --_totemCount;
            }

            void DeployTotem()
            {
                _totemCount = 4;
                _totemOldCenterX = me->GetPositionX();
                _totemOldCenterY = me->GetPositionY();
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
            } 

            void JustRespawned() override
            {
                float x, y, z, o;
                me->GetHomePosition(x, y, z, o);
                DoTeleportTo(x, y, z);
               
            }

            void EnterCombat(Unit* who) override
            {
                DoCast(me, SPELL_ANTI_AOE, true);
               
                WorldBoss12Dead = false;
            }

            void JustDied(Unit* /*killer*/) override
            {
              
                _summons.DespawnAll();
                WorldBoss12Dead = true;
            }

            void JustReachedHome()
            {
                
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

               

                _events.Update(diff);

                if (me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HEALING_WAVE:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_HEALING_WAVE);
                            _events.ScheduleEvent(EVENT_HEALING_WAVE, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            return;
                        case EVENT_RIPTIDE:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_RIPTIDE);
                            _events.ScheduleEvent(EVENT_RIPTIDE, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_SPIRIT_CLEANSE:
                            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                                DoCast(target, SPELL_SPIRIT_CLEANSE);
                            _events.ScheduleEvent(EVENT_SPIRIT_CLEANSE, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_BLOODLUST_HEROISM:
                            if (me->getFaction()) // alliance = 1
                            {
                                if (!me->HasAura(AURA_EXHAUSTION))
                                    DoCastAOE(SPELL_HEROISM);
                            }
                            else
                            {
                                if (!me->HasAura(AURA_SATED))
                                    DoCastAOE(SPELL_BLOODLUST);
                            }
                            _events.ScheduleEvent(EVENT_BLOODLUST_HEROISM, 5*MINUTE*IN_MILLISECONDS);
                            return;
                        case EVENT_HEX:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_HEX);
                            _events.ScheduleEvent(EVENT_HEX, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_EARTH_SHIELD:
                            DoCast(me, SPELL_EARTH_SHIELD);
                            _events.ScheduleEvent(EVENT_EARTH_SHIELD, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_EARTH_SHOCK:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_EARTH_SHOCK);
                            _events.ScheduleEvent(EVENT_EARTH_SHOCK, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_LAVA_LASH:
                            DoCastVictim(SPELL_LAVA_LASH);
                            _events.ScheduleEvent(EVENT_LAVA_LASH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_STORMSTRIKE:
                            DoCastVictim(SPELL_STORMSTRIKE);
                            _events.ScheduleEvent(EVENT_STORMSTRIKE, urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_DEPLOY_TOTEM:
                            if (_totemCount < 4 || me->GetDistance2d(_totemOldCenterX, _totemOldCenterY) > 20.0f)
                                DeployTotem();
                            _events.ScheduleEvent(EVENT_DEPLOY_TOTEM, 1*IN_MILLISECONDS);
                            return;
                        case EVENT_WINDFURY:
                            DoCastVictim(SPELL_WINDFURY);
                            _events.ScheduleEvent(EVENT_WINDFURY, urand(20*IN_MILLISECONDS, 60*IN_MILLISECONDS));
                            return;
                        case EVENT_THREAT_SHAMAN:
                        {
                            me->ModifyPower(POWER_MANA, me->GetMaxPower(POWER_MANA) / 3);
                            std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                            {
                                Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
                                if (unit && me->getThreatManager().getThreat(unit))
                                {
                                    if (unit->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        float dist_mod = 1.0f;
                                        float armor_mod = 0.0f;
                                        float eh = (unit->GetHealth() + 1) * (1.0f + armor_mod);
                                        float threat = dist_mod * 30000.0f / eh;
                                        me->getThreatManager().modifyThreatPercent(unit, -100);
                                        me->AddThreat(unit, 1000000.0f * threat);
                                    }
                                }
                            }
                            _events.ScheduleEvent(EVENT_THREAT_SHAMAN, 4*IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_GROWTH_SHAMAN:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_GROWTH_SHAMAN, 30000);
                            break;
                        case EVENT_CAPTURE_SOUL_SHAMAN:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_CAPTURE_SOUL_SHAMAN, 10000);
                            break;
                        default:
                            return;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
            SummonList _summons;
            uint8  _totemCount;
            float  _totemOldCenterX, _totemOldCenterY;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss12AI(creature);
        }
};

class boss_worldboss13 : public CreatureScript
{
    public:
        boss_worldboss13() : CreatureScript("boss_worldboss13") { }

        struct boss_worldboss13AI : public ScriptedAI
        {
            boss_worldboss13AI(Creature* creature) : ScriptedAI(creature) { }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_CHAINS_OF_ICE, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_DEATH_COIL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_DEATH_GRIP, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_FROST_STRIKE, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_ICEBOUND_FORTITUDE, urand(25*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_ICY_TOUCH, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_STRANGULATE, urand(5*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_AIMED_SHOT, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_DETERRENCE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_DISENGAGE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_EXPLOSIVE_SHOT, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_FROST_TRAP, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_STEADY_SHOT, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_WING_CLIP, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_WYVERN_STING, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_THREAT_DEATHKNIGHT, 5*IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_GROWTH_DEATHKNIGHT, 30000);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领鲜血之环竞技场的屌丝·银出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                if (!Announced)
                {
                    char message[1024];
                    
                    if (who->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (who->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领鲜血之环竞技场的屌丝·银正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领鲜血之环竞技场的屌丝·银正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->GetTypeId() == TYPEID_UNIT)
                    {
                        if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                        {
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领鲜血之环竞技场的屌丝·银正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领鲜血之环竞技场的屌丝·银正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        if (who->IsPet())
                        {
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领鲜血之环竞技场的屌丝·银正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领鲜血之环竞技场的屌丝·银正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领鲜血之环竞技场的屌丝·银正在遭到攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领鲜血之环竞技场的屌丝·银正在遭到攻击！　|r");
                        
                    }
                    sWorld->SendGlobalText(message, NULL);
                    Announced = true;
                }
                DoCast(me, SPELL_ANTI_AOE, true);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void JustReachedHome()
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

              

                _events.Update(diff);

                if (me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHAINS_OF_ICE:
                            DoCastVictim(SPELL_CHAINS_OF_ICE);
                            _events.ScheduleEvent(EVENT_CHAINS_OF_ICE, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_DEATH_COIL:
                            DoCastVictim(SPELL_DEATH_COIL);
                            _events.ScheduleEvent(EVENT_DEATH_COIL, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_DEATH_GRIP:
                            if (Unit* target = me->GetVictim())
                            {
                                if (me->IsInRange(target, 5.0f, 30.0f, false))
                                {
                                    DoCast(target, SPELL_DEATH_GRIP);
                                    _events.RescheduleEvent(EVENT_DEATH_GRIP, 35*IN_MILLISECONDS);
                                    return;
                                }
                            }
                            _events.RescheduleEvent(EVENT_DEATH_GRIP, 3*IN_MILLISECONDS);
                            return;
                        case EVENT_FROST_STRIKE:
                            DoCastVictim(SPELL_FROST_STRIKE);
                            _events.ScheduleEvent(EVENT_FROST_STRIKE, urand(6*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_ICEBOUND_FORTITUDE:
                            if (HealthBelowPct(50))
                            {
                                DoCast(SPELL_ICEBOUND_FORTITUDE);
                                _events.RescheduleEvent(EVENT_ICEBOUND_FORTITUDE, 60*IN_MILLISECONDS);
                            }
                            else
                                _events.RescheduleEvent(EVENT_ICEBOUND_FORTITUDE, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_ICY_TOUCH:
                            DoCastVictim(SPELL_ICY_TOUCH);
                            _events.ScheduleEvent(EVENT_ICY_TOUCH, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_STRANGULATE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            {
                                DoCast(target, SPELL_STRANGULATE);
                                _events.RescheduleEvent(EVENT_STRANGULATE, 120*IN_MILLISECONDS);
                            }
                            else
                                _events.RescheduleEvent(EVENT_STRANGULATE, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_AIMED_SHOT:
                            DoCastVictim(SPELL_AIMED_SHOT);
                            _events.ScheduleEvent(EVENT_AIMED_SHOT, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_DETERRENCE:
                            if (HealthBelowPct(30))
                            {
                                DoCast(SPELL_DETERRENCE);
                                _events.RescheduleEvent(EVENT_DETERRENCE, 150*IN_MILLISECONDS);
                            }
                            else
                                _events.RescheduleEvent(EVENT_DETERRENCE, 10*IN_MILLISECONDS);
                            return;
                        case EVENT_DISENGAGE:
                            if (Unit* target = me->GetVictim())
                                if (me->IsInRange(target, 0.0f, 10.0f, false))
                                    DoCast(SPELL_DISENGAGE);
                            _events.ScheduleEvent(EVENT_DISENGAGE, 30*IN_MILLISECONDS);
                            return;
                        case EVENT_EXPLOSIVE_SHOT:
                            DoCastVictim(SPELL_EXPLOSIVE_SHOT);
                            _events.ScheduleEvent(EVENT_EXPLOSIVE_SHOT, urand(6*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_FROST_TRAP:
                            if (Unit* target = me->GetVictim())
                                if (me->IsInRange(target, 0.0f, 10.0f, false))
                                    DoCastAOE(SPELL_FROST_TRAP);
                            _events.ScheduleEvent(EVENT_FROST_TRAP, 30*IN_MILLISECONDS);
                            return;
                        case EVENT_STEADY_SHOT:
                            DoCastVictim(SPELL_STEADY_SHOT);
                            _events.ScheduleEvent(EVENT_STEADY_SHOT, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_WING_CLIP:
                            if (Unit* target = me->GetVictim())
                            {
                                if (me->GetDistance2d(target) < 6.0f)
                                    DoCast(target, SPELL_WING_CLIP);
                            }
                            _events.ScheduleEvent(EVENT_WING_CLIP, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_WYVERN_STING:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_WYVERN_STING);
                            _events.ScheduleEvent(EVENT_WYVERN_STING, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_THREAT_DEATHKNIGHT:
                        {
                            me->ModifyPower(POWER_MANA, me->GetMaxPower(POWER_MANA) / 3);
                            std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                            {
                                Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
                                if (unit && me->getThreatManager().getThreat(unit))
                                {
                                    if (unit->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        float dist_mod = 15.0f / (15.0f + me->GetDistance2d(unit));
                                        float armor_mod = (float)unit->GetArmor() / 16635.0f;
                                        float eh = (unit->GetHealth() + 1) * (1.0f + armor_mod);
                                        float threat = dist_mod * 30000.0f / eh;
                                        me->getThreatManager().modifyThreatPercent(unit, -100);
                                        me->AddThreat(unit, 1000000.0f * threat);
                                    }
                                }
                            }
                            _events.ScheduleEvent(EVENT_THREAT_DEATHKNIGHT, 4*IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_GROWTH_DEATHKNIGHT:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_GROWTH_DEATHKNIGHT, 30000);
                            break;
                        default:
                            return;
                    }
                }
                DoSpellAttackIfReady(SPELL_SHOOT);
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss13AI(creature);
        }
};

class boss_worldboss14 : public CreatureScript
{
    public:
        boss_worldboss14() : CreatureScript("boss_worldboss14") { }

        struct boss_worldboss14AI : public ScriptedAI
        {
            boss_worldboss14AI(Creature* creature) : ScriptedAI(creature) { }

            bool Enraged;
            bool Empower;

            uint32 ConflagrationTimer;
            uint32 BlazeTimer;
            uint32 PyrogenicsTimer;
            uint32 ShadownovaTimer;
            uint32 FlamesearTimer;
            uint32 EnrageTimer;
            bool WorldBoss14Dead;
            bool Announced;

            void Reset() override
            {
               
                if (!me->IsInCombat())
                {
                    _events.Reset();
                    _events.ScheduleEvent(EVENT_THREAT_WORLDBOSS14, 5*IN_MILLISECONDS);
                    _events.ScheduleEvent(EVENT_GROWTH_WORLDBOSS14, 30000);
                    _events.ScheduleEvent(EVENT_CAPTURE_SOUL_WORLDBOSS14, 900000);
                    SetEquipmentSlots(false, 38293, 8625, EQUIP_NO_CHANGE);
                    ConflagrationTimer = 45000;
                    BlazeTimer = 100;
                    PyrogenicsTimer = 15000;
                    ShadownovaTimer = 40000;
                    EnrageTimer = 900000;
                    FlamesearTimer = 15000;
                    Enraged = false;
                    Empower = false;
                    WorldBoss14Dead = false;
                    Announced = false;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_WORLDBOSS14:
                        if (WorldBoss14Dead)
                            return 1;
                        break;
                }
                return 0;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
            } 

            void JustRespawned() override
            {
                float x, y, z, o;
                me->GetHomePosition(x, y, z, o);
                DoTeleportTo(x, y, z);
               
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领南海的任家萱、田馥甄和陈嘉桦出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                if (!Announced)
                {
                    char message[1024];
                    
                    if (who->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (who->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领南海的任家萱、田馥甄和陈嘉桦正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领南海的任家萱、田馥甄和陈嘉桦正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->GetTypeId() == TYPEID_UNIT)
                    {
                        if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                        {
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领南海的任家萱、田馥甄和陈嘉桦正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领南海的任家萱、田馥甄和陈嘉桦正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        if (who->IsPet())
                        {
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领南海的任家萱、田馥甄和陈嘉桦正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领南海的任家萱、田馥甄和陈嘉桦正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领南海的任家萱、田馥甄和陈嘉桦正在遭到攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领南海的任家萱、田馥甄和陈嘉桦正在遭到攻击！　|r");
                        
                    }
                    sWorld->SendGlobalText(message, NULL);
                    Announced = true;
                }
                DoCast(me, SPELL_ANTI_AOE, true);
               
                WorldBoss14Dead = false;
            }

            void JustDied(Unit* /*killer*/) override
            {

                WorldBoss14Dead = true;
            }

            void JustReachedHome()
            {
               
            }

            void SpellHitTarget(Unit* target, const SpellInfo* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_BLAZE:
                        target->CastSpell(target, SPELL_BLAZE_SUMMON, true);
                        break;
                    case SPELL_FIREBOLT_VOLLEY:
                    case SPELL_CONFLAGRATION:
                    case SPELL_FLAME_SEAR:
                        HandleTouchedSpells(target, SPELL_FLAME_TOUCHED);
                        break;
                    case SPELL_SHADOW_NOVA:
                        HandleTouchedSpells(target, SPELL_DARK_TOUCHED);
                        break;
                }
            }

            void HandleTouchedSpells(Unit* target, uint32 TouchedType)
            {
                switch (TouchedType)
                {
                    case SPELL_FLAME_TOUCHED:
                        if (!target->HasAura(SPELL_DARK_FLAME))
                        {
                            if (target->HasAura(SPELL_DARK_TOUCHED))
                            {
                                target->RemoveAurasDueToSpell(SPELL_DARK_TOUCHED);
                                target->CastSpell(target, SPELL_DARK_FLAME, true);
                            }
                            else
                                target->CastSpell(target, SPELL_FLAME_TOUCHED, true);
                        }
                        break;
                    case SPELL_DARK_TOUCHED:
                        if (!target->HasAura(SPELL_DARK_FLAME))
                        {
                            if (target->HasAura(SPELL_FLAME_TOUCHED))
                            {
                                target->RemoveAurasDueToSpell(SPELL_FLAME_TOUCHED);
                                target->CastSpell(target, SPELL_DARK_FLAME, true);
                            }
                            else
                                target->CastSpell(target, SPELL_DARK_TOUCHED, true);
                        }
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

               

                _events.Update(diff);

                if (me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!Empower)
                {
                                DoCast(me, SPELL_EMPOWER);
                                Empower = true;
                }

                if (!Enraged && HealthBelowPct(5))
                {
                    DoCast(me, SPELL_ENRAGE);
                    Enraged = true;
                }

                if (Empower)
                {
                    if (ShadownovaTimer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCast(false))
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_SHADOW_NOVA);
                            ShadownovaTimer= 30000+(rand32()%5000);
                        }
                    } else ShadownovaTimer -=diff;
                }
                else
                {
                    if (ConflagrationTimer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCast(false))
                        {
                            me->InterruptSpell(CURRENT_GENERIC_SPELL);
                            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                            if (target)
                                DoCast(target, SPELL_CONFLAGRATION);
                            ConflagrationTimer = 30000+(rand32()%5000);
                            BlazeTimer = 4000;
                        }
                    } else ConflagrationTimer -= diff;
                }

                if (FlamesearTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCast(false))
                    {
                        DoCast(me, SPELL_FLAME_SEAR);
                        FlamesearTimer = 15000;
                    }
                } else FlamesearTimer -=diff;

                if (PyrogenicsTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCast(false))
                    {
                        DoCast(me, SPELL_PYROGENICS, true);
                        PyrogenicsTimer = 15000;
                    }
                } else PyrogenicsTimer -= diff;

                if (BlazeTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCast(false))
                    {
                        DoCastVictim(SPELL_BLAZE);
                        BlazeTimer = 3800;
                    }
                } else BlazeTimer -= diff;

                if (EnrageTimer < diff && !Enraged)
                {
                    me->InterruptSpell(CURRENT_GENERIC_SPELL);
                    DoCast(me, SPELL_ENRAGE_LARGE);
                    Enraged = true;
                } else EnrageTimer -= diff;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_THREAT_WORLDBOSS14:
                        {
                            me->ModifyPower(POWER_MANA, me->GetMaxPower(POWER_MANA) / 3);
                            std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                            {
                                Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
                                if (unit && me->getThreatManager().getThreat(unit))
                                {
                                    if (unit->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        float dist_mod = 1.0f;
                                        float armor_mod = 0.0f;
                                        float eh = (unit->GetHealth() + 1) * (1.0f + armor_mod);
                                        float threat = dist_mod * 30000.0f / eh;
                                        me->getThreatManager().modifyThreatPercent(unit, -100);
                                        me->AddThreat(unit, 1000000.0f * threat);
                                    }
                                }
                            }
                            _events.ScheduleEvent(EVENT_THREAT_WORLDBOSS14, 4*IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_GROWTH_WORLDBOSS14:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_GROWTH_WORLDBOSS14, 30000);
                            break;
                        case EVENT_CAPTURE_SOUL_WORLDBOSS14:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_CAPTURE_SOUL_WORLDBOSS14, 10000);
                            break;
                        default:
                            return;
                    }
                }
                DoSpellAttackIfReady(SPELL_FIREBOLT_VOLLEY);
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss14AI(creature);
        }
};

class boss_worldboss15 : public CreatureScript
{
    public:
        boss_worldboss15() : CreatureScript("boss_worldboss15") { }

        struct boss_worldboss15AI : public ScriptedAI
        {
            boss_worldboss15AI(Creature* creature) : ScriptedAI(creature) { }

            bool Enraged;
            bool Empower;

            uint32 ShadowbladesTimer;
            uint32 ShadownovaTimer;
            uint32 ConfoundingblowTimer;
            uint32 ShadowimageTimer;
            uint32 ConflagrationTimer;
            uint32 EnrageTimer;
            bool WorldBoss15Dead;

            void Reset() override
            {
               
                if (!me->IsInCombat())
                {
                    _events.Reset();
                    _events.ScheduleEvent(EVENT_THREAT_WORLDBOSS15, 5*IN_MILLISECONDS);
                    _events.ScheduleEvent(EVENT_GROWTH_WORLDBOSS15, 30000);
                    _events.ScheduleEvent(EVENT_CAPTURE_SOUL_WORLDBOSS15, 900000);
                    SetEquipmentSlots(false, 45073, 8626, EQUIP_NO_CHANGE);
                    ShadowbladesTimer = 10000;
                    ShadownovaTimer = 30000;
                    ConfoundingblowTimer = 25000;
                    ShadowimageTimer = 20000;
                    ConflagrationTimer = 30000;
                    EnrageTimer = 900000;
                    Enraged = false;
                    Empower = false;
                    WorldBoss15Dead = false;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_WORLDBOSS15:
                        if (WorldBoss15Dead)
                            return 1;
                        break;
                }
                return 0;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
            } 

            void JustRespawned() override
            {
                float x, y, z, o;
                me->GetHomePosition(x, y, z, o);
                DoTeleportTo(x, y, z);
                
            }

            void EnterCombat(Unit* who) override
            {
                DoCast(me, SPELL_ANTI_AOE, true);
                
                WorldBoss15Dead = false;
            }

            void JustDied(Unit* /*killer*/) override
            {
               
                WorldBoss15Dead = true;
            }

            void JustReachedHome()
            {
              
            }

            void SpellHitTarget(Unit* target, const SpellInfo* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_SHADOW_VOLLEY:
                    case SPELL_SHADOW_BLADES:
                    case SPELL_SHADOW_NOVA:
                    case SPELL_CONFOUNDING_BLOW:
                    case SPELL_SHADOW_FURY:
                        HandleTouchedSpells(target, SPELL_DARK_TOUCHED);
                        break;
                    case SPELL_CONFLAGRATION:
                        HandleTouchedSpells(target, SPELL_FLAME_TOUCHED);
                        break;
                }
            }

            void HandleTouchedSpells(Unit* target, uint32 TouchedType)
            {
                switch (TouchedType)
                {
                    case SPELL_FLAME_TOUCHED:
                        if (!target->HasAura(SPELL_DARK_FLAME))
                        {
                            if (target->HasAura(SPELL_DARK_TOUCHED))
                            {
                                target->RemoveAurasDueToSpell(SPELL_DARK_TOUCHED);
                                target->CastSpell(target, SPELL_DARK_FLAME, true);
                            } else target->CastSpell(target, SPELL_FLAME_TOUCHED, true);
                        }
                        break;
                    case SPELL_DARK_TOUCHED:
                        if (!target->HasAura(SPELL_DARK_FLAME))
                        {
                            if (target->HasAura(SPELL_FLAME_TOUCHED))
                            {
                                target->RemoveAurasDueToSpell(SPELL_FLAME_TOUCHED);
                                target->CastSpell(target, SPELL_DARK_FLAME, true);
                            } else target->CastSpell(target, SPELL_DARK_TOUCHED, true);
                        }
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                

                _events.Update(diff);

                if (me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!Empower)
                {
                                DoCast(me, SPELL_EMPOWER);
                                Empower = true;
                }

                if (!Enraged && HealthBelowPct(5))
                {
                    DoCast(me, SPELL_ENRAGE);
                    Enraged = true;
                }

                if (Empower)
                {
                    if (ConflagrationTimer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCast(false))
                        {
                            me->InterruptSpell(CURRENT_GENERIC_SPELL);
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_CONFLAGRATION);
                            ConflagrationTimer = 30000+(rand32()%5000);
                        }
                    } else ConflagrationTimer -= diff;
                }
                else
                {
                    if (ShadownovaTimer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCast(false))
                        {
                            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                            if (target)
                                DoCast(target, SPELL_SHADOW_NOVA);
                            ShadownovaTimer = 30000+(rand32()%5000);
                        }
                    } else ShadownovaTimer -=diff;
                }

                if (ConfoundingblowTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCast(false))
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            DoCast(target, SPELL_CONFOUNDING_BLOW);
                        ConfoundingblowTimer = 20000 + (rand32()%5000);
                    }
                } else ConfoundingblowTimer -=diff;

                if (ShadowimageTimer <= diff)
                {
                    Unit* target = NULL;
                    Creature* temp = NULL;
                    for (uint8 i = 0; i<3; ++i)
                    {
                        target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                        temp = DoSpawnCreature(25214, 0, 0, 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 10000);
                        if (temp && target)
                        {
                            temp->AddThreat(target, 1000000); //don't change target(healers)
                            temp->AI()->AttackStart(target);
                        }
                    }
                    ShadowimageTimer = 20000;
                } else ShadowimageTimer -=diff;

                if (ShadowbladesTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCast(false))
                    {
                        DoCast(me, SPELL_SHADOW_BLADES);
                        ShadowbladesTimer = 10000;
                    }
                } else ShadowbladesTimer -=diff;

                if (EnrageTimer < diff && !Enraged)
                {
                    me->InterruptSpell(CURRENT_GENERIC_SPELL);
                    DoCast(me, SPELL_ENRAGE_LARGE);
                    Enraged = true;
                } else EnrageTimer -= diff;

                if (me->isAttackReady() && !me->IsNonMeleeSpellCast(false))
                {
                    //ifwe are within range melee the target
                    if (me->IsWithinMeleeRange(me->GetVictim()))
                    {
                        HandleTouchedSpells(me->GetVictim(), SPELL_DARK_TOUCHED);
                        DoSpellAttackIfReady(SPELL_SHADOW_VOLLEY);
                        me->AttackerStateUpdate(me->GetVictim());
                        me->resetAttackTimer();
                    }
                }

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_THREAT_WORLDBOSS15:
                        {
                            me->ModifyPower(POWER_MANA, me->GetMaxPower(POWER_MANA) / 3);
                            std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                            {
                                Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
                                if (unit && me->getThreatManager().getThreat(unit))
                                {
                                    if (unit->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        float dist_mod = 1.0f;
                                        float armor_mod = 0.0f;
                                        float eh = (unit->GetHealth() + 1) * (1.0f + armor_mod);
                                        float threat = dist_mod * 30000.0f / eh;
                                        me->getThreatManager().modifyThreatPercent(unit, -100);
                                        me->AddThreat(unit, 1000000.0f * threat);
                                    }
                                }
                            }
                            _events.ScheduleEvent(EVENT_THREAT_WORLDBOSS15, 4*IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_GROWTH_WORLDBOSS15:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_GROWTH_WORLDBOSS15, 30000);
                            break;
                        case EVENT_CAPTURE_SOUL_WORLDBOSS15:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_CAPTURE_SOUL_WORLDBOSS15, 10000);
                            break;
                        default:
                            return;
                    }
                }
                DoSpellAttackIfReady(SPELL_SHADOW_VOLLEY);
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss15AI(creature);
        }
};

class boss_worldboss16 : public CreatureScript
{
    public:
        boss_worldboss16() : CreatureScript("boss_worldboss16") { }

        struct boss_worldboss16AI : public ScriptedAI
        {
            boss_worldboss16AI(Creature* creature) : ScriptedAI(creature) { }

            std::list<uint64> intenseColdList;
            uint32 uiCrystalfireBreathTimer;
            uint32 uiCrystalChainsCrystalizeTimer;
            uint32 uiTailSweepTimer;
            uint32 EnrageTimer;
            bool intenseCold;
            bool bEnrage;
            bool Empower;
            bool WorldBoss16Dead;

            void Reset() override
            {
                
                if (!me->IsInCombat())
                {
                    _events.Reset();
                    _events.ScheduleEvent(EVENT_THREAT_WORLDBOSS16, 5*IN_MILLISECONDS);
                    _events.ScheduleEvent(EVENT_GROWTH_WORLDBOSS16, 30000);
                    _events.ScheduleEvent(EVENT_CAPTURE_SOUL_WORLDBOSS16, 900000);
                    SetEquipmentSlots(false, 22206, 8624, EQUIP_NO_CHANGE);
                    uiCrystalfireBreathTimer = 14*IN_MILLISECONDS;
                    uiCrystalChainsCrystalizeTimer = 11*IN_MILLISECONDS;
                    uiTailSweepTimer = 5*IN_MILLISECONDS;
                    EnrageTimer = 900000;
                    bEnrage = false;
                    Empower = false;
                    intenseCold = true;
                    intenseColdList.clear();
                    WorldBoss16Dead = false;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_WORLDBOSS16:
                        if (WorldBoss16Dead)
                            return 1;
                        break;
                }
                return 0;
            }

            void SetGUID(ObjectGuid guid, int32 id/* = 0 */) override
            {
                if (id == DATA_INTENSE_COLD)
                    intenseColdList.push_back(guid);
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
               
            } 

            void JustRespawned() override
            {
                float x, y, z, o;
                me->GetHomePosition(x, y, z, o);
                DoTeleportTo(x, y, z);

            }

            void EnterCombat(Unit* who) override
            {
                DoCast(me, SPELL_ANTI_AOE, true);
                DoCastAOE(SPELL_INTENSE_COLD);
                
                WorldBoss16Dead = false;
            }

            void JustDied(Unit* /*killer*/) override
            {

                WorldBoss16Dead = true;
            }

            void JustReachedHome()
            {

            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;


                _events.Update(diff);

                if (me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!Empower)
                {

                                DoCast(me, SPELL_EMPOWER);
                                Empower = true;

                }

                if (!bEnrage && HealthBelowPct(5))
                {
                    DoCast(me, SPELL_ENRAGE);
                    bEnrage = true;
                }

                if (uiCrystalfireBreathTimer <= diff)
                {
                    DoCastVictim(SPELL_CRYSTALFIRE_BREATH);
                    uiCrystalfireBreathTimer = 14*IN_MILLISECONDS;
                } else uiCrystalfireBreathTimer -= diff;

                if (uiTailSweepTimer <= diff)
                {
                    DoCast(me, SPELL_TAIL_SWEEP_WORLDBOSS16);
                    uiTailSweepTimer = 5*IN_MILLISECONDS;
                } else uiTailSweepTimer -= diff;

                if (uiCrystalChainsCrystalizeTimer <= diff)
                {
                    DoCast(me, SPELL_CRYSTALIZE);
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        DoCast(target, SPELL_CRYSTAL_CHAINS);
                    uiCrystalChainsCrystalizeTimer = DUNGEON_MODE(30*IN_MILLISECONDS, 11*IN_MILLISECONDS);
                } else uiCrystalChainsCrystalizeTimer -= diff;

                if (EnrageTimer < diff && !bEnrage)
                {
                    me->InterruptSpell(CURRENT_GENERIC_SPELL);
                    DoCast(me, SPELL_ENRAGE_LARGE);
                    bEnrage = true;
                } else EnrageTimer -= diff;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_THREAT_WORLDBOSS16:
                        {
                            me->ModifyPower(POWER_MANA, me->GetMaxPower(POWER_MANA) / 3);
                            std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                            {
                                Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
                                if (unit && me->getThreatManager().getThreat(unit))
                                {
                                    if (unit->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        float dist_mod = 1.0f;
                                        float armor_mod = 0.0f;
                                        float eh = (unit->GetHealth() + 1) * (1.0f + armor_mod);
                                        float threat = dist_mod * 30000.0f / eh;
                                        me->getThreatManager().modifyThreatPercent(unit, -100);
                                        me->AddThreat(unit, 1000000.0f * threat);
                                    }
                                }
                            }
                            _events.ScheduleEvent(EVENT_THREAT_WORLDBOSS16, 4*IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_GROWTH_WORLDBOSS16:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_GROWTH_WORLDBOSS16, 30000);
                            break;
                        case EVENT_CAPTURE_SOUL_WORLDBOSS16:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_CAPTURE_SOUL_WORLDBOSS16, 10000);
                            break;
                        default:
                            return;
                    }
                }
                DoSpellAttackIfReady(SPELL_FROSTBOLT_VOLLEY);
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss16AI(creature);
        }
};

class boss_worldboss17 : public CreatureScript
{
    public:
        boss_worldboss17() : CreatureScript("boss_worldboss17") { }

        struct boss_worldboss17AI : public ScriptedAI
        {
            boss_worldboss17AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_STATIC_DISRUPTION, urand(10000, 20000));
                _events.ScheduleEvent(EVENT_GUST_OF_WIND, urand(20000, 30000));
                _events.ScheduleEvent(EVENT_CALL_LIGHTNING, urand(10000, 20000));
                _events.ScheduleEvent(EVENT_ELECTRICAL_STORM, 60000);
                _events.ScheduleEvent(EVENT_RAIN, urand(47000, 52000));
                _events.ScheduleEvent(EVENT_LAVA_LASH_WORLDBOSS17, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_STORMSTRIKE_WORLDBOSS17, urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_WINDFURY_WORLDBOSS17, urand(20*IN_MILLISECONDS, 50*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_SHOCK_BLAST, urand(1*IN_MILLISECONDS, 14*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_STATIC_CHARGE_TRIGGER, urand(2*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_FORKED_LIGHTNING, urand(2*IN_MILLISECONDS, 6*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_BALL_LIGHTNING, urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_STATIC_OVERLOAD, urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_GROWTH_WORLDBOSS17, 30000);
                _events.ScheduleEvent(EVENT_CAPTURE_SOUL_WORLDBOSS17, 900000);
                TargetGUID = ObjectGuid::Empty;
                CloudGUID = ObjectGuid::Empty;
                CycloneGUID = ObjectGuid::Empty;
                StormCount = 0;
                isRaining = false;
                SetWeather(WEATHER_STATE_FINE, 0.0f);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领荆棘谷哈圭罗岛的雷神出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                if (!Announced)
                {
                    char message[1024];
                    
                    if (who->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (who->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领荆棘谷哈圭罗岛的雷神正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领荆棘谷哈圭罗岛的雷神正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->GetTypeId() == TYPEID_UNIT)
                    {
                        if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                        {
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领荆棘谷哈圭罗岛的雷神正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领荆棘谷哈圭罗岛的雷神正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        if (who->IsPet())
                        {
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领荆棘谷哈圭罗岛的雷神正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领荆棘谷哈圭罗岛的雷神正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领荆棘谷哈圭罗岛的雷神正在遭到攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领荆棘谷哈圭罗岛的雷神正在遭到攻击！　|r");
                        
                    }
                    sWorld->SendGlobalText(message, NULL);
                    Announced = true;
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void SetWeather(uint32 weather, float grade)
            {
                Map* map = me->GetMap();

                WorldPacket data(SMSG_WEATHER, (4+4+4));
                data << uint32(weather) << float(grade) << uint8(0);

                map->SendToPlayers(&data);
            }

            void HandleStormSequence(Unit* Cloud) // 1: begin, 2-9: tick, 10: end
            {
                if (StormCount < 10 && StormCount > 1)
                {
                    // deal damage
                    int32 bp0 = 80000;
                    for (uint8 i = 2; i < StormCount; ++i)
                        bp0 *= 2;

                    CellCoord p(Trinity::ComputeCellCoord(me->GetPositionX(), me->GetPositionY()));
                    Cell cell(p);
                    cell.SetNoCreate();

                    std::list<Unit*> tempUnitMap;

                    {
                        Trinity::AnyAoETargetUnitInObjectRangeCheck u_check(me, me, SIZE_OF_GRIDS);
                        Trinity::UnitListSearcher<Trinity::AnyAoETargetUnitInObjectRangeCheck> searcher(me, tempUnitMap, u_check);

                        TypeContainerVisitor<Trinity::UnitListSearcher<Trinity::AnyAoETargetUnitInObjectRangeCheck>, WorldTypeMapContainer > world_unit_searcher(searcher);
                        TypeContainerVisitor<Trinity::UnitListSearcher<Trinity::AnyAoETargetUnitInObjectRangeCheck>, GridTypeMapContainer >  grid_unit_searcher(searcher);

                        cell.Visit(p, world_unit_searcher, *me->GetMap(), *me, SIZE_OF_GRIDS);
                        cell.Visit(p, grid_unit_searcher, *me->GetMap(), *me, SIZE_OF_GRIDS);
                    }

                    // deal damage
                    for (std::list<Unit*>::const_iterator i = tempUnitMap.begin(); i != tempUnitMap.end(); ++i)
                    {
                        if (Unit* target = (*i))
                        {
                            if (Cloud && !Cloud->IsWithinDist(target, 6, false))
                                Cloud->CastCustomSpell(target, SPELL_ZAP, &bp0, NULL, NULL, true, 0, 0, me->GetGUID());
                        }
                    }

                    // visual
                    float x, y, z;
                    z = me->GetPositionZ();
                    uint8 maxCount = 5 + rand32() % 5;
                    for (uint8 i = 0; i < maxCount; ++i)
                    {
                        x = 343.0f + rand32() % 60;
                        y = 1380.0f + rand32() % 60;
                        if (Unit* trigger = me->SummonTrigger(x, y, z, 0, 2000))
                        {
                            trigger->setFaction(35);
                            trigger->SetMaxHealth(100000);
                            trigger->SetHealth(100000);
                            trigger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            if (Cloud)
                                Cloud->CastCustomSpell(trigger, /*43661*/SPELL_ZAP, &bp0, NULL, NULL, true, 0, 0, Cloud->GetGUID());
                        }
                    }
                }

                ++StormCount;

                if (StormCount > 10)
                {
                    StormCount = 0; // finish
                    me->InterruptNonMeleeSpells(false);
                    CloudGUID = ObjectGuid::Empty;
                    if (Cloud)
                        Cloud->DealDamage(Cloud, Cloud->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    SetWeather(WEATHER_STATE_FINE, 0.0f);
                    isRaining = false;
                }
                _events.ScheduleEvent(EVENT_STORM_SEQUENCE, 1000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if ((me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f) || me->GetDistance(me->GetVictim()) < 0.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_STATIC_DISRUPTION:
                            {
                                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1);
                                if (!target)
                                    target = me->GetVictim();
                                if (target)
                                {
                                    TargetGUID = target->GetGUID();
                                    DoCast(target, SPELL_STATIC_DISRUPTION, false);
                                    me->SetInFront(me->GetVictim());
                                }
                                _events.ScheduleEvent(EVENT_STATIC_DISRUPTION, urand(10000, 18000));
                                break;
                            }
                        case EVENT_GUST_OF_WIND:
                            {
                                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1);
                                if (!target)
                                    target = me->GetVictim();
                                if (target)
                                    DoCast(target, SPELL_GUST_OF_WIND);
                                _events.ScheduleEvent(EVENT_GUST_OF_WIND, urand(20000, 30000));
                                break;
                            }
                        case EVENT_CALL_LIGHTNING:
                            DoCastVictim(SPELL_CALL_LIGHTNING);
                            _events.ScheduleEvent(EVENT_CALL_LIGHTNING, urand(12000, 17000)); // totaly random timer. can't find any info on this
                            break;
                        case EVENT_ELECTRICAL_STORM:
                            {
                                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50, true);
                                if (!target)
                                {
                                    EnterEvadeMode();
                                    return;
                                }
                                target->CastSpell(target, 44007, true); // cloud visual
                                DoCast(target, SPELL_ELECTRICAL_STORM, false); // storm cyclon + visual
                                float x, y, z;
                                target->GetPosition(x, y, z);

                                Unit* Cloud = me->SummonTrigger(x, y, me->GetPositionZ()+16, 0, 15000);
                                if (Cloud)
                                {
                                    CloudGUID = Cloud->GetGUID();
                                    Cloud->SetDisableGravity(true);
                                    Cloud->StopMoving();
                                    Cloud->SetObjectScale(1.0f);
                                    Cloud->setFaction(35);
                                    Cloud->SetMaxHealth(9999999);
                                    Cloud->SetHealth(9999999);
                                    Cloud->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                }
                                StormCount = 1;
                                _events.ScheduleEvent(EVENT_ELECTRICAL_STORM, 60000); // 60 seconds(bosskillers)
                                _events.ScheduleEvent(EVENT_RAIN, urand(47000, 52000));
                                break;
                            }
                        case EVENT_RAIN:
                            if (!isRaining)
                            {
                                SetWeather(WEATHER_STATE_HEAVY_RAIN, 0.9999f);
                                isRaining = true;
                            }
                            else
                                _events.ScheduleEvent(EVENT_RAIN, 1000);
                            break;
                        case EVENT_STORM_SEQUENCE:
                            {
                                Unit* target = ObjectAccessor::GetUnit(*me, CloudGUID);
                                if (!target || !target->IsAlive())
                                {
                                    EnterEvadeMode();
                                    return;
                                }
                                else if (Unit* Cyclone = ObjectAccessor::GetUnit(*me, CycloneGUID))
                                    Cyclone->CastSpell(target, SPELL_SAND_STORM, true); // keep casting or...
                                HandleStormSequence(target);
                                break;
                            }
                        case EVENT_LAVA_LASH_WORLDBOSS17:
                            DoCastVictim(SPELL_LAVA_LASH);
                            _events.ScheduleEvent(EVENT_LAVA_LASH_WORLDBOSS17, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_STORMSTRIKE_WORLDBOSS17:
                            DoCastVictim(SPELL_STORMSTRIKE);
                            _events.ScheduleEvent(EVENT_STORMSTRIKE_WORLDBOSS17, urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_WINDFURY_WORLDBOSS17:
                            DoCastVictim(SPELL_WINDFURY);
                            _events.ScheduleEvent(EVENT_WINDFURY_WORLDBOSS17, urand(20*IN_MILLISECONDS, 60*IN_MILLISECONDS));
                            return;
                        case EVENT_SHOCK_BLAST:
                            DoCastVictim(SPELL_SHOCK_BLAST);
                            _events.ScheduleEvent(EVENT_SHOCK_BLAST, urand(1*IN_MILLISECONDS, 14*IN_MILLISECONDS));
                            return;
                        case EVENT_STATIC_CHARGE_TRIGGER:
                        {
                            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200, true);
                            if (target && !target->HasAura(SPELL_STATIC_CHARGE_TRIGGER))
                                DoCast(target, SPELL_STATIC_CHARGE_TRIGGER);
                            _events.ScheduleEvent(EVENT_STATIC_CHARGE_TRIGGER, urand(2*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        }
                        case EVENT_FORKED_LIGHTNING:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_FORKED_LIGHTNING);
                            else
                                DoCastVictim(SPELL_FORKED_LIGHTNING);
                            _events.ScheduleEvent(EVENT_FORKED_LIGHTNING, urand(2*IN_MILLISECONDS, 6*IN_MILLISECONDS));
                            return;
                        case EVENT_BALL_LIGHTNING:
                            DoCastVictim(SPELL_BALL_LIGHTNING);
                            _events.ScheduleEvent(EVENT_BALL_LIGHTNING, urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS));
                            return;
                        case EVENT_STATIC_OVERLOAD:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_STATIC_OVERLOAD);
                            _events.ScheduleEvent(EVENT_STATIC_OVERLOAD, urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS));
                            return;
                        case EVENT_GROWTH_WORLDBOSS17:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_GROWTH_WORLDBOSS17, 30000);
                            break;
                        case EVENT_CAPTURE_SOUL_WORLDBOSS17:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_CAPTURE_SOUL_WORLDBOSS17, 10000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
            ObjectGuid TargetGUID;
            ObjectGuid CycloneGUID;
            ObjectGuid CloudGUID;
            uint8  StormCount;
            bool   isRaining;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss17AI(creature);
        }
};

class boss_worldboss18 : public CreatureScript
{
    public:
        boss_worldboss18() : CreatureScript("boss_worldboss18") { }

        struct boss_worldboss18AI : public ScriptedAI
        {
            boss_worldboss18AI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                Initialize();
            }

            void Initialize()
            {
                Timer[TIMER_FLAME_BREATH] = 10000;
                Timer[TIMER_DARKNESS] = 30000;
                Timer[TIMER_ARMAGEDDON] = 2000;
                Timer[TIMER_BIG_BANG] = 120000;
                Timer[TIMER_DOOM] = 20000;
                Timer[TIMER_FIERY_COMBUSTION] = 25000;
                Timer[TIMER_GROWTH] = 65000;
                Timer[TIMER_ENRAGE] = 1800000;
                Timer[TIMER_CHECK] = 10000;

                ActiveTimers = 9;
                WaitTimer = 0;

                IsInDarkness = false;
                IsWaiting = false;

                ChangeTimers(0);
            }

            SummonList summons;

            uint8 ActiveTimers;

            uint32 Timer[9];
            uint32 WaitTimer;

            bool IsInDarkness;
            bool IsWaiting;
            bool Announced;

            void Reset() override
            {
                Initialize();
                Announced = false;
               
                summons.DespawnAll();
            }

            void ChangeTimers(uint32 WTimer)
            {
                if (WTimer > 0)
                {
                    IsWaiting = true;
                    WaitTimer = WTimer;
                }
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {

                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustSummoned(Creature* summoned) override
            {

                summoned->setFaction(me->getFaction());
                summons.Summon(summoned);
            }

            void JustRespawned() override
            {
             
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领燃烧平原龙翼小径的死亡之翼的幻象出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

        

            void EnterCombat(Unit* who) override
            {
                if (!Announced)
                {
                    char message[1024];
                    
                    if (who->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (who->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领燃烧平原龙翼小径的死亡之翼的幻象正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领燃烧平原龙翼小径的死亡之翼的幻象正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->GetTypeId() == TYPEID_UNIT)
                    {
                        if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                        {
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领燃烧平原龙翼小径的死亡之翼的幻象正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领燃烧平原龙翼小径的死亡之翼的幻象正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        if (who->IsPet())
                        {
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领燃烧平原龙翼小径的死亡之翼的幻象正在遭到联盟的攻击！　|r");
                                
                            }
                            else
                            if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                            {
                                snprintf(message, 1024, "|cffFFFF00世界首领燃烧平原龙翼小径的死亡之翼的幻象正在遭到部落的攻击！　|r");
                                
                            }
                        }
                        else
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领燃烧平原龙翼小径的死亡之翼的幻象正在遭到攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领燃烧平原龙翼小径的死亡之翼的幻象正在遭到攻击！　|r");
                        
                    }
                    sWorld->SendGlobalText(message, NULL);
                    Announced = true;
                }
               
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
               
                summons.DespawnAll();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

            
  

                if ((me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f) || me->GetDistance(me->GetVictim()) < 0.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (IsWaiting)
                {
                    if (WaitTimer <= diff)
                    {
                        IsWaiting = false;
                        ChangeTimers(0);
                    } else WaitTimer -= diff;
                }

                for (uint8 t = 0; t < ActiveTimers; ++t)
                {
                    if (Timer[t] < diff)
                    {
                        switch (t)
                        {
                            case TIMER_FLAME_BREATH:
                                if (!me->IsNonMeleeSpellCast(false))
                                {
                                    if (IsInDarkness)
                                    {
                                        Timer[TIMER_FLAME_BREATH] = urand(15000, 20000);
                                    }
                                    else
                                    {
                                        DoCastVictim(SPELL_FLAME_BREATH);
                                        ChangeTimers(2000);
                                        Timer[TIMER_FLAME_BREATH] = urand(15000, 20000);
                                    }
                                }
                                break;
                            case TIMER_DARKNESS:
                                if (!me->IsNonMeleeSpellCast(false))
                                {
                                    if (!IsInDarkness)
                                    {
                                        DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS, false);
                                        ChangeTimers(9000);
                                        Timer[TIMER_DARKNESS] = 8750;
                                        IsInDarkness = true;
                                    }
                                    else
                                    {
                                        Timer[TIMER_DARKNESS] = urand(30000, 40000);
                                        IsInDarkness = false;
                                        DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                                        DoCast(me, SPELL_GROWTH);
                                    }
                                }
                                break;
                            case TIMER_ARMAGEDDON:
                            {
                                Unit* target = NULL;
                                for (uint8 z = 0; z < 6; ++z)
                                {
                                    target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
                                    if (!target)
                                        break;
                                }
                                if (target)
                                {
                                    float x, y, z;
                                    target->GetPosition(x, y, z);
                                    me->SummonCreature(25735, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 15000);
                                }
                                Timer[TIMER_ARMAGEDDON] = 2000;
                                break;
                            }
                            case TIMER_BIG_BANG:
                                if (!me->IsNonMeleeSpellCast(false))
                                {
                                    DoCastAOE(SPELL_BIG_BANG);
                                    ChangeTimers(8000);
                                    Timer[TIMER_BIG_BANG] = urand(120000, 130000);
                                }
                                break;
                            case TIMER_DOOM:
                                if (!me->IsNonMeleeSpellCast(false))
                                {
                                    if (IsInDarkness)
                                    {
                                        Timer[TIMER_DOOM] = urand(20000, 30000);
                                    }
                                    else
                                    {
                                        DoCast(SelectTarget(SELECT_TARGET_RANDOM, 1, 100, true), SPELL_DOOM);
                                        Timer[TIMER_DOOM] = urand(20000, 30000);
                                    }
                                }
                                break;
                            case TIMER_FIERY_COMBUSTION:
                                if (!me->IsNonMeleeSpellCast(false))
                                {
                                    if (IsInDarkness)
                                    {
                                        Timer[TIMER_FIERY_COMBUSTION] = urand(25000, 35000);
                                    }
                                    else
                                    {
                                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                        {
                                            DoCast(target, SPELL_FIERY_COMBUSTION);
                                            if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_ENFEEBLE))
                                                Aura::TryRefreshStackOrCreate(spellInfo, MAX_EFFECT_MASK, target, target);
                                        }
                                        Timer[TIMER_FIERY_COMBUSTION] = urand(25000, 35000);
                                    }
                                }
                                break;
                            case TIMER_GROWTH:
                                DoCast(me, SPELL_GROWTH);
                                Timer[TIMER_GROWTH] = urand(60000, 70000);
                                break;
                            case TIMER_ENRAGE:
                                DoCast(me, SPELL_ENRAGE_LARGE);
                                Timer[TIMER_ENRAGE] = 1800000;
                                break;
                            case TIMER_CHECK:
                            {
                                std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                                uint32 ThreatSize = 0;
                                for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                                {
                                    if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                                        if (target->GetTypeId() == TYPEID_PLAYER && target->GetAreaId() == 2420)
                                            ThreatSize = ThreatSize + 1;
                                }
                                Timer[TIMER_CHECK] = 10000;
                                if (ThreatSize == 0)
                                    EnterEvadeMode();
                                break;
                            }
                        }
                    }
                }
                DoMeleeAttackIfReady();
                for (uint8 i = 0; i < ActiveTimers; ++i)
                {
                    Timer[i] -= diff;
                    if (((int32)Timer[i]) < 0) Timer[i] = 0;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss18AI(creature);
        }
};

class boss_worldboss18_1 : public CreatureScript
{
    public:
        boss_worldboss18_1() : CreatureScript("boss_worldboss18_1") { }

        struct boss_worldboss18_1AI : public ScriptedAI
        {
            boss_worldboss18_1AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_BLADESTORM, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_INTIMIDATING_SHOUT, urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_WARR_CHARGE, 1*IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_DISARM, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_OVERPOWER, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_SUNDER_ARMOR, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_SHATTERING_THROW, urand(20*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_RETALIATION, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
            }

            void JustRespawned() override
            {
            }

            void EnterCombat(Unit* who) override
            {
                me->CastSpell(me, SPELL_ALLIANCE_GUARD1_1, true);

                    me->CastSpell(me, SPELL_ALLIANCE_GUARD2_2, true);
            }

            void KilledUnit(Unit* victim) override
            {
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

           

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BLADESTORM:
                            DoCastVictim(SPELL_BLADESTORM);
                            _events.ScheduleEvent(EVENT_BLADESTORM, 150*IN_MILLISECONDS);
                            return;
                        case EVENT_INTIMIDATING_SHOUT:
                            DoCastAOE(SPELL_INTIMIDATING_SHOUT);
                            _events.ScheduleEvent(EVENT_INTIMIDATING_SHOUT, 120*IN_MILLISECONDS);
                            return;
                        case EVENT_MORTAL_STRIKE:
                            DoCastVictim(SPELL_MORTAL_STRIKE);
                            _events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_WARR_CHARGE:
                            DoCastVictim(SPELL_CHARGE);
                            _events.ScheduleEvent(EVENT_WARR_CHARGE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_DISARM:
                            DoCastVictim(SPELL_DISARM);
                            _events.ScheduleEvent(EVENT_DISARM, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_OVERPOWER:
                            DoCastVictim(SPELL_OVERPOWER);
                            _events.ScheduleEvent(EVENT_OVERPOWER, urand(20*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_SUNDER_ARMOR:
                            DoCastVictim(SPELL_SUNDER_ARMOR);
                            _events.ScheduleEvent(EVENT_SUNDER_ARMOR, urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            return;
                        case EVENT_SHATTERING_THROW:
                            if (Unit* target = me->GetVictim())
                            {
                                if (target->HasAuraWithMechanic(1 << MECHANIC_IMMUNE_SHIELD))
                                {
                                    DoCast(target, SPELL_SHATTERING_THROW);
                                    _events.RescheduleEvent(EVENT_SHATTERING_THROW, 5*MINUTE*IN_MILLISECONDS);
                                    return;
                                }
                            }
                            _events.RescheduleEvent(EVENT_SHATTERING_THROW, 3*IN_MILLISECONDS);
                            return;
                        case EVENT_RETALIATION:
                            if (HealthBelowPct(50))
                            {
                                DoCast(SPELL_RETALIATION);
                                _events.RescheduleEvent(EVENT_RETALIATION, 5*MINUTE*IN_MILLISECONDS);
                            }
                            else
                                _events.RescheduleEvent(EVENT_RETALIATION, 5*IN_MILLISECONDS);
                            return;
                        default:
                            return;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss18_1AI(creature);
        }
};

class boss_worldboss18_2 : public CreatureScript
{
    public:
        boss_worldboss18_2() : CreatureScript("boss_worldboss18_2") { }

        struct boss_worldboss18_2AI : public ScriptedAI
        {
            boss_worldboss18_2AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_ARCANE_BARRAGE, urand(1*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_ARCANE_BLAST, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_ARCANE_EXPLOSION, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_BLINK, urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_COUNTERSPELL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_FROST_NOVA, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_ICE_BLOCK, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_POLYMORPH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
            }

            void JustRespawned() override
            {
            }

            void EnterCombat(Unit* who) override
            {
                me->CastSpell(me, SPELL_ALLIANCE_GUARD2_1, true);
            }

            void KilledUnit(Unit* victim) override
            {
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;



                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARCANE_BARRAGE:
                            DoCastVictim(SPELL_ARCANE_BARRAGE);
                            _events.ScheduleEvent(EVENT_ARCANE_BARRAGE, urand(5*IN_MILLISECONDS, 7*IN_MILLISECONDS));
                            return;
                        case EVENT_ARCANE_BLAST:
                            DoCastVictim(SPELL_ARCANE_BLAST);
                            _events.ScheduleEvent(EVENT_ARCANE_BLAST, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_ARCANE_EXPLOSION:
                            DoCastAOE(SPELL_ARCANE_EXPLOSION);
                            _events.ScheduleEvent(EVENT_ARCANE_EXPLOSION, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_BLINK:
                            DoCast(SPELL_BLINK);
                            _events.ScheduleEvent(EVENT_BLINK, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_COUNTERSPELL:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_COUNTERSPELL);
                            _events.ScheduleEvent(EVENT_COUNTERSPELL, 24*IN_MILLISECONDS);
                            return;
                        case EVENT_FROST_NOVA:
                            DoCastAOE(SPELL_FROST_NOVA);
                            _events.ScheduleEvent(EVENT_FROST_NOVA, 25*IN_MILLISECONDS);
                            return;
                        case EVENT_ICE_BLOCK:
                            if (HealthBelowPct(30))
                            {
                                DoCast(SPELL_ICE_BLOCK);
                                _events.RescheduleEvent(EVENT_ICE_BLOCK, 5*MINUTE*IN_MILLISECONDS);
                            }
                            else
                                _events.RescheduleEvent(EVENT_ICE_BLOCK, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_POLYMORPH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_POLYMORPH);
                            _events.ScheduleEvent(EVENT_POLYMORPH, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
                DoSpellAttackIfReady(SPELL_FROSTBOLT_WORLDBOSS10);
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss18_2AI(creature);
        }
};

class boss_worldboss18_3 : public CreatureScript
{
public:
    boss_worldboss18_3() : CreatureScript("boss_worldboss18_3") { }

    struct boss_worldboss18_3AI : public ScriptedAI
    {
        boss_worldboss18_3AI(Creature* creature) : ScriptedAI(creature), _summons(me)
        {
            Initialize();
        }

        void Initialize()
        {
            ChainLightningTimer = 2000;
            ShockTimer = 8000;
        }

        uint32 ChainLightningTimer;
        uint32 ShockTimer;

        void Reset() override
        {
            Initialize();
            _events.Reset();
            _events.ScheduleEvent(EVENT_HEALING_WAVE, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_RIPTIDE, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_SPIRIT_CLEANSE, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_BLOODLUST_HEROISM, 20*IN_MILLISECONDS);
            _events.ScheduleEvent(EVENT_HEX, urand(5*IN_MILLISECONDS, 30*IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_EARTH_SHIELD, 1*IN_MILLISECONDS);
            _events.ScheduleEvent(EVENT_EARTH_SHOCK, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_LAVA_LASH, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_STORMSTRIKE, urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_DEPLOY_TOTEM, 1*IN_MILLISECONDS);
            _events.ScheduleEvent(EVENT_WINDFURY, urand(20*IN_MILLISECONDS, 50*IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_THREAT_SHAMAN, 5*IN_MILLISECONDS);
            _events.ScheduleEvent(EVENT_GROWTH_SHAMAN, 30000);
            _events.ScheduleEvent(EVENT_CAPTURE_SOUL_SHAMAN, 900000);
            _totemCount = 0;
            _totemOldCenterX = me->GetPositionX();
            _totemOldCenterY = me->GetPositionY();
            _summons.DespawnAll();
        }

        void EnterCombat(Unit* /*who*/) override
        {
            me->CastSpell(me, SPELL_HORDE_GUARD1_1, true);
                me->CastSpell(me, SPELL_HORDE_GUARD2_2, true);
        }

        void JustSummoned(Creature* summoned) override
        {
            _summons.Summon(summoned);
        }

        void SummonedCreatureDespawn(Creature* /*pSummoned*/) override
        {
            --_totemCount;
        }

        void DeployTotem()
        {
            _totemCount = 4;
            _totemOldCenterX = me->GetPositionX();
            _totemOldCenterY = me->GetPositionY();
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;



            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (ChainLightningTimer <= diff)
            {
                DoCastVictim(SPELL_CHAIN_LIGHTNING);
                ChainLightningTimer = 9000;
            } else ChainLightningTimer -= diff;

            if (ShockTimer <= diff)
            {
                DoCastVictim(SPELL_SHOCK);
                ShockTimer = 15000;
            } else ShockTimer -= diff;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_HEALING_WAVE:
                        if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                            DoCast(target, SPELL_HEALING_WAVE);
                        _events.ScheduleEvent(EVENT_HEALING_WAVE, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                        return;
                    case EVENT_RIPTIDE:
                        if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                            DoCast(target, SPELL_RIPTIDE);
                        _events.ScheduleEvent(EVENT_RIPTIDE, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                        return;
                    case EVENT_SPIRIT_CLEANSE:
                        if (Unit* target = DoSelectLowestHpFriendly(40.0f))
                            DoCast(target, SPELL_SPIRIT_CLEANSE);
                        _events.ScheduleEvent(EVENT_SPIRIT_CLEANSE, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                        return;
                    case EVENT_BLOODLUST_HEROISM:
                        if (me->getFaction()) // alliance = 1
                        {
                            if (!me->HasAura(AURA_EXHAUSTION))
                                DoCastAOE(SPELL_HEROISM);
                        }
                        else
                        {
                            if (!me->HasAura(AURA_SATED))
                                DoCastAOE(SPELL_BLOODLUST);
                        }
                        _events.ScheduleEvent(EVENT_BLOODLUST_HEROISM, 5*MINUTE*IN_MILLISECONDS);
                        return;
                    case EVENT_HEX:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                            DoCast(target, SPELL_HEX);
                        _events.ScheduleEvent(EVENT_HEX, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                        return;
                    case EVENT_EARTH_SHIELD:
                        DoCast(me, SPELL_EARTH_SHIELD);
                        _events.ScheduleEvent(EVENT_EARTH_SHIELD, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                        return;
                    case EVENT_EARTH_SHOCK:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            DoCast(target, SPELL_EARTH_SHOCK);
                        _events.ScheduleEvent(EVENT_EARTH_SHOCK, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                        return;
                    case EVENT_LAVA_LASH:
                        DoCastVictim(SPELL_LAVA_LASH);
                        _events.ScheduleEvent(EVENT_LAVA_LASH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                        return;
                    case EVENT_STORMSTRIKE:
                        DoCastVictim(SPELL_STORMSTRIKE);
                        _events.ScheduleEvent(EVENT_STORMSTRIKE, urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                        return;
                    case EVENT_DEPLOY_TOTEM:
                        if (_totemCount < 4 || me->GetDistance2d(_totemOldCenterX, _totemOldCenterY) > 20.0f)
                            DeployTotem();
                        _events.ScheduleEvent(EVENT_DEPLOY_TOTEM, 1*IN_MILLISECONDS);
                        return;
                    case EVENT_WINDFURY:
                        DoCastVictim(SPELL_WINDFURY);
                        _events.ScheduleEvent(EVENT_WINDFURY, urand(20*IN_MILLISECONDS, 60*IN_MILLISECONDS));
                        return;
                    default:
                        return;
                }
            }
            DoMeleeAttackIfReady();
        }

        private:
            EventMap _events;
            SummonList _summons;
            uint8  _totemCount;
            float  _totemOldCenterX, _totemOldCenterY;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_worldboss18_3AI(creature);
    }
};

class boss_worldboss18_4 : public CreatureScript
{
public:
    boss_worldboss18_4() : CreatureScript("boss_worldboss18_4") { }

    struct boss_worldboss18_4AI : public ScriptedAI
    {
        boss_worldboss18_4AI(Creature* creature) : ScriptedAI(creature)
        {
            Initialize();
        }

        void Initialize()
        {
            FadeTimer = 30000;
            SummonSkeletonTimer = 20000;
            BlackArrowTimer = 15000;
            ShotTimer = 8000;
            MultiShotTimer = 10000;
        }

        uint32 FadeTimer;
        uint32 SummonSkeletonTimer;
        uint32 BlackArrowTimer;
        uint32 ShotTimer;
        uint32 MultiShotTimer;

        void Reset() override
        {
            Initialize();
            _events.Reset();
            _events.ScheduleEvent(EVENT_AIMED_SHOT, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_DETERRENCE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_DISENGAGE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_EXPLOSIVE_SHOT, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_FROST_TRAP, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_STEADY_SHOT, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_WING_CLIP, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_WYVERN_STING, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
        }

        void EnterCombat(Unit* /*who*/) override
        {
            me->CastSpell(me, SPELL_HORDE_GUARD2_1, true);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

     

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (FadeTimer <= diff)
            {
                DoCast(me, SPELL_FADE);
                // add a blink to simulate a stealthed movement and reappearing elsewhere
                DoCast(me, SPELL_FADE_BLINK);
                FadeTimer = 30000 + rand32() % 5000;
                // if the victim is out of melee range she cast multi shot
                if (Unit* victim = me->GetVictim())
                    if (me->GetDistance(victim) > 10.0f)
                        DoCast(victim, SPELL_MULTI_SHOT);
            } else FadeTimer -= diff;

            if (SummonSkeletonTimer <= diff)
            {
                DoCast(me, SPELL_SUMMON_SKELETON);
                SummonSkeletonTimer = 20000 + rand32() % 10000;
            } else SummonSkeletonTimer -= diff;

            if (BlackArrowTimer <= diff)
            {
                if (Unit* victim = me->GetVictim())
                {
                    DoCast(victim, SPELL_BLACK_ARROW);
                    BlackArrowTimer = 15000 + rand32() % 5000;
                }
            } else BlackArrowTimer -= diff;

            if (ShotTimer <= diff)
            {
                if (Unit* victim = me->GetVictim())
                {
                    DoCast(victim, SPELL_SHOT);
                    ShotTimer = 8000 + rand32() % 2000;
                }
            } else ShotTimer -= diff;

            if (MultiShotTimer <= diff)
            {
                if (Unit* victim = me->GetVictim())
                {
                    DoCast(victim, SPELL_MULTI_SHOT);
                    MultiShotTimer = 10000 + rand32() % 3000;
                }
            } else MultiShotTimer -= diff;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_AIMED_SHOT:
                        DoCastVictim(SPELL_AIMED_SHOT);
                        _events.ScheduleEvent(EVENT_AIMED_SHOT, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                        return;
                    case EVENT_DETERRENCE:
                        if (HealthBelowPct(30))
                        {
                            DoCast(SPELL_DETERRENCE);
                            _events.RescheduleEvent(EVENT_DETERRENCE, 150*IN_MILLISECONDS);
                        }
                        else
                            _events.RescheduleEvent(EVENT_DETERRENCE, 10*IN_MILLISECONDS);
                        return;
                    case EVENT_DISENGAGE:
                        if (Unit* target = me->GetVictim())
                            if (me->IsInRange(target, 0.0f, 10.0f, false))
                                DoCast(SPELL_DISENGAGE);
                        _events.ScheduleEvent(EVENT_DISENGAGE, 30*IN_MILLISECONDS);
                        return;
                    case EVENT_EXPLOSIVE_SHOT:
                        DoCastVictim(SPELL_EXPLOSIVE_SHOT);
                        _events.ScheduleEvent(EVENT_EXPLOSIVE_SHOT, urand(6*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                        return;
                    case EVENT_FROST_TRAP:
                        if (Unit* target = me->GetVictim())
                            if (me->IsInRange(target, 0.0f, 10.0f, false))
                                DoCastAOE(SPELL_FROST_TRAP);
                        _events.ScheduleEvent(EVENT_FROST_TRAP, 30*IN_MILLISECONDS);
                        return;
                    case EVENT_STEADY_SHOT:
                        DoCastVictim(SPELL_STEADY_SHOT);
                        _events.ScheduleEvent(EVENT_STEADY_SHOT, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                        return;
                    case EVENT_WING_CLIP:
                        if (Unit* target = me->GetVictim())
                        {
                            if (me->GetDistance2d(target) < 6.0f)
                                DoCast(target, SPELL_WING_CLIP);
                        }
                        _events.ScheduleEvent(EVENT_WING_CLIP, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                        return;
                    case EVENT_WYVERN_STING:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                            DoCast(target, SPELL_WYVERN_STING);
                        _events.ScheduleEvent(EVENT_WYVERN_STING, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                        return;
                    default:
                        return;
                }
            }
            DoSpellAttackIfReady(SPELL_SHOOT);
            DoMeleeAttackIfReady();
        }

        private:
            EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_worldboss18_4AI(creature);
    }
};

class boss_worldboss19 : public CreatureScript
{
    public:
        boss_worldboss19() : CreatureScript("boss_worldboss19") { }

        struct boss_worldboss19AI : public ScriptedAI
        {
            boss_worldboss19AI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_FAN_OF_KNIVES, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_BLIND, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_CLOAK, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_BLADE_FLURRY, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_SHADOWSTEP, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_HEMORRHAGE, urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_EVISCERATE, urand(20*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_WOUND_POISON, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_THREAT_ROGUE, 5*IN_MILLISECONDS);
                me->setPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 100);
                DoCast(me, SPELL_STEALTH);
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
            } 

            void JustDied(Unit* /*killer*/) override
            {
                me->Say("你太迟了。现在，没人能救的了你的大法师了。　", LANG_UNIVERSAL);
                me->Say("古尔丹总能得到他想要的。我的生死无关紧要。　", LANG_UNIVERSAL);
            }

            void JustRespawned() override
            {
                DoCast(me, SPELL_STEALTH);
            }

            void EnterCombat(Unit* who) override
            {
                DoCast(me, SPELL_ANTI_AOE, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FAN_OF_KNIVES:
                            DoCastAOE(SPELL_FAN_OF_KNIVES);
                            _events.ScheduleEvent(EVENT_FAN_OF_KNIVES, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_BLIND:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_BLIND);
                            _events.ScheduleEvent(EVENT_BLIND, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_CLOAK:
                            if (HealthBelowPct(50))
                            {
                                DoCast(SPELL_CLOAK);
                                _events.RescheduleEvent(EVENT_CLOAK, 90*IN_MILLISECONDS);
                            }
                            else
                                _events.RescheduleEvent(EVENT_CLOAK, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_BLADE_FLURRY:
                            DoCast(SPELL_BLADE_FLURRY);
                            _events.RescheduleEvent(EVENT_BLADE_FLURRY, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_SHADOWSTEP:
                            if (Unit* target = me->GetVictim())
                            {
                                if (me->IsInRange(target, 10.0f, 40.0f, false))
                                {
                                    DoCast(target, SPELL_SHADOWSTEP);
                                    _events.RescheduleEvent(EVENT_SHADOWSTEP, 30*IN_MILLISECONDS);
                                    return;
                                }
                            }
                            _events.RescheduleEvent(EVENT_SHADOWSTEP, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_HEMORRHAGE:
                            DoCastVictim(SPELL_HEMORRHAGE);
                            _events.ScheduleEvent(EVENT_HEMORRHAGE, urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_EVISCERATE:
                            DoCastVictim(SPELL_EVISCERATE);
                            _events.ScheduleEvent(EVENT_EVISCERATE, urand(30*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_WOUND_POISON:
                            DoCastVictim(SPELL_WOUND_POISON);
                            _events.ScheduleEvent(EVENT_WOUND_POISON, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_THREAT_ROGUE:
                        {
                            std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                            {
                                Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
                                if (unit && me->getThreatManager().getThreat(unit))
                                {
                                    if (unit->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        float dist_mod = 15.0f / (15.0f + me->GetDistance2d(unit));
                                        float armor_mod = (float)unit->GetArmor() / 16635.0f;
                                        float eh = (unit->GetHealth() + 1) * (1.0f + armor_mod);
                                        float threat = dist_mod * 30000.0f / eh;
                                        me->getThreatManager().modifyThreatPercent(unit, -100);
                                        me->AddThreat(unit, 1000000.0f * threat);
                                    }
                                }
                            }
                            _events.ScheduleEvent(EVENT_THREAT_ROGUE, 4*IN_MILLISECONDS);
                            break;
                        }
                        default:
                            return;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss19AI(creature);
        }
};

class boss_worldboss19_1 : public CreatureScript
{
    public:
        boss_worldboss19_1() : CreatureScript("boss_worldboss19_1") { }

        struct boss_worldboss19_1AI : public ScriptedAI
        {
            boss_worldboss19_1AI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
            }

            bool Announced;
            bool HasWarnedPCT5 = false;
            bool HasWarnedPCT10 = false;
            bool HasWarnedPCT20 = false;
            bool HasWarnedPCT30 = false;
            bool HasWarnedPCT40 = false;
            bool HasWarnedPCT50 = false;
            bool HasWarnedPCT60 = false;
            bool HasWarnedPCT70 = false;
            bool HasWarnedPCT80 = false;
            bool HasWarnedPCT90 = false;
            bool Summoned = false;
            ObjectGuid WorldBoss19_2_GUID;
            SummonList summons;
            uint32 CheckTimer;

            void Reset() override
            {
                Announced = false;
                CheckTimer = 10000;
                HasWarnedPCT5 = false;
                HasWarnedPCT10 = false;
                HasWarnedPCT20 = false;
                HasWarnedPCT30 = false;
                HasWarnedPCT40 = false;
                HasWarnedPCT50 = false;
                HasWarnedPCT60 = false;
                HasWarnedPCT70 = false;
                HasWarnedPCT80 = false;
                HasWarnedPCT90 = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    if (Map const* map = me->GetMap())
                    {
                        uint32 AllianceCount = 0;
                        uint32 HordeCount = 0;
                        float HordeHealthPct = 100.0f;
                        Map::PlayerList const& plMap = map->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                        {
                            if (Player* player = itr->GetSource())
                            {
                                if (player->IsInWorld())
                                {
                                    if (player->GetZoneId() == 267 && player->getLevel() >= 80 && player->GetTeam() == ALLIANCE)
                                        AllianceCount++;
                                    else
                                    if (player->GetZoneId() == 267 && player->getLevel() >= 80 && player->GetTeam() == HORDE)
                                        HordeCount++;
                                }
                            }
                        }
                        for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                        {
                            if (Player* player = itr->GetSource())
                            {
                                if (player->IsInWorld())
                                {
                                    if (player->GetZoneId() == 267 && player->getLevel() >= 80 && player->GetTeam() == HORDE)
                                    {
                                        WorldPacket data(SMSG_PLAY_SOUND, 4);
                                        data << uint32(8454);
                                        player->SendDirectMessage(&data);
                                        if (HordeCount > 0)
                                        {
                                            player->ModifyHonorPoints(int32(595200/HordeCount));
                                            player->ModifyArenaPoints(int32(40000/HordeCount));
                                            player->CastSpell(player, 90042, true);
                                            if (urand(1,100) == 1)
                                            {
                                                uint32 itemId = urand(60397, 60406);
                                                player->AddItem(itemId, 1);
                                            }
                                            ChatHandler(player->GetSession()).PSendSysMessage("|cffFFFF00成功击杀联盟阵营领袖，你获得了　%u　点荣誉点数和　%u　点竞技场点数。　|r", int32(595200/HordeCount), int32(40000/HordeCount));
                                        }
                                    }
                                    else
                                    if (player->GetZoneId() == 267 && player->getLevel() >= 80 && player->GetTeam() == ALLIANCE)
                                    {
                                        WorldPacket data(SMSG_PLAY_SOUND, 4);
                                        data << uint32(8454);
                                        player->SendDirectMessage(&data);
                                        if (AllianceCount > 0)
                                        {
                                            player->ModifyHonorPoints(int32((595200*(100-HordeHealthPct)/100)/AllianceCount));
                                            player->ModifyArenaPoints(int32((40000*(100-HordeHealthPct)/100)/AllianceCount));
                                            player->CastSpell(player, 90193, true);
                                            ChatHandler(player->GetSession()).PSendSysMessage("|cffFFFF00阵营领袖被部落击杀，你获得了　%u　点荣誉点数和　%u　点竞技场点数。　|r", int32((595200*(100-HordeHealthPct)/100)/AllianceCount), int32((40000*(100-HordeHealthPct)/100)/AllianceCount));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } 

            void JustRespawned() override
            {
                summons.DespawnAll();
                float x, y, z, o;
                me->GetHomePosition(x, y, z, o);
                DoTeleportTo(x, y, z);
                    Summoned = true;
                if (Map const* map = me->GetMap())
                {
                    Map::PlayerList const& plMap = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                    {
                        if (Player* player = itr->GetSource())
                        {
                            if (player->IsInWorld())
                            {
                                if (player->GetZoneId() == 267 && player->getLevel() >= 80 && player->GetTeam() == ALLIANCE)
                                {
                                    if (!player->IsAlive())
                                    {
                                        player->ResurrectPlayer(1.0f);
                                        player->SpawnCorpseBones();
                                    }
                                    player->TeleportTo(0, -855.4674f, -568.4914f, 11.064974f, 1.58748f);
                                }
                                else
                                if (player->GetZoneId() == 267 && player->getLevel() >= 80 && player->GetTeam() == HORDE)
                                {
                                    if (!player->IsAlive())
                                    {
                                        player->ResurrectPlayer(1.0f);
                                        player->SpawnCorpseBones();
                                    }
                                    player->TeleportTo(0, -7.771f, -937.989f, 57.171268f, 2.615556f);
                                }
                            }
                        }
                    }
                }
                Trinity::RespawnDo u_do;
                Trinity::WorldObjectWorker<Trinity::RespawnDo> worker(me, u_do);
                me->VisitNearbyGridObject(me->GetGridActivationRange(), worker);
                //sWorld->setWorldState(WS_SOUTHSEA_BG, 0);
                if (!sGameEventMgr->IsActiveEvent(121))
                    sGameEventMgr->StartEvent(121, true);
                WorldPacket data(SMSG_PLAY_SOUND, 4);
                data << uint32(3439);
                sWorld->SendGlobalMessage(&data);
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界战场阵营领袖南海镇的吉安娜·普罗德摩尔出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }



            void EnterCombat(Unit* /*who*/) override
            {
                if (!Announced)
                {
                    char message[1024];
                    
                    snprintf(message, 1024, "|cffFFFF00世界战场阵营领袖南海镇的吉安娜·普罗德摩尔正在遭到部落的攻击！　|r");
                    
                    sWorld->SendGlobalText(message, NULL);
                    Announced = true;
                }
                if (HealthBelowPct(90))
                    HasWarnedPCT90 = true;
                if (HealthBelowPct(80))
                    HasWarnedPCT80 = true;
                if (HealthBelowPct(70))
                    HasWarnedPCT70 = true;
                if (HealthBelowPct(60))
                    HasWarnedPCT60 = true;
                if (HealthBelowPct(50))
                    HasWarnedPCT50 = true;
                if (HealthBelowPct(40))
                    HasWarnedPCT40 = true;
                if (HealthBelowPct(30))
                    HasWarnedPCT30 = true;
                if (HealthBelowPct(20))
                    HasWarnedPCT20 = true;
                if (HealthBelowPct(10))
                    HasWarnedPCT10 = true;
                if (HealthBelowPct(5))
                    HasWarnedPCT5 = true;
                WorldPacket data(SMSG_PLAY_SOUND, 4);
                data << uint32(8212);
                sWorld->SendGlobalMessage(&data);
                me->CastSpell(me, SPELL_ALLIANCE_GUARD2_1, true);
                me->CastSpell(me, SPELL_IMMUNE_TREAT, true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                summons.DespawnAll();
                if (sGameEventMgr->IsActiveEvent(121))
                    sGameEventMgr->StopEvent(121, true);
                WorldPacket data(SMSG_PLAY_SOUND, 4);
                data << uint32(8192);
                sWorld->SendGlobalMessage(&data);
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界战场希尔斯布莱德丘陵的南海镇被部落攻陷了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
                //sWorld->setWorldState(WS_SOUTHSEA_BG, 1);
            }

            void UpdateAI(uint32 diff) override
            {

                if (!UpdateVictim())
                    return;



                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (CheckTimer <= diff)
                {
                        CheckTimer = 10000;
                } else CheckTimer -= diff;

                if (HealthBelowPct(90) && !HasWarnedPCT90)
                {
                    if (Map const* map = me->GetMap())
                    {
                        Map::PlayerList const& plMap = map->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                        {
                            if (Player* player = itr->GetSource())
                            {
                                if (player->IsInWorld())
                                {
                                    if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                    {
                                        WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                        WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                        data1 << uint32(8173);
                                        data2 << uint32(11050);
                                        player->SendDirectMessage(&data1);
                                        player->SendDirectMessage(&data2);
                                    }
                                }
                            }
                        }
                    }
                    HasWarnedPCT90 = true;
                }
                if (HealthBelowPct(80) && !HasWarnedPCT80)
                {
                    if (Map const* map = me->GetMap())
                    {
                        Map::PlayerList const& plMap = map->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                        {
                            if (Player* player = itr->GetSource())
                            {
                                if (player->IsInWorld())
                                {
                                    if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                    {
                                        WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                        WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                        data1 << uint32(8173);
                                        data2 << uint32(11007);
                                        player->SendDirectMessage(&data1);
                                        player->SendDirectMessage(&data2);
                                    }
                                }
                            }
                        }
                    }
                    HasWarnedPCT80 = true;
                }
                if (HealthBelowPct(70) && !HasWarnedPCT70)
                {
                    if (Map const* map = me->GetMap())
                    {
                        Map::PlayerList const& plMap = map->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                        {
                            if (Player* player = itr->GetSource())
                            {
                                if (player->IsInWorld())
                                {
                                    if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                    {
                                        WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                        WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                        data1 << uint32(8173);
                                        data2 << uint32(11008);
                                        player->SendDirectMessage(&data1);
                                        player->SendDirectMessage(&data2);
                                    }
                                }
                            }
                        }
                    }
                    HasWarnedPCT70 = true;
                }
                if (HealthBelowPct(60) && !HasWarnedPCT60)
                {
                    if (Map const* map = me->GetMap())
                    {
                        Map::PlayerList const& plMap = map->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                        {
                            if (Player* player = itr->GetSource())
                            {
                                if (player->IsInWorld())
                                {
                                    if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                    {
                                        WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                        WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                        data1 << uint32(8173);
                                        data2 << uint32(11049);
                                        player->SendDirectMessage(&data1);
                                        player->SendDirectMessage(&data2);
                                    }
                                }
                            }
                        }
                    }
                    HasWarnedPCT60 = true;
                }
                if (HealthBelowPct(50) && !HasWarnedPCT50)
                {
                    if (Map const* map = me->GetMap())
                    {
                        Map::PlayerList const& plMap = map->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                        {
                            if (Player* player = itr->GetSource())
                            {
                                if (player->IsInWorld())
                                {
                                    if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                    {
                                        WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                        WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                        data1 << uint32(8173);
                                        data2 << uint32(11051);
                                        player->SendDirectMessage(&data1);
                                        player->SendDirectMessage(&data2);
                                    }
                                }
                            }
                        }
                    }
                    HasWarnedPCT50 = true;
                }
                if (HealthBelowPct(40) && !HasWarnedPCT40)
                {
                    if (Map const* map = me->GetMap())
                    {
                        Map::PlayerList const& plMap = map->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                        {
                            if (Player* player = itr->GetSource())
                            {
                                if (player->IsInWorld())
                                {
                                    if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                    {
                                        WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                        WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                        data1 << uint32(8173);
                                        data2 << uint32(11006);
                                        player->SendDirectMessage(&data1);
                                        player->SendDirectMessage(&data2);
                                    }
                                }
                            }
                        }
                    }
                    HasWarnedPCT40 = true;
                }
                if (HealthBelowPct(30) && !HasWarnedPCT30)
                {
                    if (Map const* map = me->GetMap())
                    {
                        Map::PlayerList const& plMap = map->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                        {
                            if (Player* player = itr->GetSource())
                            {
                                if (player->IsInWorld())
                                {
                                    if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                    {
                                        WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                        WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                        data1 << uint32(8173);
                                        data2 << uint32(16614);
                                        player->SendDirectMessage(&data1);
                                        player->SendDirectMessage(&data2);
                                    }
                                }
                            }
                        }
                    }
                    HasWarnedPCT30 = true;
                }
                if (HealthBelowPct(20) && !HasWarnedPCT20)
                {
                    if (Map const* map = me->GetMap())
                    {
                        Map::PlayerList const& plMap = map->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                        {
                            if (Player* player = itr->GetSource())
                            {
                                if (player->IsInWorld())
                                {
                                    if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                    {
                                        WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                        WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                        data1 << uint32(8173);
                                        data2 << uint32(11009);
                                        player->SendDirectMessage(&data1);
                                        player->SendDirectMessage(&data2);
                                    }
                                }
                            }
                        }
                    }
                    HasWarnedPCT20 = true;
                }
                if (HealthBelowPct(10) && !HasWarnedPCT10)
                {
                    if (Map const* map = me->GetMap())
                    {
                        Map::PlayerList const& plMap = map->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                        {
                            if (Player* player = itr->GetSource())
                            {
                                if (player->IsInWorld())
                                {
                                    if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                    {
                                        WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                        WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                        data1 << uint32(8173);
                                        data2 << uint32(11010);
                                        player->SendDirectMessage(&data1);
                                        player->SendDirectMessage(&data2);
                                    }
                                }
                            }
                        }
                    }
                    HasWarnedPCT10 = true;
                }
                if (HealthBelowPct(5) && !HasWarnedPCT5)
                {
                    if (Map const* map = me->GetMap())
                    {
                        Map::PlayerList const& plMap = map->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                        {
                            if (Player* player = itr->GetSource())
                            {
                                if (player->IsInWorld())
                                {
                                    if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                    {
                                        WorldPacket data(SMSG_PLAY_SOUND, 4);
                                        data << uint32(8232);
                                        player->SendDirectMessage(&data);
                                    }
                                }
                            }
                        }
                    }
                    HasWarnedPCT5 = true;
                }
                DoSpellAttackIfReady(SPELL_FROSTBOLT_WORLDBOSS10);
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss19_1AI(creature);
        }
};

class boss_worldboss19_2 : public CreatureScript
{
public:
    boss_worldboss19_2() : CreatureScript("boss_worldboss19_2") { }

    struct boss_worldboss19_2AI : public ScriptedAI
    {
        boss_worldboss19_2AI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }

        bool Announced;
        bool HasWarnedPCT5 = false;
        bool HasWarnedPCT10 = false;
        bool HasWarnedPCT20 = false;
        bool HasWarnedPCT30 = false;
        bool HasWarnedPCT40 = false;
        bool HasWarnedPCT50 = false;
        bool HasWarnedPCT60 = false;
        bool HasWarnedPCT70 = false;
        bool HasWarnedPCT80 = false;
        bool HasWarnedPCT90 = false;
        ObjectGuid WorldBoss19_1_GUID;
        SummonList summons;
        uint32 CheckTimer;

        void Reset() override
        {
            Announced = false;
            CheckTimer = 10000;
            HasWarnedPCT5 = false;
            HasWarnedPCT10 = false;
            HasWarnedPCT20 = false;
            HasWarnedPCT30 = false;
            HasWarnedPCT40 = false;
            HasWarnedPCT50 = false;
            HasWarnedPCT60 = false;
            HasWarnedPCT70 = false;
            HasWarnedPCT80 = false;
            HasWarnedPCT90 = false;
            summons.DespawnAll();
        }

        void DamageTaken(Unit* attacker, uint32 &damage) override
        {
            if (damage >= 100000)
            {
                damage = 0;
            }
            if (damage >= me->GetHealth())
            {
                if (Map const* map = me->GetMap())
                {
                    uint32 AllianceCount = 0;
                    uint32 HordeCount = 0;
                    float AllianceHealthPct = 100.0f;
                    if (Creature* WorldBoss19_1 = ObjectAccessor::GetCreature(*me, WorldBoss19_1_GUID))
                    {
                        AllianceHealthPct = WorldBoss19_1->GetHealthPct();
                    }
                    Map::PlayerList const& plMap = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                    {
                        if (Player* player = itr->GetSource())
                        {
                            if (player->IsInWorld())
                            {
                                if (player->GetZoneId() == 267 && player->getLevel() >= 80 && player->GetTeam() == ALLIANCE)
                                    AllianceCount++;
                                else
                                if (player->GetZoneId() == 267 && player->getLevel() >= 80 && player->GetTeam() == HORDE)
                                    HordeCount++;
                            }
                        }
                    }
                    for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                    {
                        if (Player* player = itr->GetSource())
                        {
                            if (player->IsInWorld())
                            {
                                if (player->GetZoneId() == 267 && player->getLevel() >= 80 && player->GetTeam() == ALLIANCE)
                                {
                                    WorldPacket data(SMSG_PLAY_SOUND, 4);
                                    data << uint32(8455);
                                    player->SendDirectMessage(&data);
                                    if (AllianceCount > 0)
                                    {
                                        player->ModifyHonorPoints(int32(595200/AllianceCount));
                                        player->ModifyArenaPoints(int32(40000/AllianceCount));
                                        player->CastSpell(player, 90042, true);
                                        if (urand(1,100) == 1)
                                        {
                                            uint32 itemId = urand(60397, 60406);
                                            player->AddItem(itemId, 1);
                                        }
                                        ChatHandler(player->GetSession()).PSendSysMessage("|cffFFFF00成功击杀部落阵营领袖，你获得了　%u　点荣誉点数和　%u　点竞技场点数。　|r", int32(595200/AllianceCount), int32(40000/AllianceCount));
                                    }
                                }
                                else
                                if (player->GetZoneId() == 267 && player->getLevel() >= 80 && player->GetTeam() == HORDE)
                                {
                                    WorldPacket data(SMSG_PLAY_SOUND, 4);
                                    data << uint32(8455);
                                    player->SendDirectMessage(&data);
                                    if (HordeCount > 0)
                                    {
                                        player->ModifyHonorPoints(int32((595200*(100-AllianceHealthPct)/100)/HordeCount));
                                        player->ModifyArenaPoints(int32((40000*(100-AllianceHealthPct)/100)/HordeCount));
                                        player->CastSpell(player, 90193, true);
                                        ChatHandler(player->GetSession()).PSendSysMessage("|cffFFFF00阵营领袖被联盟击杀，你获得了　%u　点荣誉点数和　%u　点竞技场点数。　|r", int32((595200*(100-AllianceHealthPct)/100)/HordeCount), int32((40000*(100-AllianceHealthPct)/100)/HordeCount));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } 

        void IsSummonedBy(Unit* summoner) override
        {
            if (summoner->ToCreature())
            {
                WorldBoss19_1_GUID = summoner->ToCreature()->GetGUID();
                Trinity::RespawnDo u_do;
                Trinity::WorldObjectWorker<Trinity::RespawnDo> worker(me, u_do);
                me->VisitNearbyGridObject(me->GetGridActivationRange(), worker);
                WorldPacket data(SMSG_PLAY_SOUND, 4);
                data << uint32(11803);
                sWorld->SendGlobalMessage(&data);
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界战场阵营领袖塔伦米尔的希尔瓦娜斯·风行者出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }
        }

        void JustRespawned() override
        {
            summons.DespawnAll();
            float x, y, z, o;
            me->GetHomePosition(x, y, z, o);
            DoTeleportTo(x, y, z);
            WorldPacket data(SMSG_PLAY_SOUND, 4);
            data << uint32(11803);
            sWorld->SendGlobalMessage(&data);
            char message[1024];
            
            snprintf(message, 1024, "|cffFFFF00世界战场阵营领袖塔伦米尔的希尔瓦娜斯·风行者出现了！　|r");
            
            sWorld->SendGlobalText(message, NULL);
        }



        void EnterCombat(Unit* /*who*/) override
        {
            if (!Announced)
            {
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界战场阵营领袖塔伦米尔的希尔瓦娜斯·风行者正在遭到联盟的攻击！　|r");
                
                sWorld->SendGlobalText(message, NULL);
                Announced = true;
            }
            if (HealthBelowPct(90))
                HasWarnedPCT90 = true;
            if (HealthBelowPct(80))
                HasWarnedPCT80 = true;
            if (HealthBelowPct(70))
                HasWarnedPCT70 = true;
            if (HealthBelowPct(60))
                HasWarnedPCT60 = true;
            if (HealthBelowPct(50))
                HasWarnedPCT50 = true;
            if (HealthBelowPct(40))
                HasWarnedPCT40 = true;
            if (HealthBelowPct(30))
                HasWarnedPCT30 = true;
            if (HealthBelowPct(20))
                HasWarnedPCT20 = true;
            if (HealthBelowPct(10))
                HasWarnedPCT10 = true;
            if (HealthBelowPct(5))
                HasWarnedPCT5 = true;
            WorldPacket data(SMSG_PLAY_SOUND, 4);
            data << uint32(8174);
            sWorld->SendGlobalMessage(&data);
            me->CastSpell(me, SPELL_HORDE_GUARD2_1, true);
            me->CastSpell(me, SPELL_IMMUNE_TREAT, true);
        }

        void JustDied(Unit* /*killer*/) override
        {
            summons.DespawnAll();
            if (sGameEventMgr->IsActiveEvent(121))
                sGameEventMgr->StopEvent(121, true);
            WorldPacket data(SMSG_PLAY_SOUND, 4);
            data << uint32(8192);
            sWorld->SendGlobalMessage(&data);
            char message[1024];
            
            snprintf(message, 1024, "|cffFFFF00世界战场希尔斯布莱德丘陵的塔伦米尔被联盟攻陷了！　|r");
            
            sWorld->SendGlobalText(message, NULL);
            //sWorld->setWorldState(WS_SOUTHSEA_BG, 1);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

 

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (CheckTimer <= diff)
            {
                    CheckTimer = 10000;
            } else CheckTimer -= diff;

            if (HealthBelowPct(90) && !HasWarnedPCT90)
            {
                if (Map const* map = me->GetMap())
                {
                    Map::PlayerList const& plMap = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                    {
                        if (Player* player = itr->GetSource())
                        {
                            if (player->IsInWorld())
                            {
                                if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                {
                                    WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                    WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                    data1 << uint32(8213);
                                    data2 << uint32(16312);
                                    player->SendDirectMessage(&data1);
                                    player->SendDirectMessage(&data2);
                                }
                            }
                        }
                    }
                }
                HasWarnedPCT90 = true;
            }
            if (HealthBelowPct(80) && !HasWarnedPCT80)
            {
                if (Map const* map = me->GetMap())
                {
                    Map::PlayerList const& plMap = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                    {
                        if (Player* player = itr->GetSource())
                        {
                            if (player->IsInWorld())
                            {
                                if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                {
                                    WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                    WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                    data1 << uint32(8213);
                                    data2 << uint32(16310);
                                    player->SendDirectMessage(&data1);
                                    player->SendDirectMessage(&data2);
                                }
                            }
                        }
                    }
                }
                HasWarnedPCT80 = true;
            }
            if (HealthBelowPct(70) && !HasWarnedPCT70)
            {
                if (Map const* map = me->GetMap())
                {
                    Map::PlayerList const& plMap = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                    {
                        if (Player* player = itr->GetSource())
                        {
                            if (player->IsInWorld())
                            {
                                if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                {
                                    WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                    WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                    data1 << uint32(8213);
                                    data2 << uint32(16313);
                                    player->SendDirectMessage(&data1);
                                    player->SendDirectMessage(&data2);
                                }
                            }
                        }
                    }
                }
                HasWarnedPCT70 = true;
            }
            if (HealthBelowPct(60) && !HasWarnedPCT60)
            {
                if (Map const* map = me->GetMap())
                {
                    Map::PlayerList const& plMap = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                    {
                        if (Player* player = itr->GetSource())
                        {
                            if (player->IsInWorld())
                            {
                                if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                {
                                    WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                    WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                    data1 << uint32(8213);
                                    data2 << uint32(16314);
                                    player->SendDirectMessage(&data1);
                                    player->SendDirectMessage(&data2);
                                }
                            }
                        }
                    }
                }
                HasWarnedPCT60 = true;
            }
            if (HealthBelowPct(50) && !HasWarnedPCT50)
            {
                if (Map const* map = me->GetMap())
                {
                    Map::PlayerList const& plMap = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                    {
                        if (Player* player = itr->GetSource())
                        {
                            if (player->IsInWorld())
                            {
                                if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                {
                                    WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                    WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                    data1 << uint32(8213);
                                    data2 << uint32(17033);
                                    player->SendDirectMessage(&data1);
                                    player->SendDirectMessage(&data2);
                                }
                            }
                        }
                    }
                }
                HasWarnedPCT50 = true;
            }
            if (HealthBelowPct(40) && !HasWarnedPCT40)
            {
                if (Map const* map = me->GetMap())
                {
                    Map::PlayerList const& plMap = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                    {
                        if (Player* player = itr->GetSource())
                        {
                            if (player->IsInWorld())
                            {
                                if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                {
                                    WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                    WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                    data1 << uint32(8213);
                                    data2 << uint32(17046);
                                    player->SendDirectMessage(&data1);
                                    player->SendDirectMessage(&data2);
                                }
                            }
                        }
                    }
                }
                HasWarnedPCT40 = true;
            }
            if (HealthBelowPct(30) && !HasWarnedPCT30)
            {
                if (Map const* map = me->GetMap())
                {
                    Map::PlayerList const& plMap = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                    {
                        if (Player* player = itr->GetSource())
                        {
                            if (player->IsInWorld())
                            {
                                if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                {
                                    WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                    WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                    data1 << uint32(8213);
                                    data2 << uint32(17045);
                                    player->SendDirectMessage(&data1);
                                    player->SendDirectMessage(&data2);
                                }
                            }
                        }
                    }
                }
                HasWarnedPCT30 = true;
            }
            if (HealthBelowPct(20) && !HasWarnedPCT20)
            {
                if (Map const* map = me->GetMap())
                {
                    Map::PlayerList const& plMap = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                    {
                        if (Player* player = itr->GetSource())
                        {
                            if (player->IsInWorld())
                            {
                                if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                {
                                    WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                    WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                    data1 << uint32(8213);
                                    data2 << uint32(16309);
                                    player->SendDirectMessage(&data1);
                                    player->SendDirectMessage(&data2);
                                }
                            }
                        }
                    }
                }
                HasWarnedPCT20 = true;
            }
            if (HealthBelowPct(10) && !HasWarnedPCT10)
            {
                if (Map const* map = me->GetMap())
                {
                    Map::PlayerList const& plMap = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                    {
                        if (Player* player = itr->GetSource())
                        {
                            if (player->IsInWorld())
                            {
                                if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                {
                                    WorldPacket data1(SMSG_PLAY_SOUND, 4);
                                    WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                    data1 << uint32(8213);
                                    data2 << uint32(16301);
                                    player->SendDirectMessage(&data1);
                                    player->SendDirectMessage(&data2);
                                }
                            }
                        }
                    }
                }
                HasWarnedPCT10 = true;
            }
            if (HealthBelowPct(5) && !HasWarnedPCT5)
            {
                if (Map const* map = me->GetMap())
                {
                    Map::PlayerList const& plMap = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                    {
                        if (Player* player = itr->GetSource())
                        {
                            if (player->IsInWorld())
                            {
                                if (player->GetZoneId() == 267 && player->getLevel() >= 80)
                                {
                                    WorldPacket data(SMSG_PLAY_SOUND, 4);
                                    data << uint32(8456);
                                    player->SendDirectMessage(&data);
                                }
                            }
                        }
                    }
                }
                HasWarnedPCT5 = true;
            }
            DoSpellAttackIfReady(SPELL_SHOOT);
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_worldboss19_2AI(creature);
    }
};

class boss_worldboss19_3 : public CreatureScript
{
    public:
        boss_worldboss19_3() : CreatureScript("boss_worldboss19_3") { }

        struct boss_worldboss19_3AI : public ScriptedAI
        {
            boss_worldboss19_3AI(Creature* creature) : ScriptedAI(creature) { }

            void IsSummonedBy(Unit* summoner) override
            {
                DoCast(me, SPELL_STEALTH);
                CheckTimer = 10000;
                if (summoner->GetTypeId() == TYPEID_PLAYER)
                    AttackStart(summoner);
            }

            uint32 CheckTimer;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_FAN_OF_KNIVES, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_BLIND, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_CLOAK, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_BLADE_FLURRY, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_SHADOWSTEP, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_HEMORRHAGE, urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_EVISCERATE, urand(20*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_WOUND_POISON, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_THREAT_ROGUE, 5*IN_MILLISECONDS);
                me->setPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 100);
                DoCast(me, SPELL_STEALTH);
                CheckTimer = 10000;
            }

            void UpdateAI(uint32 diff) override
            {
                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (CheckTimer <= diff)
                {
                    CheckTimer = 10000;
                } else CheckTimer -= diff;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FAN_OF_KNIVES:
                            DoCastAOE(SPELL_FAN_OF_KNIVES);
                            _events.ScheduleEvent(EVENT_FAN_OF_KNIVES, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_BLIND:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_BLIND);
                            _events.ScheduleEvent(EVENT_BLIND, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_CLOAK:
                            if (HealthBelowPct(50))
                            {
                                DoCast(SPELL_CLOAK);
                                _events.RescheduleEvent(EVENT_CLOAK, 90*IN_MILLISECONDS);
                            }
                            else
                                _events.RescheduleEvent(EVENT_CLOAK, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_BLADE_FLURRY:
                            DoCast(SPELL_BLADE_FLURRY);
                            _events.RescheduleEvent(EVENT_BLADE_FLURRY, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_SHADOWSTEP:
                            if (Unit* target = me->GetVictim())
                            {
                                if (me->IsInRange(target, 10.0f, 40.0f, false))
                                {
                                    DoCast(target, SPELL_SHADOWSTEP);
                                    _events.RescheduleEvent(EVENT_SHADOWSTEP, 30*IN_MILLISECONDS);
                                    return;
                                }
                            }
                            _events.RescheduleEvent(EVENT_SHADOWSTEP, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_HEMORRHAGE:
                            DoCastVictim(SPELL_HEMORRHAGE);
                            _events.ScheduleEvent(EVENT_HEMORRHAGE, urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_EVISCERATE:
                            DoCastVictim(SPELL_EVISCERATE);
                            _events.ScheduleEvent(EVENT_EVISCERATE, urand(30*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_WOUND_POISON:
                            DoCastVictim(SPELL_WOUND_POISON);
                            _events.ScheduleEvent(EVENT_WOUND_POISON, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_THREAT_ROGUE:
                        {
                            std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
                            {
                                Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
                                if (unit && me->getThreatManager().getThreat(unit))
                                {
                                    if (unit->GetTypeId() == TYPEID_PLAYER)
                                    {
                                        float dist_mod = 15.0f / (15.0f + me->GetDistance2d(unit));
                                        float armor_mod = (float)unit->GetArmor() / 16635.0f;
                                        float eh = (unit->GetHealth() + 1) * (1.0f + armor_mod);
                                        float threat = dist_mod * 30000.0f / eh;
                                        me->getThreatManager().modifyThreatPercent(unit, -100);
                                        me->AddThreat(unit, 1000000.0f * threat);
                                    }
                                }
                            }
                            _events.ScheduleEvent(EVENT_THREAT_ROGUE, 4*IN_MILLISECONDS);
                            break;
                        }
                        default:
                            return;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss19_3AI(creature);
        }
};

class boss_worldboss20 : public CreatureScript
{
    public:
        boss_worldboss20() : CreatureScript("boss_worldboss20") { }

        struct boss_worldboss20AI : public ScriptedAI
        {
            boss_worldboss20AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_1, urand(6000, 10000));
                _events.ScheduleEvent(EVENT_2, 7000);
                _events.ScheduleEvent(EVENT_3, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_4, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_5, 3000);
                _events.ScheduleEvent(EVENT_6, 30000);
                _events.ScheduleEvent(EVENT_7, 900000);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领昆莱山粗麻小径的怒之煞出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                char message[1024];
                
                if (who->GetTypeId() == TYPEID_PLAYER)
                {
                    if (who->ToPlayer()->GetTeam()==ALLIANCE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领昆莱山粗麻小径的怒之煞正在遭到联盟的攻击！　|r");
                        
                    }
                    else
                    if (who->ToPlayer()->GetTeam()==HORDE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领昆莱山粗麻小径的怒之煞正在遭到部落的攻击！　|r");
                        
                    }
                }
                else
                if (who->GetTypeId() == TYPEID_UNIT)
                {
                    if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                    {
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领昆莱山粗麻小径的怒之煞正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领昆莱山粗麻小径的怒之煞正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->IsPet())
                    {
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领昆莱山粗麻小径的怒之煞正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领昆莱山粗麻小径的怒之煞正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领昆莱山粗麻小径的怒之煞正在遭到攻击！　|r");
                        
                    }
                }
                else
                {
                    snprintf(message, 1024, "|cffFFFF00世界首领昆莱山粗麻小径的怒之煞正在遭到攻击！　|r");
                    
                }
                sWorld->SendGlobalText(message, NULL);
                Announced = true;
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if ((me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f) || me->GetDistance(me->GetVictim()) < 0.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_1:
                            DoCastVictim(SPELL_SHADOW_VOLLEY);
                            _events.ScheduleEvent(EVENT_1, urand(4000, 6000));
                            break;
                        case EVENT_2:
                            DoCastVictim(SPELL_CLEAVE);
                            _events.ScheduleEvent(EVENT_2, urand(8000, 12000));
                            break;
                        case EVENT_3:
                            DoCastVictim(SPELL_THUNDERCLAP);
                            _events.ScheduleEvent(EVENT_3, urand(10000, 14000));
                            break;
                        case EVENT_4:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_SHADOW_TRAP);
                            _events.ScheduleEvent(EVENT_4, urand(10000, 14000));
                            break;
                        case EVENT_5:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_ARCANE_ORB);
                            _events.ScheduleEvent(EVENT_5, 3000);
                            break;
                        case EVENT_6:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_6, 30000);
                            break;
                        case EVENT_7:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_7, 10000);
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss20AI(creature);
        }
};

class boss_worldboss21 : public CreatureScript
{
    public:
        boss_worldboss21() : CreatureScript("boss_worldboss21") { }

        struct boss_worldboss21AI : public ScriptedAI
        {
            boss_worldboss21AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_1, urand(6000, 10000));
                _events.ScheduleEvent(EVENT_2, 7000);
                _events.ScheduleEvent(EVENT_3, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_4, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_5, 3000);
                _events.ScheduleEvent(EVENT_6, 30000);
                _events.ScheduleEvent(EVENT_7, 900000);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领四风谷滚雷台的炮舰出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                char message[1024];
                
                if (who->GetTypeId() == TYPEID_PLAYER)
                {
                    if (who->ToPlayer()->GetTeam()==ALLIANCE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领四风谷滚雷台的炮舰正在遭到联盟的攻击！　|r");
                        
                    }
                    else
                    if (who->ToPlayer()->GetTeam()==HORDE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领四风谷滚雷台的炮舰正在遭到部落的攻击！　|r");
                        
                    }
                }
                else
                if (who->GetTypeId() == TYPEID_UNIT)
                {
                    if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                    {
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领四风谷滚雷台的炮舰正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领四风谷滚雷台的炮舰正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->IsPet())
                    {
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领四风谷滚雷台的炮舰正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领四风谷滚雷台的炮舰正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领四风谷滚雷台的炮舰正在遭到攻击！　|r");
                        
                    }
                }
                else
                {
                    snprintf(message, 1024, "|cffFFFF00世界首领四风谷滚雷台的炮舰正在遭到攻击！　|r");
                    
                }
                sWorld->SendGlobalText(message, NULL);
                Announced = true;
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if ((me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f) || me->GetDistance(me->GetVictim()) < 0.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_1:
                            DoCastVictim(SPELL_SHADOW_VOLLEY);
                            _events.ScheduleEvent(EVENT_1, urand(4000, 6000));
                            break;
                        case EVENT_2:
                            DoCastVictim(SPELL_CLEAVE);
                            _events.ScheduleEvent(EVENT_2, urand(8000, 12000));
                            break;
                        case EVENT_3:
                            DoCastVictim(SPELL_THUNDERCLAP);
                            _events.ScheduleEvent(EVENT_3, urand(10000, 14000));
                            break;
                        case EVENT_4:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_SHADOW_TRAP);
                            _events.ScheduleEvent(EVENT_4, urand(10000, 14000));
                            break;
                        case EVENT_5:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_ARCANE_ORB);
                            _events.ScheduleEvent(EVENT_5, 3000);
                            break;
                        case EVENT_6:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_6, 30000);
                            break;
                        case EVENT_7:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_7, 10000);
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss21AI(creature);
        }
};

class boss_worldboss22 : public CreatureScript
{
    public:
        boss_worldboss22() : CreatureScript("boss_worldboss22") { }

        struct boss_worldboss22AI : public ScriptedAI
        {
            boss_worldboss22AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_1, urand(6000, 10000));
                _events.ScheduleEvent(EVENT_2, 7000);
                _events.ScheduleEvent(EVENT_3, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_4, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_5, 3000);
                _events.ScheduleEvent(EVENT_6, 30000);
                _events.ScheduleEvent(EVENT_7, 900000);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领雷神岛雷神之基的暴风领主纳拉克出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                char message[1024];
                
                if (who->GetTypeId() == TYPEID_PLAYER)
                {
                    if (who->ToPlayer()->GetTeam()==ALLIANCE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领雷神岛雷神之基的暴风领主纳拉克正在遭到联盟的攻击！　|r");
                        
                    }
                    else
                    if (who->ToPlayer()->GetTeam()==HORDE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领雷神岛雷神之基的暴风领主纳拉克正在遭到部落的攻击！　|r");
                        
                    }
                }
                else
                if (who->GetTypeId() == TYPEID_UNIT)
                {
                    if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                    {
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领雷神岛雷神之基的暴风领主纳拉克正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领雷神岛雷神之基的暴风领主纳拉克正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->IsPet())
                    {
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领雷神岛雷神之基的暴风领主纳拉克正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领雷神岛雷神之基的暴风领主纳拉克正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领雷神岛雷神之基的暴风领主纳拉克正在遭到攻击！　|r");
                        
                    }
                }
                else
                {
                    snprintf(message, 1024, "|cffFFFF00世界首领雷神岛雷神之基的暴风领主纳拉克正在遭到攻击！　|r");
                    
                }
                sWorld->SendGlobalText(message, NULL);
                Announced = true;
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if ((me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f) || me->GetDistance(me->GetVictim()) < 0.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_1:
                            DoCastVictim(SPELL_SHADOW_VOLLEY);
                            _events.ScheduleEvent(EVENT_1, urand(4000, 6000));
                            break;
                        case EVENT_2:
                            DoCastVictim(SPELL_CLEAVE);
                            _events.ScheduleEvent(EVENT_2, urand(8000, 12000));
                            break;
                        case EVENT_3:
                            DoCastVictim(SPELL_THUNDERCLAP);
                            _events.ScheduleEvent(EVENT_3, urand(10000, 14000));
                            break;
                        case EVENT_4:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_SHADOW_TRAP);
                            _events.ScheduleEvent(EVENT_4, urand(10000, 14000));
                            break;
                        case EVENT_5:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_ARCANE_ORB);
                            _events.ScheduleEvent(EVENT_5, 3000);
                            break;
                        case EVENT_6:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_6, 30000);
                            break;
                        case EVENT_7:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_7, 10000);
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss22AI(creature);
        }
};

class boss_worldboss23 : public CreatureScript
{
    public:
        boss_worldboss23() : CreatureScript("boss_worldboss23") { }

        struct boss_worldboss23AI : public ScriptedAI
        {
            boss_worldboss23AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_1, urand(6000, 10000));
                _events.ScheduleEvent(EVENT_2, 7000);
                _events.ScheduleEvent(EVENT_3, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_4, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_5, 3000);
                _events.ScheduleEvent(EVENT_6, 30000);
                _events.ScheduleEvent(EVENT_7, 900000);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领巨兽岛的乌达斯塔出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                char message[1024];
                
                if (who->GetTypeId() == TYPEID_PLAYER)
                {
                    if (who->ToPlayer()->GetTeam()==ALLIANCE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领巨兽岛的乌达斯塔正在遭到联盟的攻击！　|r");
                        
                    }
                    else
                    if (who->ToPlayer()->GetTeam()==HORDE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领巨兽岛的乌达斯塔正在遭到部落的攻击！　|r");
                        
                    }
                }
                else
                if (who->GetTypeId() == TYPEID_UNIT)
                {
                    if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                    {
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领巨兽岛的乌达斯塔正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领巨兽岛的乌达斯塔正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->IsPet())
                    {
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领巨兽岛的乌达斯塔正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领巨兽岛的乌达斯塔正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领巨兽岛的乌达斯塔正在遭到攻击！　|r");
                        
                    }
                }
                else
                {
                    snprintf(message, 1024, "|cffFFFF00世界首领巨兽岛的乌达斯塔正在遭到攻击！　|r");
                    
                }
                sWorld->SendGlobalText(message, NULL);
                Announced = true;
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if ((me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f) || me->GetDistance(me->GetVictim()) < 0.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_1:
                            DoCastVictim(SPELL_SHADOW_VOLLEY);
                            _events.ScheduleEvent(EVENT_1, urand(4000, 6000));
                            break;
                        case EVENT_2:
                            DoCastVictim(SPELL_CLEAVE);
                            _events.ScheduleEvent(EVENT_2, urand(8000, 12000));
                            break;
                        case EVENT_3:
                            DoCastVictim(SPELL_THUNDERCLAP);
                            _events.ScheduleEvent(EVENT_3, urand(10000, 14000));
                            break;
                        case EVENT_4:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_SHADOW_TRAP);
                            _events.ScheduleEvent(EVENT_4, urand(10000, 14000));
                            break;
                        case EVENT_5:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_ARCANE_ORB);
                            _events.ScheduleEvent(EVENT_5, 3000);
                            break;
                        case EVENT_6:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_6, 30000);
                            break;
                        case EVENT_7:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_7, 10000);
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss23AI(creature);
        }
};

class boss_worldboss24 : public CreatureScript
{
    public:
        boss_worldboss24() : CreatureScript("boss_worldboss24") { }

        struct boss_worldboss24AI : public ScriptedAI
        {
            boss_worldboss24AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_1, urand(6000, 10000));
                _events.ScheduleEvent(EVENT_2, 7000);
                _events.ScheduleEvent(EVENT_3, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_4, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_5, 3000);
                _events.ScheduleEvent(EVENT_6, 30000);
                _events.ScheduleEvent(EVENT_7, 900000);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的玉珑出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                char message[1024];
                
                if (who->GetTypeId() == TYPEID_PLAYER)
                {
                    if (who->ToPlayer()->GetTeam()==ALLIANCE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的玉珑正在遭到联盟的攻击！　|r");
                        
                    }
                    else
                    if (who->ToPlayer()->GetTeam()==HORDE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的玉珑正在遭到部落的攻击！　|r");
                        
                    }
                }
                else
                if (who->GetTypeId() == TYPEID_UNIT)
                {
                    if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                    {
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的玉珑正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的玉珑正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->IsPet())
                    {
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的玉珑正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的玉珑正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的玉珑正在遭到攻击！　|r");
                        
                    }
                }
                else
                {
                    snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的玉珑正在遭到攻击！　|r");
                    
                }
                sWorld->SendGlobalText(message, NULL);
                Announced = true;
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if ((me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f) || me->GetDistance(me->GetVictim()) < 0.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_1:
                            DoCastVictim(SPELL_SHADOW_VOLLEY);
                            _events.ScheduleEvent(EVENT_1, urand(4000, 6000));
                            break;
                        case EVENT_2:
                            DoCastVictim(SPELL_CLEAVE);
                            _events.ScheduleEvent(EVENT_2, urand(8000, 12000));
                            break;
                        case EVENT_3:
                            DoCastVictim(SPELL_THUNDERCLAP);
                            _events.ScheduleEvent(EVENT_3, urand(10000, 14000));
                            break;
                        case EVENT_4:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_SHADOW_TRAP);
                            _events.ScheduleEvent(EVENT_4, urand(10000, 14000));
                            break;
                        case EVENT_5:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_ARCANE_ORB);
                            _events.ScheduleEvent(EVENT_5, 3000);
                            break;
                        case EVENT_6:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_6, 30000);
                            break;
                        case EVENT_7:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_7, 10000);
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss24AI(creature);
        }
};

class boss_worldboss25 : public CreatureScript
{
    public:
        boss_worldboss25() : CreatureScript("boss_worldboss25") { }

        struct boss_worldboss25AI : public ScriptedAI
        {
            boss_worldboss25AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_1, urand(6000, 10000));
                _events.ScheduleEvent(EVENT_2, 7000);
                _events.ScheduleEvent(EVENT_3, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_4, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_5, 3000);
                _events.ScheduleEvent(EVENT_6, 30000);
                _events.ScheduleEvent(EVENT_7, 900000);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的赤精出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                char message[1024];
                
                if (who->GetTypeId() == TYPEID_PLAYER)
                {
                    if (who->ToPlayer()->GetTeam()==ALLIANCE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的赤精正在遭到联盟的攻击！　|r");
                        
                    }
                    else
                    if (who->ToPlayer()->GetTeam()==HORDE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的赤精正在遭到部落的攻击！　|r");
                        
                    }
                }
                else
                if (who->GetTypeId() == TYPEID_UNIT)
                {
                    if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                    {
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的赤精正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的赤精正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->IsPet())
                    {
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的赤精正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的赤精正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的赤精正在遭到攻击！　|r");
                        
                    }
                }
                else
                {
                    snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的赤精正在遭到攻击！　|r");
                    
                }
                sWorld->SendGlobalText(message, NULL);
                Announced = true;
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if ((me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f) || me->GetDistance(me->GetVictim()) < 0.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_1:
                            DoCastVictim(SPELL_SHADOW_VOLLEY);
                            _events.ScheduleEvent(EVENT_1, urand(4000, 6000));
                            break;
                        case EVENT_2:
                            DoCastVictim(SPELL_CLEAVE);
                            _events.ScheduleEvent(EVENT_2, urand(8000, 12000));
                            break;
                        case EVENT_3:
                            DoCastVictim(SPELL_THUNDERCLAP);
                            _events.ScheduleEvent(EVENT_3, urand(10000, 14000));
                            break;
                        case EVENT_4:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_SHADOW_TRAP);
                            _events.ScheduleEvent(EVENT_4, urand(10000, 14000));
                            break;
                        case EVENT_5:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_ARCANE_ORB);
                            _events.ScheduleEvent(EVENT_5, 3000);
                            break;
                        case EVENT_6:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_6, 30000);
                            break;
                        case EVENT_7:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_7, 10000);
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss25AI(creature);
        }
};

class boss_worldboss26 : public CreatureScript
{
    public:
        boss_worldboss26() : CreatureScript("boss_worldboss26") { }

        struct boss_worldboss26AI : public ScriptedAI
        {
            boss_worldboss26AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_1, urand(6000, 10000));
                _events.ScheduleEvent(EVENT_2, 7000);
                _events.ScheduleEvent(EVENT_3, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_4, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_5, 3000);
                _events.ScheduleEvent(EVENT_6, 30000);
                _events.ScheduleEvent(EVENT_7, 900000);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的雪怒出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                char message[1024];
                
                if (who->GetTypeId() == TYPEID_PLAYER)
                {
                    if (who->ToPlayer()->GetTeam()==ALLIANCE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的雪怒正在遭到联盟的攻击！　|r");
                        
                    }
                    else
                    if (who->ToPlayer()->GetTeam()==HORDE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的雪怒正在遭到部落的攻击！　|r");
                        
                    }
                }
                else
                if (who->GetTypeId() == TYPEID_UNIT)
                {
                    if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                    {
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的雪怒正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的雪怒正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->IsPet())
                    {
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的雪怒正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的雪怒正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的雪怒正在遭到攻击！　|r");
                        
                    }
                }
                else
                {
                    snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的雪怒正在遭到攻击！　|r");
                    
                }
                sWorld->SendGlobalText(message, NULL);
                Announced = true;
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if ((me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f) || me->GetDistance(me->GetVictim()) < 0.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_1:
                            DoCastVictim(SPELL_SHADOW_VOLLEY);
                            _events.ScheduleEvent(EVENT_1, urand(4000, 6000));
                            break;
                        case EVENT_2:
                            DoCastVictim(SPELL_CLEAVE);
                            _events.ScheduleEvent(EVENT_2, urand(8000, 12000));
                            break;
                        case EVENT_3:
                            DoCastVictim(SPELL_THUNDERCLAP);
                            _events.ScheduleEvent(EVENT_3, urand(10000, 14000));
                            break;
                        case EVENT_4:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_SHADOW_TRAP);
                            _events.ScheduleEvent(EVENT_4, urand(10000, 14000));
                            break;
                        case EVENT_5:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_ARCANE_ORB);
                            _events.ScheduleEvent(EVENT_5, 3000);
                            break;
                        case EVENT_6:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_6, 30000);
                            break;
                        case EVENT_7:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_7, 10000);
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss26AI(creature);
        }
};

class boss_worldboss27 : public CreatureScript
{
    public:
        boss_worldboss27() : CreatureScript("boss_worldboss27") { }

        struct boss_worldboss27AI : public ScriptedAI
        {
            boss_worldboss27AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_1, urand(6000, 10000));
                _events.ScheduleEvent(EVENT_2, 7000);
                _events.ScheduleEvent(EVENT_3, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_4, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_5, 3000);
                _events.ScheduleEvent(EVENT_6, 30000);
                _events.ScheduleEvent(EVENT_7, 900000);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的砮皂出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                char message[1024];
                
                if (who->GetTypeId() == TYPEID_PLAYER)
                {
                    if (who->ToPlayer()->GetTeam()==ALLIANCE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的砮皂正在遭到联盟的攻击！　|r");
                        
                    }
                    else
                    if (who->ToPlayer()->GetTeam()==HORDE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的砮皂正在遭到部落的攻击！　|r");
                        
                    }
                }
                else
                if (who->GetTypeId() == TYPEID_UNIT)
                {
                    if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                    {
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的砮皂正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的砮皂正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->IsPet())
                    {
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的砮皂正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的砮皂正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的砮皂正在遭到攻击！　|r");
                        
                    }
                }
                else
                {
                    snprintf(message, 1024, "|cffFFFF00世界首领永恒岛天神庭院的砮皂正在遭到攻击！　|r");
                    
                }
                sWorld->SendGlobalText(message, NULL);
                Announced = true;
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if ((me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f) || me->GetDistance(me->GetVictim()) < 0.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_1:
                            DoCastVictim(SPELL_SHADOW_VOLLEY);
                            _events.ScheduleEvent(EVENT_1, urand(4000, 6000));
                            break;
                        case EVENT_2:
                            DoCastVictim(SPELL_CLEAVE);
                            _events.ScheduleEvent(EVENT_2, urand(8000, 12000));
                            break;
                        case EVENT_3:
                            DoCastVictim(SPELL_THUNDERCLAP);
                            _events.ScheduleEvent(EVENT_3, urand(10000, 14000));
                            break;
                        case EVENT_4:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_SHADOW_TRAP);
                            _events.ScheduleEvent(EVENT_4, urand(10000, 14000));
                            break;
                        case EVENT_5:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_ARCANE_ORB);
                            _events.ScheduleEvent(EVENT_5, 3000);
                            break;
                        case EVENT_6:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_6, 30000);
                            break;
                        case EVENT_7:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_7, 10000);
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss27AI(creature);
        }
};

class boss_worldboss28 : public CreatureScript
{
    public:
        boss_worldboss28() : CreatureScript("boss_worldboss28") { }

        struct boss_worldboss28AI : public ScriptedAI
        {
            boss_worldboss28AI(Creature* creature) : ScriptedAI(creature)
            {
            }

            bool Announced;

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_1, urand(6000, 10000));
                _events.ScheduleEvent(EVENT_2, 7000);
                _events.ScheduleEvent(EVENT_3, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_4, urand(14000, 18000));
                _events.ScheduleEvent(EVENT_5, 3000);
                _events.ScheduleEvent(EVENT_6, 30000);
                _events.ScheduleEvent(EVENT_7, 900000);
                Announced = false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage) override
            {
                if (damage >= 100000)
                {
                    damage = 0;
                }
                if (damage >= me->GetHealth())
                {
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (target->GetTypeId() == TYPEID_PLAYER)
                                target->CastSpell(target, 90040, true);
                        }
                    }
                }
            } 

            void JustRespawned() override
            {
                char message[1024];
                
                snprintf(message, 1024, "|cffFFFF00世界首领永恒岛斡耳朵圣殿的野牛人火神斡耳朵斯出现了！　|r");
                
                sWorld->SendGlobalText(message, NULL);
            }

            void EnterCombat(Unit* who) override
            {
                char message[1024];
                
                if (who->GetTypeId() == TYPEID_PLAYER)
                {
                    if (who->ToPlayer()->GetTeam()==ALLIANCE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领永恒岛斡耳朵圣殿的野牛人火神斡耳朵斯正在遭到联盟的攻击！　|r");
                        
                    }
                    else
                    if (who->ToPlayer()->GetTeam()==HORDE)
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领永恒岛斡耳朵圣殿的野牛人火神斡耳朵斯正在遭到部落的攻击！　|r");
                        
                    }
                }
                else
                if (who->GetTypeId() == TYPEID_UNIT)
                {
                    if (who->ToCreature()->GetIAmABot() || who->ToCreature()->GetIAmABotsPet())
                    {
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛斡耳朵圣殿的野牛人火神斡耳朵斯正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->ToCreature()->GetBotOwner() && who->ToCreature()->GetBotOwner()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛斡耳朵圣殿的野牛人火神斡耳朵斯正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    if (who->IsPet())
                    {
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==ALLIANCE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛斡耳朵圣殿的野牛人火神斡耳朵斯正在遭到联盟的攻击！　|r");
                            
                        }
                        else
                        if (who->GetOwner()->ToPlayer() && who->GetOwner()->ToPlayer()->GetTeam()==HORDE)
                        {
                            snprintf(message, 1024, "|cffFFFF00世界首领永恒岛斡耳朵圣殿的野牛人火神斡耳朵斯正在遭到部落的攻击！　|r");
                            
                        }
                    }
                    else
                    {
                        snprintf(message, 1024, "|cffFFFF00世界首领永恒岛斡耳朵圣殿的野牛人火神斡耳朵斯正在遭到部落的攻击！　|r");
                        
                    }
                }
                else
                {
                    snprintf(message, 1024, "|cffFFFF00世界首领永恒岛斡耳朵圣殿的野牛人火神斡耳朵斯正在遭到部落的攻击！　|r");
                    
                }
                sWorld->SendGlobalText(message, NULL);
                Announced = true;
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
            }

            void JustDied(Unit* /*killer*/) override
            {
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if ((me->GetDistance(me->GetVictim()) > 50.00f && me->GetDistance(me->GetVictim()) < 533.00f) || me->GetDistance(me->GetVictim()) < 0.00f)
                    DoCastVictim(SPELL_SUMMON);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_1:
                            DoCastVictim(SPELL_SHADOW_VOLLEY);
                            _events.ScheduleEvent(EVENT_1, urand(4000, 6000));
                            break;
                        case EVENT_2:
                            DoCastVictim(SPELL_CLEAVE);
                            _events.ScheduleEvent(EVENT_2, urand(8000, 12000));
                            break;
                        case EVENT_3:
                            DoCastVictim(SPELL_THUNDERCLAP);
                            _events.ScheduleEvent(EVENT_3, urand(10000, 14000));
                            break;
                        case EVENT_4:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_SHADOW_TRAP);
                            _events.ScheduleEvent(EVENT_4, urand(10000, 14000));
                            break;
                        case EVENT_5:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_ARCANE_ORB);
                            _events.ScheduleEvent(EVENT_5, 3000);
                            break;
                        case EVENT_6:
                            DoCast(me, SPELL_GROWTH);
                            _events.ScheduleEvent(EVENT_6, 30000);
                            break;
                        case EVENT_7:
                            DoCast(me, SPELL_ENRAGE_LARGE);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            _events.ScheduleEvent(EVENT_7, 10000);
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_worldboss28AI(creature);
        }
};

class npc_svt_spirit_guide : public CreatureScript
{
    public:
        npc_svt_spirit_guide() : CreatureScript("npc_svt_spirit_guide") { }

        struct npc_svt_spirit_guideAI : public ScriptedAI
        {
            npc_svt_spirit_guideAI(Creature* creature) : ScriptedAI(creature) { }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    DoCast(me, SPELL_CHANNEL_SPIRIT_HEAL);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_svt_spirit_guideAI(creature);
        }
};

class go_ashran_tower_alliance : public GameObjectScript
{
    public:
        go_ashran_tower_alliance() : GameObjectScript("go_ashran_tower_alliance") { }

        void OnDamaged(GameObject* go, Player* /*player*/) override
        {
            WorldPacket data(SMSG_PLAY_SOUND, 4);
            data << uint32(8212);
            sWorld->SendGlobalMessage(&data);
            char message[1024];
            
            snprintf(message, 1024, "|cffFFFF00世界战场阿什兰的风暴之盾堡垒的联盟能量塔正在遭到部落的攻击！　|r");
            
            sWorld->SendGlobalText(message, NULL);

        }

        void OnDestroyed(GameObject* go, Player* /*player*/) override
        {
            if (sGameEventMgr->IsActiveEvent(124))
                sGameEventMgr->StopEvent(124, true);
            WorldPacket data(SMSG_PLAY_SOUND, 4);
            data << uint32(8192);
            sWorld->SendGlobalMessage(&data);
            char message[1024];
            
            snprintf(message, 1024, "|cffFFFF00世界战场阿什兰的风暴之盾堡垒被部落攻陷了！　|r");
         
            sWorld->SendGlobalText(message, NULL);

            if (Map const* map = go->GetMap())
            {
                uint32 AllianceCount = 0;
                uint32 HordeCount = 0;
                Map::PlayerList const& plMap = map->GetPlayers();
                for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                {
                    if (Player* player = itr->GetSource())
                    {
                        if (player->IsInWorld())
                        {
                            if (player->GetZoneId() == 6941 && player->getLevel() >= 80 && player->GetTeam() == ALLIANCE)
                                AllianceCount++;
                            else
                            if (player->GetZoneId() == 6941 && player->getLevel() >= 80 && player->GetTeam() == HORDE)
                                HordeCount++;
                        }
                    }
                }
                for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                {
                    if (Player* player = itr->GetSource())
                    {
                        if (player->IsInWorld())
                        {
                            if (player->GetZoneId() == 6941 && player->getLevel() >= 80 && player->GetTeam() == HORDE)
                            {
                                WorldPacket data(SMSG_PLAY_SOUND, 4);
                                data << uint32(8454);
                                player->SendDirectMessage(&data);
                                if (HordeCount > 0)
                                {
                                    player->ModifyHonorPoints(int32(595200/HordeCount));
                                    player->ModifyArenaPoints(int32(40000/HordeCount));
                                    player->CastSpell(player, 90163, true);
                                    if (urand(1,100) == 1)
                                    {
                                        uint32 itemId = urand(60397, 60406);
                                        player->AddItem(itemId, 1);
                                    }
                                    ChatHandler(player->GetSession()).PSendSysMessage("|cffFFFF00成功摧毁联盟能量塔，你获得了　%u　点荣誉点数和　%u　点竞技场点数。　|r", int32(595200/HordeCount), int32(40000/HordeCount));
                                }
                            }
                            else
                            if (player->GetZoneId() == 6941 && player->getLevel() >= 80 && player->GetTeam() == ALLIANCE)
                            {
                                WorldPacket data(SMSG_PLAY_SOUND, 4);
                                data << uint32(8454);
                                player->SendDirectMessage(&data);
                                if (AllianceCount > 0)
                                {
                                    player->ModifyHonorPoints(int32(148800/AllianceCount));
                                    player->ModifyArenaPoints(int32(12000/AllianceCount));
                                    player->CastSpell(player, 90194, true);
                                    ChatHandler(player->GetSession()).PSendSysMessage("|cffFFFF00联盟能量塔被部落摧毁，你获得了　%u　点荣誉点数和　%u　点竞技场点数。　|r", int32(148800/AllianceCount), int32(12000/AllianceCount));
                                }
                            }
                        }
                    }
                }
            }
            if (sGameEventMgr->IsActiveEvent(122))
                sGameEventMgr->StopEvent(122, true);
            //sWorld->setWorldState(WS_ASHRAN_BG, 1);
        }
};

class go_ashran_tower_horde : public GameObjectScript
{
    public:
        go_ashran_tower_horde() : GameObjectScript("go_ashran_tower_horde") { }

        void OnDamaged(GameObject* go, Player* /*player*/) override
        {
            WorldPacket data(SMSG_PLAY_SOUND, 4);
            data << uint32(8174);
            sWorld->SendGlobalMessage(&data);
            char message[1024];
            
            snprintf(message, 1024, "|cffFFFF00世界战场阿什兰的战争之矛岗哨的部落能量塔正在遭到联盟的攻击！　|r");
     
            sWorld->SendGlobalText(message, NULL);

        }

        void OnDestroyed(GameObject* go, Player* /*player*/) override
        {
            if (sGameEventMgr->IsActiveEvent(124))
                sGameEventMgr->StopEvent(124, true);
            WorldPacket data(SMSG_PLAY_SOUND, 4);
            data << uint32(8192);
            sWorld->SendGlobalMessage(&data);
            char message[1024];
            
            snprintf(message, 1024, "|cffFFFF00世界战场阿什兰的战争之矛岗哨被联盟攻陷了！　|r");

            sWorld->SendGlobalText(message, NULL);

            if (Map const* map = go->GetMap())
            {
                uint32 AllianceCount = 0;
                uint32 HordeCount = 0;
                Map::PlayerList const& plMap = map->GetPlayers();
                for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                {
                    if (Player* player = itr->GetSource())
                    {
                        if (player->IsInWorld())
                        {
                            if (player->GetZoneId() == 6941 && player->getLevel() >= 80 && player->GetTeam() == ALLIANCE)
                                AllianceCount++;
                            else
                            if (player->GetZoneId() == 6941 && player->getLevel() >= 80 && player->GetTeam() == HORDE)
                                HordeCount++;
                        }
                    }
                }
                for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
                {
                    if (Player* player = itr->GetSource())
                    {
                        if (player->IsInWorld())
                        {
                            if (player->GetZoneId() == 6941 && player->getLevel() >= 80 && player->GetTeam() == ALLIANCE)
                            {
                                WorldPacket data(SMSG_PLAY_SOUND, 4);
                                data << uint32(8455);
                                player->SendDirectMessage(&data);
                                if (AllianceCount > 0)
                                {
                                    player->ModifyHonorPoints(int32(595200/AllianceCount));
                                    player->ModifyArenaPoints(int32(40000/AllianceCount));
                                    player->CastSpell(player, 90163, true);
                                    if (urand(1,100) == 1)
                                    {
                                        uint32 itemId = urand(60397, 60406);
                                        player->AddItem(itemId, 1);
                                    }
                                    ChatHandler(player->GetSession()).PSendSysMessage("|cffFFFF00成功摧毁部落能量塔，你获得了　%u　点荣誉点数和　%u　点竞技场点数。　|r", int32(595200/AllianceCount), int32(40000/AllianceCount));
                                }
                            }
                            else
                            if (player->GetZoneId() == 6941 && player->getLevel() >= 80 && player->GetTeam() == HORDE)
                            {
                                WorldPacket data(SMSG_PLAY_SOUND, 4);
                                data << uint32(8455);
                                player->SendDirectMessage(&data);
                                if (HordeCount > 0)
                                {
                                    player->ModifyHonorPoints(int32(148800/HordeCount));
                                    player->ModifyArenaPoints(int32(12000/HordeCount));
                                    player->CastSpell(player, 90194, true);
                                    ChatHandler(player->GetSession()).PSendSysMessage("|cffFFFF00部落能量塔被联盟摧毁，你获得了　%u　点荣誉点数和　%u　点竞技场点数。　|r", int32(148800/HordeCount), int32(12000/HordeCount));
                                }
                            }
                        }
                    }
                }
            }
            if (sGameEventMgr->IsActiveEvent(122))
                sGameEventMgr->StopEvent(122, true);
            //sWorld->setWorldState(WS_ASHRAN_BG, 1);
        }
};

class npc_ghbb_flying_vehicle : public CreatureScript
{
    public:
        npc_ghbb_flying_vehicle() : CreatureScript("npc_ghbb_flying_vehicle") { }

        struct npc_ghbb_flying_vehicleAI : public VehicleAI
        {
            npc_ghbb_flying_vehicleAI(Creature* creature) : VehicleAI(creature)
            {
            }

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetFacingToObject(summoner);
                Position pos = summoner->GetPosition();
                pos.m_positionZ = pos.m_positionZ + 3.0f;
                me->GetMotionMaster()->MovePoint(2, pos);
            }

            void MovementInform(uint32 type, uint32 id) override
            {
                if (type == POINT_MOTION_TYPE && id == 2)
                    me->SetDisableGravity(false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ghbb_flying_vehicleAI(creature);
        }
};

class spell_ghbb_auto_repair : public SpellScriptLoader
{
    public:
        spell_ghbb_auto_repair() : SpellScriptLoader("spell_ghbb_auto_repair") { }

        class spell_ghbb_auto_repair_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ghbb_auto_repair_SpellScript);

            void CheckCooldownForTarget()
            {
                if (GetHitUnit()->HasAuraEffect(90174, EFFECT_2))   // Check presence of dummy aura indicating cooldown
                {
                    PreventHitEffect(EFFECT_0);
                    PreventHitDefaultEffect(EFFECT_1);
                    PreventHitDefaultEffect(EFFECT_2);
                }
            }

            void HandleScript(SpellEffIndex /*eff*/)
            {
                Vehicle* vehicle = GetHitUnit()->GetVehicleKit();
                if (!vehicle)
                    return;

                Unit* driver = vehicle->GetPassenger(0);
                if (!driver)
                    return;

                if (driver->GetTypeId() == TYPEID_PLAYER)
                {
                    if (driver->ToPlayer()->GetTeam() == ALLIANCE && driver->GetAreaId() != 7100)
                        return;
                    else
                    if (driver->ToPlayer()->GetTeam() == HORDE && driver->GetAreaId() != 7099)
                        return;
                }

                driver->TextEmote("载具已修理完毕，弹药已补充完毕。　", driver, true);
                vehicle->GetBase()->SetFullHealth();
                if (vehicle->GetBase()->getPowerType() == POWER_MANA)
                    vehicle->GetBase()->SetPower(POWER_MANA, vehicle->GetBase()->GetMaxPower(POWER_MANA));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_ghbb_auto_repair_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
                BeforeHit += SpellHitFn(spell_ghbb_auto_repair_SpellScript::CheckCooldownForTarget);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ghbb_auto_repair_SpellScript();
        }
};

class npc_ghbb_tonk_mine : public CreatureScript
{
public:
    npc_ghbb_tonk_mine() : CreatureScript("npc_ghbb_tonk_mine") { }

    struct npc_ghbb_tonk_mineAI : public ScriptedAI
    {
        npc_ghbb_tonk_mineAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void Reset() override
        {
            events.ScheduleEvent(EVENT_PROXIMITY_MINE_ARM, 3000);
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_PROXIMITY_MINE_ARM:
                        DoCast(me, 90186);
                        events.ScheduleEvent(EVENT_PROXIMITY_MINE_DETONATION, 43197000);
                        break;
                    case EVENT_PROXIMITY_MINE_DETONATION:
                        if (me->HasAura(90186))
                            DoCastAOE(90180);
                        me->DespawnOrUnsummon(500);
                        break;
                    default:
                        break;
                }
            }
        }

    private:
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ghbb_tonk_mineAI(creature);
    }
};

class npc_ghbb_proximity_mine : public CreatureScript
{
    public:
        npc_ghbb_proximity_mine() : CreatureScript("npc_ghbb_proximity_mine") { }

        struct npc_ghbb_proximity_mineAI : public ScriptedAI
        {
            npc_ghbb_proximity_mineAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                events.ScheduleEvent(EVENT_PROXIMITY_MINE_ARM, 3000);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PROXIMITY_MINE_ARM:
                            DoCast(me, 90184);
                            events.ScheduleEvent(EVENT_PROXIMITY_MINE_DETONATION, 43197000);
                            break;
                        case EVENT_PROXIMITY_MINE_DETONATION:
                            if (me->HasAura(90184))
                                DoCastAOE(63009);
                            me->DespawnOrUnsummon(500);
                            break;
                        default:
                            break;
                    }
                }
            }

        private:
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ghbb_proximity_mineAI(creature);
        }
};

class spell_ghbb_proximity_mines : public SpellScriptLoader
{
    public:
        spell_ghbb_proximity_mines() : SpellScriptLoader("spell_ghbb_proximity_mines") { }

        class spell_ghbb_proximity_mines_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ghbb_proximity_mines_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(90183))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                for (uint8 i = 0; i < 10; ++i)
                    GetCaster()->CastSpell(GetCaster(), 90183, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_ghbb_proximity_mines_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ghbb_proximity_mines_SpellScript();
        }
};

class spell_ghbb_proximity_trigger : public SpellScriptLoader
{
    public:
        spell_ghbb_proximity_trigger() : SpellScriptLoader("spell_ghbb_proximity_trigger") { }

        class spell_ghbb_proximity_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ghbb_proximity_trigger_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(63009))
                    return false;
                return true;
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove(GetExplTargetWorldObject());

                if (targets.empty())
                    FinishCast(SPELL_FAILED_NO_VALID_TARGETS);
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                GetCaster()->CastSpell((Unit*)NULL, 63009, true);
                if (GetCaster()->GetTypeId() == TYPEID_UNIT)
                    GetCaster()->ToCreature()->DespawnOrUnsummon(500);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ghbb_proximity_trigger_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHit += SpellEffectFn(spell_ghbb_proximity_trigger_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ghbb_proximity_trigger_SpellScript();
        }
};

class spell_ghbb_tonk_trigger : public SpellScriptLoader
{
    public:
        spell_ghbb_tonk_trigger() : SpellScriptLoader("spell_ghbb_tonk_trigger") { }

        class spell_ghbb_tonk_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ghbb_tonk_trigger_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(90180))
                    return false;
                return true;
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove(GetExplTargetWorldObject());

                if (targets.empty())
                    FinishCast(SPELL_FAILED_NO_VALID_TARGETS);
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                GetCaster()->CastSpell((Unit*)NULL, 90180, true);
                if (GetCaster()->GetTypeId() == TYPEID_UNIT)
                    GetCaster()->ToCreature()->DespawnOrUnsummon(500);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ghbb_tonk_trigger_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHit += SpellEffectFn(spell_ghbb_tonk_trigger_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ghbb_tonk_trigger_SpellScript();
        }
};


void AddSC_REFORGER_NPC()
{
    new REFORGER_NPC;
    new REFORGE_PLAYER;

  new boss_worldboss1();
    new boss_worldboss2();
    new boss_worldboss3();
    new boss_worldboss4();
    new boss_worldboss5();
    new boss_worldboss6();
    new boss_worldboss7();
    new boss_worldboss8();
    new boss_worldboss9();
    new boss_worldboss10();
    new boss_worldboss11();
    new boss_worldboss12();
    new boss_worldboss13();
    new boss_worldboss14();
    new boss_worldboss15();
    new boss_worldboss16();
    new boss_worldboss17();
    new boss_worldboss18();
    new boss_worldboss18_1();
    new boss_worldboss18_2();
    new boss_worldboss18_3();
    new boss_worldboss18_4();
    new boss_worldboss19();
    new boss_worldboss19_1();
    new boss_worldboss19_2();
    new boss_worldboss19_3();
    new boss_worldboss20();
    new boss_worldboss21();
    new boss_worldboss22();
    new boss_worldboss23();
    new boss_worldboss24();
    new boss_worldboss25();
    new boss_worldboss26();
    new boss_worldboss27();
    new boss_worldboss28();
    new npc_svt_spirit_guide();
    new go_ashran_tower_alliance();
    new go_ashran_tower_horde();
    new npc_ghbb_flying_vehicle();
    new spell_ghbb_auto_repair();
    new npc_ghbb_tonk_mine();
    new npc_ghbb_proximity_mine();
    new spell_ghbb_proximity_mines();
    new spell_ghbb_proximity_trigger();
    new spell_ghbb_tonk_trigger();
}

#undef FOR_REFORGE_ITEMS
