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
SDName: Boss_Loatheb
SD%Complete: 99
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define SPELL_DEATHBLOOM            29865
#define H_SPELL_DEATHBLOOM          55053
#define SPELL_NECROTIC_AURA         55593
#define SPELL_INEVITABLE_DOOM       29204
#define H_SPELL_INEVITABLE_DOOM     55052
#define SPELL_ENRAGE                26662
//spore
#define SPELL_FUNGAL_CREEP			29232
#define SPELL_SUMM_SPORE			29234

#define HEAL_AURA					-1000000

struct MANGOS_DLL_DECL boss_loathebAI : public ScriptedAI
{
    boss_loathebAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		IsHeroicMode = pCreature->GetMap()->IsHeroic();
		Reset();
	}

    uint32 DeathBloom_Timer;
    uint32 Enrage_Timer;
    uint32 InevitableDoom_Timer;
    uint32 Necrotic_Timer;
    uint32 Summon_Timer;
	bool HasYelled;
	 bool IsHeroicMode;
	 ScriptedInstance* pInstance;

    void Reset()
    {
        DeathBloom_Timer = 5000;
        InevitableDoom_Timer = 120000;
		Necrotic_Timer = 10000;
        Summon_Timer = 8000;
		Enrage_Timer = 7*60*1000;
		HasYelled = false;
		if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_PLAG_LOAT_DOOR)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
		pInstance->SetData(DATA_LOATHEB_EVENT,NOT_STARTED);

    }
	void JustDied(Unit*)
	{
		if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_PLAG_LOAT_DOOR)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
		pInstance->SetData(DATA_LOATHEB_EVENT,DONE);

	}
	void Aggro(Unit*)
	 {
		 if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_PLAG_LOAT_DOOR)))
			pDoor->SetGoState(GO_STATE_READY);
		 pInstance->SetData(DATA_LOATHEB_EVENT,IN_PROGRESS);
	 }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if (DeathBloom_Timer < diff)
        {
			Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0);
			if(target)
				DoCast(target,IsHeroicMode ? H_SPELL_DEATHBLOOM : SPELL_DEATHBLOOM);
            DeathBloom_Timer = 15000;
        }else DeathBloom_Timer -= diff;

		if(Necrotic_Timer < 6000 && !HasYelled)
		{
			DoScriptText(HEAL_AURA,m_creature);
			HasYelled = true;
		}
        if (Necrotic_Timer < diff)
        {
            DoCast(m_creature,SPELL_NECROTIC_AURA);
			HasYelled = false;
            Necrotic_Timer = 20000;
        }else Necrotic_Timer -= diff;

        if (InevitableDoom_Timer < diff)
        {
			DoCast(m_creature,IsHeroicMode ? H_SPELL_INEVITABLE_DOOM : SPELL_INEVITABLE_DOOM);
            InevitableDoom_Timer = 30000;
        }else InevitableDoom_Timer -= diff;

        if (Enrage_Timer < diff)
        {
            DoCast(m_creature,SPELL_ENRAGE);
            Enrage_Timer = 30000;
        }else Enrage_Timer -= diff;

        //Summon_Timer
        if (Summon_Timer < diff)
        {
            DoCast(m_creature,SPELL_SUMM_SPORE);
            Summon_Timer = 28000+rand()%4000;
        } else Summon_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
struct MANGOS_DLL_DECL mob_sporeAI : public ScriptedAI
{
    mob_sporeAI(Creature* pCreature) : ScriptedAI(pCreature){Reset();}
    void Reset(){}
	void Aggro(Unit* who){}
	void DamageTaken(Unit* done_bye,uint32 &damage)
	{
		if(damage < m_creature->GetHealth())
			return;

		DoCast(m_creature,SPELL_FUNGAL_CREEP,true);
	}
};

CreatureAI* GetAI_boss_loatheb(Creature* pCreature)
{
    return new boss_loathebAI(pCreature);
}
CreatureAI* GetAI_mob_spore(Creature* pCreature)
{
    return new mob_sporeAI(pCreature);
}

void AddSC_boss_loatheb()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_loatheb";
    newscript->GetAI = &GetAI_boss_loatheb;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_spore";
    newscript->GetAI = &GetAI_mob_spore;
    newscript->RegisterSelf();
}
