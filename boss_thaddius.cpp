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
SDName: Boss_Thaddius
SD%Complete: 90
SDComment: Placeholder.Need Some Corepatch
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

//Stalagg
#define SAY_STAL_AGGRO          -1533023
#define SAY_STAL_SLAY           -1533024
#define SAY_STAL_DEATH          -1533025

#define SPELL_POWERSURGE        28134

//Feugen
#define SAY_FEUG_AGGRO          -1533026
#define SAY_FEUG_SLAY           -1533027
#define SAY_FEUG_DEATH          -1533028

#define SPELL_MANABURN          28135

#define SPELL_MAGNETIC_PULL         28337

//Thaddus
#define SAY_GREET               -1533029
#define SAY_AGGRO1              -1533030
#define SAY_AGGRO2              -1533031
#define SAY_AGGRO3              -1533032
#define SAY_SLAY                -1533033
#define SAY_ELECT               -1533034
#define SAY_DEATH               -1533035
#define SAY_SCREAM1             -1533036
#define SAY_SCREAM2             -1533037
#define SAY_SCREAM3             -1533038
#define SAY_SCREAM4             -1533039

#define SPELL_BALL_LIGHTNING                28299

#define SPELL_POLARITY_SHIFT				28089

#define SPELL_CHAIN_LIGHTNING               28167
#define H_SPELL_CHAIN_LIGHTNING             54531

#define SPELL_BESERK                        26662

#define MAXRANGE_FROM_TESLA_COIL 20.0f
#define NPC_TESLA_COIL 16218
//generic
#define C_TESLA_COIL                        16218           //the coils (emotes "Tesla Coil overloads!")
struct MANGOS_DLL_DECL boss_thaddiusAI : public Scripted_NoMovementAI
{
	    boss_thaddiusAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_bIsHeroicMode = pCreature->GetMap()->IsHeroic();
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }
	bool m_bIsHeroicMode;
	ScriptedInstance* m_pInstance;
	Creature* Stalagg;
	Creature* Feugen;

	uint32 CheckInRange;
	uint32 ChainLightingTimer;
	uint32 PolarityShiftTimer;
	bool FeugenAndStalaggKilled;
	void Reset()
	{
		m_pInstance->SetData(DATA_THADDIUS_EVENT,NOT_STARTED);
		Stalagg = (Creature *)Unit::GetUnit(*m_creature,m_pInstance->GetData64(NPC_STALAGG));
		Feugen = (Creature *)Unit::GetUnit(*m_creature,m_pInstance->GetData64(NPC_FEUGEN));
		CheckInRange = 1000;
		ChainLightingTimer = 15000;
		PolarityShiftTimer = 30000;
		if(!Feugen || !Stalagg)
		{
			FeugenAndStalaggKilled = true;
			return;
		}
		if(!Stalagg->isAlive() || !Feugen->isAlive())
		{
			FeugenAndStalaggKilled = false;
			Stalagg->Respawn();
			Feugen->Respawn();
		}
	}
	void Aggro(Unit *)
	{
		m_pInstance->SetData(DATA_THADDIUS_EVENT,IN_PROGRESS);
	}
	void JustDied(Unit* Killer)
	{
		m_pInstance->SetData(DATA_THADDIUS_EVENT,DONE);
	}
	void UpdateAI(const uint32 diff)
    {
		if(m_creature->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE))
		{
			if(FeugenAndStalaggKilled)
			{
				m_creature->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
				m_creature->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
			}
		}
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		if(CheckInRange < diff)
		{
			if(!m_creature->IsWithinDist(m_creature->getVictim(),5.0f,false))
			{
			bool InRange = false;
			Unit* pTemp;
			std::list<HostilReference*>::iterator i = m_creature->getThreatManager().getThreatList().begin();
            for (i = m_creature->getThreatManager().getThreatList().begin(); i!=m_creature->getThreatManager().getThreatList().end(); ++i)
            {
                pTemp = Unit::GetUnit((*m_creature),(*i)->getUnitGuid());
				if(m_creature->IsWithinDist(pTemp,5.0f,false))
				{
					InRange = true;
					//anti exploit
					m_creature->getThreatManager().tauntApply(pTemp);
				}
            }
			if(!InRange)
				DoCast(m_creature,SPELL_BALL_LIGHTNING);
			}
			CheckInRange = 1000;
		}else CheckInRange -= diff;
		if(ChainLightingTimer < diff)
		{
			Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0);
			if(target)
				DoCast(target,m_bIsHeroicMode ? H_SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING,true);
			ChainLightingTimer = 15000;
		}else ChainLightingTimer -= diff;
		if(PolarityShiftTimer < diff)
		{
			DoCast(m_creature,SPELL_POLARITY_SHIFT);
			PolarityShiftTimer = 30000;
		}else PolarityShiftTimer -= diff;
		DoMeleeAttackIfReady();
	}

};
struct MANGOS_DLL_DECL mob_stalaggAI : public ScriptedAI
{
    mob_stalaggAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroicMode = pCreature->GetMap()->IsHeroic();
		Reset();
	}
	ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;

	uint32 magnetic_pull_timer;
	Creature* Feugen;
	Creature* Thaddius;
	uint32 CheckTimer;
	bool ReadyToCheck;
    void Reset()
	{
		magnetic_pull_timer = 30000;
		CheckTimer = 5000;
		Feugen = (Creature*)Unit::GetUnit(*m_creature,m_pInstance->GetData64(NPC_FEUGEN));
		Thaddius = (Creature*)Unit::GetUnit(*m_creature,m_pInstance->GetData64(NPC_THADDIUS));
		if(!Feugen)
		{
			m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
			m_pInstance->SetData(DATA_THADDIUS_EVENT,IN_PROGRESS);
		}
		if(!Feugen->isAlive())
			Feugen->Respawn();
	}
	void Aggro(Unit* who)
	{
		m_pInstance->SetData(DATA_THADDIUS_EVENT,SPECIAL);
	}
	void JustDied(Unit *)
	{
		if(Feugen && Feugen->isAlive())
		{
			SetFeuCheck(true,Feugen);
		}else{
			m_pInstance->SetData(DATA_THADDIUS_EVENT,IN_PROGRESS);
			if(Thaddius)
			((boss_thaddiusAI*)Thaddius->AI())->FeugenAndStalaggKilled = true;
		}
	}
	void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		//magnetic pull is currently bugged neeed corepatch - ph this
		if(ReadyToCheck)
		{
		if(CheckTimer < diff)
		{
			if(!Feugen->isAlive())
				Feugen->Respawn();
			ReadyToCheck = false;
			CheckTimer = 5000;
		}else CheckTimer -= diff;
		}
		DoMeleeAttackIfReady();
	}
	void SetFeuCheck(bool,Creature*);
};
struct MANGOS_DLL_DECL mob_feugenAI : public ScriptedAI
{
    mob_feugenAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroicMode = pCreature->GetMap()->IsHeroic();
		Reset();
	}
	ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;

	uint32 magnetic_pull_timer;
	Creature* Stalagg;
	Creature* Thaddius;
	uint32 CheckTimer;
	bool ReadyToCheck;
    void Reset()
	{
		magnetic_pull_timer = 30000;
		CheckTimer = 5000;
		Stalagg = (Creature*)Unit::GetUnit(*m_creature,m_pInstance->GetData64(NPC_FEUGEN));
		Thaddius = (Creature*)Unit::GetUnit(*m_creature,m_pInstance->GetData64(NPC_THADDIUS));
		if(!Stalagg)
		{
			m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
			m_pInstance->SetData(DATA_THADDIUS_EVENT,IN_PROGRESS);
		}
		if(!Stalagg->isAlive())
			Stalagg->Respawn();

	}
	void Aggro(Unit* who)
	{
		m_pInstance->SetData(DATA_THADDIUS_EVENT,SPECIAL);
	}
	void JustDied(Unit *)
	{
		if(Stalagg && Stalagg->isAlive())
		{
			SetStalCheck(true,Stalagg);
		}else{
			if(Thaddius)
			((boss_thaddiusAI*)Thaddius->AI())->FeugenAndStalaggKilled = true;
			m_pInstance->SetData(DATA_THADDIUS_EVENT,IN_PROGRESS);
		}
	}
	void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		if(ReadyToCheck)
		{
		if(CheckTimer < diff)
		{
			if(!Stalagg->isAlive())
				Stalagg->Respawn();
			ReadyToCheck = false;
			CheckTimer = 5000;
		}else CheckTimer -= diff;
		}
		DoMeleeAttackIfReady();
	}
	void SetStalCheck(bool,Creature*);
};
void mob_feugenAI::SetStalCheck(bool val,Creature* pStal)
{
	((mob_stalaggAI*)pStal->AI())->ReadyToCheck = val;
}
void mob_stalaggAI::SetFeuCheck(bool val,Creature* pFeu)
{
	((mob_feugenAI*)pFeu->AI())->ReadyToCheck = val;
}

CreatureAI* GetAI_boss_thaddius(Creature* pCreature)
{
    return new boss_thaddiusAI(pCreature);
}

CreatureAI* GetAI_mob_feugen(Creature* pCreature)
{
    return new mob_feugenAI(pCreature);
}

CreatureAI* GetAI_mob_stalagg(Creature* pCreature)
{
    return new mob_stalaggAI(pCreature);
}
void AddSC_boss_thaddius()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_thaddius";
    newscript->GetAI = &GetAI_boss_thaddius;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_feugen";
    newscript->GetAI = &GetAI_mob_feugen;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_stalagg";
    newscript->GetAI = &GetAI_mob_stalagg;
    newscript->RegisterSelf();
}
