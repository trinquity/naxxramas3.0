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
SDName: Boss_Maexxna
SD%Complete: 99
SDComment: need webwrap core fix or anything && add script (from acid if possible)
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define SPELL_WEBTRAP           28622                       //Spell is normally used by the webtrap on the wall NOT by Maexxna

#define SPELL_WEBSPRAY          29484
#define H_SPELL_WEBSPRAY        54125
#define SPELL_POISONSHOCK       28741
#define H_SPELL_POISONSHOCK     54122
#define SPELL_NECROTICPOISON    28776
#define H_SPELL_NECROTICPOISON  54121
#define SPELL_FRENZY            54123
#define H_SPELL_FRENZY          54124

#define MOB_WEBWRAP 16486

#define NPC_SPIDERLING 17055

#define NUM_OF_WRAP_POS 4
struct Wrap
{
	float x,y,z;
};
//only 4 required
Wrap WrapPos[] =
{
	{3546.796,-3869.082,296.450},
	{3531.271,-3847.424,299.450},
	{3497.067,-3843.384,302.384},
	{3483.33,-3836.238,306.9},
	{3498,-3833,305}
};

struct MANGOS_DLL_DECL boss_maexxnaAI : public ScriptedAI
{
    boss_maexxnaAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
	SpellEntry *WebWrap = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_WEBTRAP);
		if(WebWrap)
			WebWrap->EffectApplyAuraName[0] = 0;
	pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    IsHeroicMode = pCreature->GetMap()->IsHeroic();
	Reset();
	}
	ScriptedInstance* pInstance;
    bool IsHeroicMode;
    uint32 WebTrap_Timer;
    uint32 WebSpray_Timer;
    uint32 PoisonShock_Timer;
    uint32 NecroticPoison_Timer;
    uint32 SummonSpiderling_Timer;
	uint64 PlayersInWrap[NUM_OF_WRAP_POS];
	int pos;
	bool found;
    bool Enraged;

    void Reset()
    {
		WebTrap_Timer = 20000;                           //20 sec init, 40 sec normal
        WebSpray_Timer = 40000;                             //40 seconds
        PoisonShock_Timer = 20000;                          //20 seconds
        NecroticPoison_Timer = 30000;                       //30 seconds
		SummonSpiderling_Timer = 30000;                    //30 sec init, 40 sec normal
        Enraged = false;
		pos = 0;
		found = false;
		if(pInstance)
			pInstance->SetData(DATA_MAEXXNA_EVENT,NOT_STARTED);
		if(pInstance->GetData(DATA_FAERLINA_EVENT) == DONE)
		{
		if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_ARAC_MAEX_OUTER_DOOR)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
		}
		if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_ARAC_MAEX_INNER_DOOR)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
		for(int i = 0;i<NUM_OF_WRAP_POS;i++)
			PlayersInWrap[i] = 0;
    }

	void Aggro(Unit *)
	{
		if(pInstance)
			pInstance->SetData(DATA_MAEXXNA_EVENT,IN_PROGRESS);
		if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_ARAC_MAEX_INNER_DOOR)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
	}
	void JustDied(Unit *)
	{
		if(pInstance)
			pInstance->SetData(DATA_MAEXXNA_EVENT,DONE);
		if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_ARAC_MAEX_OUTER_DOOR)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
		if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_ARAC_MAEX_INNER_DOOR)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
	}
    void DoCastWebWrap()
    {	found = false;
		for(int i = 0; i < NUM_OF_WRAP_POS;i++)
		{
			if(PlayersInWrap[i] == 0)
			{
				found = true;
				pos = i;
				break;
			}
		}
		if(!found)
			return;

        Unit* target = SelectUnit(SELECT_TARGET_RANDOM,1);
		if(!target)
			return;
		PlayersInWrap[pos] = target->GetGUID();
		Creature* Wrap = m_creature->SummonCreature(MOB_WEBWRAP,WrapPos[pos].x,WrapPos[pos].y,WrapPos[pos].z,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN
,60000);
		DoTeleportPlayer(target,WrapPos[pos].x,WrapPos[pos].y,WrapPos[pos].z,0);
		target->CastSpell(target,SPELL_WEBTRAP,true);
		Wrap->AddThreat(target,0.0f);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        //WebTrap_Timer
        if (WebTrap_Timer < diff)
        {
            DoCastWebWrap();
			if(IsHeroicMode)
				DoCastWebWrap();
            WebTrap_Timer = 40000;
        }else WebTrap_Timer -= diff;

        //WebSpray_Timer
        if (WebSpray_Timer < diff)
        {
			DoCast(m_creature, IsHeroicMode ? H_SPELL_WEBSPRAY : SPELL_WEBSPRAY);
            WebSpray_Timer = 40000;
        }else WebSpray_Timer -= diff;

        //PoisonShock_Timer
        if (PoisonShock_Timer < diff)
        {
			DoCast(m_creature->getVictim(), IsHeroicMode ? H_SPELL_POISONSHOCK : SPELL_POISONSHOCK);
            PoisonShock_Timer = 20000;
        }else PoisonShock_Timer -= diff;

        //NecroticPoison_Timer
        if (NecroticPoison_Timer < diff)
        {
			DoCast(m_creature->getVictim(), IsHeroicMode ? H_SPELL_NECROTICPOISON : SPELL_NECROTICPOISON);
            NecroticPoison_Timer = 30000;
        }else NecroticPoison_Timer -= diff;

        //SummonSpiderling_Timer
        if (SummonSpiderling_Timer < diff)
        {
			int num_spiders = 8+rand()%2;
			for(int i = 0;i<num_spiders;i++)
			{
				Creature* c = m_creature->SummonCreature(NPC_SPIDERLING,m_creature->GetPositionX(),m_creature->GetPositionX(),m_creature->GetPositionX(),m_creature->GetOrientation(),TEMPSUMMON_DEAD_DESPAWN,60000);
				Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0);
				if(target)
					c->AddThreat(target,0.0f);
			}
            SummonSpiderling_Timer = 40000;
        }else SummonSpiderling_Timer -= diff;

        //Enrage if not already enraged and below 30%
        if (!Enraged && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 30)
        {
			DoCast(m_creature,IsHeroicMode ? H_SPELL_FRENZY : SPELL_FRENZY);
            Enraged = true;
        }

        DoMeleeAttackIfReady();
    }
};
struct MANGOS_DLL_DECL mob_webwrapAI : public Scripted_NoMovementAI
{
    mob_webwrapAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
	{
		pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		Reset();
	}
	ScriptedInstance* pInstance;
	Unit* wrap;
	Creature* maexxna;
	void Reset()
    {
        wrap = NULL;
		maexxna = (Creature*)Unit::GetUnit(*m_creature,pInstance->GetData64(NPC_MAEXXNA));
    }
	void Aggro(Unit* who)
	{
		wrap = who;
	}
	void JustDied(Unit*)
	{
		if(!wrap)
			return;
		wrap->RemoveAurasDueToSpell(SPELL_WEBTRAP);
		for(int i = 0;i<NUM_OF_WRAP_POS;i++)
		{
			if(((boss_maexxnaAI*)maexxna->AI())->PlayersInWrap[i] == wrap->GetGUID())
			{
				((boss_maexxnaAI*)maexxna->AI())->PlayersInWrap[i] = 0;
			}
		}
		m_creature->RemoveCorpse();
	}
    void UpdateAI(const uint32 diff) {}
};
CreatureAI* GetAI_mob_webwrap(Creature* pCreature)
{
    return new mob_webwrapAI(pCreature);
}

CreatureAI* GetAI_boss_maexxna(Creature* pCreature)
{
    return new boss_maexxnaAI(pCreature);
}

void AddSC_boss_maexxna()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_maexxna";
    newscript->GetAI = &GetAI_boss_maexxna;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_webwrap";
    newscript->GetAI = &GetAI_mob_webwrap;
    newscript->RegisterSelf();
}
