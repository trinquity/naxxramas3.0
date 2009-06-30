#include "precompiled.h"
#include "def_naxxramas.h"

#define TO_TELE_X 0
#define TO_TELE_Y 0
#define TO_TELE_Z 0
bool GOnaxx_teleporter_thaddius(Player* pPlayer, GameObject* pGo)
{
ScriptedInstance* m_pInstance;
m_pInstance = (ScriptedInstance*)pGo->GetInstanceData();
if(m_pInstance->GetData(DATA_THADDIUS_EVENT) == DONE)
	pPlayer->relocate(TO_TELE_X,TO_TELE_Y,TO_TELE_Z);
}
bool GOnaxx_teleporter_loatheb(Player* pPlayer, GameObject* pGo)
{
ScriptedInstance* m_pInstance;
m_pInstance = (ScriptedInstance*)pGo->GetInstanceData();
if(m_pInstance->GetData(DATA_LOATHEB_EVENT) == DONE)
	pPlayer->relocate(TO_TELE_X,TO_TELE_Y,TO_TELE_Z);
}
bool GOnaxx_teleporter_maexxna(Player* pPlayer, GameObject* pGo)
{
ScriptedInstance* m_pInstance;
m_pInstance = (ScriptedInstance*)pGo->GetInstanceData();
if(m_pInstance->GetData(DATA_MAEXXNA_EVENT) == DONE)
	pPlayer->relocate(TO_TELE_X,TO_TELE_Y,TO_TELE_Z);
}
bool GOnaxx_teleporter_horseman(Player* pPlayer, GameObject* pGo)
{
ScriptedInstance* m_pInstance;
m_pInstance = (ScriptedInstance*)pGo->GetInstanceData();
if(m_pInstance->GetData(DATA_HORSEMAN_EVENT) == DONE)
	pPlayer->relocate(TO_TELE_X,TO_TELE_Y,TO_TELE_Z);
}
void AddSC_naxxramas()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "naxx_teleporter_thaddius";
    newscript->pGOHello = &GOnaxx_teleporter_thaddius;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "naxx_teleporter_horseman";
    newscript->pGOHello = &GOnaxx_teleporter_horseman;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "naxx_teleporter_maexxna";
    newscript->pGOHello = &GOnaxx_teleporter_maexxna;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "naxx_teleporter_loatheb";
    newscript->pGOHello = &GOnaxx_teleporter_loatheb;
    newscript->RegisterSelf();
}