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
SDName: Boss_Sapphiron
SD%Complete: 99
SDComment: 
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"

#define EMOTE_BREATH            -1533082
#define EMOTE_ENRAGE            -1533083

#define SPELL_ICEBOLT           28522
#define SPELL_FROST_BREATH      29318
#define SPELL_FROST_AURA        28531
#define H_SPELL_FROST_AURA		55799
#define SPELL_LIFE_DRAIN        28542
#define H_SPELL_LIFE_DRAIN      55665
#define SPELL_BLIZZARD          28547
#define SPELL_BESERK            26662

#define CENTER_X 3522.59
#define CENTER_Y -5234.49
#define CENTER_Z 137.62

struct MANGOS_DLL_DECL boss_sapphironAI : public ScriptedAI
{
    boss_sapphironAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		isHeroicMode = pCreature->GetMap()->IsHeroic();
		SpellEntry *LifeDrain = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_LIFE_DRAIN);
		if(LifeDrain)
			LifeDrain->MaxAffectedTargets = 2;
		SpellEntry *LifeDrain_h = (SpellEntry*)GetSpellStore()->LookupEntry(H_SPELL_LIFE_DRAIN);
		if(LifeDrain_h)
			LifeDrain_h->MaxAffectedTargets = 5;
		pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }
	ScriptedInstance* pInstance;
	bool isHeroicMode;
    uint32 Icebolt_Count;
	uint32 Icebolt_Max_Count;
    uint32 Icebolt_Timer;
    uint32 FrostBreath_Timer;
    uint32 LifeDrain_Timer;
    uint32 Blizzard_Timer;
    uint32 Fly_Timer;
    uint32 Beserk_Timer;
    uint32 phase;
    uint32 land_Timer;
	Unit* IceBoltTargets[3];
    void Reset()
    {
        FrostBreath_Timer = 2500;
        LifeDrain_Timer = 24000;
        Blizzard_Timer = 20000;
        Fly_Timer = 45000;
        Icebolt_Timer = 4000;
        land_Timer = 2000+FrostBreath_Timer;
        Beserk_Timer = 15*60*1000;
        phase = 1;
		Icebolt_Max_Count = isHeroicMode ? 3 : 2;
		Icebolt_Count = 0;
    }
	void Aggro(Unit* who)
	{
		DoCast(m_creature,isHeroicMode ? H_SPELL_FROST_AURA : SPELL_FROST_AURA);
	}

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if (phase == 1)
        {
            if (LifeDrain_Timer < diff)
            {
				DoCast(m_creature,isHeroicMode ? H_SPELL_LIFE_DRAIN : SPELL_LIFE_DRAIN);
                LifeDrain_Timer = 24000;
            }else LifeDrain_Timer -= diff;

            if (Blizzard_Timer < diff)
            {
                if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,1))
                    DoCast(target,SPELL_BLIZZARD);
                Blizzard_Timer = 20000;
            }else Blizzard_Timer -= diff;

            if (m_creature->GetHealth()*100 / m_creature->GetMaxHealth() > 10)
            {
				if (Fly_Timer < diff && m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE)
                {
					m_creature->GetMotionMaster()->MovementExpired();
					m_creature->GetMotionMaster()->MovePoint(0,CENTER_X,CENTER_Y,CENTER_Z);
                    Icebolt_Timer = 4000;
                }else Fly_Timer -= diff;
            }
        }else{
			if(Icebolt_Count < Icebolt_Max_Count)
			{
				if (Icebolt_Timer < diff)
				{
		            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
					{
	                    DoCast(target,SPELL_ICEBOLT);
						IceBoltTargets[Icebolt_Count] = target;
					}
					if(Icebolt_Count == Icebolt_Max_Count-1)
					{
						DoScriptText(EMOTE_BREATH, m_creature);
						FrostBreath_Timer = 7000;
						land_Timer = FrostBreath_Timer+2000;
					}
					Icebolt_Count++;
				    Icebolt_Timer = 3000;
				}else Icebolt_Timer -= diff;
			}else{
				if (FrostBreath_Timer < diff)
                {
                    DoCast(m_creature,SPELL_FROST_BREATH);
                    FrostBreath_Timer = 7000;
                }else FrostBreath_Timer -= diff;
				if (land_Timer < diff)
				{
				    phase = 1;
					for(int i = 0;i<Icebolt_Max_Count;i++)
					{
						if(IceBoltTargets[i]->isAlive() && IceBoltTargets[i]->HasAura(SPELL_ICEBOLT,0))
							IceBoltTargets[i]->RemoveAurasDueToSpell(SPELL_ICEBOLT);
					}
					m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
					m_creature->RemoveMonsterMoveFlag(MONSTER_MOVE_LEVITATING);
					m_creature->GetMotionMaster()->MovementExpired();
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    Fly_Timer = 45000;
					}else land_Timer -= diff;
			}

		}
           if (Beserk_Timer < diff)
            {
                DoScriptText(EMOTE_ENRAGE, m_creature);
                DoCast(m_creature,SPELL_BESERK);
                Beserk_Timer = 300000;
            }else Beserk_Timer -= diff;

        if (phase!=2)
            DoMeleeAttackIfReady();
    }
	void MovementInform(uint32 type,uint32 id)
	{
		if(type != POINT_MOTION_TYPE && id != 0)
			return;
		 m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
		 m_creature->AddMonsterMoveFlag(MONSTER_MOVE_LEVITATING);
		 phase = 2;
	}
};

CreatureAI* GetAI_boss_sapphiron(Creature* pCreature)
{
    return new boss_sapphironAI(pCreature);
}

void AddSC_boss_sapphiron()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_sapphiron";
    newscript->GetAI = &GetAI_boss_sapphiron;
    newscript->RegisterSelf();
}
