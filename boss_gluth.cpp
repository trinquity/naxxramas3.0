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
SDName: Boss_Gluth
SD%Complete: 70
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define EMOTE_ZOMBIE                    -1533119

#define SPELL_MORTALWOUND 25646
#define SPELL_DECIMATE 54426
#define SPELL_ENRAGE 28371
#define SPELL_ENRAGE_H 54427
#define SPELL_BERSERK 26662

#define NPC_ZOMBIE_CHOW 16360
#define SUMM_POS_NUM 9
float AddSummPos[SUMM_POS_NUM][3] = 
{
	{3269.590,-3161.287,297.423},
	{3277.797,-3170.352,297.423},
	{3267.049,-3172.820,297.423},
	{3252.157,-3132.135,297.423},
	{3259.990,-3126.590,297.423},
	{3259.815,-3137.576,297.423},
	{3308.030,-3132.135,297.423},
	{3303.046,-3180.682,297.423},
	{3313.283,-3180.766,297.423}
};
struct MANGOS_DLL_DECL boss_gluthAI : public ScriptedAI
{
    boss_gluthAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroicMode = pCreature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;

    uint32 MortalWound_Timer;
    uint32 Decimate_Timer;
    uint32 Enrage_Timer;
    uint32 Summon_Timer;
	int SummonNum;

    uint32 m_uiBerserkTimer;

    void Reset()
    {
        MortalWound_Timer = 8000;
        Decimate_Timer = 100000;
        Enrage_Timer = 60000;
        Summon_Timer = 10000;
		SummonNum = m_bIsHeroicMode ? 2 : 1;
        m_uiBerserkTimer = MINUTE*8*IN_MILISECONDS;
		if(GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_CONS_GLUT_EXIT_DOOR)))
			pDoor->SetGoState(GO_STATE_READY);
    }
	void Aggro(Unit *)
	{
		if(GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_CONS_GLUT_EXIT_DOOR)))
			pDoor->SetGoState(GO_STATE_READY);
	}
	void JustDied(Unit *)
	{
		if(GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_CONS_GLUT_EXIT_DOOR)))
			pDoor->SetGoState(GO_STATE_ACTIVE);
	}
	void JustSummoned(Creature* p)
	{
		Unit* pPlayer = SelectUnit(SELECT_TARGET_RANDOM,0);
		if(pPlayer)
			p->AddThreat(pPlayer,0.0f);
	}

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        //MortalWound_Timer
        if (MortalWound_Timer < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_MORTALWOUND);
            MortalWound_Timer = 10000;
        }else MortalWound_Timer -= diff;
		//need corepatch
        //Decimate_Timer
        if (Decimate_Timer < diff)
        {
            DoCast(m_creature,SPELL_DECIMATE);
            Decimate_Timer = 100000;
        }else Decimate_Timer -= diff;

        //Enrage_Timer
        if (Enrage_Timer < diff)
        {
            DoCast(m_creature, m_bIsHeroicMode ? SPELL_ENRAGE_H : SPELL_ENRAGE);
            Enrage_Timer = 60000;
        }else Enrage_Timer -= diff;

        //Summon_Timer
        if (Summon_Timer < diff)
        {
			for(int i = 0;i<SummonNum;i++)
			{
				int pos = rand()%SUMM_POS_NUM;
				m_creature->SummonCreature(NPC_ZOMBIE_CHOW,AddSummPos[pos][0],AddSummPos[pos][1],AddSummPos[pos][2],0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000);						
			}
            Summon_Timer = 10000;
        } else Summon_Timer -= diff;

        //m_uiBerserkTimer
        if (m_uiBerserkTimer < diff)
        {
            DoCast(m_creature, SPELL_BERSERK, true);
            m_uiBerserkTimer = MINUTE*5*IN_MILISECONDS;
        }else m_uiBerserkTimer -= diff;

        DoMeleeAttackIfReady();
    }
};
struct MANGOS_DLL_DECL mob_zombie_chowAI : public ScriptedAI
{
    mob_zombie_chowAI(Creature* pCreature) : ScriptedAI(pCreature){Reset();}
	Unit* gluth;
    void Reset()
	{
		gluth = NULL;
	}
	void Aggro(Unit* who){}
	void JustDied(Unit*)
	{
		m_creature->RemoveCorpse();
	}
	//workaround
	void SpellHit(Unit* caster,const SpellEntry* spell)
	{
		if(spell->Id != SPELL_DECIMATE)
			return;
		gluth = caster;

		m_creature->GetMotionMaster()->MovementExpired();
		m_creature->GetMotionMaster()->MovePoint(0,caster->GetPositionX(),caster->GetPositionY(),caster->GetPositionZ());
	}
	void MovementInform(uint32 type,uint32 id)
	{
		if(type != POINT_MOTION_TYPE && id != 0)
			return;
		if(gluth)
			gluth->ModifyHealth((gluth->GetMaxHealth()*5/100)+gluth->GetHealth());
		m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
	}
};
CreatureAI* GetAI_boss_gluth(Creature* pCreature)
{
    return new boss_gluthAI(pCreature);
}
CreatureAI* GetAI_mob_zombie_chow(Creature* pCreature)
{
    return new mob_zombie_chowAI(pCreature);
}


void AddSC_boss_gluth()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_gluth";
    newscript->GetAI = &GetAI_boss_gluth;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_zombie_chow";
    newscript->GetAI = &GetAI_mob_zombie_chow;
    newscript->RegisterSelf();
}
