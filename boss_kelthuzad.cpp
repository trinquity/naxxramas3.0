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
SDName: Boss_KelThuzud
SD%Complete: 99
SDComment: whoo majd ne feleljtsem el a forciklust lecserelni 4mp-nkenti summra a guardiannal
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

//when shappiron dies. dialog between kel and lich king (in this order)
#define SAY_SAPP_DIALOG1            -1533084
#define SAY_SAPP_DIALOG2_LICH       -1533085
#define SAY_SAPP_DIALOG3            -1533086
#define SAY_SAPP_DIALOG4_LICH       -1533087
#define SAY_SAPP_DIALOG5            -1533088

//when cat dies
#define SAY_CAT_DIED                -1533089

//when each of the 4 wing bosses dies
#define SAY_TAUNT1                  -1533090
#define SAY_TAUNT2                  -1533091
#define SAY_TAUNT3                  -1533092
#define SAY_TAUNT4                  -1533093

#define SAY_SUMMON_MINIONS          -1533105                //start of phase 1

#define SAY_AGGRO1                  -1533094                //start of phase 2
#define SAY_AGGRO2                  -1533095
#define SAY_AGGRO3                  -1533096

#define SAY_SLAY1                   -1533097
#define SAY_SLAY2                   -1533098

#define SAY_DEATH                   -1533099

#define SAY_CHAIN1                  -1533100
#define SAY_CHAIN2                  -1533101
#define SAY_FROST_BLAST             -1533102

#define SAY_REQUEST_AID             -1533103                //start of phase 3
#define SAY_ANSWER_REQUEST          -1533104                //lich king answer

#define SAY_SPECIAL1_MANA_DET       -1533106
#define SAY_SPECIAL3_MANA_DET       -1533107
#define SAY_SPECIAL2_DISPELL        -1533108

#define NPC_SOLDIER 16427
#define NPC_ABOMINATION 16428
#define NPC_WEAVER 16429

#define NPC_GUARDIAN 16441
struct SpawnLoc
{
	float x,y,z;
};
struct Spawn
{
	uint32 Entry,time;
};
Spawn AddTimers[] = 
{
	{NPC_SOLDIER,3200},
	{NPC_ABOMINATION,28000},
	{NPC_WEAVER,28000}
};
SpawnLoc Center[] = 
{
	{3716.48,-5106.77,141.29}
};
SpawnLoc AddSpawnLoc[] =
{
	{3744.87,-5149.07,143.17},
	{3727.09,-5054.34,143.19},
	{3758.60,-5078.39,143.17},
	{3705.97,-5157.78,143.17},
	{3673.85,-5134.24,143.17},
	{3663.89,-5095.40,143.22},
	{3688.13,-5064.07,143.17}
};


//spells to be casted
#define SPELL_FROST_BOLT            28478
#define H_SPELL_FROST_BOLT          55802
#define SPELL_FROST_BOLT_NOVA       28479
#define H_SPELL_FROST_BOLT_NOVA     55807

#define SPELL_CHAINS_OF_KELTHUZAD   28410                   //casted spell should be 28408. Also as of 303, heroic only
#define SPELL_MANA_DETONATION       27819
#define SPELL_SHADOW_FISURE         27810
#define SPELL_FROST_BLAST           27808

#define SPELL_VOID_BLAST 27812 //shadow fisure

struct MANGOS_DLL_DECL boss_kelthuzadAI : public ScriptedAI
{
    boss_kelthuzadAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		isHeroicMode = pCreature->GetMap()->IsHeroic();
		SpellEntry *FrostBlast = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_FROST_BLAST);
		if(FrostBlast)
			FrostBlast->EffectTriggerSpell[1] = 29879;
        Reset();
    }
	bool isHeroicMode;
	ScriptedInstance* pInstance;
    uint64 GuardiansOfIcecrown[4];
    uint32 GuardiansOfIcecrown_Count;
    uint32 FrostBolt_Timer;
    uint32 FrostBoltNova_Timer;
    uint32 ChainsOfKelthuzad_Timer;
    uint32 ManaDetonation_Timer;
    uint32 ShadowFisure_Timer;
    uint32 FrostBlast_Timer;

    uint32 Phase1_Timer;
	uint32 Phase1SummTimer[3];
	uint32 phase;

    void Reset()
    {
        FrostBolt_Timer = (rand()%60)*1000;                 //It won't be more than a minute without cast it
        FrostBoltNova_Timer = 15000;                        //Cast every 15 seconds
        ChainsOfKelthuzad_Timer = (rand()%30+30)*1000;      //Cast no sooner than once every 30 seconds
        ManaDetonation_Timer = 20000;                       //Seems to cast about every 20 seconds
        ShadowFisure_Timer = 25000;                         //25 seconds
        FrostBlast_Timer = (rand()%30+30)*1000;             //Random time between 30-60 seconds
		GuardiansOfIcecrown_Count = isHeroicMode ? 4 : 2;
		SetCombatMovement(false);
        for(int i=0; i<GuardiansOfIcecrown_Count; i++)
        {
            if (GuardiansOfIcecrown[i])
            {
                //delete creature
                Unit* pUnit = Unit::GetUnit((*m_creature), GuardiansOfIcecrown[i]);
                if (pUnit && pUnit->isAlive())
                    pUnit->DealDamage(pUnit, pUnit->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

                GuardiansOfIcecrown[i] = 0;
            }
        }

        Phase1_Timer = 228000;                             //Phase 1 lasts 3 48
		for(int i = 0;i<3;i++)
			Phase1SummTimer[i] = AddTimers[i].time;

		phase = 1;
		if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_KELTHUZAD_DOOR)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
		pInstance->SetData(DATA_KELTHUZAD_EVENT,NOT_STARTED);
    }

    void KilledUnit()
    {
        if (rand()%2)
            DoScriptText(SAY_SLAY1, m_creature);
        else
            DoScriptText(SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, m_creature);
		if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_KELTHUZAD_DOOR)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
		pInstance->SetData(DATA_KELTHUZAD_EVENT,DONE);
    }

    void Aggro(Unit* who)
    {
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }
		m_creature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
		m_creature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_KELTHUZAD_DOOR)))
			pDoor->SetGoState(GO_STATE_READY);
		 pInstance->SetData(DATA_KELTHUZAD_EVENT,IN_PROGRESS);
    }
	void JustSummoned(Creature *p)
	{
		switch(p->GetEntry())
		{
		case NPC_SOLDIER:
		case NPC_WEAVER:
		case NPC_ABOMINATION:
		case NPC_GUARDIAN:
		if(Unit* pTemp = SelectUnit(SELECT_TARGET_RANDOM,0))
			p->AddThreat(pTemp,0.0f);
		break;
		default:
		break;
		}
	}
    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		if(phase == 1)
		{
			if(m_creature->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE))
			{
				m_creature->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
				m_creature->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
			}
			if(m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != IDLE_MOTION_TYPE)
			{
				m_creature->GetMotionMaster()->MovementExpired();
				m_creature->GetMotionMaster()->MoveIdle();
			}

			for(int i = 0;i<3;i++)
			{
				if(Phase1SummTimer[i] < diff)
				{
					int j = rand()%7;
					m_creature->SummonCreature(AddTimers[i].Entry,AddSpawnLoc[j].x,AddSpawnLoc[j].y,AddSpawnLoc[j].z,0,TEMPSUMMON_DEAD_DESPAWN,900000);
					Phase1SummTimer[i] = AddTimers[i].time;
				}else Phase1SummTimer[i] -= diff;
			}
			if(Phase1_Timer < diff)
			{
				phase = 2;
			}else Phase1_Timer -= diff;
		}else if(phase == 2 || phase == 3)
		{
			if(m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
			{
				SetCombatMovement(true);
				m_creature->GetMotionMaster()->MovementExpired();
				m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
			}
        //Check for Frost Bolt
        if (FrostBolt_Timer < diff)
        {
			DoCast(m_creature->getVictim(),isHeroicMode ? H_SPELL_FROST_BOLT : SPELL_FROST_BOLT);
            FrostBolt_Timer = (rand()%60)*1000;
        }else FrostBolt_Timer -= diff;

        //Check for Frost Bolt Nova
        if (FrostBoltNova_Timer < diff)
        {
			DoCast(m_creature->getVictim(),isHeroicMode ? H_SPELL_FROST_BOLT_NOVA : SPELL_FROST_BOLT_NOVA);
            FrostBoltNova_Timer = 15000;
        }else FrostBoltNova_Timer -= diff;

        //Check for Chains Of Kelthuzad
		if (ChainsOfKelthuzad_Timer < diff && isHeroicMode)
        {
            //DoCast(m_creature->getVictim(),SPELL_CHAINS_OF_KELTHUZAD);

            //if (rand()%2)
                //DoScriptText(SAY_CHAIN1, m_creature);
            //else
                //DoScriptText(SAY_CHAIN2, m_creature);

            ChainsOfKelthuzad_Timer = (rand()%30+30)*1000;
        }else ChainsOfKelthuzad_Timer -= diff;

        //Check for Mana Detonation
        if (ManaDetonation_Timer < diff)
        {
				Unit *target = NULL;
				std::list<HostilReference *> t_list = m_creature->getThreatManager().getThreatList();
				std::vector<Unit *> target_list;
				for(std::list<HostilReference *>::iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
				{
					target = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());
					if (target->GetTypeId() != TYPEID_PLAYER)
						continue;

                //has mana
					if (target && target->GetPower(POWER_MANA) > 1 )
						target_list.push_back(target);

                target = NULL;
            }

            if (target_list.size())
                target = *(target_list.begin()+rand()%target_list.size());
            else
                target = NULL;
			if(target)
            DoCast(target,SPELL_MANA_DETONATION);
            ManaDetonation_Timer = 20000;
        }else ManaDetonation_Timer -= diff;

        //Check for Shadow Fissure
        if (ShadowFisure_Timer < diff)
        {
			if(Unit* pTemp = SelectUnit(SELECT_TARGET_RANDOM,0))
            DoCast(pTemp,SPELL_SHADOW_FISURE);

            ShadowFisure_Timer = 25000;
        }else ShadowFisure_Timer -= diff;

        //Check for Frost Blast
        if (FrostBlast_Timer < diff)
        {
			if(Unit* pTemp = SelectUnit(SELECT_TARGET_RANDOM,isHeroicMode ? 0 : 1))
				DoCast(pTemp,SPELL_FROST_BLAST);
            FrostBlast_Timer = (rand()%30+30)*1000;
        }else FrostBlast_Timer -= diff;
		}
		if(m_creature->GetHealth()*100 / m_creature->GetMaxHealth() < 45 && phase != 3)
		{
			phase = 3;
			for(int i = 0;i<GuardiansOfIcecrown_Count;i++)
			{
			Creature* pUnit = m_creature->SummonCreature(NPC_GUARDIAN,AddSpawnLoc[i].x,AddSpawnLoc[i].y,AddSpawnLoc[i].z,0,TEMPSUMMON_DEAD_DESPAWN,900000);
			GuardiansOfIcecrown[i] = pUnit->GetGUID();
			}
		}
		if(phase != 1)
			DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_kelthuzadAI(Creature* pCreature)
{
    return new boss_kelthuzadAI(pCreature);
}
#define SPELL_DARK_BLAST 28457 //soldier
#define H_SPELL_DARK_BLAST 55714 //soldier
#define SPELL_WAIL_OF_SOULS 28459 // weaver
#define H_SPELL_WAIL_OF_SOULS 55765 // weaver
#define SPELL_MORTAL_WOUND 28467 //abomination

struct MANGOS_DLL_DECL mob_kelthuzzad_addAI : public ScriptedAI
{
    mob_kelthuzzad_addAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		isHeroic = pCreature->GetMap()->IsHeroic();
		Reset();
	}
	bool isHeroic;
	uint32 check_timer;
	uint32 Entry;
    void Reset()
	{
		Entry = m_creature->GetEntry();
		check_timer = 1000;
	}
	void Aggro(Unit* who){}
	void JustDied(Unit*)
	{
		m_creature->RemoveCorpse();
	}
	void UpdateAI(const uint32 diff)
	{
		 if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		if(check_timer < diff)
		{
			switch(Entry)
			{
			case NPC_SOLDIER:
			{
				if(Player* p = GetPlayerAtMinimumRange(5.0f))
					DoCast(m_creature,isHeroic ? H_SPELL_DARK_BLAST : SPELL_DARK_BLAST);
			}
			break;
			case NPC_WEAVER:
			{
				if(m_creature->IsWithinDist(m_creature->getVictim(),10.0f,false))
					DoCast(m_creature->getVictim(),isHeroic ? H_SPELL_WAIL_OF_SOULS : SPELL_WAIL_OF_SOULS);
			}
			break;	
			case NPC_ABOMINATION:
			{
				DoCast(m_creature->getVictim(),SPELL_MORTAL_WOUND);
			}
			break;
			}
			check_timer = 1500;
		}else check_timer -= diff;
	}
	
};
CreatureAI* GetAI_mob_kelthuzzad_add(Creature* pCreature)
{
    return new mob_kelthuzzad_addAI(pCreature);
}
void AddSC_boss_kelthuzad()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_kelthuzad";
    newscript->GetAI = &GetAI_boss_kelthuzadAI;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_kelthuzad_add";
    newscript->GetAI = &GetAI_mob_kelthuzzad_add;
    newscript->RegisterSelf();
}
