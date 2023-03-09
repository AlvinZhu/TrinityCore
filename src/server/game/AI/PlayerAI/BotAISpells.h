
#ifndef _BOT_AI_SPELLS_H_
#define _BOT_AI_SPELLS_H_

#include "ScriptSystem.h"
#include "BotAITool.h"

class BotWarriorSpells
{
public:
	void InitializeSpells(Player* player);

protected:
	uint32 WarriorIDLE_AOEAddLife;// = 47440;			// ����ŭ��
	uint32 WarriorIDLE_AOEAddPower;// = 47436;			// ��ǿŭ��

	uint32 WarriorWeapon_Status;// = 2457;
	uint32 WarriorDefance_Status;// = 71;
	uint32 WarriorRage_Status;// = 2458;

	uint32 WarriorCommon_PowerAtt;// = 47450;			// Ӣ�´��
	uint32 WarriorCommon_PowerThrow;// = 57755;			// Ӣ��Ͷ��
	uint32 WarriorCommon_PowerRelife;// = 55694;		// ��ŭ�ָ����п�ʱʹ�û�Ѫ��
	uint32 WarriorCommon_ClearCtrl;// = 18499;			// ��֮ŭ���Ƴ����ƣ�
	uint32 WarriorCommon_AOEFear;// = 5246;			// ��ս��ΧȺ�־�
	uint32 WarriorCommon_SweepAtt;// = 47520;			// ˳��ն
	uint32 WarriorCommon_AddPower;// = 2687;			// ��ŭ��
	uint32 WarriorCommon_AOEDecPower;// = 47437;		// Ⱥ�����ǿ��

	uint32 WarriorDefance_HPojia;// = 47498;			// ������
	uint32 WarriorDefance_Fuchou;// = 57823;			// ����
	uint32 WarriorDefance_ShieldBlock;// = 2565;		// ���Ƹ�
	uint32 WarriorDefance_ShieldAtt;// = 47488;			// �����ͻ�
	uint32 WarriorDefance_Pojia;// = 7386;				// �Ƽ�
	uint32 WarriorDefance_MaxLife;// = 12975;			// �Ƹ�����
	uint32 WarriorDefance_ShiledWall;// = 871;			// ��ǽ
	uint32 WarriorDefance_Disarm;// = 676;				// ��е
	uint32 WarriorDefance_Support;// = 3411;			// Ԯ��
	uint32 WarriorDefance_Conk;// = 12809;				// �������
	uint32 WarriorDefance_AOEConk;// = 46968;			// ǰ����Χ����
	uint32 WarriorDefance_Pull;// = 355;				// ����

	uint32 WarriorWeapon_SwordStorm;// = 46924;			// ���з籩
	uint32 WarriorWeaponDefance_AOEAtt;// = 47502;		// ����һ��
	uint32 WarriorWeaponDefance_Bleed;// = 47465;		// ˺��
	uint32 WarriorWeaponDefance_SpellReflect;// = 23920;	// ��������
	uint32 WarriorWeaponDefance_ShieldHit;// = 72;		// �ܻ���ʩ����ϣ�

	uint32 WarriorWeapon_HighThrow;// = 64382;			// ���޵�Ͷ��
	uint32 WarriorWeapon_Charge;// = 11578;			// ���
	uint32 WarriorWeapon_Suppress;// = 7384;			// ѹ��
	uint32 WarriorWeapon_Backstorm;// = 20230;			// �����籩
	uint32 WarriorWeapon_DeadAtt;// = 47486;			// �������

	uint32 WarriorWeaponRage_FullKill;// = 47471;		// նɱ
	uint32 WarriorWeaponRage_WinAttack;// = 34428;		// ��ʤ׷��
	uint32 WarriorWeaponRage_Backfillet;// = 1715;		// �Ͻ�

	uint32 WarriorRage_Harsh;// = 12323;				// �̶�ŭ��
	uint32 WarriorRage_HeadAtt;// = 6552;				// ȭ����ʩ����ϣ�
	uint32 WarriorRage_Intercept;// = 20252;			// ����
	uint32 WarriorRage_Whirlwind;// = 1680;			// ����ն
	uint32 WarriorRage_Impertinency;// = 1719;			// ³ç
	uint32 WarriorRage_Needdead;// = 12292;			// ����֮Ը
	uint32 WarriorRage_Bloodthirsty;// = 23881;			// ��Ѫ
	uint32 WarriorRage_ReIntercept;// = 60970;			// ����ƶ����ƺͽ������CD
};

class BotPaladinSpells
{
public:
	void InitializeSpells(Player* player);

protected:
	uint32 PaladinIDLE_MountAura;// 32223			�����ٶȹ⻷
	uint32 PaladinIDLE_CastAura;// 19746			ʩ���⻷
	uint32 PaladinIDLE_JudgeAura;// 54043			�ͽ�⻷
	uint32 PaladinIDLE_ArmorAura;// 48942			�����⻷
	uint32 PaladinIDLE_AOEGuardWish;// 25899			Ⱥ��ӻ�ף��(����ϵ)
	uint32 PaladinIDLE_GuardWish;// 20911			�ӻ�ף��(����ϵ)
	uint32 PaladinIDLE_AOEKingWish;// 25898			Ⱥ������ף��
	uint32 PaladinIDLE_KingWish;// 20217			����ף��
	uint32 PaladinIDLE_AOEWitWish;// 48938			Ⱥ���ǻ�ף��
	uint32 PaladinIDLE_WitWish;// 48936				�ǻ�ף��
	uint32 PaladinIDLE_AOEStrWish;// 48934			Ⱥ������ף��
	uint32 PaladinIDLE_StrWish;// 48932				����ף��
	uint32 PaladinIDLE_JusticeRage;// 25780			����֮ŭ ����ʹ��
	uint32 PaladinIDLE_Revive;// 48950				�������

	uint32 PaladinGuard_UnShield;// 53601			��ʥ����
	uint32 PaladinGuard_FreeAura;// 1044			����ף��
	uint32 PaladinGuard_Invincible;// 642			ʥ���� �޵�
	uint32 PaladinGuard_Sacrifice;// 6940			����֮�� Ŀ������ת��
	uint32 PaladinGuard_AOESacrifice;// 64205		��ʥ���� �Ŷ�����ת��(����ϵ)
	uint32 PaladinGuard_BlockShield;// 48952			��ʥ��(����ϵ)
	uint32 PaladinGuard_PhyImmune;// 10278			Ŀ����������
	uint32 PaladinGuard_Pull;// 62124				����

	uint32 PaladinAssist_UpPower;// 31884			����� UP�˺�
	uint32 PaladinAssist_RevengeStamp;// 31801		����ʥӡ ����5���˺����
	uint32 PaladinAssist_LifeStamp;// 20165			����ʥӡ ������Ѫ
	uint32 PaladinAssist_ManaStamp;// 20166			�ǻ�ʥӡ ��������
	uint32 PaladinAssist_JusticeStamp;// 21084		����ʥӡ �������
	uint32 PaladinAssist_StunStamp;// 20164			����ʥӡ �������ʻ���
	uint32 PaladinAssist_ComStamp;// 20375			����ʥӡ ������ǿ(�ͽ�ϵ)
	uint32 PaladinAssist_Confession;// 20066			��� ����(�ͽ�ϵ)
	uint32 PaladinAssist_StunMace;// 10308			�Ʋ�֮�� ����
	uint32 PaladinAssist_ReviveMana;// 54428			����BUF
	uint32 PaladinAssist_HealCrit;// 20216			next���Ʊ���(��ʥϵ)
	uint32 PaladinAssist_LowMana;// 31842			������������(��ʥϵ)
	uint32 PaladinAssist_AuraUP;// 31821			�⻷Ч����ǿBUF(��ʥϵ)
	uint32 PaladinAssist_Dispel;// 4987				��ɢħ���ж�

	uint32 PaladinHeal_FastHoly;// 48785			ʥ������
	uint32 PaladinHeal_BigHoly;// 48782				ʥ����
	uint32 PaladinHeal_FullHoly;// 48788			ʥ��

	uint32 PaladinMelee_AOEOffertory;// 48819		����
	uint32 PaladinMelee_KillMace;// 48806			նɱ֮�� Ŀ�����20%Ѫ
	uint32 PaladinMelee_FlyShield;// 48827			����ɶ�(����ϵ)
	uint32 PaladinMelee_ShieldAtt;// 61411			���ƴ��(����ϵ)
	uint32 PaladinMelee_MaceAtt;// 53595			����֮��(����ϵ)
	uint32 PaladinMelee_HolyAtt;// 48825			��ʥ���(��ʥϵ)
	uint32 PaladinMelee_LifeJudge;// 20271			��������
	uint32 PaladinMelee_ManaJudge;// 53408			�ǻ�����
	uint32 PaladinMelee_FleeJudge;// 53407			��������
	uint32 PaladinMelee_WeaponAtt;// 35395			ʮ�־����(�ͽ�ϵ)
	uint32 PaladinMelee_HolyStrom;// 53385			��ʥ�籩(�ͽ�ϵ)

	uint32 PaladinFlag_MomentHoly;// 59578			ս������ ˲��ʥ��
	uint32 PaladinFlag_Discipline;// 25771			���� DEBUFF
};

class BotDeathknightSpells
{
public:
	void InitializeSpells(Player* player);

protected:
	uint32 DKStatus_Frost;// 48263			��˪����
	uint32 DKStatus_Evil;// 48265			а������
	uint32 DKStatus_Blood;// 48266			��Ѫ����

	uint32 DKIDLE_Buffer;// 57623			ȫ������������BUFF
	uint32 DKIDLE_SummonPet;//	46584		�ٻ�ʳʬ��
	uint32 DKIDLE_SummonAllPets;//	42650	�ٻ�ʳʬ�����

	uint32 DKBlock_Silence;// 47476			��ĬĿ��
	uint32 DKBlock_Cast;// 47528			���ʩ��

	uint32 DKPulls_Pull;// 56222			���峰��
	uint32 DKPulls_DKPull;// 49576			����֮��

	uint32 DKDefense_MgcShield;// 48707		��ħ����
	uint32 DKDefense_NoMgcArea;// 51052		��ħ�����򣬶��Լ�ʩ�ţ������ڷ�ħ��(а��ϵ)
	uint32 DKDefense_Contract;// 48743		ɱ��BB��Ѫ40%
	uint32 DKDefense_IceBody;// 48792		DK��ǽ
	uint32 DKDefense_IceArmor;// 51271		ͭǽ����(�ṩ���׺�����20��)(����ϵ)
	uint32 DKDefense_BoneShield;// 49222	�׹�֮��(а��ϵ)

	uint32 DKAssist_RuneLife;// 48982		����������ת������Ϊ����(��Ѫϵ)
	uint32 DKAssist_BloodBrand;// 49005		��Ѫӡ�ǣ�Ŀ�깥����Ŀ���Ѫ(��Ѫϵ)
	uint32 DKAssist_Frenzied;// 49016		���ң���Ŀ�������˺�UP(��Ѫϵ)
	uint32 DKAssist_BloodBuf;// 55233		�����Ƹ�����(��Ѫϵ)
	uint32 DKAssist_SummonRuneWeapon;// 49028	�ٻ���������������ս��(��Ѫϵ)
	uint32 DKAssist_RuneWeapon;// 47568		����ȫ���ģ�25��������
	uint32 DKAssist_Infect;// 50842			Ŀ��ı�Ѫ������ɢ��Ⱦ
	uint32 DKAssist_RuneShunt;// 45529		�������� Ѫ����ת��������
	uint32 DKAssist_IceLock;// 45524		��������Ŀ��10�����ƶ��ٶȵ�
	uint32 DKAssist_DeadRevive;// 49895		�������ƣ�����Ŀ���������ָ�
	uint32 DKAssist_NonFear;// 49039		�������߿־��Ȼ�(����ϵ)
	uint32 DKAssist_NextCrit;// 49796		������˪���������ɼ�������˪�������(����ϵ)
	uint32 DKAssist_EatIce;// 49203			����֮��������10�뷶ΧĿ�꣬�Ǽ����˺����(����ϵ)
	uint32 DKAssist_PetPower;// 63560		ʳʬ�����(а��ϵ)
	uint32 DKAssist_SummonFlyAtt;// 49206	�ٻ���ը��(а��ϵ)

	uint32 DKAttack_IceSickness;// 49909	��˪������������ʱ�߳��
	uint32 DKAttack_NearAOE;// 49941		��ս��ΧAOE����Ѫ�ͱ������˺�UP
	uint32 DKAttack_AreaAOE;// 49938		��ή���㣬ָ����ΧAOE
	uint32 DKAttack_BloodAtt;// 49930		��Ѫ�������м���ʱ�˺�UP
	uint32 DKAttack_ShadowAtt;// 49921		��Ӱ�����Ѫ����
	uint32 DKAttack_FrostAtt;// 55268		��˪���(����ϵ)
	uint32 DKAttack_DoDestroy;// 51425		���ɼ��������˺�
	uint32 DKAttack_RuneAttack;// 56815		���Ĵ��������ѹ��
	uint32 DKAttack_LifeAttack;// 49924		�����ָ�������һ������5%Ѫ
	uint32 DKAttack_IceWindAtt;// 51411		������(����ϵ)
	uint32 DKAttack_CorpseExplosion;// 51328	ʬ��(а��ϵ)
	uint32 DKAttack_NaturalAtt;// 55271		���ִ��(а��ϵ)
	uint32 DKAttack_CoreAtt;// 55262		������(��Ѫϵ)
};

class BotRogueSpells
{
public:
	void InitializeSpells(Player* player);

protected:
	uint32 RogueGuard_Sneak;// 1784				Ǳ��
	uint32 RogueGuard_ShadowCloak;// 31224		��Ӱ����
	uint32 RogueGuard_Disappear;// 26889		��ʧ
	uint32 RogueGuard_Dodge;// 26669			����
	uint32 RogueGuard_Sprint;// 11305			����

	uint32 RogueSneak_Stick;// 51724			�ƹ�
	uint32 RogueSneak_Premeditate;// 14183		Ԥı(����ϵ)
	uint32 RogueSneak_Ambush;// 48691			����
	uint32 RogueSneak_Surprise;// 1833			͵Ϯ

	uint32 RogueAssist_ShadowDance;// 51713		��Ӱ֮��(����ϵ)
	uint32 RogueAssist_ShadowFlash;// 36554		��Ӱ��(����ϵ)
	uint32 RogueAssist_ReadyCD;// 14185			Ԥ��(����ϵ)
	uint32 RogueAssist_Blind;// 2094			��ä
	uint32 RogueAssist_Disarm;// 51722			��е
	uint32 RogueAssist_NextCrit;// 14177		��Ѫ next����(��ɱϵ)
	uint32 RogueAssist_blood;// 51662			��Ѫ Ŀ����Ѫʱʩ��(��ɱϵ)
	uint32 RogueAssist_FastEnergy;// 13750		�������ٻظ�(ս��ϵ)
	uint32 RogueAssist_BlockCast;// 1766		���� ���ʩ��
	uint32 RogueAssist_Paralyze;// 1776			��� ̱��Ŀ��
	uint32 RogueAssist_FastSpeed;// 13877		�ӹ����ٶ�20(ս��ϵ)

	uint32 RogueAOE_Knife;// 51723				���з���
	uint32 RogueAOE_AllDance;// 51690			����ɱ��(ս��ϵ)

	uint32 RogueAttack_Blood;// 48660			��Ѫ(����ϵ)
	uint32 RogueAttack_Ghost;// 14278			���ȹ���(����ϵ) ������+����
	uint32 RogueAttack_Injure;// 48666			����(��ɱϵ)
	uint32 RogueAttack_PoisonAtt;// 5938		����
	uint32 RogueAttack_BackAtt;// 48657			����
	uint32 RogueAttack_EvilAtt;// 48638			а�񹥻�

	uint32 RogueAttack_Damage;// 48668			�޹�
	uint32 RogueAttack_Separate;// 48672		����
	uint32 RogueAttack_Stun;// 8643				����
	uint32 RogueAttack_PoisonDmg;// 57993		����
	uint32 RogueAttack_Incision;// 6774			�и�(�����ٶ��սἼ)
	uint32 RogueRange_Throw;// 48674			����Ͷ��

	uint32 RogueFlag_Dance;// 51713				��Ӱ֮����
};

class BotDruidSpells
{
public:
	void InitializeSpells(Player* player);

protected:
	uint32 DruidIDLE_FerityWish;// 48469		Ұ��ף��
	uint32 DruidIDLE_AOEFerityWish;// 48470		Ⱥ��Ұ��ף��
	uint32 DruidIDLE_Revive;// 50769			�������
	uint32 DruidIDLE_CombatReive;// 20484		ս���������

	uint32 DruidStatus_Travel;// 783			������̬
	uint32 DruidStatus_Bear;// 9634				������̬
	uint32 DruidStatus_Cat;// 768				�Ա���̬
	uint32 DruidStatus_Bird;// 24858			������̬(ƽ��ϵ)
	uint32 DruidStatus_Tree;// 33891			������̬(�ָ�ϵ)

	uint32 DruidGuard_Sneak;// 5215				�Ա�Ǳ��
	uint32 DruidGuard_Harden;// 22812			��Ƥ�� ����20%
	uint32 DruidGuard_Thorns;// 53307			������ ��������
	uint32 DruidGuard_AutoTwine;// 53312		�ܻ�����Ŀ��
	uint32 DruidGuard_Twine;// 53308			����Ŀ��
	uint32 DruidGuard_Control;// 33786			���� ����Ŀ��Ŀ�����
	uint32 DruidGuard_Pofu;// 61336				Ұ����̬�Ƹ�����(Ұ��ϵ)
	uint32 DruidGuard_TreeMan;// 33831			��Ȼ֮���ٻ�����(ƽ��ϵ)

	uint32 DruidAssist_PersonSpirit;// 770		����̬����֮��
	uint32 DruidAssist_BeastSpirit;// 16857		Ұ����̬����֮��
	uint32 DruidAssist_Active;// 29166			���� Ŀ�����
	uint32 DruidAssist_DecCruse;// 2782			��³���������
	uint32 DruidAssist_DecCruel;// 2893			��³���ⶾ��BUF

	uint32 DruidCast_Moonfire;// 48463			�»� �����˺�����
	uint32 DruidCast_Insect;// 48468			��Ⱥ �����˺�����(ƽ��ϵ)
	uint32 DruidCast_Anger;// 48461				��ŭ
	uint32 DruidCast_Spark;// 48465				�ǻ� cast��

	uint32 DruidAOE_Hurricane;// 48467			쫷� AOE
	uint32 DruidAOE_Typhoon;// 61384			̨�� AOE(ƽ��ϵ)
	uint32 DruidAOE_FallStar;// 48505			Ⱥ��׹�� AOE(ƽ��ϵ)

	uint32 DruidHeal_Nourishing;// 50464		�̲��� С�����Ѫ �г�������Ч��ʱ��ǿ
	uint32 DruidHeal_Relife;// 48441			�ش��� С������Ѫ
	uint32 DruidHeal_Coalescence;// 48443		������ С��Ѫ
	uint32 DruidHeal_Touch;// 48378				����֮�� ���Ѫ
	uint32 DruidHeal_LifeBurst;// 48451			�������� ����Ч������ʱ�ټ�Ѫ
	uint32 DruidHeal_MergerLife;// 18562		Ѹ������ �̲��ش������� ��Ѫ(�ָ�ϵ)
	uint32 DruidHeal_MomentHeal;// 17116		��Ȼ֮�� nextHealʩ��˲��(�ָ�ϵ)

	uint32 DruidHeal_AOETranquility;// 48447		����
	uint32 DruidHeal_AOEFerity;// 53251			Ұ��Ⱥ������(�ָ�ϵ)

	uint32 DruidCat_Stun;// 49802				è ���� �սἼ
	uint32 DruidCat_Bite;// 48577				è ����ҧ �����˺��սἼ
	uint32 DruidCat_Roar;// 52610				è ���� ���Լ����˺����սἼ
	uint32 DruidCat_Separate;// 49800			è ���� �����˺��սἼ

	uint32 DruidCat_Tiger;// 50213				è �ͻ�֮ŭ �˺�BUF
	uint32 DruidCat_FastMove;// 33357			è ����
	uint32 DruidCat_Charge;// 49376				è ���(Ұ��ϵ)
	uint32 DruidCat_Surprise;// 49803			è Ǳ��ʱ��1������
	uint32 DruidCat_Sack;// 48579				è Ǳ��ʱ��1���˺�
	uint32 DruidCat_Claw;// 48570				è צ��
	uint32 DruidCat_BackStab;// 48572			è ����
	uint32 DruidCat_Attack;// 62078				è �ӻ�Ⱥ��
	uint32 DruidCat_Sweep;// 48574				è ��Ѫ����
	uint32 DruidCat_Laceration;// 48566			è ����(Ұ��ϵ)

	uint32 DruidBear_DecAtt;// 48560			�� ����ǿ��
	uint32 DruidBear_AddPower;// 5229			�� ��ŭ��
	uint32 DruidBear_PowerLife;// 22842			�� ŭ����Ѫ
	uint32 DruidBear_Laceration;// 48564		�� ����(Ұ��ϵ)
	uint32 DruidBear_Sweep;// 48568				�� ��Ѫ����
	uint32 DruidBear_Attack;// 48562			�� �ӻ�Ⱥ��
	uint32 DruidBear_NextAtt;// 48480			�� Next������ǿ
	uint32 DruidBear_Stun;// 8983				�� ����
	uint32 DruidBear_Charge;// 16979			�� ���(Ұ��ϵ)
};

class BotHunterSpells
{
public:
	void InitializeSpells(Player* player);

protected:
	uint32 HunterIDLE_SummonPet;// 883			�ٻ�����
	uint32 HunterIDLE_RevivePet;// 982			�������
	uint32 HunterIDLE_ManaAura;// 34074			���������ػ�
	uint32 HunterIDLE_DodgeAura;// 13163		�����ػ�
	uint32 HunterIDLE_EagleAura;// 27044		����ػ�
	uint32 HunterIDLE_DragonAura;// 61847		��ӥ�ػ� ����������ػ�
	uint32 HunterIDLE_ShotAura;// 19506			ǿ���⻷(���ϵ)

	uint32 HunterTrap_FarFrozen;// 60192		Զ�̱�������
	uint32 HunterTrap_Frozen;// 14311			��������
	uint32 HunterTrap_Ice;// 13809				��˪����
	uint32 HunterTrap_Viper;// 34600			��������
	uint32 HunterTrap_Explode;// 49067			��ը����
	uint32 HunterTrap_Fire;// 49056				��������
	uint32 HunterTrap_Shot;// 63672				���Ǽ�(����ϵ)

	uint32 HunterAssist_ClearRoot;// 53271		�Ƴ�����
	uint32 HunterAssist_PetCommand;// 34026		����ɱ��
	uint32 HunterAssist_HealPet;// 48990		��������
	uint32 HunterAssist_PetStun;// 19577		�������Ŀ��(Ұ��ϵ)
	uint32 HunterAssist_PetRage;// 19574		������Լ���(Ұ��ϵ)
	uint32 HunterAssist_Stamp;// 53338			����ӡ��
	uint32 HunterAssist_FalseDead;// 5384		����
	uint32 HunterAssist_BackJump;// 781			����
	uint32 HunterAssist_FastSpeed;// 3045		�������BUF
	uint32 HunterAssist_ReadyCD;// 23989		׼������CD(���ϵ)
	uint32 HunterAssist_Mislead;// 34477		��

	uint32 HunterMelee_BackRoot;// 48999		�мܺ󷴻�����(����ϵ)
	uint32 HunterMelee_NoDamage;// 19263		���� �޷�����
	uint32 HunterMelee_DecSpeed;// 2974			ˤ�� ��ս���ƶ���
	uint32 HunterMelee_NextAtt;// 48996			next��ս������ǿ
	uint32 HunterMelee_MeleeAtt;// 53339		��ս����

	uint32 HunterDebug_Damage;// 49001			�˺�����
	uint32 HunterDebug_Mana;// 3034				��������
	uint32 HunterDebug_Sleep;// 49012			��˯����(����ϵ)

	uint32 HunterShot_AOEShot;// 58434			AOE���
	uint32 HunterShot_CharmShot;// 19503		������(����ϵ)
	uint32 HunterShot_Explode;// 60053			��ը���(����ϵ)
	uint32 HunterShot_Aim;// 49050				��׼���(���ϵ)
	uint32 HunterShot_Silence;// 34490			��Ĭ���(���ϵ)
	uint32 HunterShot_Shock;// 5116				�����
	uint32 HunterShot_Cast;// 49052				�ȹ���� ʩ��ʱ������
	uint32 HunterShot_MgcShot;// 49045			�������
	uint32 HunterShot_KillShot;// 61006			��ɱ��� Ŀ��20%����Ѫնɱʽ���
	uint32 HunterShot_MulShot;// 49048			�������
	uint32 HunterShot_QMLShot;// 53209			���������(���ϵ)
};

class BotShamanSpells
{
public:
	void InitializeSpells(Player* player);

protected:
	uint32 ShamanIDLE_LifeWeapon;// 51994		��������
	uint32 ShamanIDLE_IceWeapon;// 58796		��˪����
	uint32 ShamanIDLE_FireWeapon;// 58790		��������
	uint32 ShamanIDLE_PhyWeapon;// 10399		ʯ������
	uint32 ShamanIDLE_FastWeapon;// 58804		��ŭ����
	uint32 ShamanIDLE_Revive;// 49277			�������

	uint32 ShamanShield_Earth;// 49284			���֮��(�ָ�ϵ)
	uint32 ShamanShield_Water;// 57960			ˮ֮��
	uint32 ShamanShield_Lightning;// 49281		����֮��

	uint32 ShamanAssist_Frog;// 51514			��������
	uint32 ShamanAssist_HealCrit;// 55198		next3�������Ʊ���(�ָ�ϵ)
	uint32 ShamanAssist_MomentHeal;// 16188		nextHealʩ��˲��(�ָ�ϵ)
	uint32 ShamanAssist_MomentCast;// 16166		nextCastʩ��˲��(Ԫ��ϵ)
	uint32 ShamanAssist_BlockCast;// 57994		���ʩ��
	uint32 ShamanAssist_Cleansing;// 8012		����Ŀ��2��BUFF
	uint32 ShamanAssist_FireNova;// 61657		�û���ͼ��AOEһ��
	uint32 ShamanAssist_Heroic;// 32182			����Ӣ��
	uint32 ShamanAssist_DecCruel;// 526			�����ⶾ �⼲��

	uint32 ShamanAtt_StormStrike;// 17364		�籩���(��ǿϵ)
	uint32 ShamanAtt_FireStrike;// 60103		���Ҵ��(��ǿϵ)

	uint32 ShamanCast_LightningArrow;// 49238	�����
	uint32 ShamanCast_LightningChain;// 49271	������
	uint32 ShamanCast_LightningStorm;// 59159	����籩(Ԫ��ϵ)
	uint32 ShamanCast_FireThud;// 49233			�������
	uint32 ShamanCast_IceThud;// 49236			��˪���
	uint32 ShamanCast_EarthThud;// 49231		������
	uint32 ShamanCast_FireStrike;// 60043		���Ҵ�� Զ�̷���

	uint32 ShamanHealth_Fast;// 49276			��������
	uint32 ShamanHealth_Bast;// 49273			��������
	uint32 ShamanHealth_Chain;// 55459			������
	uint32 ShamanHealth_Torrent;// 61301		���� ˲��С����+��������(�ָ�ϵ)
	uint32 ShamanDispel_Refine;// 51886			��ɢ

	uint32 ShamanTotem_Recycle;// 36936			����ͼ��

	uint32 ShamanTotem_Life;// 58757			����ͼ��
	uint32 ShamanTotem_Mana;// 58774			����ͼ��
	uint32 ShamanTotem_BMana;// 16190			����ͼ��

	uint32 ShamanTotem_SummonFire;// 2894		�ٻ���Ԫ��ͼ��
	uint32 ShamanTotem_MgcPower;// 57722		��������ͼ��(Ԫ��ϵ)
	uint32 ShamanTotem_Attack;// 58704			�������湥��ͼ��
	uint32 ShamanTotem_AOEAttack;// 58734		��������Ⱥ��ͼ��
	uint32 ShamanTotem_MgcHeal;// 58656			��������������Ч��ͼ��

	uint32 ShamanTotem_DecMove;// 2484			���ƶ��ٶ�ͼ��
	uint32 ShamanTotem_SummonSoil;// 2062		�ٻ���Ԫ��ͼ��
	uint32 ShamanTotem_PhyPower;// 58643		��������ͼ��
	uint32 ShamanTotem_Armor;// 58753			����ͼ��

	uint32 ShamanTotem_AbsorbBuff;// 8177		�����к�BUFFͼ��
	uint32 ShamanTotem_AttSpeed;// 8512			��ս�����ٶ�ͼ��
	uint32 ShamanTotem_MgcSpeed;// 3738			����ʩ���ٶ�ͼ��

	uint32 ShamanFlag_NoHeroic;// 57723			�޷�giveӢ��״̬��ʶ
};

class BotMageSpells
{
public:
	void InitializeSpells(Player* player);

protected:
	uint32 MageIDLE_ManaGem;// 42985		������ʯ
	uint32 MageIDLE_ArcaneMagic;// 42995	�����ǻ�
	uint32 MageIDLE_AOEArcaneMagic;// 43002	ȫ������ǻ�
	uint32 MageIDLE_MgcArmor;// 43024		ħ����
	uint32 MageIDLE_FrostArmor;// 43008		������
	uint32 MageIDLE_IceArmor;// 7301		˪����
	uint32 MageIDLE_FireArmor;// 43046		����װ��
	uint32 MageIDLE_MagicAdd;// 43017		ħ����Ч
	uint32 MageIDLE_MagicDec;// 43015		ħ��ѹ��
	uint32 MageIDLE_SummonRite;// 58659		�ٻ�����

	uint32 MageGuard_MagicShield;// 43020	��������
	uint32 MageGuard_FrostShield;// 43039	��������
	uint32 MageGuard_FrostScherm;// 45438	��������
	uint32 MageGuard_FrostNova;// 42917		��������
	uint32 MageGuard_FireBreath;// 42950	��֮��Ϣ
	uint32 MageGuard_FireNova;// 42945		��������

	uint32 MageAssist_Mirror;// 55342		��ʦ����
	uint32 MageAssist_Rouse;// 12051		����
	uint32 MageAssist_Stealth;// 66			������
	uint32 MageAssist_Teleport;// 1953		������
	uint32 MageAssist_DecCurse;// 475		�������
	uint32 MageAssist_Grace;// 12043		��������
	uint32 MageAssist_ArcanePower;// 12042	����ǿ��
	uint32 MageAssist_CastSpeed;// 12472	����Ѫ��
	uint32 MageAssist_FastColddown;// 11958	���ñ�ϵ����CD
	uint32 MageAssist_FrostPet;// 31687		�ٻ�ˮԪ��
	uint32 MageAssist_FireCritAura;// 11129	����3�����ӻ��汩��AURA

	uint32 MageConfine_BreakCast;// 2139	��������
	uint32 MageConfine_AuraSteal;// 30449	BUFF͵ȡ
	uint32 MageConfine_ArcaneSlow;// 31589	��������
	uint32 MageConfine_ToSheep;// 12826		����
	uint32 MageConfine_Freeze;// 44572		���ᱻ������Ŀ��

	uint32 MageAOE_ArcaneExplode;// 42921	������ը
	uint32 MageAOE_Snowstorm;// 42940		����ѩ
	uint32 MageAOE_IcePiton;// 42931		��׶��
	uint32 MageAOE_FireStorm;// 42926		����籩

	uint32 MageArcane_Barrage;// 44781		������Ļ
	uint32 MageArcane_Bullet;// 42846		�����ɵ�
	uint32 MageArcane_ArcaneShock;// 42897	�������

	uint32 MageFrost_IceArrow;// 42842		������
	uint32 MageFrost_IceLance;// 42914		��ǹ
	uint32 MageFrost_FFArrow;// 47610		˪���

	uint32 MageFire_FireArrow;// 42833		������
	uint32 MageFire_FireShock;// 42873		������
	uint32 MageFire_Firing;// 42859			����
	uint32 MageFire_BigFireBall;// 42891	�����
	uint32 MageFire_FireBomb;// 55360		����ը��

	uint32 MagePet_FrostNova;// 33395		ˮ������������

	uint32 MageFlag_FireStun;// 64343		Aura:���Ի�����
	uint32 MageFlag_FastFStorm;// 54741		Aura:����籩˲��
	uint32 MageFlag_FastBFBall;// 48108		Aura:˲�������
	uint32 MageFlag_FastFFArrow;// 57761	Aura:˲��˪���
	uint32 MageFlag_CanFrozen;// 74396		Aura:Ŀ�굱�ɱ�����״̬
	uint32 MageFlag_Scherm;// 45438			Aura:����״̬��
};

class BotWarlockSpells
{
public:
	void InitializeSpells(Player* player);

protected:
	uint32 WarlockIDLE_LowArmor;// 696			�ͼ�����
	uint32 WarlockIDLE_Armor;// 47889			�м�����
	uint32 WarlockIDLE_HighArmor;// 47893		�߼�����
	uint32 WarlockIDLE_SoulLink;// 19028		�����������(��ħϵ)
	uint32 WarlockIDLE_ShadowShield;// 47891		��Ӱ��
	uint32 WarlockIDLE_SummonFireDemon;// 688	�ٻ�С��
	uint32 WarlockIDLE_SummonHollowDemon;// 697	�ٻ����
	uint32 WarlockIDLE_SummonSuccubus;// 712		�ٻ���ħ
	uint32 WarlockIDLE_SummonDogDemon;// 691		�ٻ���Ȯ
	uint32 WarlockIDLE_SummonGuardDemon;// 30146	�ٻ���������
	uint32 WarlockIDLE_FastSummon;// 18708		�����ٻ�(��ħϵ)
	uint32 WarlockIDLE_OpenGate;// 48018		�������ͷ���
	uint32 WarlockIDLE_TeleGate;// 48020		˲�Ƶ����ͷ���
	uint32 WarlockIDLE_SummonRite;// 29893		�����ʽ

	uint32 WarlockDemon_ToDemon;// 47241		������ħ
	uint32 WarlockDemon_Charge;// 54785			��ħ���
	uint32 WarlockDemon_MeleeAOE;// 50581		��ħ˳��ն
	uint32 WarlockDemon_Sacrifice;// 50589		��ħ�׼�

	uint32 WarlockAssist_DemonPower;// 47193		ǿ���ٻ��Ķ�ħ(��ħϵ)
	uint32 WarlockAssist_ExtractMana;// 59092	�������ϳ���(ʹ��ϵ)
	uint32 WarlockAssist_ConvertMana;// 57946	�������� Ѫת��
	uint32 WarlockAssist_StealLife;// 47857		��ȡ����
	uint32 WarlockAssist_StealMana;// 5138		��ȡ����
	uint32 WarlockAssist_BaseFear;// 6215		�־���
	uint32 WarlockAssist_FastFear;// 47860		��������
	uint32 WarlockAssist_AOEFear;// 17928		Ⱥ��־���

	uint32 WarlockAOE_MeleeFire;// 61290		��սǰ��AOE
	uint32 WarlockAOE_RainFire;// 47820			������AOE
	uint32 WarlockAOE_ShadowRage;// 47847		��Ӱ֮ŭAOEȺ�����(����ϵ)

	uint32 WarlockCurse_UpDmg;// 47865			Ԫ���˺��������
	uint32 WarlockCurse_MoveLow;// 18223		ƣ������ �ƶ��ٶȽ���(ʹ��ϵ)
	uint32 WarlockCurse_MgcDmg;// 47864			ʹ������ DOT�˺�
	uint32 WarlockCurse_MeleeLow;// 50511		�������� ��ǿ����
	uint32 WarlockCurse_CastLow;// 11719		�������� ʩ��ʱ���ӳ�

	uint32 WarlockDot_LeechSoul;// 59164		�ǻ���(ʹ��ϵ)
	uint32 WarlockDot_HighDmg;// 47843			ʹ�ද��(ʹ��ϵ)
	uint32 WarlockDot_LowDmg;// 47813			������
	uint32 WarlockDot_AOEDmg;// 47836			�������� dot��Χ�˺�
	uint32 WarlockDot_Sacrifice;// 47811		�����׼�

	uint32 WarlockCast_ShadowArrow;// 47809		��Ӱ֮��
	uint32 WarlockCast_ShadowShock;// 47827		��Ӱ���24��(����ϵ)
	uint32 WarlockCast_ChaosArrow;// 59172		����֮��(����ϵ)
	uint32 WarlockCast_FullBurn;// 47838		ȼ�� ���׼�ʱ��ǿЧ��(����ϵ)
	uint32 WarlockCast_FireBurn;// 17962		���� �����׼�(����ϵ)
	uint32 WarlockCast_BigFireBall;// 47825		�������

	uint32 WarlockPetDemon_Charge;// 47996		��ħpet���
	uint32 WarlockPetDemon_Melee;// 47994		��ħpet˳��ն
	uint32 WarlockPetDog_MagicBuf;// 57567		��Ȯpet�Ŷ�BUF
	uint32 WarlockPetDog_EatMgc;// 48011		��Ȯpet��ʳħ��
	uint32 WarlockPetDog_Bite;// 54053			��Ȯpetҧ��
	uint32 WarlockPetDog_Silence;// 19647		��Ȯpet��Ĭ
	uint32 WarlockPetSuccubus_Lash;// 47992		��ħpet�޴�
	uint32 WarlockPetSuccubus_Sneak;// 7870		��ħpetǱ��
	uint32 WarlockPetSuccubus_Charm;// 6358		��ħpet�Ȼ�
	uint32 WarlockPetGhost_Shield;// 47983		С��pet���
	uint32 WarlockPetGhost_Sneak;// 4511		С��petǱ��
	uint32 WarlockPetGhost_Stamp;// 47982		С��petѪӡ��
	uint32 WarlockPetGhost_FireArrow;// 47964	С��pet�����

	uint32 WarlockFlag_SoulItem;// 6265			�����Ƭ����
	uint32 WarlockFlag_SoulLink;// 25228		�����������BUF
	uint32 WarlockFlag_OpenGate;// 48018		���ͷ���BUF
	uint32 WarlockFlag_Sacrifice;// 50589		��ħ�׼�BUF
};

class BotPriestSpells
{
public:
	void InitializeSpells(Player* player);

protected:
	uint32 PriestIDLE_AllHardRes;// 48162			ȫ�����
	uint32 PriestIDLE_HardRes;// 48161				�������
	uint32 PriestIDLE_SoulFire;// 48168				����֮��
	uint32 PriestIDLE_AllSpiritRes;// 48074			ȫ�徫��
	uint32 PriestIDLE_SpiritRes;// 48073			���徫��
	uint32 PriestIDLE_Bloodsucker;// 15286			��Ѫ��ӵ��
	uint32 PriestIDLE_AllShadowRes;// 48170			ȫ�尵Ӱ����
	uint32 PriestIDLE_ShadowRes;// 48169			���尵Ӱ����
	uint32 PriestIDLE_ShadowStatus;// 15473			��Ӱ��̬
	uint32 PriestIDLE_Revive;// 48171				�������

	uint32 PriestGuard_ShadowFear;// 64044			����־�(��Ӱϵ)
	uint32 PriestGuard_AOEFear;// 10890				��ս��ΧȺ�־�
	uint32 PriestGuard_DefFear;// 6346				���־���
	uint32 PriestGuard_RecoverMana;// 47585			��Ӱ����������(��Ӱϵ)
	uint32 PriestGuard_DmgAnnul;// 33206			ʹ��ѹ��(����ϵ)
	uint32 PriestGuard_DefShield;// 48066			������ ��
	uint32 PriestGuard_SelfHealth;// 48173			�������� �Լ�˲������(��ʥϵ)
	uint32 PriestGuard_GuardSoul;// 47788			����ػ� target����ʱ�ػ�(��ʥϵ)

	uint32 PriestAssist_SoulAbs;// 14751			����רעnext0����(����ϵ)
	uint32 PriestAssist_AddHolyPower;// 10060		����ע��targetʩ�����ٺͱ���(����ϵ)
	uint32 PriestAssist_AllDispel;// 32375			ȫ����ɢħ��
	uint32 PriestAssist_Dispel;// 988				������ɢħ��
	uint32 PriestAssist_ShadowDemon;// 34433			��Ӱ��ħ
	uint32 PriestAssist_Silence;// 15487			��Ӱ��Ĭ(��Ӱϵ)
	uint32 PriestAssist_AllResMana;// 64901			����ȫ�����
	uint32 PriestAssist_AllResLife;// 64843			����ȫ���Ѫ
	uint32 PriestAssist_DecIllness;// 552			������ɢ������BUF

	uint32 PriestDebuf_Ache;// 48125				ʹ
	uint32 PriestDebuf_Drown;// 48158				��
	uint32 PriestDebuf_Plague;// 48300				����

	uint32 PriestAOE_ShadowExplode;// 53023			��Ӱ��ըAOE(��Ӱϵ)
	uint32 PriestAOE_HolyNova;// 48078				��ʥ����

	uint32 PriestShadow_ShadowTouch;// 48160			��Ѫ֮��(��Ӱϵ)
	uint32 PriestShadow_Knocking;// 48127			������
	uint32 PriestShadow_Lech;// 48156				�����̢(��Ӱϵ)
	uint32 PriestHoly_Smite;// 48123				�ͻ�
	uint32 PriestHoly_BigFire;// 48135				��ʥ֮��
	uint32 PriestPrecept_ManaBurn;// 8129			����ȼ��

	uint32 PriestHeal_ZeroHeal;// 2050				�μ�����
	uint32 PriestHeal_LowHeal;// 6064				�ͼ�����
	uint32 PriestHeal_Resume;// 48068				�ָ�
	uint32 PriestHeal_FastHeal;// 48071				��������
	uint32 PriestHeal_BigHeal;// 48063				������
	uint32 PriestHeal_LinkHeal;// 48120				�����Լ���Ŀ������
	uint32 PriestHeal_UnionHeal;// 48113			���ϵ��� target next heal
	uint32 PriestHeal_RingHeal;// 48089				����֮��(��ʥϵ)
	uint32 PriestHeal_AOEHeal;// 48072				���Ƶ��� aoe heal
	uint32 PriestHeal_Awareness;// 53007			���� ����3�������ӵ�����Ŀ��(����ϵ)

	uint32 PriestFlag_DeadSoul;// 27827				����֮�� ����������
	uint32 PriestFlag_NonShield;// 6788				���Զ�DEBUFF
};

#endif // !_BOT_AI_SPELLS_H_
