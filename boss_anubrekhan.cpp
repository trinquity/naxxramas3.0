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
SDName: Boss_Anubrekhan
SD%Complete: 99
SDComment:areatrigger 4119 pofázás, summon issue
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define SAY_GREET                   -1533000
#define SAY_AGGRO1                  -1533001
#define SAY_AGGRO2                  -1533002
#define SAY_AGGRO3                  -1533003
#define SAY_TAUNT1                  -1533004
#define SAY_TAUNT2                  -1533005
#define SAY_TAUNT3                  -1533006
#define SAY_TAUNT4                  -1533007
#define SAY_SLAY                    -1533008

#define SPELL_IMPALE                28783                    //May be wrong spell id. Causes more dmg than I expect
#define SPELL_IMPALE_H              56090
#define SPELL_LOCUSTSWARM           28785                    //This is a self buff that triggers the dmg debuff
#define SPELL_LOCUSTSWARM_H         54021
#define SPELL_GUARD_ACID_SPLIT		28969
#define SPELL_GUARD_ACID_SPLIT_H	56098
#define SPELL_GUARD_CLEAVE			40504
#define SPELL_GUARD_FRENZY			8269
#define SPELL_SELF_SPAWN_5          29105                    //This spawns 5 corpse scarabs ontop of us (most likely the pPlayer casts this on death)
#define SPELL_SELF_SPAWN_10         28864                  //This is used by the crypt guards when they die
#define NPC_CRYPT_GUARD             16573
#define SPELL_ENRAGE				26662

struct MANGOS_DLL_DECL boss_anubrekhanAI : public ScriptedAI
{
    boss_anubrekhanAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroicMode = pCreature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;
	bool able_to_summon;
	bool enraged;
    uint32 Impale_Timer;
    uint32 LocustSwarm_Timer;
	uint32 Summon_Timer;
	uint32 Enrage_Timer;

    void Reset()
    {
        Impale_Timer = 15000;                               //15 seconds
        LocustSwarm_Timer = 80000 + (rand()%40000);         //Random time between 80 seconds and 2 minutes for initial cast
		Summon_Timer = 0;
		Enrage_Timer = 600000;
		able_to_summon = false;
		enraged = false;
		
		if(m_pInstance)
		m_pInstance->SetData(DATA_ANUBREKHAN_EVENT,NOT_STARTED);
		if(GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_ARAC_ANUB_GATE)))
			pDoor->SetGoState(GO_STATE_READY);
		if(GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_ARAC_ANUB_DOOR)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
    }

    void KilledUnit(Unit* pVictim)
    {
        //Force the player to spawn corpse scarabs via spell
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            pVictim->CastSpell(pVictim, SPELL_SELF_SPAWN_5, true);

        DoScriptText(SAY_SLAY, m_creature);
    }
	void JustDied(Unit *)
	{
		if(m_pInstance)
		m_pInstance->SetData(DATA_ANUBREKHAN_EVENT,DONE);
		if(GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_ARAC_ANUB_GATE)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
		if(GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_ARAC_ANUB_DOOR)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
	}

    void Aggro(Unit *who)
    {
		if(m_pInstance)
		m_pInstance->SetData(DATA_ANUBREKHAN_EVENT,IN_PROGRESS);
		if(GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_ARAC_ANUB_GATE)))
			pDoor->SetGoState(GO_STATE_READY);
		if(GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_ARAC_ANUB_DOOR)))
			pDoor->SetGoState(GO_STATE_READY);
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }
    }

	void JustSummoned(Creature* c)
	{
		Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0);
		if(target)
			c->AddThreat(target,0.0f);
	}
    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        //Impale_Timer
        if (Impale_Timer < diff)
        {
            //Cast Impale on a random target
            //Do NOT cast it when we are afflicted by locust swarm
            if (!m_creature->HasAura(SPELL_LOCUSTSWARM) || !m_creature->HasAura(SPELL_LOCUSTSWARM_H))
            {
                if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,1))
                    DoCast(target, m_bIsHeroicMode ? SPELL_IMPALE_H : SPELL_IMPALE);
            }

            Impale_Timer = 15000;
        }else Impale_Timer -= diff;

        //LocustSwarm_Timer
        if (LocustSwarm_Timer < diff)
        {
            DoCast(m_creature, m_bIsHeroicMode ? SPELL_LOCUSTSWARM_H : SPELL_LOCUSTSWARM);
			Summon_Timer = 20000;
			able_to_summon = true;
            LocustSwarm_Timer = 90000;
        }else LocustSwarm_Timer -= diff;
		//summon
		if(able_to_summon)
		{
			if(Summon_Timer < diff)
			{
			m_creature->SummonCreature(NPC_CRYPT_GUARD,m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ(),m_creature->GetOrientation(),TEMPSUMMON_DEAD_DESPAWN,600000);
			able_to_summon = false;
			}else Summon_Timer -= diff;
		}
		//enrage
		if(Enrage_Timer < diff && !enraged)
		{
			DoCast(m_creature,SPELL_ENRAGE);
			enraged = true;
		}else Enrage_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
struct MANGOS_DLL_DECL mob_crypt_guardAI : public ScriptedAI
{
	    mob_crypt_guardAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsHeroicMode = pCreature->GetMap()->IsHeroic();
        Reset();
    }
	bool m_bIsHeroicMode;
	bool Frenzied;
    uint32 acid_split_Timer;
    uint32 cleave_timer;

	void Reset()
	{
		acid_split_Timer = 0;
		cleave_timer = 0;
		Frenzied = false;
	}
	void JustDied(Unit* Killer)
	{
		DoCast(m_creature,SPELL_SELF_SPAWN_10,true);
	}
	void UpdateAI(const uint32 diff)
    {
		//following timer are bad
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		if(acid_split_Timer < diff)
		{
			DoCast(m_creature->getVictim(),m_bIsHeroicMode ? SPELL_GUARD_ACID_SPLIT_H : SPELL_GUARD_ACID_SPLIT);
			acid_split_Timer = 9000;
		}else acid_split_Timer -= diff;
		if(cleave_timer < diff)
		{
			DoCast(m_creature,SPELL_GUARD_CLEAVE);
			cleave_timer = 10000;
		}else cleave_timer -= diff;
		if(m_creature->GetHealth()/m_creature->GetMaxHealth() * 100 < 40 && !Frenzied )
		{
			DoCast(m_creature,SPELL_GUARD_FRENZY);
			Frenzied = true;
		}
		DoMeleeAttackIfReady();
	}

};

CreatureAI* GetAI_boss_anubrekhan(Creature* pCreature)
{
    return new boss_anubrekhanAI(pCreature);
}
CreatureAI* GetAI_mob_crypt_guard(Creature* pCreature)
{
    return new mob_crypt_guardAI(pCreature);
}


void AddSC_boss_anubrekhan()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_anubrekhan";
    newscript->GetAI = &GetAI_boss_anubrekhan;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_crypt_guard";
    newscript->GetAI = &GetAI_mob_crypt_guard;
    newscript->RegisterSelf();
}
