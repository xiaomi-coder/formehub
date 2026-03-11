#include "../../Includes.h"

void EntityList::UpdateEntities()
{
	m_vecEntities.clear();

	for(CEntityIdentity* pEntity = g_Interfaces.m_GameEntitySystem.m_pFirst; pEntity != nullptr; pEntity = pEntity->m_pNext())
	{
		C_BaseEntity* pBaseEntity = reinterpret_cast<C_BaseEntity*>(pEntity->m_pInstance());
		if (!pBaseEntity)
			continue;

		std::uintptr_t uBase = reinterpret_cast<std::uintptr_t>(pBaseEntity);
		if (uBase < 0x10000) continue;

		const std::string strSchemaName = pBaseEntity->GetSchemaName();
		if (strSchemaName.empty()) continue;

		const FNV1A_t uSchemaNameHash = FNV1A::Hash(strSchemaName.c_str());

		switch (uSchemaNameHash)
		{
		case FNV1A::HashConst("CCSPlayerController"):
		{
			m_vecEntities.emplace_back(EntityObject_t(pBaseEntity, pBaseEntity->GetRefEHandle().GetEntryIndex(), EEntityType::ENTITY_PLAYER));
			break;
		}
		case FNV1A::HashConst("C_PlantedC4"):
		case FNV1A::HashConst("CPlantedC4"):
		{
			m_vecEntities.emplace_back(EntityObject_t(pBaseEntity, pBaseEntity->GetRefEHandle().GetEntryIndex(), EEntityType::ENTITY_PLANTEDC4));
			break;
		}
		}
	}
}
