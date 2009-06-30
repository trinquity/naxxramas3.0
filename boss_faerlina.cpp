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
SDName: Boss_Faerlina
SD%Complete: 99
SDComment: areatrigger pofázás
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define SAY_GREET                   -1533009
#define SAY_AGGRO1                  -1533010
#define SAY_AGGRO2                  -1533011
#define SAY_AGGRO3                  -1533012
#define SAY_AGGRO4                  -1533013
#define SAY_SLAY1                   -1533014
#define SAY_SLAY2                   -1533015
#define SAY_DEATH                   -1533016

//#define SOUND_RANDOM_AGGRO  8955                            //soundId containing the 4 aggro sounds, we not using this

#define SPELL_POSIONBOLT_VOLLEY     28796
#define H_SPELL_POSIONBOLT_VOLLEY   54098
#define SPELL_ENRAGE                28798
#define H_SPELL_ENRAGE              54100

#define SPELL_RAINOFFIRE            28794 
#define H_SPELL_RAINOFFIRE          58936

#define WORSHIPPER_FIREBALL			54095
#define WORSHIPPER_FIREBALL_H		54096

#define FOLLOWER_CHARGE				56107
#define FOLLOWER_SILENCE			54093

#define DISTANCE_CHECK 20

struct MANGOS_DLL_DECL boss_faerlinaAI : public ScriptedAI
{
    boss_faerlinaAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        isHeroicMode = pCreature->GetMap()->IsHeroic();
		Reset();
	}
	ScriptedInstance* pInstance;
    uint32 PoisonBoltVolley_Timer;
    uint32 RainOfFire_Timer;
    uint32 Enrage_Timer;
	bool is_enraged;
	bool isHeroicMode;

    void Reset()
    {
        PoisonBoltVolley_Timer = 8000;
        RainOfFire_Timer = 16000;
        Enrage_Timer = 60000;
        is_enraged = false;
		pInstance->SetData(DATA_FAERLINA_EVENT,NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        switch (rand()%4)
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
            case 3: DoScriptText(SAY_AGGRO4, m_creature); break;
        }
		pInstance->SetData(DATA_FAERLINA_EVENT,IN_PROGRESS);
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
		pInstance->SetData(DATA_FAERLINA_EVENT,DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        //PoisonBoltVolley_Timer
        if (PoisonBoltVolley_Timer < diff && !is_enraged)
        {
			DoCast(m_creature->getVictim(),isHeroicMode ? H_SPELL_POSIONBOLT_VOLLEY : SPELL_POSIONBOLT_VOLLEY);
            PoisonBoltVolley_Timer = 11000;
        }else PoisonBoltVolley_Timer -= diff;

        //RainOfFire_Timer
        if (RainOfFire_Timer < diff)
        {
            Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0);
			if(target)
			m_creature->CastSpell(target->GetPositionX(),target->GetPositionY(),target->GetPositionZ(),isHeroicMode ? H_SPELL_RAINOFFIRE : SPELL_RAINOFFIRE,true);

            RainOfFire_Timer = 16000;
        }else RainOfFire_Timer -= diff;

        //Enrage_Timer
        if (Enrage_Timer < diff)
        {
			is_enraged = true;
			DoCast(m_creature,isHeroicMode ? H_SPELL_ENRAGE : SPELL_ENRAGE);
            Enrage_Timer = 61000;
        }else Enrage_Timer -= diff;

		if(is_enraged && !m_creature->HasAura(SPELL_ENRAGE,0))
		{
			is_enraged = false;
		}

        DoMeleeAttackIfReady();
    }
};
struct MANGOS_DLL_DECL mob_naxxramas_followerAI : public ScriptedAI
{
	     mob_naxxramas_followerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {

        IsHeroicMode = pCreature->GetMap()->IsHeroic();
        Reset();
    }
	bool IsHeroicMode;

	bool silence_ready;
    uint32 charge_timer;
	uint32 silence_timer;

	void Reset()
	{
		charge_timer = 15000;
		silence_timer = 0;
		silence_ready = false;
	}

	void UpdateAI(const uint32 diff)
    {
		//following timer are bad
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		if(charge_timer < diff)
		{
			Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0);
			if(target)
			DoCast(target,FOLLOWER_CHARGE);
			charge_timer = 15000;
			silence_timer = 6000;
			silence_ready = true;
		}else charge_timer -= diff;

		if(silence_timer < diff && silence_ready)
		{
			DoCast(m_creature,FOLLOWER_SILENCE);
			silence_ready = false;
		}else silence_timer -= diff;
		
		DoMeleeAttackIfReady();
	}
};

struct MANGOS_DLL_DECL mob_naxxramas_WorshipperAI : public ScriptedAI
{
	    mob_naxxramas_WorshipperAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        isHeroicMode = pCreature->GetMap()->IsHeroic();
        Reset();
    }
	ScriptedInstance* pInstance;
	bool isHeroicMode;

    uint32 fireball_timer;
	Creature* Faerlina;

	void Reset()
	{
		fireball_timer = 0+rand()%5000;
		Faerlina = (Creature*)Unit::GetUnit(*m_creature,pInstance->GetData64(NPC_FAERLINA));
	}
	void JustDied(Unit* Killer)
	{
		if(!Faerlina)
			return;
		if(Faerlina->HasAura(isHeroicMode ? H_SPELL_ENRAGE : SPELL_ENRAGE,0) && m_creature->GetDistance(Faerlina) < DISTANCE_CHECK)
		{
			Faerlina->RemoveAurasDueToSpell(isHeroicMode ? H_SPELL_ENRAGE : SPELL_ENRAGE);
			((boss_faerlinaAI*)Faerlina->AI())->PoisonBoltVolley_Timer += 30000;
		}else{
			((boss_faerlinaAI*)Faerlina->AI())->Enrage_Timer -= 30000;
		}

	}

	void UpdateAI(const uint32 diff)
    {
		//following timer are bad
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		if(fireball_timer < diff)
		{
			DoCast(m_creature->getVictim(),WORSHIPPER_FIREBALL);
			fireball_timer = 2500+rand()%1000;
		}else fireball_timer -= diff;
	}
};

CreatureAI* GetAI_boss_faerlina(Creature* pCreature)
{
    return new boss_faerlinaAI(pCreature);
}
CreatureAI* GetAI_mob_naxxramas_Worshipper(Creature* pCreature)
{
    return new mob_naxxramas_WorshipperAI(pCreature);
}
CreatureAI* GetAI_mob_naxxramas_follower(Creature* pCreature)
{
    return new mob_naxxramas_followerAI(pCreature);
}
void AddSC_boss_faerlina()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_faerlina";
    newscript->GetAI = &GetAI_boss_faerlina;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_naxxramas_worshipper";
    newscript->GetAI = &GetAI_mob_naxxramas_Worshipper;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_naxxramas_follower";
    newscript->GetAI = &GetAI_mob_naxxramas_follower;
    newscript->RegisterSelf();
}
