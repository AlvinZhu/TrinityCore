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

// This is where scripts' loading functions should be declared:

void AddSC_pbotaccount_commandscript();
void AddSC_pbotai_commandscript();

// Transmogrification
void AddSC_PWS_Transmogrification();
void AddSC_CS_Transmogrification();

// Reforging
void AddSC_REFORGER_NPC();

// Enchant NPC
void AddSC_npc_enchantment();

void AddSC_death_knight_bot();
void AddSC_druid_bot();
void AddSC_hunter_bot();
void AddSC_mage_bot();
void AddSC_paladin_bot();
void AddSC_priest_bot();
void AddSC_rogue_bot();
void AddSC_shaman_bot();
void AddSC_warlock_bot();
void AddSC_warrior_bot();
void AddSC_script_bot_commands();

// The name of this function should match:
// void Add${NameOfDirectory}Scripts()
void AddCustomScripts()
{
	AddSC_pbotaccount_commandscript();
	AddSC_pbotai_commandscript();

	// Transmogrification
	AddSC_PWS_Transmogrification();
	AddSC_CS_Transmogrification();

	// Reforging
	AddSC_REFORGER_NPC();

	// Enchant NPC
	AddSC_npc_enchantment();

	AddSC_death_knight_bot();
	AddSC_druid_bot();
	AddSC_hunter_bot();
	AddSC_mage_bot();
	AddSC_paladin_bot();
	AddSC_priest_bot();
	AddSC_rogue_bot();
	AddSC_shaman_bot();
	AddSC_warlock_bot();
	AddSC_warrior_bot();
	AddSC_script_bot_commands();
}
