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
SD%Complete: 0
SDComment: Placeholder. Includes Feugen & Stalagg.
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

#define SPELL_CHARGE_POSITIVE_DMGBUFF       29659
#define SPELL_CHARGE_POSITIVE_NEARDMG       28059

#define SPELL_CHARGE_NEGATIVE_DMGBUFF       29660
#define SPELL_CHARGE_NEGATIVE_NEARDMG       28084

#define SPELL_CHAIN_LIGHTNING               28167
#define H_SPELL_CHAIN_LIGHTNING             54531

#define SPELL_BESERK                        26662

#define MAXRANGE_FROM_TESLA_COIL 20.0f
#define NPC_TESLA_COIL 16218
//generic
#define C_TESLA_COIL                        16218           //the coils (emotes "Tesla Coil overloads!")

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

	Creature* tesla_coil;

	uint32 magnetic_pull_timer;
	Creature* Feugen;
	Unit* StalTarget;
	uint32 CheckTimer;
	bool ReadyToCheck;
	bool magnetic_pull_app;
	uint32 magnetic_pull_after;
    void Reset()
	{
		magnetic_pull_timer = 30000;
		CheckTimer = 5000;
		tesla_coil = GetClosestCreatureWithEntry(m_creature,NPC_TESLA_COIL,MAXRANGE_FROM_TESLA_COIL);
		Feugen = (Creature*)Unit::GetUnit(*m_creature,m_pInstance->GetData64(NPC_FEUGEN));
		if(!Feugen || !tesla_coil)
			m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
	}
	void Aggro(Unit* who){}
	void JustDied(Unit *)
	{
		if(Feugen && Feugen->isAlive())
		{
			SetFeuCheck(true,Feugen);
		}
		m_pInstance->SetData(DATA_THADDIUS_EVENT,IN_PROGRESS);
	}
	void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		if(magnetic_pull_timer < diff)
		{
			if(Feugen->isAlive() && Feugen->getVictim())
			{
				m_creature->StopMoving();
				Feugen->StopMoving();
				m_creature->GetMotionMaster()->MovementExpired();
				Feugen->GetMotionMaster()->MovementExpired();
				DoCast(Feugen->getVictim(),SPELL_MAGNETIC_PULL);
				Feugen->CastSpell(m_creature->getVictim(),SPELL_MAGNETIC_PULL,false);
				StalTarget = Feugen->getVictim();
				Feugen->getThreatManager().tauntApply(m_creature->getVictim());
				m_creature->getThreatManager().tauntApply(StalTarget);
				//allow attack after 1seconds
				magnetic_pull_app = true;
				magnetic_pull_after = 1000;

			}
			magnetic_pull_timer = 30000;
		}else magnetic_pull_timer -= diff;
		if(magnetic_pull_app)
		{
			if(magnetic_pull_after < diff)
			{
				m_creature->GetMotionMaster()->MovementExpired();
				m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
				Feugen->GetMotionMaster()->MoveChase(Feugen->getVictim());
				magnetic_pull_app = false;
			}else magnetic_pull_after -= diff;
		}
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

	Creature* tesla_coil;

	uint32 magnetic_pull_timer;
	Creature* Stalagg;
	uint32 CheckTimer;
	bool ReadyToCheck;
    void Reset()
	{
		magnetic_pull_timer = 30000;
		CheckTimer = 5000;
		tesla_coil = GetClosestCreatureWithEntry(m_creature,NPC_TESLA_COIL,MAXRANGE_FROM_TESLA_COIL);
		Stalagg = (Creature*)Unit::GetUnit(*m_creature,m_pInstance->GetData64(NPC_FEUGEN));
		if(!Stalagg || !tesla_coil)
		{
			m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
			m_pInstance->SetData(DATA_THADDIUS_EVENT,IN_PROGRESS);
		}
	}
	void Aggro(Unit* who){}
	void JustDied(Unit *)
	{
		if(Stalagg && Stalagg->isAlive())
		{
			SetStalCheck(true,Stalagg);
		}else
			m_pInstance->SetData(DATA_THADDIUS_EVENT,IN_PROGRESS);
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
	void Reset()
	{
		Stalagg = (Creature *)Unit::GetUnit(*m_creature,m_pInstance->GetData64(NPC_STALAGG));
		Feugen = (Creature *)Unit::GetUnit(*m_creature,m_pInstance->GetData64(NPC_FEUGEN));
		if(!Stalagg || !Feugen)
			m_pInstance->SetData(DATA_THADDIUS_EVENT,SPECIAL);
		if(!Stalagg->isAlive() || !Feugen->isAlive())
		{
			Stalagg->Respawn();
			Feugen->Respawn();
		}
	}
	void JustDied(Unit* Killer)
	{
	}
	void UpdateAI(const uint32 diff)
    {
		if(m_creature->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE))
		{
			if(m_pInstance->GetData(DATA_THADDIUS_EVENT) == SPECIAL)
			{
				m_creature->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
				m_creature->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
			}
		}
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		DoMeleeAttackIfReady();
	}

};