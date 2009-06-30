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
SDName: Boss_Gothik
SD%Complete: 99
SDComment: Texts
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define ADD_DESPAWN_TIMER			600000
#define SAY_SPEECH                  -1533040
#define SAY_KILL                    -1533041
#define SAY_DEATH                   -1533042
#define SAY_TELEPORT                -1533043

//Gothik
#define SPELL_HARVESTSOUL           28679
#define SPELL_SHADOWBOLT            29317
#define H_SPELL_SHADOWBOLT          56405

//Unrelenting Trainee
#define SPELL_DEATH_PLAGUE         55604
#define H_SPELL_DEATH_PLAGUE		55645

//Unrelenting Deathknight
#define SPELL_SHADOW_MARK           27825

//Unrelenting Rider
#define SPELL_UNHOLY_AURA           55606
#define H_SPELL_UNHOLY_AURA         55608
#define SPELL_SHADOWBOLT_VOLLEY     27831                   //Search thru targets and find those who have the SHADOW_MARK to cast this on
#define H_SPELL_SHADOWBOLT_VOLLEY   55638

//Spectral Trainee
#define SPELL_ARCANE_EXPLOSION      27989
#define H_SPELL_ARCANE_EXPLOSION      56407

//Spectral Deathknight
#define SPELL_WHIRLWIND             28334

//Spectral Rider
#define SPELL_LIFEDRAIN             24300
#define H_SPELL_LIFEDRAIN			55646

#define UNHOLY_FRENZY				55648
#define H_UNHOLY_FRENZY				27995
//USES SAME UNHOLY AURA AS UNRELENTING RIDER

//Spectral Horse
#define SPELL_STOMP                 27993

#define NPC_TRAINEE 16124
#define NPC_DEATH_KNIGHT 16125
#define NPC_RIDER 16126

#define NPC_S_TRAINEE 16127
#define NPC_S_DEATH_KNIGHT 16148
#define NPC_S_RIDER 16150
#define NPC_S_HORSE 16149

struct SpawnLoc
{
	float x,y,z;
};
//normal weaves with timers
uint32 N_Weaves[20][5] =
{
	{0,0,0,0,30000},
	{NPC_TRAINEE,NPC_TRAINEE,0,0,20000},
	{NPC_TRAINEE,NPC_TRAINEE,0,0,20000},
	{NPC_TRAINEE,NPC_TRAINEE,0,0,10000},
	{NPC_DEATH_KNIGHT,0,0,0,10000},
	{NPC_TRAINEE,NPC_TRAINEE,0,0,15000},
	{NPC_DEATH_KNIGHT,0,0,0,10000},
	{NPC_TRAINEE,NPC_TRAINEE,0,0,15000},
	{NPC_DEATH_KNIGHT,NPC_TRAINEE,NPC_TRAINEE,0,10000},
	{NPC_RIDER,0,0,0,10000},
	{NPC_TRAINEE,NPC_TRAINEE,0,0,5000},
	{NPC_DEATH_KNIGHT,0,0,0,15000},
	{NPC_TRAINEE,NPC_TRAINEE,NPC_RIDER,0,10000},
	{NPC_DEATH_KNIGHT,NPC_DEATH_KNIGHT,0,0,10000},
	{NPC_TRAINEE,NPC_TRAINEE,0,0,10000},
	{NPC_RIDER,0,0,0,5000},
	{NPC_DEATH_KNIGHT,0,0,0,5000},
	{NPC_TRAINEE,NPC_TRAINEE,0,0,20000},
	{NPC_RIDER,NPC_DEATH_KNIGHT,NPC_TRAINEE,NPC_TRAINEE,15000},
	{NPC_TRAINEE,NPC_TRAINEE,0,0,30000}
};
//heroic weaves with timers
uint32 H_Weaves[19][7] =
{
	{0,0,0,0,0,0,30000},
	{NPC_TRAINEE,NPC_TRAINEE,NPC_TRAINEE,0,0,0,20000},	
	{NPC_TRAINEE,NPC_TRAINEE,NPC_TRAINEE,0,0,0,20000},
	{NPC_TRAINEE,NPC_TRAINEE,NPC_TRAINEE,0,0,0,10000},
	{NPC_DEATH_KNIGHT,NPC_DEATH_KNIGHT,0,0,0,0,10000},
	{NPC_TRAINEE,NPC_TRAINEE,NPC_TRAINEE,0,0,0,15000},
	{NPC_DEATH_KNIGHT,NPC_DEATH_KNIGHT,0,0,0,0,5000},
	{NPC_TRAINEE,NPC_TRAINEE,NPC_TRAINEE,0,0,0,20000},
	{NPC_TRAINEE,NPC_TRAINEE,NPC_TRAINEE,NPC_DEATH_KNIGHT,NPC_DEATH_KNIGHT,0,10000},
	{NPC_TRAINEE,NPC_TRAINEE,NPC_TRAINEE,0,0,0,10000},
	{NPC_RIDER,0,0,0,0,0,5000},
	{NPC_TRAINEE,NPC_TRAINEE,NPC_TRAINEE,0,0,0,15000},
	{NPC_RIDER,0,0,0,0,0,10000},
	{NPC_DEATH_KNIGHT,NPC_DEATH_KNIGHT,0,0,0,0,10000},
	{NPC_RIDER,0,0,0,0,0,10000},
	{NPC_RIDER,NPC_TRAINEE,NPC_TRAINEE,NPC_TRAINEE,0,0,5000},
	{NPC_DEATH_KNIGHT,NPC_TRAINEE,NPC_TRAINEE,NPC_TRAINEE,0,0,5000},
	{NPC_RIDER,NPC_TRAINEE,NPC_TRAINEE,NPC_TRAINEE,0,0,20000},
	{NPC_RIDER,NPC_DEATH_KNIGHT,NPC_DEATH_KNIGHT,NPC_TRAINEE,NPC_TRAINEE,NPC_TRAINEE,30000}
};
//living side
SpawnLoc LeftSide[] =
{
	{2714,-3429,269},
	{2692,-3429,269},
	{2669,-3429,269}
};
//ud side
SpawnLoc RightSide[] =
{
	{2665,-3340,269},
	{2682,-3305,269},
	{2700,-3322,269}
};
SpawnLoc GothikTele[] =
{
	//1. is a "living side"
	{2692,-3395,268},
	{2687,-3327,268}
};

struct MANGOS_DLL_DECL boss_gothikAI : public Scripted_NoMovementAI
{
	 boss_gothikAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) 
	{
	pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    IsHeroicMode = pCreature->GetMap()->IsHeroic();
	Reset();
	}
	 bool IsHeroicMode;
	 ScriptedInstance* pInstance;

	 uint32 Shbolt_Timer;
	 uint32 Harvester_Soul_Timer;
	 uint32 TeleportTimer;

	 uint32 current_spawn_timer;
	 uint32 phase;
	 int weaves_num;
	 int curr_weave;
	 int mob_num;
	 int currentplace;
	 
	 void Reset()
	 {
		 weaves_num = IsHeroicMode ? 18 : 19;
		 mob_num = IsHeroicMode ? 6 : 4;
		 curr_weave = 0;
		 current_spawn_timer = IsHeroicMode ? H_Weaves[curr_weave][mob_num] : N_Weaves[curr_weave][mob_num];
		 Shbolt_Timer = 0;
		 Harvester_Soul_Timer = 15000;
		 TeleportTimer = 10000;
		 phase = 0;
		 currentplace = 0;
		 if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_MILI_GOTH_COMBAT_GATE)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
		 if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_MILI_GOTH_ENTRY_GATE)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
		 if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_MILI_GOTH_EXIT_GATE)))
			pDoor->SetGoState(GO_STATE_READY);
	 }
	 void Aggro(Unit *plr)
	 {
		 uint64 center_door = pInstance->GetData64(GO_MILI_GOTH_COMBAT_GATE);
		 pInstance->DoUseDoorOrButton(center_door,0);
		 uint64 door = pInstance->GetData64(GO_MILI_GOTH_ENTRY_GATE);
		 pInstance->DoUseDoorOrButton(door,0);
		 m_creature->SetInCombatWithZone();
	 }
	 void JustDied(Unit *)
	 {
		 uint64 door = pInstance->GetData64(GO_MILI_GOTH_ENTRY_GATE);
		 pInstance->DoUseDoorOrButton(door,0);
		 door = pInstance->GetData64(GO_MILI_GOTH_EXIT_GATE);
		 pInstance->DoUseDoorOrButton(door,0);
		  
		
	 }
	 void UpdateAI(const uint32 diff)
	 {
		 if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		 switch(phase)
		 {
		 case 0:
			 if(curr_weave > weaves_num)
			 {
				 phase++;
				 m_creature->Relocate(GothikTele[currentplace].x,GothikTele[currentplace].y,GothikTele[currentplace].z,0);
				 return;
			 }
			 if(current_spawn_timer < diff)
			 {
				 for(int i = 0; i < mob_num;i++)
				 {
					 if(IsHeroicMode)
					 {
						 if(H_Weaves[curr_weave][i] == 0)
							 break;
							int spawn_pos = rand()%3;
							Creature* C = m_creature->SummonCreature(H_Weaves[curr_weave][i],LeftSide[spawn_pos].x,LeftSide[spawn_pos].y,LeftSide[spawn_pos].z,0,TEMPSUMMON_DEAD_DESPAWN,ADD_DESPAWN_TIMER);
							C->Attack(m_creature->getVictim(),true);
					 }else{
						  if(N_Weaves[curr_weave][i] == 0)
							 break;
							int spawn_pos = rand()%3;
							Creature* C = m_creature->SummonCreature(N_Weaves[curr_weave][i],LeftSide[spawn_pos].x,LeftSide[spawn_pos].y,LeftSide[spawn_pos].z,0,TEMPSUMMON_DEAD_DESPAWN,ADD_DESPAWN_TIMER);
							C->Attack(m_creature->getVictim(),true);
						 }
					}
				 current_spawn_timer = IsHeroicMode ? H_Weaves[curr_weave][mob_num] : N_Weaves[curr_weave][mob_num];
				 curr_weave++;
			 }else current_spawn_timer -= diff;
		 break;
		 case 1:
			 if( (m_creature->GetHealth()*100 /m_creature->GetMaxHealth()) < 30)
			 {
				 phase++;
				 uint64 center_door = pInstance->GetData64(GO_MILI_GOTH_COMBAT_GATE);
				 pInstance->DoUseDoorOrButton(center_door,0);
				 return;
			 }
			 if(TeleportTimer < diff)
			 {
				 m_creature->CastStop();
				 currentplace = (currentplace == 0) ? 1 : 0;
				m_creature->Relocate(GothikTele[currentplace].x,GothikTele[currentplace].y,GothikTele[currentplace].z,0);
				TeleportTimer = 10000;
			 }else TeleportTimer -= diff;
		 break;
		 }
		 if(phase != 0)
		 {
			 if(Shbolt_Timer < diff)
			 {
				 DoCast(m_creature->getVictim(),IsHeroicMode ? H_SPELL_SHADOWBOLT : SPELL_SHADOWBOLT);
				 Shbolt_Timer = 1000;
			 }else Shbolt_Timer -= diff;
			 if(Harvester_Soul_Timer < diff)
			 {
				 m_creature->CastStop();
				 DoCast(m_creature,SPELL_HARVESTSOUL,true);
				 Harvester_Soul_Timer = 15000;
			 }else Harvester_Soul_Timer -= diff;
		 }

	 }
	 

};
struct MANGOS_DLL_DECL mob_unrelenting_traineeAI : public ScriptedAI
{
	mob_unrelenting_traineeAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
    IsHeroicMode = pCreature->GetMap()->IsHeroic();
	Reset();
	}
	bool IsHeroicMode;
	uint32 spelltimer;
	
	void Reset()
	{
		spelltimer = 3000;
	}
	void Aggro()
	{
	}
	void JustDied(Unit *killer)
	{
		int i = rand()%3;
		Creature* c = m_creature->SummonCreature(NPC_S_TRAINEE,RightSide[i].x,RightSide[i].y,RightSide[i].z,0,TEMPSUMMON_DEAD_DESPAWN,ADD_DESPAWN_TIMER);
		c->Attack(killer,true);
		m_creature->RemoveCorpse();

	}
	void UpdateAI(const uint32 diff)
	{
		 if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		 if(spelltimer < diff)
		 {
			 DoCast(m_creature->getVictim(),IsHeroicMode ? H_SPELL_DEATH_PLAGUE : SPELL_DEATH_PLAGUE);
			 spelltimer = 3000;
		 }else spelltimer -= diff;
	}

};
struct MANGOS_DLL_DECL mob_unrelenting_death_knightAI : public ScriptedAI
{
	mob_unrelenting_death_knightAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}
	uint32 spelltimer;
	void Reset()
	{
		spelltimer = 0;
	}
	void Aggro()
	{
	}
	void JustDied(Unit *killer)
	{
		int i =  rand()%3;
		Creature* c = m_creature->SummonCreature(NPC_S_DEATH_KNIGHT,RightSide[i].x,RightSide[i].y,RightSide[i].z,0,TEMPSUMMON_DEAD_DESPAWN,ADD_DESPAWN_TIMER);
		c->Attack(killer,true);
		m_creature->RemoveCorpse();
	}
	void UpdateAI(const uint32 diff)
	{
		 if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		 if(spelltimer < diff)
		 {
			 DoCast(m_creature,SPELL_SHADOW_MARK);
			 spelltimer = 20000;
		 }else spelltimer -= diff;
	}

};
struct MANGOS_DLL_DECL mob_unrelenting_riderAI : public ScriptedAI
{
	mob_unrelenting_riderAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
    IsHeroicMode = pCreature->GetMap()->IsHeroic();
	Reset();
	}
	bool IsHeroicMode;
	uint32 spelltimer;
	void Reset()
	{
		spelltimer = 5000;
	}
	void Aggro()
	{
		DoCast(m_creature,IsHeroicMode ? H_SPELL_UNHOLY_AURA : SPELL_UNHOLY_AURA);
	}
	void JustDied(Unit *killer)
	{
		int i =  rand()%3;
		Creature* c = m_creature->SummonCreature(NPC_S_RIDER,RightSide[i].x,RightSide[i].y,RightSide[i].z,0,TEMPSUMMON_DEAD_DESPAWN,ADD_DESPAWN_TIMER);
		c->Attack(killer,true);
		Creature* c_h = m_creature->SummonCreature(NPC_S_HORSE,RightSide[i].x,RightSide[i].y,RightSide[i].z,0,TEMPSUMMON_DEAD_DESPAWN,ADD_DESPAWN_TIMER);
		c->Attack(killer,true);
		m_creature->RemoveCorpse();
	}
	void UpdateAI(const uint32 diff)
	{
		 if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		 if( spelltimer < diff)
		 {
			 DoCast(m_creature,IsHeroicMode ? H_SPELL_SHADOWBOLT_VOLLEY : SPELL_SHADOWBOLT_VOLLEY );
			 spelltimer = 10000;
		 }else spelltimer -= diff;
	}

};
struct MANGOS_DLL_DECL mob_spectral_traineeAI : public ScriptedAI
{
	mob_spectral_traineeAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
    IsHeroicMode = pCreature->GetMap()->IsHeroic();
	Reset();
	}
	bool IsHeroicMode;
	uint32 spelltimer;
	void Reset()
	{
		spelltimer = 4000;
	}
	void Aggro()
	{
	}
	void JustDied(Unit *)
	{
		m_creature->RemoveCorpse();
	}
	void UpdateAI(const uint32 diff)
	{
		 if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		 if(spelltimer < diff)
		 {
			 DoCast(m_creature,IsHeroicMode ? H_SPELL_ARCANE_EXPLOSION : SPELL_ARCANE_EXPLOSION);
			 spelltimer = 7000;
		 }else spelltimer -= diff;
	}

};
struct MANGOS_DLL_DECL mob_spectral_death_knightAI : public ScriptedAI
{
	mob_spectral_death_knightAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
    IsHeroicMode = pCreature->GetMap()->IsHeroic();
	Reset();
	}
	bool IsHeroicMode;
	uint32 spelltimer;
	void Reset()
	{
		spelltimer = 5000;
	}
	void Aggro()
	{
	}
	void JustDied(Unit *)
	{
		m_creature->RemoveCorpse();
	}
	void UpdateAI(const uint32 diff)
	{
		 if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		 if(spelltimer < diff)
		 {
			 DoCast(m_creature,SPELL_WHIRLWIND);
			 spelltimer = 15000;
		 }else spelltimer -= diff;
	}

};
struct MANGOS_DLL_DECL mob_spectral_riderAI : public ScriptedAI
{
	mob_spectral_riderAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
    IsHeroicMode = pCreature->GetMap()->IsHeroic();
	Reset();
	}
	bool IsHeroicMode;
	uint32 spelltimer;
	uint32 frienzytimer;
	void Reset()
	{
		spelltimer = 8000;
		frienzytimer = 5000;
	}
	void Aggro()
	{
		DoCast(m_creature,IsHeroicMode ? H_SPELL_UNHOLY_AURA : SPELL_UNHOLY_AURA);
	}
	void JustDied(Unit *)
	{
		m_creature->RemoveCorpse();
	}
	void UpdateAI(const uint32 diff)
	{
		 if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		 if(spelltimer < diff)
		 {
			 DoCast(m_creature->getVictim(),IsHeroicMode ? H_SPELL_LIFEDRAIN : SPELL_LIFEDRAIN);
			 spelltimer = 8000;
		 }else spelltimer -= diff;
		if(frienzytimer <diff)
		{
			Creature* targ = GetClosestCreatureWithEntry(m_creature,NPC_S_DEATH_KNIGHT,25);
			DoCast(targ,IsHeroicMode ? H_UNHOLY_FRENZY : UNHOLY_FRENZY);
			frienzytimer = 10000;
		}else frienzytimer -= diff;

	}

};
struct MANGOS_DLL_DECL mob_spectral_horseAI : public ScriptedAI
{
	mob_spectral_horseAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
    IsHeroicMode = pCreature->GetMap()->IsHeroic();
	Reset();
	}
	bool IsHeroicMode;
	uint32 spelltimer;
	void Reset()
	{
		spelltimer = 8000;
	}
	void Aggro()
	{
	}
	void JustDied(Unit *)
	{
		m_creature->RemoveCorpse();
	}
	void UpdateAI(const uint32 diff)
	{
		 if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		 if(spelltimer < diff)
		 {
			 DoCast(m_creature,SPELL_STOMP);
			 spelltimer = 9000;
		 }else spelltimer -= diff;
	}

};
CreatureAI* GetAI_boss_gothik(Creature* pCreature)
{
    return new boss_gothikAI(pCreature);
}
CreatureAI* GetAI_mob_unrelenting_death_knight(Creature* pCreature)
{
    return new mob_unrelenting_death_knightAI(pCreature);
}
CreatureAI* GetAI_mob_spectral_horse(Creature* pCreature)
{
    return new mob_spectral_horseAI(pCreature);
}
CreatureAI* GetAI_mob_spectral_rider(Creature* pCreature)
{
    return new mob_spectral_riderAI(pCreature);
}
CreatureAI* GetAI_mob_unrelenting_rider(Creature* pCreature)
{
    return new mob_unrelenting_riderAI(pCreature);
}
CreatureAI* GetAI_mob_unrelenting_trainee(Creature* pCreature)
{
    return new mob_unrelenting_traineeAI(pCreature);
}
CreatureAI* GetAI_mob_spectral_death_knight(Creature* pCreature)
{
    return new mob_spectral_death_knightAI(pCreature);
}
CreatureAI* GetAI_mob_spectral_trainee(Creature* pCreature)
{
    return new mob_spectral_traineeAI(pCreature);
}
void AddSC_boss_gothik()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_gothik";
    newscript->GetAI = &GetAI_boss_gothik;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_unrelenting_trainee";
    newscript->GetAI = &GetAI_mob_unrelenting_trainee;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_unrelenting_death_knight";
    newscript->GetAI = &GetAI_mob_unrelenting_death_knight;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_spectral_horse";
    newscript->GetAI = &GetAI_mob_spectral_horse;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_spectral_rider";
    newscript->GetAI = &GetAI_mob_spectral_rider;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_unrelenting_rider";
    newscript->GetAI = &GetAI_mob_unrelenting_rider;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_spectral_death_knight";
    newscript->GetAI = &GetAI_mob_spectral_death_knight;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_spectral_trainee";
    newscript->GetAI = &GetAI_mob_spectral_trainee;
    newscript->RegisterSelf();
	
}