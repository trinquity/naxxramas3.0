/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Instance_Naxxramas
SD%Complete: 10
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define ENCOUNTERS              15

struct MANGOS_DLL_DECL instance_naxxramas : public ScriptedInstance
{
    instance_naxxramas(Map* pMap) : ScriptedInstance(pMap) {Initialize();}

    std::string strInstData;
    uint32 m_uiEncounter[ENCOUNTERS];

	uint64 GluthGUID;

	uint64 ThaddiusGUID;
	uint64 FeugenGUID;
	uint64 StalaggGUID;

	uint64 KelthuzadGUID;
	uint64 FaerlinaGUID;

	uint64 MaexxnaGUID;

	uint64 horseman_ThaneGUID;
	uint64 horseman_BaronGUID;
	uint64 horseman_ZeilekGUID;
	uint64 horseman_BlaumeuxGUID;

	uint64 GothicGateGUID;
	uint64 GoticEGateGUID;
	uint64 GoticIGateGUID;
	uint64 NothEntryGateGUID;
	uint64 NothExitGateGUID;
	uint64 HorseManChestGUID;
	uint64 HorseManDoorGUID;
	uint64 LoathebDoorGUID;
	uint64 PatchwerkExitDoorGUID;
	uint64 GluthExitDoorGUID;

    void Initialize()
    {
        for(uint8 i = 0; i < ENCOUNTERS; i++)
            m_uiEncounter[i] = NOT_STARTED;

    }

    void OnCreatureCreate(Creature* pCreature)
    {
		switch(pCreature->GetEntry())
		{
		case NPC_ZELIEK: horseman_ZeilekGUID = pCreature->GetGUID();break;
		case NPC_RIVENDARE: horseman_BaronGUID = pCreature->GetGUID();break;
		case NPC_THANE: horseman_ThaneGUID = pCreature->GetGUID();break;
		case NPC_BLAUMEUX: horseman_BlaumeuxGUID = pCreature->GetGUID();break;
		case NPC_KELTHUZAD: KelthuzadGUID = pCreature->GetGUID();break;
		case NPC_GLUTH: GluthGUID = pCreature->GetGUID();break;
		case NPC_FEUGEN: FeugenGUID = pCreature->GetGUID();break;
		case NPC_STALAGG: StalaggGUID = pCreature->GetGUID();break;
		case NPC_THADDIUS: ThaddiusGUID = pCreature->GetGUID();break;
		case NPC_FAERLINA: FaerlinaGUID = pCreature->GetGUID();break;
		}

    }

    void OnObjectCreate(GameObject* pGo)
    {
		switch(pGo->GetEntry())
		{
		case GO_MILI_GOTH_COMBAT_GATE: GothicGateGUID = pGo->GetGUID();break;
		case GO_MILI_GOTH_EXIT_GATE: GoticEGateGUID = pGo->GetGUID();break;
		case GO_MILI_GOTH_ENTRY_GATE: GoticIGateGUID = pGo->GetGUID();break;
		case GO_CHEST_HORSEMEN_NORM:
		case GO_CHEST_HORSEMEN_HERO: HorseManChestGUID = pGo->GetGUID();break;
		case GO_MILI_HORSEMEN_DOOR: HorseManDoorGUID = pGo->GetGUID();break;
		case GO_PLAG_NOTH_ENTRY_DOOR: NothEntryGateGUID= pGo->GetGUID();break;
		case GO_PLAG_NOTH_EXIT_DOOR: NothExitGateGUID = pGo->GetGUID();break;
		case GO_PLAG_LOAT_DOOR: LoathebDoorGUID = pGo->GetGUID();break;
		case GO_CONS_PATH_EXIT_DOOR: PatchwerkExitDoorGUID = pGo->GetGUID();break;
		case GO_CONS_GLUT_EXIT_DOOR: GluthExitDoorGUID = pGo->GetGUID();break;
		}
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
		if(uiType < ENCOUNTERS)
			m_uiEncounter[uiType] = uiData;
    }

    uint32 GetData(uint32 uiType)
    {
		if(uiType <ENCOUNTERS)
			return m_uiEncounter[uiType];
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
		switch(uiData)
		{
		case NPC_FAERLINA: return FaerlinaGUID;
		case NPC_ZELIEK:  return horseman_ZeilekGUID;
		case NPC_RIVENDARE: return horseman_BaronGUID;
		case NPC_THANE: return horseman_ThaneGUID;
		case NPC_BLAUMEUX: return horseman_BlaumeuxGUID;
		case NPC_FEUGEN: return FeugenGUID;
		case NPC_STALAGG: return StalaggGUID;
		case NPC_THADDIUS: return ThaddiusGUID;

		case GO_MILI_GOTH_COMBAT_GATE: return GothicGateGUID;
		case GO_MILI_GOTH_ENTRY_GATE: return GoticIGateGUID;
		case GO_MILI_GOTH_EXIT_GATE: return GoticEGateGUID;
		case GO_PLAG_NOTH_ENTRY_DOOR: return NothEntryGateGUID;
		case GO_PLAG_NOTH_EXIT_DOOR: return NothExitGateGUID;
		case GO_CHEST_HORSEMEN_NORM:
		case GO_CHEST_HORSEMEN_HERO:
			return HorseManChestGUID;
		case GO_MILI_HORSEMEN_DOOR: return HorseManDoorGUID;
		case GO_PLAG_LOAT_DOOR:return LoathebDoorGUID;
		case GO_CONS_PATH_EXIT_DOOR: return PatchwerkExitDoorGUID;
		case GO_CONS_GLUT_EXIT_DOOR: return GluthExitDoorGUID;
		}
        return 0;
    }
};

InstanceData* GetInstanceData_instance_naxxramas(Map* pMap)
{
    return new instance_naxxramas(pMap);
}

void AddSC_instance_naxxramas()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "instance_naxxramas";
    pNewScript->GetInstanceData = &GetInstanceData_instance_naxxramas;
    pNewScript->RegisterSelf();
}
