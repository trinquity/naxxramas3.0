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
SDName: Boss_Grobbulus
SD%Complete: 0
SDComment: Place holder
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define SPELL_SMILE_SPRAY 28157
#define H_SPELL_SMILE_SPRAY 54364
#define SPELL_POISON_CLOUD 28240
#define SPELL_MUTATING_INJECTION 28169 //this spell triggers 28206 when expried/dispelled
#define SPELL_MUTAGEN 28206

#define SPELL_ENRAGE 26527

struct MANGOS_DLL_DECL boss_grobbulusAI : public ScriptedAI
{
    boss_grobbulusAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		IsHeroicMode = pCreature->GetMap()->IsHeroic();
		Reset();
	}

    uint32 PoisonCloud_Timer;
    uint32 MutatingInjection_Timer;
    uint32 SmileSpray_Timer;
	uint32 Enrage_Timer;
	Unit* mutating_target;
	bool IsHeroicMode;
	ScriptedInstance* pInstance;

    void Reset()
    {
        PoisonCloud_Timer = 15000;
        MutatingInjection_Timer = 20000;
		SmileSpray_Timer = 10000;
		mutating_target = NULL;
		Enrage_Timer = IsHeroicMode ? 12*60*1000 : 9*60*1000 ;
		if(pInstance->GetData(DATA_PATCHWERK_EVENT) == DONE)
			if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_CONS_PATH_EXIT_DOOR)))
				pDoor->SetGoState(GO_STATE_ACTIVE);
		pInstance->SetData(DATA_GROBBULUS_EVENT,NOT_STARTED);

    }
	void JustDied(Unit*)
	{
		if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_CONS_PATH_EXIT_DOOR)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
		pInstance->SetData(DATA_GROBBULUS_EVENT,DONE);

	}
	void Aggro(Unit*)
	 {
		if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_CONS_PATH_EXIT_DOOR)))
			pDoor->SetGoState(GO_STATE_READY);
		 pInstance->SetData(DATA_GROBBULUS_EVENT,IN_PROGRESS);
	 }	
    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if (SmileSpray_Timer < diff)
        {
			//need fix - doesn't summon creature!
			DoCast(m_creature,IsHeroicMode ? H_SPELL_SMILE_SPRAY : SPELL_SMILE_SPRAY);
            SmileSpray_Timer = 12000;
        }else SmileSpray_Timer -= diff;

		//workarund.
		if(mutating_target && !mutating_target->HasAura(SPELL_MUTATING_INJECTION,0))
		{
			DoCast(mutating_target,SPELL_MUTAGEN);
			mutating_target = NULL;
		}
        if (MutatingInjection_Timer < diff)
        {
			mutating_target = SelectUnit(SELECT_TARGET_RANDOM,0);
            DoCast(mutating_target,SPELL_MUTATING_INJECTION);
            MutatingInjection_Timer = 20000;
        }else MutatingInjection_Timer -= diff;

        if (PoisonCloud_Timer < diff)
        {
			DoCast(m_creature,SPELL_POISON_CLOUD);
            PoisonCloud_Timer = 15000;
        }else PoisonCloud_Timer -= diff;

        if (Enrage_Timer < diff)
        {
            DoCast(m_creature,SPELL_ENRAGE);
            Enrage_Timer = 30000;
        }else Enrage_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_grobbulus(Creature* pCreature)
{
    return new boss_grobbulusAI(pCreature);
}
void AddSC_boss_grobbulus()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_grobbulus";
    newscript->GetAI = &GetAI_boss_grobbulus;
    newscript->RegisterSelf();
}