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
SDName: Boss_Noth
SD%Complete: 99
SDComment: ADD ai acidbol ha lehet...
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

enum
{
    SAY_AGGRO1                          = -1533075,
    SAY_AGGRO2                          = -1533076,
    SAY_AGGRO3                          = -1533077,
    SAY_SUMMON                          = -1533078,
    SAY_SLAY1                           = -1533079,
    SAY_SLAY2                           = -1533080,
    SAY_DEATH                           = -1533081,

    SPELL_BLINK                         = 29211,            //29208, 29209 and 29210 too
    SPELL_CRIPPLE                       = 29212,
    SPELL_CRIPPLE_H                     = 54814,
    SPELL_CURSE_PLAGUEBRINGER           = 29213,
    SPELL_CURSE_PLAGUEBRINGER_H         = 54835,
	SPELL_ENRAGE						= 47008,

    NPC_PLAGUED_CONS					= 16982,
	NPC_PLAGUED_CHAMPION				= 16983,
	NPC_PLAGUED_GUARDIAN				= 16981,

    NPC_PLAGUED_WARRIOR                 = 16984

};
// Teleport position of Noth on his balcony
float TelePos[2][4] = 
{
	//balcony
	{2631.370,-3529.680,274.040,6.277},
	//ground
	{2685,-3502,261,6.277}
};
float SpawnLocs[3][3] =
{
	{2704.60,-3459,263.89},
	{2663.22,-3460.11,265.00},
	{2727.21,-3514.78,264.09}
};
uint32 PhaseTimers[2] =
{
	{110000},
	{70000}

};
uint32 Spawns[3][3] =
{
	{NPC_PLAGUED_CHAMPION,NPC_PLAGUED_CHAMPION,NPC_PLAGUED_CHAMPION},
	{NPC_PLAGUED_CHAMPION,NPC_PLAGUED_GUARDIAN,NPC_PLAGUED_GUARDIAN},
	{NPC_PLAGUED_GUARDIAN,NPC_PLAGUED_GUARDIAN,NPC_PLAGUED_GUARDIAN}
};

// IMPORTANT: BALCONY TELEPORT NOT ADDED YET! WILL BE ADDED SOON!
// Dev note 26.12.2008: When is soon? :)

struct MANGOS_DLL_DECL boss_nothAI : public ScriptedAI
{
    boss_nothAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroicMode = pCreature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;

    uint32 Blink_Timer;
    uint32 Curse_Timer;
    uint32 Summon_Timer;
	int Summon_Num;
	uint32 phase;
	uint32 PhaseTimer;
	uint32 EnrageTimer;

    void Reset()
    {
        Blink_Timer = 25000;
        Curse_Timer = 4000;
        Summon_Timer = 12000;
		EnrageTimer = 470000;
		PhaseTimer = PhaseTimers[0];
		Summon_Num = m_bIsHeroicMode ? 3 : 2;
		if(GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_PLAG_NOTH_EXIT_DOOR)))
			pDoor->SetGoState(GO_STATE_READY);
		if(GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_PLAG_NOTH_ENTRY_DOOR)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
    }

    void Aggro(Unit *who)
    {
        switch (rand()%3)
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }
		if(GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_PLAG_NOTH_EXIT_DOOR)))
			pDoor->SetGoState(GO_STATE_READY);
		if(GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_PLAG_NOTH_ENTRY_DOOR)))
			pDoor->SetGoState(GO_STATE_READY);
    }

    void JustSummoned(Creature* summoned)
    {
        if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
            summoned->AddThreat(target,0.0f);
    }

    void KilledUnit(Unit* victim)
    {
        switch (rand()%2)
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
        }
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, m_creature);
		if(GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_PLAG_NOTH_EXIT_DOOR)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
		if(GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_PLAG_NOTH_ENTRY_DOOR)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

		if(EnrageTimer < diff)
		{
			DoCast(m_creature,SPELL_ENRAGE);
			EnrageTimer = 300000;
		}else EnrageTimer -= diff;
		switch(phase)
		{
		case 0:
        //Blink_Timer
        if (m_bIsHeroicMode && Blink_Timer < diff)
        {
            DoCast(m_creature,SPELL_CRIPPLE);
            DoCast(m_creature,SPELL_BLINK);

            Blink_Timer = 25000;
        }else Blink_Timer -= diff;

        //Curse_Timer
        if (Curse_Timer < diff)
        {
			DoCast(m_creature,m_bIsHeroicMode ? SPELL_CURSE_PLAGUEBRINGER_H : SPELL_CURSE_PLAGUEBRINGER);
            Curse_Timer = 28000;
        }else Curse_Timer -= diff;

        //Summon_Timer
        if (Summon_Timer < diff)
        {
            DoScriptText(SAY_SUMMON, m_creature);

            for(uint8 i = 0; i < Summon_Num; i++)
                Creature* summoned = m_creature->SummonCreature(NPC_PLAGUED_WARRIOR,SpawnLocs[i][0],SpawnLocs[i][1],SpawnLocs[i][2],0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000);

            Summon_Timer = 30000;
        } else Summon_Timer -= diff;
		DoMeleeAttackIfReady();
		break;
		case 1:
			if (Summon_Timer < diff)
			{
            DoScriptText(SAY_SUMMON, m_creature);

            for(uint8 i = 0; i < Summon_Num; i++)
			{
                Creature* summoned = m_creature->SummonCreature(NPC_PLAGUED_WARRIOR,SpawnLocs[i][0],SpawnLocs[i][1],SpawnLocs[i][2],0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000);
				m_creature->SummonCreature(NPC_PLAGUED_CONS,summoned->GetPositionX(),summoned->GetPositionY(),summoned->GetPositionZ(),0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000);
			}

            Summon_Timer = 80000;
        } else Summon_Timer -= diff;
		break;
		}
		if(PhaseTimer < diff)
		{
			m_creature->GetMotionMaster()->MovementExpired();
			m_creature->Relocate(TelePos[phase][0],TelePos[phase][1],TelePos[phase][2],TelePos[phase][3]);
			if(phase == 0)
			{
				m_creature->GetMotionMaster()->MoveIdle();
				m_creature->StopMoving();
			}else{
			SetCombatMovement(true);
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
			}
			Summon_Timer = 5000;
			phase = (phase == 1) ? 0 : 1;
			PhaseTimer = PhaseTimers[phase];
		}else PhaseTimer -= diff;
    }
};

CreatureAI* GetAI_boss_noth(Creature* pCreature)
{
    return new boss_nothAI(pCreature);
}

void AddSC_boss_noth()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_noth";
    newscript->GetAI = &GetAI_boss_noth;
    newscript->RegisterSelf();
}
