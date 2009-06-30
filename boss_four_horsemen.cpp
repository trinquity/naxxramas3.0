#include "precompiled.h"
#include "def_naxxramas.h"

//all horsemen
#define SPELL_BESERK                26662

//lady blaumeux
#define SAY_BLAU_AGGRO              -1533044
#define SAY_BLAU_TAUNT1             -1533045
#define SAY_BLAU_TAUNT2             -1533046
#define SAY_BLAU_TAUNT3             -1533047
#define SAY_BLAU_SPECIAL            -1533048
#define SAY_BLAU_SLAY               -1533049
#define SAY_BLAU_DEATH              -1533050

#define SPELL_MARK_OF_BLAUMEUX      28833
#define SPELL_UNYILDING_PAIN        57381
#define SPELL_VOIDZONE              28863
#define H_SPELL_VOIDZONE            57463
#define SPELL_SHADOW_BOLT           57374
#define H_SPELL_SHADOW_BOLT         57464

//baron rivendare
#define SAY_RIVE_AGGRO1             -1533065
#define SAY_RIVE_AGGRO2             -1533066
#define SAY_RIVE_AGGRO3             -1533067
#define SAY_RIVE_SLAY1              -1533068
#define SAY_RIVE_SLAY2              -1533069
#define SAY_RIVE_SPECIAL            -1533070
#define SAY_RIVE_TAUNT1             -1533071
#define SAY_RIVE_TAUNT2             -1533072
#define SAY_RIVE_TAUNT3             -1533073
#define SAY_RIVE_DEATH              -1533074

#define SPELL_MARK_OF_RIVENDARE     28834
#define SPELL_UNHOLY_SHADOW         28882
#define H_SPELL_UNHOLY_SHADOW       57369

//thane korthazz
#define SAY_KORT_AGGRO              -1533051
#define SAY_KORT_TAUNT1             -1533052
#define SAY_KORT_TAUNT2             -1533053
#define SAY_KORT_TAUNT3             -1533054
#define SAY_KORT_SPECIAL            -1533055
#define SAY_KORT_SLAY               -1533056
#define SAY_KORT_DEATH              -1533057

#define SPELL_MARK_OF_KORTHAZZ      28832
#define SPELL_METEOR                26558                   // m_creature->getVictim() auto-area spell but with a core problem

//sir zeliek
#define SAY_ZELI_AGGRO              -1533058
#define SAY_ZELI_TAUNT1             -1533059
#define SAY_ZELI_TAUNT2             -1533060
#define SAY_ZELI_TAUNT3             -1533061
#define SAY_ZELI_SPECIAL            -1533062
#define SAY_ZELI_SLAY               -1533063
#define SAY_ZELI_DEATH              -1533064

#define SPELL_MARK_OF_ZELIEK        28835
#define SPELL_HOLY_WRATH            28883
#define H_SPELL_HOLY_WRATH          57466
#define SPELL_HOLY_BOLT             57376
#define H_SPELL_HOLY_BOLT           57465

#define SPELL_CONDEMNATION			57377

struct riderpos
{
uint32 npc;
float x,y,z;
};
riderpos RidersPos[] =
{
	//Thane - jobb felso
	{NPC_THANE,2534,-3022,241.344894},
	//baron - bal felso
	{NPC_RIVENDARE,2586,-2960,241.344894},
	//blaumeux - jobb also
	{NPC_BLAUMEUX,2465,-2952,241.344894},
	//zeilek - bal also
	{NPC_ZELIEK,2518,-2898,241.344894}
};
struct MANGOS_DLL_DECL boss_horsemenAI : public ScriptedAI
{
     boss_horsemenAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
	pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
	IsHeroicMode = pCreature->GetMap()->IsHeroic();
	Reset();
	}
	ScriptedInstance* pInstance;
    bool IsHeroicMode;
	bool InCombat;
	bool ReadyForAnything;
    void Reset(){}
	void EnterEvadeMode()
	{
		 m_creature->RemoveAllAuras();
		 m_creature->DeleteThreatList();
		 m_creature->CombatStop(true);
		 m_creature->LoadCreaturesAddon();
		 InCombat = false;
		 if(pInstance->GetData(DATA_FOUR_HORSEMAN_EVENT) != NOT_STARTED)
			 pInstance->SetData(DATA_FOUR_HORSEMAN_EVENT,NOT_STARTED);
		 if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_MILI_HORSEMEN_DOOR)))
			 if(pDoor->GetGoState() != GO_STATE_ACTIVE)
				pDoor->SetGoState(GO_STATE_ACTIVE);

	   m_creature->SetLootRecipient(NULL);
	   	if (m_creature->isAlive())
		{
	       m_creature->GetMotionMaster()->MoveTargetedHome();
		for(int i = 0;i<4;i++)
		{
			if(RidersPos[i].npc == m_creature->GetEntry())
				continue;
			Creature* Temp = (Creature*)Unit::GetUnit(*m_creature,pInstance->GetData64(RidersPos[i].npc));
			if(!Temp)
				continue;
			if(!Temp->isAlive())
				Temp->Respawn();
			if(((boss_horsemenAI*)Temp->AI())->InCombat == true)
			{

				Temp->AI()->EnterEvadeMode();
			}
		}
			
		}	   
	   Reset();
	}
	void AttackStart(Unit* who)
	{
		if (!who)
        return;
		if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_MILI_HORSEMEN_DOOR)))
			if(pDoor->GetGoState() != GO_STATE_READY)
				pDoor->SetGoState(GO_STATE_READY);
		if(pInstance->GetData(DATA_FOUR_HORSEMAN_EVENT) != IN_PROGRESS)
			 pInstance->SetData(DATA_FOUR_HORSEMAN_EVENT,IN_PROGRESS);
		
		if (m_creature->Attack(who, true))
		{
		InCombat = true;
        m_creature->AddThreat(who, 0.0f);
        m_creature->SetInCombatWith(who);
        who->SetInCombatWith(m_creature);
		for(int i = 0;i<4;i++)
		{
			if(m_creature->GetEntry() == RidersPos[i].npc)
			{
				m_creature->SetSpeed(MOVE_WALK,7.0f);
				m_creature->GetMotionMaster()->MovementExpired();
				m_creature->GetMotionMaster()->MovePoint(0,RidersPos[i].x,RidersPos[i].y,RidersPos[i].z);
			}
		}
		}
	}
	void JustDied(Unit *killer)
	{
		int died_riders = 1;
		for(int i = 0;i<4;i++)
		{
			Creature* Temp = (Creature*)Unit::GetUnit(*m_creature,pInstance->GetData64(RidersPos[i].npc));
			if(Temp && !Temp->isAlive())
				died_riders++;
		}
		if(died_riders == 4)
		{
			pInstance->SetData(DATA_FOUR_HORSEMAN_EVENT,DONE);
			if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_MILI_HORSEMEN_DOOR)))
				 if(pDoor->GetGoState() != GO_STATE_ACTIVE)
					pDoor->SetGoState(GO_STATE_ACTIVE);
			if(GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_CHEST_HORSEMEN_NORM)))
				pDoor->Respawn();
		}

	}
    void Aggro(Unit *who)
    {
    }
	//return closest unit(player)
	Unit* ClosestPlayer()
	{
			 Unit* pTemp = NULL;
			 float pTemp_d = 0;
			 float closest_distance = m_creature->GetDistance2d(m_creature->getVictim());
			 Unit* closest_player = m_creature->getVictim();
			 
            std::list<HostilReference*>::iterator i = m_creature->getThreatManager().getThreatList().begin();

            for (i = m_creature->getThreatManager().getThreatList().begin(); i!=m_creature->getThreatManager().getThreatList().end(); ++i)
            {
				pTemp = Unit::GetUnit((*m_creature),(*i)->getUnitGuid());
				if(pTemp->GetTypeId() != TYPEID_PLAYER)
					continue;
				pTemp_d = pTemp->GetDistance2d(m_creature);
				if(pTemp_d < closest_distance)
				{
					closest_player = pTemp;
					closest_distance = pTemp_d;
				}
			}
			return closest_player;
	}
	Unit* GetRandomPlayerInRange()
	{
		Unit *target = NULL;
            std::list<HostilReference *> t_list = m_creature->getThreatManager().getThreatList();
            std::vector<Unit *> target_list;
            for(std::list<HostilReference *>::iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
            {
                target = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());
                if (target->GetTypeId() != TYPEID_PLAYER)
                    continue;

                //45 yard radius maximum
                if (target && target->IsWithinDist(m_creature, 45.0f, false))
                    target_list.push_back(target);

                target = NULL;
            }

            if (target_list.size())
                target = *(target_list.begin()+rand()%target_list.size());
            else
                target = NULL;
		return target;
	}
	void MovementInform(uint32 type,uint32 id)
	{
		if(type != POINT_MOTION_TYPE && id != 0)
			return;
		ReadyForAnything = true;
		for(int i = 0;i<4;i++)
		{
			if(RidersPos[i].npc == m_creature->GetEntry())
				m_creature->Relocate(RidersPos[i].x,RidersPos[i].y,RidersPos[i].z);
		}
	}
};

struct MANGOS_DLL_DECL boss_thane_korthazzAI : public boss_horsemenAI
{
    boss_thane_korthazzAI(Creature* pCreature) : boss_horsemenAI(pCreature)
	{
	Reset();
	}
	ScriptedInstance* pInstance;
    bool IsHeroicMode;
    uint32 Mark_Timer;
    uint32 Meteor_Timer;

    void Reset()
    {
        Mark_Timer = 20000;                                 // First Horsemen Mark is applied at 20 sec.
        Meteor_Timer = 15000;                               // wrong
		ReadyForAnything = false;
    }
    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		if(!ReadyForAnything)
			return;

		if(m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != TARGETED_MOTION_TYPE)
		{
			m_creature->GetMotionMaster()->MovementExpired();
			SetCombatMovement(true);
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
		}
        // Mark of Korthazz
        if (Mark_Timer < diff)
        {
            DoCast(m_creature,SPELL_MARK_OF_KORTHAZZ);
            Mark_Timer = 12000;
        }else Mark_Timer -= diff;

        // Meteor
        if (Meteor_Timer < diff)
        {
			Unit* target = GetRandomPlayerInRange();
			if(target)
				DoCast(target,SPELL_METEOR);
            Meteor_Timer = 20000;                           // wrong
        }else Meteor_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_thane_korthazz(Creature* pCreature)
{
    return new boss_thane_korthazzAI(pCreature);
}
struct MANGOS_DLL_DECL boss_rivendare_naxxAI : public boss_horsemenAI
{
    boss_rivendare_naxxAI(Creature* pCreature) : boss_horsemenAI(pCreature)
	{
	Reset();
	}
	ScriptedInstance* pInstance;

	uint32 Mark_Timer;
	uint32 Unholy_Shadow_Timer;
	void Reset()
    {
		Mark_Timer = 20000;
		Unholy_Shadow_Timer = 15000;
		ReadyForAnything = false;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		if(!ReadyForAnything)
			return;
		if(m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != TARGETED_MOTION_TYPE)
		{
			m_creature->GetMotionMaster()->MovementExpired();
			SetCombatMovement(true);
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
		}
		if(Mark_Timer < diff)
		{
			DoCast(m_creature,SPELL_MARK_OF_RIVENDARE);
			Mark_Timer = 12000;
		}else Mark_Timer -= diff;

		if(Unholy_Shadow_Timer < diff)
		{
			DoCast(m_creature->getVictim(),IsHeroicMode ? H_SPELL_UNHOLY_SHADOW : SPELL_UNHOLY_SHADOW);
			Unholy_Shadow_Timer = 15000;
		}else Unholy_Shadow_Timer -= diff;
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_rivendare_naxx(Creature* pCreature)
{
    return new boss_rivendare_naxxAI(pCreature);
}

struct MANGOS_DLL_DECL boss_lady_blaumeuxAI : public boss_horsemenAI
{
    boss_lady_blaumeuxAI(Creature* pCreature) : boss_horsemenAI(pCreature)
	{
	Reset();
	}

    uint32 Mark_Timer;
    uint32 VoidZone_Timer;
	uint32 ShadowBolt_Timer;
	uint32 distance_timer;
	Unit* closest_player;

    void Reset()
    {
        Mark_Timer = 20000;                                 // First Horsemen Mark is applied at 20 sec.
        VoidZone_Timer = 12000;                             // right
		distance_timer = 5000;
		ShadowBolt_Timer = 10000; //must be later than distance_timer
		ReadyForAnything = false;
		closest_player = NULL;
	}
    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		if(!ReadyForAnything)
			return;

		if(m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != IDLE_MOTION_TYPE)
		{
			m_creature->GetMotionMaster()->MovementExpired();
			m_creature->GetMotionMaster()->MoveIdle();
			m_creature->StopMoving();
		}
		//if we have valid target, cast shbolt else raidwiping spell
		if(ShadowBolt_Timer < diff)
		{
			closest_player = ClosestPlayer();
			if(closest_player && closest_player->IsWithinDist(m_creature, 45.0f, false))
			{
				DoCast(closest_player,IsHeroicMode ? H_SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT);
			}else{
				DoCast(m_creature,SPELL_UNYILDING_PAIN);
			}
			ShadowBolt_Timer = 2000;
		}else ShadowBolt_Timer -= diff;

        // Mark of Blaumeux
        if (Mark_Timer < diff)
        {
			m_creature->CastStop();
            DoCast(m_creature,SPELL_MARK_OF_BLAUMEUX,true);
            Mark_Timer = 12000;
        }else Mark_Timer -= diff;

        // Void Zone
        if (VoidZone_Timer < diff)
        {
			Unit* target = GetRandomPlayerInRange();
			if(target)
				DoCast(target,SPELL_VOIDZONE,true);
            VoidZone_Timer = 12000;
        }else VoidZone_Timer -= diff;
    }
};

CreatureAI* GetAI_boss_lady_blaumeux(Creature* pCreature)
{
    return new boss_lady_blaumeuxAI(pCreature);
}
struct MANGOS_DLL_DECL boss_sir_zeliekAI : public boss_horsemenAI
{
    boss_sir_zeliekAI(Creature* pCreature) : boss_horsemenAI(pCreature)
	{
	Reset();
	}
    uint32 Mark_Timer;
    uint32 HolyWrath_Timer;

	 uint32 distance_timer;
	 uint32 HolyBolt_Timer;
	 Unit* closest_player;
    void Reset()
    {
        Mark_Timer = 20000;                                 // First Horsemen Mark is applied at 20 sec.
        HolyWrath_Timer = 12000;                            // right
		distance_timer = 5000;
		HolyBolt_Timer = 11000;
		ReadyForAnything = false;
		closest_player = NULL;
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;
		if(!ReadyForAnything)
			return;

		//set movementgenerator
		if(m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != IDLE_MOTION_TYPE)
		{
			m_creature->GetMotionMaster()->MovementExpired();
			m_creature->GetMotionMaster()->MoveIdle();
			m_creature->StopMoving();
		}

		//if we have valid target, cast holybolt else raidwiping spell
		if(HolyBolt_Timer < diff)
		{
			closest_player = ClosestPlayer();
			if (closest_player && closest_player->IsWithinDist(m_creature, 45.0f, false))
			{
				DoCast(closest_player,IsHeroicMode ? H_SPELL_HOLY_BOLT : SPELL_HOLY_BOLT);
			}else{
				DoCast(m_creature,SPELL_CONDEMNATION);
			}
			HolyBolt_Timer = 2000;
		}else HolyBolt_Timer -= diff;

        // Mark of Zeliek
        if (Mark_Timer < diff)
        {
			m_creature->CastStop();
            DoCast(m_creature,SPELL_MARK_OF_ZELIEK,true);
            Mark_Timer = 12000;
        }else Mark_Timer -= diff;

        // Holy Wrath
        if (HolyWrath_Timer < diff)
        {
			Unit* target = GetRandomPlayerInRange();
			if(target)
			DoCast(target,SPELL_HOLY_WRATH,true);
            HolyWrath_Timer = 12000;
        }else HolyWrath_Timer -= diff;
    }
};

CreatureAI* GetAI_boss_sir_zeliek(Creature* pCreature)
{
    return new boss_sir_zeliekAI(pCreature);
}
void AddSC_boss_four_horsemen()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_lady_blaumeux";
    newscript->GetAI = &GetAI_boss_lady_blaumeux;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_rivendare_naxx";
    newscript->GetAI = &GetAI_boss_rivendare_naxx;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_thane_korthazz";
    newscript->GetAI = &GetAI_boss_thane_korthazz;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_sir_zeliek";
    newscript->GetAI = &GetAI_boss_sir_zeliek;
    newscript->RegisterSelf();
}
