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

#include "ScriptedCreature.h"
#include "Spell.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "ObjectMgr.h"
#include "AreaBoundary.h"
#include "BotGroupAI.h"
#include "Group.h"
#include "Pet.h"
#include "Vehicle.h"

// Spell summary for ScriptedAI::SelectSpell
struct TSpellSummary
{
    uint8 Targets;                                          // set of enum SelectTarget
    uint8 Effects;                                          // set of enum SelectEffect
} extern* SpellSummary;

void SummonList::DoZoneInCombat(uint32 entry, float maxRangeToNearestTarget)
{
    for (StorageType::iterator i = storage_.begin(); i != storage_.end();)
    {
        Creature* summon = ObjectAccessor::GetCreature(*me, *i);
        ++i;
        if (summon && summon->IsAIEnabled
                && (!entry || summon->GetEntry() == entry))
        {
            summon->AI()->DoZoneInCombat(nullptr, maxRangeToNearestTarget);
        }
    }
}

void SummonList::DespawnEntry(uint32 entry)
{
    for (StorageType::iterator i = storage_.begin(); i != storage_.end();)
    {
        Creature* summon = ObjectAccessor::GetCreature(*me, *i);
        if (!summon)
            i = storage_.erase(i);
        else if (summon->GetEntry() == entry)
        {
            i = storage_.erase(i);
            summon->DespawnOrUnsummon();
        }
        else
            ++i;
    }
}

void SummonList::DespawnAll()
{
    while (!storage_.empty())
    {
        Creature* summon = ObjectAccessor::GetCreature(*me, storage_.front());
        storage_.pop_front();
        if (summon)
            summon->DespawnOrUnsummon();
    }
}

void SummonList::RemoveNotExisting()
{
    for (StorageType::iterator i = storage_.begin(); i != storage_.end();)
    {
        if (ObjectAccessor::GetCreature(*me, *i))
            ++i;
        else
            i = storage_.erase(i);
    }
}

bool SummonList::HasEntry(uint32 entry) const
{
    for (StorageType::const_iterator i = storage_.begin(); i != storage_.end(); ++i)
    {
        Creature* summon = ObjectAccessor::GetCreature(*me, *i);
        if (summon && summon->GetEntry() == entry)
            return true;
    }

    return false;
}

ScriptedAI::ScriptedAI(Creature* creature) : CreatureAI(creature),
    IsFleeing(false),
    _isCombatMovementAllowed(true)
{
    _isHeroic = me->GetMap()->IsHeroic();
    _difficulty = Difficulty(me->GetMap()->GetSpawnMode());
}

void ScriptedAI::AttackStartNoMove(Unit* who)
{
    if (!who)
        return;

    if (me->Attack(who, true))
        DoStartNoMovement(who);
}

void ScriptedAI::AttackStart(Unit* who)
{
    if (IsCombatMovementAllowed())
        CreatureAI::AttackStart(who);
    else
        AttackStartNoMove(who);
}

void ScriptedAI::UpdateAI(uint32 /*diff*/)
{
    //Check if we have a current target
    if (!UpdateVictim())
        return;

    DoMeleeAttackIfReady();
}

void ScriptedAI::DoStartMovement(Unit* victim, float distance, float angle)
{
    if (victim)
        me->GetMotionMaster()->MoveChase(victim, distance, angle);
}

void ScriptedAI::DoStartNoMovement(Unit* victim)
{
    if (!victim)
        return;

    me->GetMotionMaster()->MoveIdle();
}

void ScriptedAI::DoStopAttack()
{
    if (me->GetVictim())
        me->AttackStop();
}

void ScriptedAI::DoCastSpell(Unit* target, SpellInfo const* spellInfo, bool triggered)
{
    if (!target || me->IsNonMeleeSpellCast(false))
        return;

    me->StopMoving();
    me->CastSpell(target, spellInfo, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE);
}

void ScriptedAI::DoPlaySoundToSet(WorldObject* source, uint32 soundId)
{
    if (!source)
        return;

    if (!sSoundEntriesStore.LookupEntry(soundId))
    {
        TC_LOG_ERROR("scripts", "Invalid soundId %u used in DoPlaySoundToSet (Source: TypeId %u, GUID %u)", soundId, source->GetTypeId(), source->GetGUID().GetCounter());
        return;
    }

    source->PlayDirectSound(soundId);
}

Creature* ScriptedAI::DoSpawnCreature(uint32 entry, float offsetX, float offsetY, float offsetZ, float angle, uint32 type, uint32 despawntime)
{
    return me->SummonCreature(entry, me->GetPositionX() + offsetX, me->GetPositionY() + offsetY, me->GetPositionZ() + offsetZ, angle, TempSummonType(type), despawntime);
}

SpellInfo const* ScriptedAI::SelectSpell(Unit* target, uint32 school, uint32 mechanic, SelectTargetType targets, uint32 powerCostMin, uint32 powerCostMax, float rangeMin, float rangeMax, SelectEffect effects)
{
    //No target so we can't cast
    if (!target)
        return nullptr;

    //Silenced so we can't cast
    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
        return nullptr;

    //Using the extended script system we first create a list of viable spells
    SpellInfo const* apSpell[MAX_CREATURE_SPELLS];
    memset(apSpell, 0, MAX_CREATURE_SPELLS * sizeof(SpellInfo*));

    uint32 spellCount = 0;

    SpellInfo const* tempSpell = nullptr;

    //Check if each spell is viable(set it to null if not)
    for (uint32 i = 0; i < MAX_CREATURE_SPELLS; i++)
    {
        tempSpell = sSpellMgr->GetSpellInfo(me->m_spells[i]);

        //This spell doesn't exist
        if (!tempSpell)
            continue;

        // Targets and Effects checked first as most used restrictions
        //Check the spell targets if specified
        if (targets && !(SpellSummary[me->m_spells[i]].Targets & (1 << (targets-1))))
            continue;

        //Check the type of spell if we are looking for a specific spell type
        if (effects && !(SpellSummary[me->m_spells[i]].Effects & (1 << (effects-1))))
            continue;

        //Check for school if specified
        if (school && (tempSpell->SchoolMask & school) == 0)
            continue;

        //Check for spell mechanic if specified
        if (mechanic && tempSpell->Mechanic != mechanic)
            continue;

        //Make sure that the spell uses the requested amount of power
        if (powerCostMin && tempSpell->ManaCost < powerCostMin)
            continue;

        if (powerCostMax && tempSpell->ManaCost > powerCostMax)
            continue;

        //Continue if we don't have the mana to actually cast this spell
        if (tempSpell->ManaCost > me->GetPower(Powers(tempSpell->PowerType)))
            continue;

        //Check if the spell meets our range requirements
        if (rangeMin && me->GetSpellMinRangeForTarget(target, tempSpell) < rangeMin)
            continue;
        if (rangeMax && me->GetSpellMaxRangeForTarget(target, tempSpell) > rangeMax)
            continue;

        //Check if our target is in range
        if (me->IsWithinDistInMap(target, float(me->GetSpellMinRangeForTarget(target, tempSpell))) || !me->IsWithinDistInMap(target, float(me->GetSpellMaxRangeForTarget(target, tempSpell))))
            continue;

        //All good so lets add it to the spell list
        apSpell[spellCount] = tempSpell;
        ++spellCount;
    }

    //We got our usable spells so now lets randomly pick one
    if (!spellCount)
        return nullptr;

    return apSpell[urand(0, spellCount - 1)];
}

void ScriptedAI::DoResetThreat()
{
    if (!me->CanHaveThreatList() || me->getThreatManager().isThreatListEmpty())
    {
        TC_LOG_ERROR("scripts", "DoResetThreat called for creature that either cannot have threat list or has empty threat list (me entry = %d)", me->GetEntry());
        return;
    }

    ThreatContainer::StorageType threatlist = me->getThreatManager().getThreatList();

    for (ThreatContainer::StorageType::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
    {
        Unit* unit = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
        if (unit && DoGetThreat(unit))
            DoModifyThreatPercent(unit, -100);
    }
}

float ScriptedAI::DoGetThreat(Unit* unit)
{
    if (!unit)
        return 0.0f;
    return me->getThreatManager().getThreat(unit);
}

void ScriptedAI::DoModifyThreatPercent(Unit* unit, int32 pct)
{
    if (!unit)
        return;
    me->getThreatManager().modifyThreatPercent(unit, pct);
}

void ScriptedAI::DoTeleportTo(float x, float y, float z, uint32 time)
{
    me->Relocate(x, y, z);
    float speed = me->GetDistance(x, y, z) / ((float)time * 0.001f);
    me->MonsterMoveWithSpeed(x, y, z, speed);
}

void ScriptedAI::DoTeleportTo(const float position[4])
{
    me->NearTeleportTo(position[0], position[1], position[2], position[3]);
}

void ScriptedAI::DoTeleportPlayer(Unit* unit, float x, float y, float z, float o)
{
    if (!unit)
        return;

    if (Player* player = unit->ToPlayer())
        player->TeleportTo(unit->GetMapId(), x, y, z, o, TELE_TO_NOT_LEAVE_COMBAT);
    else
        TC_LOG_ERROR("scripts", "Creature %s Tried to teleport non-player unit (%s) to x: %f y:%f z: %f o: %f. Aborted.",
            me->GetGUID().ToString().c_str(), unit->GetGUID().ToString().c_str(), x, y, z, o);
}

void ScriptedAI::DoTeleportAll(float x, float y, float z, float o)
{
    Map* map = me->GetMap();
    if (!map->IsDungeon())
        return;

    Map::PlayerList const& PlayerList = map->GetPlayers();
    for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        if (Player* player = itr->GetSource())
            if (player->IsAlive())
                player->TeleportTo(me->GetMapId(), x, y, z, o, TELE_TO_NOT_LEAVE_COMBAT);
}

Unit* ScriptedAI::DoSelectLowestHpFriendly(float range, uint32 minHPDiff)
{
    Unit* unit = nullptr;
    Trinity::MostHPMissingInRange u_check(me, range, minHPDiff);
    Trinity::UnitLastSearcher<Trinity::MostHPMissingInRange> searcher(me, unit, u_check);
    me->VisitNearbyObject(range, searcher);

    return unit;
}

std::list<Creature*> ScriptedAI::DoFindFriendlyCC(float range)
{
    std::list<Creature*> list;
    Trinity::FriendlyCCedInRange u_check(me, range);
    Trinity::CreatureListSearcher<Trinity::FriendlyCCedInRange> searcher(me, list, u_check);
    me->VisitNearbyObject(range, searcher);

    return list;
}

std::list<Creature*> ScriptedAI::DoFindFriendlyMissingBuff(float range, uint32 uiSpellid)
{
    std::list<Creature*> list;
    Trinity::FriendlyMissingBuffInRange u_check(me, range, uiSpellid);
    Trinity::CreatureListSearcher<Trinity::FriendlyMissingBuffInRange> searcher(me, list, u_check);
    me->VisitNearbyObject(range, searcher);

    return list;
}

Player* ScriptedAI::GetPlayerAtMinimumRange(float minimumRange)
{
    Player* player = nullptr;

    CellCoord pair(Trinity::ComputeCellCoord(me->GetPositionX(), me->GetPositionY()));
    Cell cell(pair);
    cell.SetNoCreate();

    Trinity::PlayerAtMinimumRangeAway check(me, minimumRange);
    Trinity::PlayerSearcher<Trinity::PlayerAtMinimumRangeAway> searcher(me, player, check);
    TypeContainerVisitor<Trinity::PlayerSearcher<Trinity::PlayerAtMinimumRangeAway>, GridTypeMapContainer> visitor(searcher);

    cell.Visit(pair, visitor, *me->GetMap(), *me, minimumRange);

    return player;
}

void ScriptedAI::SetEquipmentSlots(bool loadDefault, int32 mainHand /*= EQUIP_NO_CHANGE*/, int32 offHand /*= EQUIP_NO_CHANGE*/, int32 ranged /*= EQUIP_NO_CHANGE*/)
{
    if (loadDefault)
    {
        me->LoadEquipment(me->GetOriginalEquipmentId(), true);
        return;
    }

    if (mainHand >= 0)
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(mainHand));

    if (offHand >= 0)
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, uint32(offHand));

    if (ranged >= 0)
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, uint32(ranged));
}

void ScriptedAI::SetCombatMovement(bool allowMovement)
{
    _isCombatMovementAllowed = allowMovement;
}

enum NPCs
{
    NPC_BROODLORD   = 12017,
    NPC_VOID_REAVER = 19516,
    NPC_JAN_ALAI    = 23578,
    NPC_SARTHARION  = 28860
};

// Hacklike storage used for misc creatures that are expected to evade of outside of a certain area.
// It is assumed the information is found elswehere and can be handled by the core. So far no luck finding such information/way to extract it.
/*bool ScriptedAI::EnterEvadeIfOutOfCombatArea(uint32 const diff)
{
    if (_evadeCheckCooldown <= diff)
        _evadeCheckCooldown = 2500;
    else
    {
        _evadeCheckCooldown -= diff;
        return false;
    }

    if (me->IsInEvadeMode() || !me->GetVictim())
        return false;

    float x = me->GetPositionX();
    float y = me->GetPositionY();
    float z = me->GetPositionZ();

    switch (me->GetEntry())
    {
        case NPC_BROODLORD:                                         // broodlord (not move down stairs)
            if (z > 448.60f)
                return false;
            break;
        case NPC_VOID_REAVER:                                         // void reaver (calculate from center of room)
            if (me->GetDistance2d(432.59f, 371.93f) < 105.0f)
                return false;
            break;
        case NPC_JAN_ALAI:                                         // jan'alai (calculate by Z)
            if (z > 12.0f)
                return false;
            break;
        case NPC_SARTHARION:                                         // sartharion (calculate box)
            if (x > 3218.86f && x < 3275.69f && y < 572.40f && y > 484.68f)
                return false;
            break;
        default: // For most of creatures that certain area is their home area.
            TC_LOG_INFO("misc", "TSCR: EnterEvadeIfOutOfCombatArea used for creature entry %u, but does not have any definition. Using the default one.", me->GetEntry());
            uint32 homeAreaId = me->GetMap()->GetAreaId(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ());
            if (me->GetAreaId() == homeAreaId)
                return false;
    }

    EnterEvadeMode();
    return true;
}*/

// BossAI - for instanced bosses
BossAI::BossAI(Creature* creature, uint32 bossId) : ScriptedAI(creature),
    instance(creature->GetInstanceScript()),
    summons(creature),
    _bossId(bossId)
{
    if (instance)
        SetBoundary(instance->GetBossBoundary(bossId));
    scheduler.SetValidator([this]
    {
        return !me->HasUnitState(UNIT_STATE_CASTING);
    });
}

void BossAI::_Reset()
{
    if (!me->IsAlive())
        return;

    me->SetCombatPulseDelay(0);
    me->ResetLootMode();
    events.Reset();
    summons.DespawnAll();
    scheduler.CancelAll();
    if (instance)
        instance->SetBossState(_bossId, NOT_STARTED);
}

void BossAI::_JustDied()
{
    events.Reset();
    summons.DespawnAll();
    scheduler.CancelAll();
    if (instance)
        instance->SetBossState(_bossId, DONE);
}

void BossAI::_EnterCombat()
{
    if (instance)
    {
        // bosses do not respawn, check only on enter combat
        if (!instance->CheckRequiredBosses(_bossId))
        {
            EnterEvadeMode(EVADE_REASON_SEQUENCE_BREAK);
            return;
        }
        instance->SetBossState(_bossId, IN_PROGRESS);
    }

    me->SetCombatPulseDelay(5);
    me->setActive(true);
    DoZoneInCombat();
    ScheduleTasks();
}

bool BossAI::CanRespawn()
{
    if (instance && instance->GetBossState(_bossId) == DONE)
        return false;

    return true;
}

void BossAI::TeleportCheaters()
{
    float x, y, z;
    me->GetPosition(x, y, z);

    ThreatContainer::StorageType threatList = me->getThreatManager().getThreatList();
    for (ThreatContainer::StorageType::const_iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
        if (Unit* target = (*itr)->getTarget())
            if (target->GetTypeId() == TYPEID_PLAYER && !CheckBoundary(target))
                target->NearTeleportTo(x, y, z, 0);
}

void BossAI::JustSummoned(Creature* summon)
{
    summons.Summon(summon);
    if (me->IsInCombat())
        DoZoneInCombat(summon);
}

void BossAI::SummonedCreatureDespawn(Creature* summon)
{
    summons.Despawn(summon);
}

void BossAI::UpdateAI(uint32 diff)
{
    if (!UpdateVictim())
        return;

    events.Update(diff);

    if (me->HasUnitState(UNIT_STATE_CASTING))
        return;

    while (uint32 eventId = events.ExecuteEvent())
        ExecuteEvent(eventId);

    DoMeleeAttackIfReady();
}

void BossAI::_DespawnAtEvade(uint32 delayToRespawn, Creature* who)
{
    if (delayToRespawn < 2)
    {
        TC_LOG_ERROR("scripts", "_DespawnAtEvade called with delay of %u seconds, defaulting to 2.", delayToRespawn);
        delayToRespawn = 2;
    }

    if (!who)
        who = me;

    if (TempSummon* whoSummon = who->ToTempSummon())
    {
        TC_LOG_WARN("scripts", "_DespawnAtEvade called on a temporary summon.");
        whoSummon->UnSummon();
        return;
    }

    uint32 corpseDelay = who->GetCorpseDelay();
    uint32 respawnDelay = who->GetRespawnDelay();

    who->SetCorpseDelay(1);
    who->SetRespawnDelay(delayToRespawn - 1);

    who->DespawnOrUnsummon();

    who->SetCorpseDelay(corpseDelay);
    who->SetRespawnDelay(respawnDelay);

    if (instance && who == me)
        instance->SetBossState(_bossId, FAIL);
}

// WorldBossAI - for non-instanced bosses

WorldBossAI::WorldBossAI(Creature* creature) :
    ScriptedAI(creature),
    summons(creature) { }

void WorldBossAI::_Reset()
{
    if (!me->IsAlive())
        return;

    events.Reset();
    summons.DespawnAll();
}

void WorldBossAI::_JustDied()
{
    events.Reset();
    summons.DespawnAll();
}

void WorldBossAI::_EnterCombat()
{
    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true);
    if (target)
        AttackStart(target);
}

void WorldBossAI::JustSummoned(Creature* summon)
{
    summons.Summon(summon);
    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true);
    if (target)
        summon->AI()->AttackStart(target);
}

void WorldBossAI::SummonedCreatureDespawn(Creature* summon)
{
    summons.Despawn(summon);
}

void WorldBossAI::UpdateAI(uint32 diff)
{
    if (!UpdateVictim())
        return;

    events.Update(diff);

    if (me->HasUnitState(UNIT_STATE_CASTING))
        return;

    while (uint32 eventId = events.ExecuteEvent())
        ExecuteEvent(eventId);

    DoMeleeAttackIfReady();
}

// SD2 grid searchers.
Creature* GetClosestCreatureWithEntry(WorldObject* source, uint32 entry, float maxSearchRange, bool alive /*= true*/)
{
    return source->FindNearestCreature(entry, maxSearchRange, alive);
}

GameObject* GetClosestGameObjectWithEntry(WorldObject* source, uint32 entry, float maxSearchRange)
{
    return source->FindNearestGameObject(entry, maxSearchRange);
}

void GetCreatureListWithEntryInGrid(std::list<Creature*>& list, WorldObject* source, uint32 entry, float maxSearchRange)
{
    source->GetCreatureListWithEntryInGrid(list, entry, maxSearchRange);
}

void GetGameObjectListWithEntryInGrid(std::list<GameObject*>& list, WorldObject* source, uint32 entry, float maxSearchRange)
{
    source->GetGameObjectListWithEntryInGrid(list, entry, maxSearchRange);
}

void GetPlayerListInGrid(std::list<Player*>& list, WorldObject* source, float maxSearchRange)
{
    source->GetPlayerListInGrid(list, maxSearchRange);
}

//////////////////////////////////////////////////////////////////////////
// Bot AI command
//////////////////////////////////////////////////////////////////////////

bool NeedBotAttackCreature::UpdateProcess(std::list<ObjectGuid>& freeBots)
{
	Creature* pCreature = atMap->GetCreature(needCreature);
	if (!pCreature || !pCreature->IsAlive())
	{
		allUsedBots.clear();
		return false;
	}
	if (pCreature && !pCreature->IsVisible())
	{
		allUsedBots.clear();
		return true;
	}

	while (int32(allUsedBots.size()) < needCount)
	{
		if (freeBots.empty())
			break;
		allUsedBots.push_back(*freeBots.begin());
		freeBots.erase(freeBots.begin());
	}
	std::list<std::list<ObjectGuid>::iterator > needClearBot;
	for (std::list<ObjectGuid>::iterator itBot = allUsedBots.begin(); itBot != allUsedBots.end(); itBot++)
	{
		ObjectGuid& guid = *itBot;
		Player* player = ObjectAccessor::FindPlayer(guid);
		if (!player || !player->IsAlive() || player->GetMap() != atMap)
			needClearBot.push_back(itBot);
		else if (player->GetDistance(pCreature->GetPosition()) < 120)
			player->SetSelection(needCreature);
	}
	for (std::list<ObjectGuid>::iterator itClear : needClearBot)
	{
		allUsedBots.erase(itClear);
	}
	return true;
}

void BotAttackCreature::UpdateNeedAttackCreatures(uint32 diff, ScriptedAI* affiliateAI, bool attackMain)
{
	currentTick -= int32(diff);
	if (currentTick >= 0)
		return;
	currentTick = updateGap;
	if (!mainCreature)
		return;

	if (!affiliateAI)
		return;
	if (allNeedCreatures.empty())
		return;
	std::list<Player*> allBots;
	affiliateAI->SearchTargetPlayerAllGroup(allBots, 120);
	std::list<ObjectGuid> allBotGUIDs;
	for (Player* player : allBots)
	{
		ObjectGuid guid = player->GetGUID();
		bool canPush = true;
		for (NeedBotAttackCreature* pNeed : allNeedCreatures)
		{
			if (pNeed->IsThisUsedBot(guid))
			{
				canPush = false;
				break;
			}
		}
		if (canPush)
			allBotGUIDs.push_back(guid);
	}
	std::list<std::list<NeedBotAttackCreature*>::iterator > needClears;
	for (std::list<NeedBotAttackCreature*>::iterator itNeed = allNeedCreatures.begin(); itNeed != allNeedCreatures.end(); itNeed++)
	{
		NeedBotAttackCreature* pNeed = *itNeed;
		bool ing = pNeed->UpdateProcess(allBotGUIDs);
		if (!ing)
			needClears.push_back(itNeed);
	}
	for (std::list<NeedBotAttackCreature*>::iterator itClear : needClears)
	{
		NeedBotAttackCreature* pNeed = *itClear;
		delete pNeed;
		allNeedCreatures.erase(itClear);
	}
	if (attackMain && mainCreature && mainCreature->IsAlive())
	{
		for (ObjectGuid guid : allBotGUIDs)
		{
			Player* player = ObjectAccessor::FindPlayer(guid);
			if (player && player->IsAlive() && player->GetMap() == mainCreature->GetMap())
				player->SetSelection(mainCreature->GetGUID());
		}
	}
	else if (attackMain)
	{
		mainCreature = NULL;
	}
}

void BotAttackCreature::AddNewCreatureNeedAttack(Creature* pCreature, int32 needBotCount)
{
	if (!pCreature || !pCreature->IsAlive() || needBotCount < 0 || pCreature == mainCreature)
		return;
	if (pCreature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
		return;
	Map* atMap = pCreature->GetMap();
	if (!atMap)
		return;
	ObjectGuid guid = pCreature->GetGUID();
	for (NeedBotAttackCreature* pNeed : allNeedCreatures)
	{
		if (pNeed->IsThisCreature(guid))
			return;
	}
	NeedBotAttackCreature* pNeedCreature = new NeedBotAttackCreature(atMap, needBotCount, guid);
	allNeedCreatures.push_back(pNeedCreature);
}

void ScriptedAI::GetInViewBotPlayers(std::list<Player*>& outPlayers, float range)
{
	Map::PlayerList const& players = me->GetMap()->GetPlayers();
	for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
	{
		Player* player = i->GetSource();
		if (!player || !player->IsAlive() || !player->IsPlayerBot())
			continue;
		if (!me->InSamePhase(player->GetPhaseMask()))
			continue;
		if (me->GetDistance(player->GetPosition()) > range)
			continue;
		if(!me->IsWithinLOSInMap(player))
			continue;
		outPlayers.push_back(player);
	}
}

void ScriptedAI::SearchTargetPlayerAllGroup(std::list<Player*>& players, float range)
{
	if (range < 3.0f)
		return;
	ObjectGuid targetGUID = me->GetTarget();
	Player* targetPlayer = NULL;
	if (targetGUID == ObjectGuid::Empty)
	{
		std::list<Player*> playersNearby;
		me->GetPlayerListInGrid(playersNearby, range);
		if (playersNearby.empty())
			return;
		for (Player* p : playersNearby)
		{
			if (p->IsAlive() && p->GetMap() == me->GetMap())
			{
				targetPlayer = p;
				targetGUID = p->GetGUID();
				break;
			}
		}
	}
	if (!targetPlayer)
		targetPlayer = ObjectAccessor::FindPlayer(targetGUID);
	if (!targetPlayer || targetPlayer->GetMap() != me->GetMap())
		return;
	players.clear();
	players.push_back(targetPlayer);

	Group* pGroup = targetPlayer->GetGroup();
	if (!pGroup || pGroup->isBFGroup())
		return;
	Group::MemberSlotList const& memList = pGroup->GetMemberSlots();
	for (Group::MemberSlot const& slot : memList)
	{
		Player* player = ObjectAccessor::FindPlayer(slot.guid);
		if (!player || !player->IsAlive() || targetPlayer->GetMap() != player->GetMap() ||
			!player->IsInWorld() || player == targetPlayer || !player->IsPlayerBot())
			continue;
		if (me->GetDistance(player->GetPosition()) > range)
			continue;
		players.push_back(player);
	}
}

void ScriptedAI::PickBotPullMeToPosition(Position pullPos, ObjectGuid fliterTarget)
{
	std::list<BotGroupAI*> tankAIs;// , healAIs;
	std::list<Player*> targets;
	SearchTargetPlayerAllGroup(targets, 120);
	BotGroupAI* pNearTankAI = NULL;
	float nearTankAIDist = 999999;
	BotGroupAI* pNearHealAI[3] = { NULL };
	float nearHealAIDist[3] = { 999999 };
	for (Player* player : targets)
	{
		if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
		{
			if (pGroupAI->IsHealerBotAI())
			{
				float thisDist = me->GetDistance(player->GetPosition());
				for (int i = 0; i < 3; i++)
				{
					if (thisDist < nearHealAIDist[i])
					{
						pNearHealAI[i] = pGroupAI;
						nearHealAIDist[i] = thisDist;
						break;
					}
				}
			}
			else if (pGroupAI->IsTankBotAI())
			{
				if (fliterTarget != ObjectGuid::Empty)
				{
					Unit* tankTarget = player->GetSelectedUnit();
					if (tankTarget != NULL && tankTarget->GetGUID() == fliterTarget)
					{
						if (pNearTankAI == NULL)
						{
							if (urand(0, 99) > 50)
								continue;
						}
						else
							continue;
					}
				}
				tankAIs.push_back(pGroupAI);
				float thisDist = me->GetDistance(player->GetPosition());
				if (thisDist < nearTankAIDist)
				{
					pNearTankAI = pGroupAI;
					nearTankAIDist = thisDist;
				}
			}
		}
	}
	if (pNearTankAI == NULL)
		return;
	for (BotGroupAI* pGroupAI : tankAIs)
	{
		if (pGroupAI == pNearTankAI)
		{
			pGroupAI->ClearTankTarget();
			pGroupAI->AddTankTarget(me);
			pGroupAI->SetTankPosition(pullPos);
			pGroupAI->GetAIPayer()->SetSelection(me->GetGUID());
			for (int i = 0; i < 3; i++)
			{
				if (pNearHealAI[i])
					pNearHealAI[i]->GetAIPayer()->SetSelection(me->GetGUID());
			}
		}
		else
		{
			pGroupAI->ClearTankTarget();
			pGroupAI->GetAIPayer()->SetSelection(ObjectGuid::Empty);
		}
	}
}

bool ScriptedAI::ExistPlayerBotByRange(float range)
{
	std::list<Player*> targets;
	SearchTargetPlayerAllGroup(targets, range);
	return targets.size() > 0;
}

void ScriptedAI::BotBlockCastingMe()
{
	std::list<Player*> targets;
	SearchTargetPlayerAllGroup(targets, 120);
	for (Player* player : targets)
	{
		if (player->HasUnitState(UNIT_STATE_CASTING))
			continue;
		if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
		{
			if (pGroupAI->TryBlockCastingByTarget(me))
				return;
		}
	}
}

void ScriptedAI::ClearBotMeTarget(bool all)
{
	std::list<Player*> targets;
	SearchTargetPlayerAllGroup(targets, 120);
	for (Player* player : targets)
	{
		if (Pet* pPet = player->GetPet())
		{
			if (pPet->GetVictim() == me)
			{
				if (WorldSession* pSession = player->GetSession())
				{
					pSession->HandlePetActionHelper(pPet, pPet->GetGUID(), 1, 7, ObjectGuid::Empty);
				}
			}
		}
		if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
		{
			if (all || !pGroupAI->IsTankBotAI())
			{
				player->SetSelection(ObjectGuid::Empty);
				pGroupAI->ToggleFliterCreature(me, true);
			}
		}
	}
}

void ScriptedAI::BotAllMovetoFarByDistance(Unit* pUnit, float range, float dist, float offset)
{
	float onceAngle = float(M_PI) * 2.0f / 8.0f;
	std::list<Position> allPosition;
	for (float angle = 0.0f; angle < (float(M_PI) * 2.0f); angle += onceAngle)
	{
		Position& pos = pUnit->GetFirstCollisionPosition(dist, angle);
		float dist = pUnit->GetDistance(pos);
		if (!pUnit->IsWithinLOS(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ()))
			continue;
		allPosition.push_back(pos);
	}
	if (allPosition.empty())
		return;
	Position targetPos;
	float maxDist = 0.0f;
	for (Position pos : allPosition)
	{
		float distance = pUnit->GetDistance(pos);
		if (distance > maxDist)
		{
			maxDist = distance;
			targetPos = pos;
		}
	}
	if (maxDist < dist * 0.1f)
		return;
	targetPos.m_positionZ = pUnit->GetMap()->GetHeight(pUnit->GetPhaseMask(), targetPos.GetPositionX(), targetPos.GetPositionY(), targetPos.GetPositionZ());
	std::list<Player*> targets;
	SearchTargetPlayerAllGroup(targets, range);
	for (Player* player : targets)
	{
		if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
		{
			Position offsetPos = Position(targetPos.GetPositionX() + frand(-offset, offset),
				targetPos.GetPositionY() + frand(-offset, offset), targetPos.GetPositionZ());
			pGroupAI->ClearCruxMovement();
			pGroupAI->SetCruxMovement(targetPos);
		}
	}
}

void ScriptedAI::BotCruxFlee(uint32 durTime, ObjectGuid fliter)
{
	std::list<Player*> targets;
	SearchTargetPlayerAllGroup(targets, 120);
	for (Player* player : targets)
	{
		if (fliter != ObjectGuid::Empty)
		{
			if (player->GetGUID() == fliter)
				continue;
		}
		if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
		{
			pGroupAI->AddCruxFlee(durTime, me);
		}
	}
}

void ScriptedAI::BotRndCruxMovement(float dist)
{
	if (dist < 1.0f)
		return;
	std::list<Player*> playersNearby;
	me->GetPlayerListInGrid(playersNearby, 120);
	if (playersNearby.empty())
		return;
	for (Player* player : playersNearby)
	{
		if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
		{
			pGroupAI->RndCruxMovement(dist);
		}
	}
}

void ScriptedAI::BotCruxFleeByRange(float range)
{
	if (range < 3.0f)
		return;
	std::list<Player*> targets;
	SearchTargetPlayerAllGroup(targets, range);
	for (Player* player : targets)
	{
		if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
		{
			Position targetPos;
			if (!BotUtility::FindFirstCollisionPosition(player, range, me, targetPos))
				continue;
			//if (pGroupAI->GetAIPayer()->HasUnitState(UNIT_STATE_CASTING))
			//	pGroupAI->GetAIPayer()->CastStop();
			pGroupAI->SetCruxMovement(targetPos);
			//player->SetSelection(ObjectGuid::Empty);
		}
	}
}

void ScriptedAI::BotCruxFleeByRange(float range, Unit* pCenter)
{
	if (range < 3.0f || !pCenter)
		return;
	std::list<Player*> targets;
	SearchTargetPlayerAllGroup(targets, range);
	for (Player* player : targets)
	{
		if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
		{
			Position targetPos;
			if (!BotUtility::FindFirstCollisionPosition(player, range, pCenter, targetPos))
				continue;
			pGroupAI->SetCruxMovement(targetPos);
			//player->SetSelection(ObjectGuid::Empty);
		}
	}
}

void ScriptedAI::BotCruxFleeByArea(float range, float fleeDist, Unit* pCenter)
{
	if (range < 3.0f || fleeDist < 3.0f || !pCenter)
		return;
	Position& centerPos = pCenter->GetPosition();
	std::list<Player*> players;
	SearchTargetPlayerAllGroup(players, 80);
	for (Player* player : players)
	{
		if (player->GetDistance(centerPos) > range)
			continue;
		if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
		{
			Position targetPos;
			if (!BotUtility::FindFirstCollisionPosition(player, fleeDist, pCenter, targetPos))
				continue;
			pGroupAI->SetCruxMovement(targetPos);
			//player->SetSelection(ObjectGuid::Empty);
		}
	}
}

void ScriptedAI::BotAllTargetMe(bool all)
{
	std::list<Player*> targets;
	SearchTargetPlayerAllGroup(targets, 120);
	for (Player* player : targets)
	{
		if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
		{
			if (all)
				player->SetSelection(me->GetGUID());
			else if (!pGroupAI->IsTankBotAI() && !pGroupAI->IsHealerBotAI())
				player->SetSelection(me->GetGUID());
		}
	}
}

void ScriptedAI::BotPhysicsDPSTargetMe(Unit* pUnit)
{
	std::list<Player*> targets;
	SearchTargetPlayerAllGroup(targets, 120);
	for (Player* player : targets)
	{
		if (!player->IsPlayerBot())
			continue;
		if (player->getClass() == Classes::CLASS_ROGUE || player->getClass() == Classes::CLASS_HUNTER)
			player->SetSelection(me->GetGUID());
		else if (player->getClass() == Classes::CLASS_WARRIOR)
		{
			if(player->FindTalentType() != 2)
				player->SetSelection(pUnit->GetGUID());
		}
		else
		{
			if (player->GetSelectedUnit() == pUnit)
			{
				if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
				{
					if (!pGroupAI->IsHealerBotAI() && !pGroupAI->IsTankBotAI())
						player->SetSelection(ObjectGuid::Empty);
				}
			}
		}
	}
}

void ScriptedAI::BotMagicDPSTargetMe(Unit* pUnit)
{
	std::list<Player*> targets;
	SearchTargetPlayerAllGroup(targets, 120);
	for (Player* player : targets)
	{
		if (!player->IsPlayerBot())
			continue;
		if (player->getClass() == Classes::CLASS_ROGUE || player->getClass() == Classes::CLASS_HUNTER ||
			player->getClass() == Classes::CLASS_WARRIOR)
		{
			if (player->GetSelectedUnit() == pUnit)
			{
				if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
				{
					if (!pGroupAI->IsHealerBotAI() && !pGroupAI->IsTankBotAI())
						player->SetSelection(ObjectGuid::Empty);
				}
			}
		}
		else
		{
			if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
			{
				if (!pGroupAI->IsHealerBotAI() && !pGroupAI->IsTankBotAI())
					player->SetSelection(pUnit->GetGUID());
			}
		}
	}
}

void ScriptedAI::BotAverageCreatureTarget(std::vector<Creature*>& targets, float searchRange)
{
	if (targets.empty() || searchRange < 3.0f)
		return;
	std::queue<Player*> players;
	std::list<Player*> searchUnits;
	SearchTargetPlayerAllGroup(searchUnits, searchRange);
	for (Player* player : searchUnits)
	{
		players.push(player);
	}
	while (!players.empty())
	{
		for (Creature* pCreature : targets)
		{
			Player* player = players.front();
			players.pop();
			if (pCreature)
				player->SetSelection(pCreature->GetGUID());
			if (players.empty())
				break;
		}
	}
}

void ScriptedAI::BotAllotCreatureTarget(std::vector<Creature*>& targets, float searchRange, uint32 onceCount)
{
	if (onceCount < 1 || targets.empty() || searchRange < 3.0f)
		return;
	std::queue<Player*> players;
	std::list<Player*> searchUnits;
	SearchTargetPlayerAllGroup(searchUnits, searchRange);
	for (Player* player : searchUnits)
	{
		players.push(player);
	}
	while (!players.empty())
	{
		for (Creature* pCreature : targets)
		{
			int32 allot = int32(onceCount);
			while (!players.empty() && allot > 0)
			{
				--allot;
				Player* player = players.front();
				players.pop();
				if (pCreature)
					player->SetSelection(pCreature->GetGUID());
			}
			if (players.empty())
				break;
		}
	}
}

void ScriptedAI::BotAllToSelectionTarget(Unit* pUnit, float searchRange, bool all)
{
	if (!pUnit)
		return;
	ObjectGuid guid = pUnit->GetGUID();
	std::list<Player*> searchUnits;
	SearchTargetPlayerAllGroup(searchUnits, searchRange);
	for (Player* player : searchUnits)
	{
		if (player->GetTarget() == guid)
			continue;
		if (all)
			player->SetSelection(pUnit->GetGUID());
		else if (BotGroupAI* pAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
		{
			if (!pAI->IsTankBotAI())
				player->SetSelection(pUnit->GetGUID());
		}
	}
}

void ScriptedAI::BotAllFullDispel(bool enables)
{
	std::list<Player*> targets;
	SearchTargetPlayerAllGroup(targets, 120);
	for (Player* player : targets)
	{
		if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
		{
			if (enables)
				pGroupAI->StartFullDispel();
			else
				pGroupAI->ClearFullDispel();
		}
	}
}

void ScriptedAI::BotAllFullDispelByDecPoison(bool enables)
{
	std::list<Player*> targets;
	SearchTargetPlayerAllGroup(targets, 120);
	for (Player* player : targets)
	{
		if (player->getClass() != Classes::CLASS_DRUID && player->getClass() != Classes::CLASS_SHAMAN)
			continue;
		if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
		{
			if (enables)
				pGroupAI->StartFullDispel();
			else
				pGroupAI->ClearFullDispel();
		}
	}
}

void ScriptedAI::BotFleeLineByAngle(Unit* center, float angle, bool force)
{
	angle = Position::NormalizeOrientation(angle);
	std::list<Player*> playersNearby;
	center->GetPlayerListInGrid(playersNearby, center->GetObjectSize() + 80.0f);
	for (Player* player : playersNearby)
	{
		if (!player->IsPlayerBot() || !player->IsInWorld() || player->GetMap() != center->GetMap() || !player->IsAlive())
			continue;
		float fleeRange = center->GetDistance(player->GetPosition());
		if (fleeRange <= 0)
			fleeRange = center->GetObjectSize() + 1.0f;
		float pangle = center->GetAngle(&player->GetPosition()) - angle;
		if (pangle >= 0 && pangle <= float(M_PI_4))
		{
			if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
			{
				float fleeAngle = Position::NormalizeOrientation(angle + float(M_PI_4));
				Position fleePos = Position(center->GetPositionX() + fleeRange * std::cosf(fleeAngle),
					center->GetPositionY() + fleeRange * std::sinf(fleeAngle), player->GetPositionZ(), player->GetOrientation());
				fleePos.m_positionZ = player->GetMap()->GetHeight(player->GetPhaseMask(), fleePos.GetPositionX(), fleePos.GetPositionY(), fleePos.GetPositionZ());
				//if (pGroupAI->GetAIPayer()->HasUnitState(UNIT_STATE_CASTING))
				//	pGroupAI->GetAIPayer()->CastStop();
				if (force)
					pGroupAI->ClearCruxMovement();
				pGroupAI->SetCruxMovement(fleePos);
			}
		}
		else if (pangle < 0 && pangle >= float(-M_PI_4))
		{
			if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
			{
				float fleeAngle = Position::NormalizeOrientation(angle - float(M_PI_4));
				Position fleePos = Position(center->GetPositionX() + fleeRange * std::cosf(fleeAngle),
					center->GetPositionY() + fleeRange * std::sinf(fleeAngle), player->GetPositionZ(), player->GetOrientation());
				fleePos.m_positionZ = player->GetMap()->GetHeight(player->GetPhaseMask(), fleePos.GetPositionX(), fleePos.GetPositionY(), fleePos.GetPositionZ());
				//if (pGroupAI->GetAIPayer()->HasUnitState(UNIT_STATE_CASTING))
				//	pGroupAI->GetAIPayer()->CastStop();
				if (force)
					pGroupAI->ClearCruxMovement();
				pGroupAI->SetCruxMovement(fleePos);
			}
		}
	}
}

void ScriptedAI::BotSwitchPullTarget(Unit* pTarget)
{
	if (!pTarget || !pTarget->ToCreature())
		return;
	Player* pTargetPlayer = ObjectAccessor::FindPlayer(pTarget->GetTarget());
	if (pTargetPlayer && pTargetPlayer->IsAlive())
	{
		pTargetPlayer->SetSelection(ObjectGuid::Empty);
		if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(pTargetPlayer->GetAI()))
			pGroupAI->ClearTankTarget();
	}
	std::list<Player*> playersNearby;
	pTarget->GetPlayerListInGrid(playersNearby, pTarget->GetObjectSize() + 80.0f);
	for (Player* player : playersNearby)
	{
		if (player && player == pTargetPlayer)
			continue;
		if (!player->IsPlayerBot() || !player->IsInWorld() || player->GetMap() != pTarget->GetMap() || !player->IsAlive())
			continue;
		if (BotGroupAI* pGroupAI = dynamic_cast<BotGroupAI*>(player->GetAI()))
		{
			if (pGroupAI->IsTankBotAI())
			{
				if (pGroupAI->ProcessPullSpell(pTarget))
					return;
			}
		}
	}
}

void ScriptedAI::BotVehicleChaseTarget(Unit* pTarget, float distance)
{
	if (!pTarget || !pTarget->ToCreature())
		return;
	std::list<Player*> playersNearby;
	me->GetPlayerListInGrid(playersNearby, distance * 2);
	if (playersNearby.empty())
		return;
	for (Player* bot : playersNearby)
	{
		if (!bot->IsPlayerBot() || !bot->IsAlive() || bot->GetMap() != pTarget->GetMap())
			continue;
		if (bot->GetTarget() != pTarget->GetGUID())
			bot->SetSelection(pTarget->GetGUID());
		if (BotGroupAI* pBotAI = dynamic_cast<BotGroupAI*>(bot->GetAI()))
		{
			pBotAI->SetVehicle3DNextMoveGap(8.0f);
			pBotAI->SetVehicle3DMoveTarget(pTarget, distance);
		}
		Unit* vehBase = bot->GetCharm();
		if (!vehBase || !vehBase->IsAlive() || vehBase->GetMap() != pTarget->GetMap())
			continue;
		if (vehBase->GetTarget() != pTarget->GetGUID())
			vehBase->SetTarget(pTarget->GetGUID());
		if (vehBase->HasSpell(57092) && !vehBase->HasAura(57092))
			vehBase->CastSpell(vehBase, 57092);
		float power = (float)vehBase->GetPower(POWER_ENERGY) / (float)vehBase->GetMaxPower(POWER_ENERGY);
		if (power >= 0.4f)
		{
			uint8 combo = bot->GetComboPoints();
			if (combo > 4)
			{
				if (vehBase->GetHealthPct() < 75 && urand(0, 99) > 60)
				{
					if (vehBase->HasSpell(57108) && !vehBase->HasAura(57108) && urand(0, 99) > 60)
						vehBase->CastSpell(vehBase, 57108);
					else if (vehBase->HasSpell(57143))
						vehBase->CastSpell(vehBase, 57143);
				}
				else if (vehBase->HasSpell(56092))
					vehBase->CastSpell(pTarget, 56092);
			}
			else
			{
				if (vehBase->GetHealthPct() < 75 && vehBase->HasSpell(57090) && urand(0, 99) > 60)
					vehBase->CastSpell(vehBase, 57090);
				else if (vehBase->HasSpell(56091))
					vehBase->CastSpell(pTarget, 56091);
			}
		}
	}
}

void ScriptedAI::BotUseGOTarget(GameObject* pGO)
{
	if (!pGO)
		return;
	std::list<Player*> playersNearby;
	me->GetPlayerListInGrid(playersNearby, 100);
	if (playersNearby.empty())
		return;
	ObjectGuid goGUID = pGO->GetGUID();
	std::map<float, Player*> distPlayers;
	for (Player* bot : playersNearby)
	{
		if (!bot->IsPlayerBot() || !bot->IsAlive() || bot->GetMap() != pGO->GetMap())
			continue;
		if (bot->HasUnitState(UNIT_STATE_CASTING))
			continue;
		distPlayers[bot->GetDistance(pGO->GetPosition())] = bot;
	}
	for (std::map<float, Player*>::iterator itDist = distPlayers.begin();
		itDist != distPlayers.end();
		itDist++)
	{
		Player* bot = itDist->second;
		if (BotGroupAI* pAI = dynamic_cast<BotGroupAI*>(bot->GetAI()))
		{
			if (pAI->SetMovetoUseGOTarget(goGUID))
				return;
		}
	}
}
