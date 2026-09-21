#include "MONSTER/MonsterSpawn.h"

#include "Windows.h"

#include <D2BitManip.h>
#include <D2CMP.h>
#include <D2Collision.h>
#include <D2DataTbls.h>
#include <D2Dungeon.h>
#include <D2Monsters.h>
#include <DataTbls/LevelsTbls.h>
#include <DataTbls/MonsterIds.h>
#include <DataTbls/SkillsIds.h>
#include <D2Skills.h>
#include <D2StatList.h>
#include <D2States.h>


#include "AI/AiGeneral.h"
#include "GAME/Clients.h"
#include "ITEMS/Items.h"
#include "MONSTER/Monster.h"
#include "MONSTER/MonsterAI.h"
#include "MONSTER/MonsterMode.h"
#include "MONSTER/MonsterRegion.h"
#include "MONSTER/MonsterUnique.h"
#include "QUESTS/Quests.h"
#include "SKILLS/SkillAss.h"
#include "UNIT/SUnit.h"


//D2Game.0x6FC68350
int32_t __fastcall sub_6FC68350(int32_t nMonsterId, D2ActiveRoomStrc* pRoom, int32_t nX, int32_t nY, int32_t a5)
{
    D2MonStatsTxt* pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(nMonsterId);
    if (pMonStatsTxtRecord)
    {
        nMonsterId = pMonStatsTxtRecord->nBaseId;
    }

    if (nMonsterId < 0 || nMonsterId >= sgptDataTables->nMonStatsTxtRecordCount)
    {
        return 1;
    }

    int32_t nSpawnX = 0;
    int32_t nSpawnY = 0;
    uint16_t nMask = 0;

    switch (nMonsterId)
    {
    case MONSTER_CROWNEST1:
        nSpawnX = nX;
        nSpawnY = nY + 3;
        nMask = COLLIDE_MASK_MONSTER_PATH;
        break;

    case MONSTER_SARCOPHAGUS:
        nSpawnX = nX;
        nSpawnY = nY + 2;
        nMask = COLLIDE_MASK_MONSTER_PATH;
        break;

    case MONSTER_VILEMOTHER1:
        if (a5)
        {
            return 1;
        }

        nSpawnX = nX;
        nSpawnY = nY;
        nMask = COLLIDE_MASK_MONSTER_PATH;
        break;

    case MONSTER_SUCKERNEST1:
        nSpawnX = nX - 2;
        nSpawnY = nY - 2;
        nMask = 0x1C0;
        break;

    case MONSTER_EVILHUT:
        nSpawnX = nX + 2;
        nSpawnY = nY + 4;
        nMask = COLLIDE_MASK_MONSTER_PATH;
        break;

    default:
        return 1;
    }

    pRoom = D2GAME_GetRoom_6FC52070(pRoom, nSpawnX, nSpawnY);
    if (!pRoom || COLLISION_CheckMaskWithSize(pRoom, nSpawnX, nSpawnY, 2, nMask))
    {
        return 0;
    }

    return 1;
}

//D2Game.0x6FC68630
int32_t __fastcall sub_6FC68630(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nSkillId, D2UnitStrc* pTarget, int32_t nX, int32_t nY)
{
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 0;
    }

    if (nSkillId == SKILL_NEST)
    {
        return 0;
    }

    if (pSkillsTxtRecord->dwFlags[1] & gdwBitMasks[SKILLSFLAGINDEX2_TGTPLACECHECK])
    {
        if (pTarget && pTarget->dwFlags & UNITFLAG_TARGETABLE)
        {
            return COLLISION_CheckAnyCollisionWithPattern(UNITS_GetRoom(pTarget), CLIENTS_GetUnitX(pTarget), CLIENTS_GetUnitY(pTarget), PATH_GetUnitCollisionPattern(pTarget), COLLIDE_MASK_MONSTER_PATH) == 0;
        }

        return 0;
    }

    if (nSkillId == SKILL_ANDRIALSPRAY)
    {
        return pTarget != nullptr;
    }

    const uint16_t nSrvDoFunc = pSkillsTxtRecord->wSrvDoFunc;
    if (nSrvDoFunc == 77 || nSrvDoFunc == 78)
    {
        if (!pTarget || MONSTERS_IsDead(pUnit))
        {
            return 0;
        }

        const int32_t nUnitX = CLIENTS_GetUnitX(pUnit);
        const int32_t nUnitY = CLIENTS_GetUnitY(pUnit);

        const int32_t nTargetX = 2 * CLIENTS_GetUnitX(pTarget) - nUnitX;
        const int32_t nTargetY = 2 * CLIENTS_GetUnitY(pTarget) - nUnitY;

        D2CoordStrc coords = {};
        coords.nX = nTargetX;
        coords.nY = nTargetY;
        
        D2ActiveRoomStrc* pRoom = COLLISION_GetFreeCoordinates(UNITS_GetRoom(pUnit), &coords, UNITS_GetUnitSizeX(pUnit), COLLIDE_MASK_MONSTER_PATH, 0);
        if (!pRoom || DUNGEON_IsRoomInTown(pRoom) || COLLISION_CheckAnyCollisionWithPattern(pRoom, nTargetX, nTargetY, PATH_GetUnitCollisionPattern(pUnit), COLLIDE_MASK_MONSTER_PATH))
        {
            return 0;
        }

        return D2Common_11025(nUnitX, nUnitY, nTargetX, nTargetY, pRoom, 0xC01) != 0;
    }

    if (nSkillId == SKILL_DIABPRISON)
    {
        return sub_6FC6A810(pGame, UNITS_GetRoom(pTarget), 0, 0, pTarget, MONSTER_BONEPRISON1, 1);
    }

    if (nSkillId == SKILL_MONTELEPORT)
    {
        if (!pUnit)
        {
            return 0;
        }

        D2ActiveRoomStrc* pRoom = D2GAME_GetRoom_6FC52070(UNITS_GetRoom(pUnit), nX, nY);
        if (!pRoom || DUNGEON_IsRoomInTown(pRoom) || COLLISION_CheckAnyCollisionWithPattern(pRoom, nX, nY, PATH_GetUnitCollisionPattern(pUnit), COLLIDE_MASK_MONSTER_PATH))
        {
            return 0;
        }

        return D2Common_11025(CLIENTS_GetUnitX(pUnit), CLIENTS_GetUnitY(pUnit), nX, nY, pRoom, 0xC01) != 0;
    }

    if (nSrvDoFunc == 67)
    {
        if (!pUnit)
        {
            return 0;
        }

        D2ActiveRoomStrc* pRoom = D2GAME_GetRoom_6FC52070(UNITS_GetRoom(pUnit), nX, nY);
        if (!pRoom || DUNGEON_IsRoomInTown(pRoom))
        {
            return 0;
        }

        return D2Common_11025(CLIENTS_GetUnitX(pUnit), CLIENTS_GetUnitY(pUnit), nX, nY, pRoom, 0xC01) != 0;
    }

    if (nSkillId == SKILL_DESERTTURRET)
    {
        if (!pUnit)
        {
            return 0;
        }

        D2ActiveRoomStrc* pRoom = D2GAME_GetRoom_6FC52070(UNITS_GetRoom(pUnit), nX, nY);
        if (!pRoom || DUNGEON_IsRoomInTown(pRoom))
        {
            return 0;
        }

        return D2Common_11025(CLIENTS_GetUnitX(pUnit), CLIENTS_GetUnitY(pUnit), nX, nY, pRoom, COLLIDE_MASK_RADIAL_BARRIER) != 0;
    }

    return 1;
}

//D2Game.0x6FC68CC0
int32_t __fastcall MONSTERSPAWN_GetResurrectMode(D2UnitStrc* pUnit, int32_t a2)
{
    if (!a2 || !pUnit)
    {
        return MONMODE_NEUTRAL;
    }

    D2MonStats2Txt* pMonStats2TxtRecord = MONSTERREGION_GetMonStats2TxtRecord(pUnit->dwClassId);
    if (!pMonStats2TxtRecord)
    {
        return MONMODE_NEUTRAL;
    }

    const int32_t nSkillId = pMonStats2TxtRecord->nResurrectSkill;
    if (nSkillId > 0 && nSkillId < sgptDataTables->nSkillsTxtRecordCount)
    {
        UNITS_SetUsedSkill(pUnit, SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, nSkillId));
    }

    if (pMonStats2TxtRecord->nResurrectMode < 16)
    {
        return pMonStats2TxtRecord->nResurrectMode;
    }

    return MONMODE_NEUTRAL;
}

//D2Game.0x6FC68D70
D2UnitStrc* __fastcall sub_6FC68D70(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nMonsterId, int32_t nAnimMode, int32_t a5, int16_t nFlags)
{
    D2UnkMonCreateStrc monCreate = {};

    monCreate.nUnitGUID = 0;
    monCreate.nMonsterId = nMonsterId;
    monCreate.nAnimMode = nAnimMode;
    monCreate.pGame = pGame;
    monCreate.pRoomCoordList = nullptr;
    monCreate.pRoom = UNITS_GetRoom(pUnit);
    monCreate.field_20 = a5;
    monCreate.nFlags = nFlags;
    monCreate.nX = CLIENTS_GetUnitX(pUnit);
    monCreate.nY = CLIENTS_GetUnitY(pUnit);

    return D2GAME_SpawnNormalMonster_6FC68E30(&monCreate);
}

//1.10f:D2Game.0x6FC68E30) --------------------------------------------------------
//1.11: D2Game.0x6FCE79E0
D2UnitStrc* __fastcall D2GAME_SpawnNormalMonster_6FC68E30(D2UnkMonCreateStrc* pMonCreate)
{
	// Retail 1.10f at D2Game.0x6FC68E30, reconstructed from the live DLL.
	static const int32_t dword_6FD28BC8[][2] = {
		{ 0, -3 },
		{ 3, 0 },
		{ 0, 3 },
		{ -3, 0 },
	};

	if (!pMonCreate)
	{
		return nullptr;
	}

	const int32_t nMonsterId = pMonCreate->nMonsterId;
	D2MonStatsTxt* pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(nMonsterId);
	if (!pMonStatsTxtRecord)
	{
		return nullptr;
	}

	D2MonStats2Txt* pMonStats2TxtRecord = MONSTERREGION_GetMonStats2TxtRecord(nMonsterId);
	if (!pMonStats2TxtRecord)
	{
		return nullptr;
	}

	D2ActiveRoomStrc* pRoom = pMonCreate->pRoom;
	const int16_t nFlags = pMonCreate->nFlags;

	uint32_t nCollisionFlags = COLLIDE_MASK_MONSTER_PATH;
	switch (pMonStats2TxtRecord->nSpawnCol)
	{
	case 1:
		nCollisionFlags = 0x1C0;
		break;
	case 2:
		nCollisionFlags = COLLIDE_MASK_PLACEMENT;
		break;
	case 3:
		nCollisionFlags = 0;
		break;
	default:
		nCollisionFlags = COLLIDE_MASK_MONSTER_PATH;
		break;
	}

	RECT spawnRect = {};
	int32_t nRoomCoordListIndex = 0;
	int32_t bUseRoomCoordList = 0;
	if (pMonCreate->pRoomCoordList)
	{
		int32_t nLeft = pMonCreate->pRoomCoordList->pBox[1].nPosX;
		int32_t nTop = pMonCreate->pRoomCoordList->pBox[1].nPosY;
		int32_t nRight = pMonCreate->pRoomCoordList->pBox[1].nWidth;
		int32_t nBottom = pMonCreate->pRoomCoordList->pBox[1].nHeight;
		DUNGEON_GameTileToSubtileCoords(&nLeft, &nTop);
		DUNGEON_GameTileToSubtileCoords(&nRight, &nBottom);
		spawnRect.left = nLeft;
		spawnRect.top = nTop;
		spawnRect.right = nRight;
		spawnRect.bottom = nBottom;
		bUseRoomCoordList = 1;
		nRoomCoordListIndex = pMonCreate->pRoomCoordList->nIndex;
	}
	else if (pRoom)
	{
		D2DrlgCoordsStrc roomCoords = {};
		DUNGEON_GetRoomCoordinates(pRoom, &roomCoords);
		spawnRect.left = roomCoords.nSubtileX;
		spawnRect.top = roomCoords.nSubtileY;
		spawnRect.right = roomCoords.nSubtileX + roomCoords.nSubtileWidth;
		spawnRect.bottom = roomCoords.nSubtileY + roomCoords.nSubtileHeight;
	}

	if (!pRoom)
	{
		return nullptr;
	}

	D2SeedStrc* pSeed = &pRoom->pSeed;

	int32_t nRadius = 0;
	int32_t nMaxRadius = 0;
	if (pMonCreate->field_20 >= 0)
	{
		nRadius = 3;
		nMaxRadius = 3 * pMonCreate->field_20;
	}

	int32_t nSpawnX = 0;
	int32_t nSpawnY = 0;

	const int32_t bFloorSpawn = pMonStats2TxtRecord->nSpawnCol == 1
		&& (nMonsterId < MONSTER_TENTACLE1 || nMonsterId > MONSTER_TENTACLEHEAD3)
		&& nMonsterId != MONSTER_HELLMETEOR;

	if (bFloorSpawn)
	{
		int32_t nFloorCount = 0;
		D2DrlgTileDataStrc* pFloorTiles = DUNGEON_GetFloorTilesFromRoom(pRoom, &nFloorCount);
		if (pFloorTiles && nFloorCount > 0)
		{
			int32_t nIndex = ITEMS_RollLimitedRandomNumber(pSeed, nFloorCount);
			if (!nIndex)
			{
				nIndex = 1;
			}

			D2DrlgCoordsStrc roomCoords = {};
			DUNGEON_GetRoomCoordinates(pRoom, &roomCoords);
			const int32_t nStartIndex = nIndex - 1;
			if (nIndex != nStartIndex)
			{
				while (1)
				{
					D2DrlgTileDataStrc* pTileData = &pFloorTiles[nIndex];
					if (pTileData->pTile && (D2CMP_10079_GetTileFlags(pTileData->pTile) & TILE_FLAGS_WATER))
					{
						const int32_t nTileX = 5 * (roomCoords.nTileXPos + pTileData->nPosX) + 3;
						const int32_t nTileY = 5 * (roomCoords.nTileYPos + pTileData->nPosY) + 3;
						if (!COLLISION_CheckMask(pRoom, nTileX, nTileY, COLLIDE_MONSTER))
						{
							int32_t bPlaced = 0;
							for (int32_t i = 0; i < 4; ++i)
							{
								const int32_t nTestX = nTileX + dword_6FD28BC8[i][0];
								const int32_t nTestY = nTileY + dword_6FD28BC8[i][1];
								if (!COLLISION_CheckMaskWithSize(pRoom, nTestX, nTestY, 2, 0x1C09u))
								{
									nSpawnX = nTestX;
									nSpawnY = nTestY;
									bPlaced = 1;
									break;
								}
							}
							if (bPlaced)
							{
								break;
							}
						}
					}

					nIndex = (nIndex + 1) % nFloorCount;
					if (nIndex == nStartIndex)
					{
						break;
					}
				}
			}
		}
	}
	else
	{
		if (nRadius > nMaxRadius)
		{
			return nullptr;
		}

		while (nRadius <= nMaxRadius)
		{
			const int32_t nOriginX = pMonCreate->nX;
			const int32_t nOriginY = pMonCreate->nY;
			const uint64_t lSeed0 = SEED_RollRandomNumber(pSeed);
			const int32_t nHalf = nRadius;
			int32_t nStepX = 0;
			int32_t nStepY = 0;
			int32_t nOffsetX = 0;
			int32_t nOffsetY = 0;

			if (lSeed0 & 1)
			{
				nOffsetX = nRadius;
				nOffsetY = ITEMS_RollLimitedRandomNumber(pSeed, nHalf);
				nStepX = 0;
				nStepY = 1;
			}
			else
			{
				nOffsetX = ITEMS_RollLimitedRandomNumber(pSeed, nHalf);
				nOffsetY = nRadius;
				nStepX = 1;
				nStepY = 0;
			}

			const uint64_t lSeed1 = SEED_RollRandomNumber(pSeed);
			if (lSeed1 & 1)
			{
				nOffsetX = -nOffsetX;
			}
			const uint64_t lSeed2 = SEED_RollRandomNumber(pSeed);
			if (lSeed2 & 1)
			{
				nOffsetY = -nOffsetY;
			}

			int32_t nX = nOriginX + nOffsetX;
			int32_t nY = nOriginY + nOffsetY;
			const int32_t nLeft = nOriginX - nRadius;
			const int32_t nRight = nOriginX + nRadius;
			const int32_t nTop = nOriginY - nRadius;
			const int32_t nBottom = nOriginY + nRadius;
			int32_t nSteps = 8 * nRadius;
			if (!nSteps)
			{
				nSteps = 1;
			}

			if (nSteps > 0)
			{
				while (nSteps > 0)
				{
					if (nX == nLeft && nY == nTop)
					{
						nStepX = 1;
						nStepY = 0;
					}
					if (nX == nRight)
					{
						if (nY == nTop)
						{
							nStepX = 0;
							nStepY = 1;
						}
						if (nY == nBottom)
						{
							nStepX = -1;
							nStepY = 0;
						}
					}
					if (nX == nLeft)
					{
						if (nY == nBottom)
						{
							nStepX = 0;
							nStepY = -1;
						}
						if (nY == nTop && nX == nRight && nY == nBottom)
						{
							nStepX = 0;
							nStepY = 0;
						}
					}

					nX += nStepX;
					nY += nStepY;

					POINT pt = {};
					pt.x = nX;
					pt.y = nY;
					if (PtInRect(&spawnRect, pt))
					{
						if (!bUseRoomCoordList || D2Common_10095(pRoom, nX, nY) == nRoomCoordListIndex)
						{
							if (sub_6FC68350(pMonCreate->nMonsterId, pRoom, nX, nY, 1)
								&& (!COLLISION_CheckMaskWithSize(pRoom, nX, nY, pMonStats2TxtRecord->nSizeX, nCollisionFlags)
									|| (int8_t)nFlags < 0))
							{
								nSpawnX = nX;
								nSpawnY = nY;
								nRadius = nMaxRadius;
								break;
							}
						}
					}

					--nSteps;
				}
			}

			nRadius += 3;
		}
	}

	if (nSpawnX < pRoom->tCoords.nSubtileX || nSpawnX >= pRoom->tCoords.nSubtileX + pRoom->tCoords.nSubtileWidth
		|| nSpawnY < pRoom->tCoords.nSubtileY || nSpawnY >= pRoom->tCoords.nSubtileY + pRoom->tCoords.nSubtileHeight)
	{
		D2ActiveRoomStrc** ppRooms = nullptr;
		int32_t nNumRooms = 0;
		DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRooms, &nNumRooms);

		D2ActiveRoomStrc* pFoundRoom = nullptr;
		for (int32_t i = 0; i < nNumRooms; ++i)
		{
			D2ActiveRoomStrc* pAdj = ppRooms[i];
			if (!pAdj)
			{
				continue;
			}

			if (nSpawnX >= pAdj->tCoords.nSubtileX && nSpawnX < pAdj->tCoords.nSubtileX + pAdj->tCoords.nSubtileWidth
				&& nSpawnY >= pAdj->tCoords.nSubtileY && nSpawnY < pAdj->tCoords.nSubtileY + pAdj->tCoords.nSubtileHeight)
			{
				pFoundRoom = pAdj;
				break;
			}
		}

		pRoom = pFoundRoom;
	}

	if (!nSpawnX || !nSpawnY)
	{
		return nullptr;
	}

	if (nFlags & 1)
	{
		return (D2UnitStrc*)1;
	}

	const char nAllocFlags = (nFlags & 0x20) ? 3 : 1;
	D2UnitStrc* pUnit = SUNIT_AllocUnitData(UNIT_MONSTER, pMonCreate->nMonsterId, nSpawnX, nSpawnY, pMonCreate->pGame, pRoom, nAllocFlags, pMonCreate->nAnimMode, pMonCreate->nUnitGUID);
	if (!pUnit)
	{
		return nullptr;
	}

	int32_t bNeverCount = nFlags & 8;
	if (pMonStatsTxtRecord->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_NEVERCOUNT])
	{
		bNeverCount = 1;
	}

	sub_6FC68180(pMonCreate->pGame->pMonReg, pRoom, pUnit, bNeverCount);

	D2RoomCoordListStrc* pRoomCoordList = pMonCreate->pRoomCoordList;
	if (!pRoomCoordList)
	{
		pRoomCoordList = D2Common_10096(pRoom, nSpawnX, nSpawnY);
	}

	if (pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData)
	{
		pUnit->pMonsterData->pVision = (uint32_t)pRoomCoordList;
	}

	if (pMonStatsTxtRecord->nAlign == 1)
	{
		sub_6FCBDD30(pUnit, 2u, 1);
		pUnit->dwFlags |= UNITFLAG_NOTC;
	}
	else if (pMonStatsTxtRecord->nAlign == 2)
	{
		sub_6FCBDD30(pUnit, 1u, 1);
	}
	else
	{
		sub_6FCBDD30(pUnit, 0, 1);
	}

	if (!(nFlags & 2))
	{
		MONSTERSPAWN_SetupNormalMods(pMonCreate, pUnit);
	}

	MONSTERSPAWN_SetupBossMods(pMonCreate, pUnit);

	if (!(nFlags & 0x40))
	{
		sub_6FC69C00(pMonCreate, pUnit);
	}

	return pUnit;
}

//1.10: Inlined
//1.14d: 0x005B21B0
void __fastcall MONSTERSPAWN_SetupNormalMods(D2UnkMonCreateStrc* pMonCreate, D2UnitStrc* pUnit)
{
	int32_t nMonsterId = pMonCreate->nMonsterId;
	D2GameStrc* pGame = pMonCreate->pGame;

	D2MonStatsTxt* pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(nMonsterId);
	if (pMonStatsTxtRecord)
	{
		nMonsterId = pMonStatsTxtRecord->nBaseId;
	}

	if (nMonsterId < 0 || nMonsterId >= sgptDataTables->nMonStatsTxtRecordCount)
	{
		return;
	}

	int32_t nUMod = 0;
	int32_t bUnique = 0;
	int32_t bHasMod = 0;

	switch (nMonsterId)
	{
	case MONSTER_BRUTE2:
		nUMod = MONUMOD_IGNOREAC;
		bHasMod = 1;
		break;
	case MONSTER_SCARAB1:
		nUMod = MONUMOD_CHARGEDBOLTS;
		bHasMod = 1;
		break;
	case MONSTER_MUMMY1:
		nUMod = MONUMOD_POISDEATH;
		bHasMod = 1;
		break;
	case MONSTER_DURIEL:
		D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, 11, 0);
		nUMod = MONUMOD_QUESTMOD;
		bUnique = 1;
		bHasMod = 1;
		break;
	case MONSTER_ANCIENTBARB1:
	case MONSTER_ANCIENTBARB2:
	case MONSTER_ANCIENTBARB3:
		nUMod = MONUMOD_QUESTMOD;
		bUnique = 1;
		bHasMod = 1;
		break;
	case MONSTER_TRAP_MELEE:
		nUMod = MONUMOD_SPCDAMAGE;
		bHasMod = 1;
		break;
	case MONSTER_SUICIDEMINION1:
		nUMod = MONUMOD_FREEZINDEATH;
		bHasMod = 1;
		break;
	case MONSTER_REANIMATEDHORDE1:
		nUMod = MONUMOD_SELFRESURRECT;
		bHasMod = 1;
		break;
	case MONSTER_FROZENHORROR1:
		nUMod = MONUMOD_ICESHATTERDEATH;
		bHasMod = 1;
		break;
	default:
		if (nMonsterId >= MONSTER_BONEPRISON1 && nMonsterId <= MONSTER_BONEPRISON4)
		{
			nUMod = MONUMOD_KILLMINIONSDEATH;
			bHasMod = 1;
		}
		else if (nMonsterId > MONSTER_EVILHOLE5 && nMonsterId <= MONSTER_TRAP_LIGHTNING)
		{
			nUMod = MONUMOD_SPCDAMAGE;
			bHasMod = 1;
		}
		break;
	}

	if (bHasMod)
	{
		D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, nUMod, bUnique);
	}
}




//D2Game.0x6FC69B60
void __fastcall MONSTERSPAWN_EquipAncientBarbarians(D2UnitStrc* pUnit)
{
    constexpr Unk dword_6FD28B08[] =
    {
        { ' dsb', BODYLOC_RARM, 1, 0, 0 },
        { ' xat', BODYLOC_RARM, 1, 0, 0 },
        { ' uov', BODYLOC_RARM, 1, 0, 0 },
        { ' wot', BODYLOC_LARM, 1, 0, 0 },
        { ' xat', BODYLOC_LARM, 1, 0, 0 },
        { ' nir', BODYLOC_RRIN, 1, 0, 0 },
        { ' dlf', BODYLOC_TORSO, 1, 0, 0 },
        { ' lgh', BODYLOC_GLOVES, 1, 0, 0 },
        { ' dlf', BODYLOC_TORSO, 1, 0, 0 },
        { ' tbh', BODYLOC_FEET, 1, 0, 0 },
        { ' tbh', BODYLOC_FEET, 1, 0, 0 },
        { ' nrc', BODYLOC_HEAD, 1, 0, 0 },
    };

    if (!pUnit)
    {
        return;
    }

    D2GameStrc* pGame = SUNIT_GetGameFromUnit(pUnit);
    const Unk* pItemInfo = &dword_6FD28B08[pUnit->dwClassId - MONSTER_ANCIENTBARB1];
    const int32_t nLevel = STATLIST_UnitGetStatValue(pUnit, STAT_LEVEL, 0);

    for (int32_t i = 0; i < 4; ++i)
    {
        Unk itemInfo = {};

        itemInfo.nItemCode = pItemInfo->nItemCode;
        itemInfo.nBodyLoc = pItemInfo->nBodyLoc;
        itemInfo.unk0x05 = pItemInfo->unk0x05;
        itemInfo.unk0x06 = pItemInfo->unk0x06;
        itemInfo.unk0x07 = pItemInfo->unk0x07;

        int32_t nItemId = 0;
        D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemRecordFromItemCode(itemInfo.nItemCode, &nItemId);
        if (pGame->nDifficulty == DIFFMODE_NIGHTMARE)
        {
            itemInfo.nItemCode = pItemsTxtRecord->dwUberCode;
        }
        else if (pGame->nDifficulty == DIFFMODE_HELL)
        {
            itemInfo.nItemCode = pItemsTxtRecord->dwUltraCode;
        }

        sub_6FC61190(pGame, pUnit, &itemInfo, nLevel, 4u);
        pItemInfo += 3;
    }
}

//1.10: Inlined
//1.14d: 0x005B1CF0
void __fastcall MONSTERSPAWN_SetupBossMods(D2UnkMonCreateStrc* pMonCreate, D2UnitStrc* pUnit)
{
    int32_t nMonsterId = pMonCreate->nMonsterId;
    D2GameStrc* pGame = pMonCreate->pGame;

    D2MonStatsTxt* pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(nMonsterId);
    int32_t nBaseId;
    if (pMonStatsTxtRecord)
    {
        nBaseId = pMonStatsTxtRecord->nBaseId;
    }
    else
    {
        nBaseId = nMonsterId;
    }

    nBaseId = (nBaseId >= 0 && nBaseId < sgptDataTables->nMonStatsTxtRecordCount) ? nBaseId : -1;

    switch (nBaseId)
    {
    case MONSTER_ANDARIEL:
#ifdef D2_VERSION_HAS_UBERS
        if (nMonsterId == MONSTER_UBERANDARIEL)
        {
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_POISONFIELD, 1);
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_EXTRAFAST, 1);
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_MULTISHOT, 1);
            return;
        }
#endif
        D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_QUESTMOD, 1);
        QUESTS_CreateChainRecord(pGame, pUnit, 6);
        return;
    case MONSTER_WARRIV2:
        // this call does nothing
        //nBaseId = (MONSTER_WARRIV2 >= 0 && MONSTER_WARRIV2 < sgptDataTables->nMonStatsTxtRecordCount) ? MONSTER_WARRIV2 : -1;
        //D2GAME_QUESTS_WarrivSpawn_6FC95740(CLIENTS_GetUnitX(pUnit), CLIENTS_GetUnitY(pUnit), nBaseId);
        return;
    case MONSTER_DURIEL:
#ifdef D2_VERSION_HAS_UBERS
        if (nMonsterId == MONSTER_UBERDURIEL)
        {
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_EXTRAFAST, 1);
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_COLDCHANT, 1);
            return;
        }
#endif
        QUESTS_CreateChainRecord(pGame, pUnit, 13);
        QUESTS_CreateChainRecord(pGame, pUnit, 9);
        return;
    case MONSTER_RADAMENT:
        QUESTS_CreateChainRecord(pGame, pUnit, 8);
        return;
    case MONSTER_MEPHISTO:
#ifdef D2_VERSION_HAS_UBERS
        if (nMonsterId == MONSTER_UBERMEPHISTO)
        {
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_QUESTMOD, 1);
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_AURACHANT, 1);
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_LIGHTCHANT, 1);
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_MAGICRESISTANT, 1);
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_EXTRAFAST, 1);
            return;
        }
#endif
        QUESTS_CreateChainRecord(pGame, pUnit, 20);
        D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_QUESTMOD, 1);
        return;
    case MONSTER_DIABLO:
#ifdef D2_VERSION_HAS_UBERS
        if (nMonsterId == MONSTER_UBERDIABLO)
        {
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_QUESTMOD, 1);
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_MAGICRESISTANT, 1);
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_EXTRAFAST, 1);
            return;
        }
#endif
        D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_QUESTMOD, 1);
        QUESTS_CreateChainRecord(pGame, pUnit, 23);
        return;
    case MONSTER_SUMMONER:
        QUESTS_CreateChainRecord(pGame, pUnit, 12);
        MONSTERUNIQUE_ToggleUnitFlag(pUnit, 0x800, 1);
        MONSTER_ToggleSummonerFlag(pUnit, 1, 1);
        return;
    case MONSTER_IZUAL:
#ifdef D2_VERSION_HAS_UBERS
        if (nMonsterId == MONSTER_UBERIZUAL)
        {
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_EXTRAFAST, 1);
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_COLDCHANT, 1);
            return;
        }
#endif
        D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_QUESTMOD, 1);
        QUESTS_CreateChainRecord(pGame, pUnit, 22);
        return;
    case MONSTER_BLOODRAVEN:
        D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_BRAVENDEATH, 1);
        D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_QUESTMOD, 1);
        QUESTS_CreateChainRecord(pGame, pUnit, 2);
        STATES_ToggleState(pUnit, STATE_CORPSE_NOSELECT, 1);
        return;
    case MONSTER_MAGGOTQUEEN1:
        D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_POISONFIELD, 1);
        D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_QUESTMOD, 1);
        return;
    case MONSTER_FIREGOLEM:
        D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_CORPSEBOOMDEATH, 0);
        return;
    case MONSTER_BONEPRISON1:
    case MONSTER_BONEPRISON2:
    case MONSTER_BONEPRISON3:
    case MONSTER_BONEPRISON4:
        MONSTERUNIQUE_ToggleUnitFlag(pUnit, 0x20000, 1);
        return;
    case MONSTER_COMPELLINGORB:
        QUESTS_CreateChainRecord(pGame, pUnit, 19);
        MONSTERUNIQUE_ToggleUnitFlag(pUnit, 0x20000, 1);
        D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_QUESTMOD, 1);
        return;
    case MONSTER_SMITH:
        D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_QUESTMOD, 1);
        return;
    case MONSTER_FETISH11:
        QUESTS_CreateChainRecord(pGame, pUnit, 17);
        return;
    case MONSTER_HEPHASTO:
        QUESTS_CreateChainRecord(pGame, pUnit, 24);
        return;
    case MONSTER_PRISONDOOR:
        QUESTS_CreateChainRecord(pGame, pUnit, 32);
        return;
    case MONSTER_NIHLATHAKBOSS:
        QUESTS_CreateChainRecord(pGame, pUnit, 34);
        D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_QUESTMOD, 1);
        return;
    case MONSTER_ANCIENTBARB1:
        MONSTERSPAWN_EquipAncientBarbarians(pUnit);
        return;
    case MONSTER_BAALCRAB:
#ifdef D2_VERSION_HAS_UBERS
        if (nMonsterId == MONSTER_UBERBAAL)
        {
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_QUESTMOD, 1);
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_COLDCHANT, 1);
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_MAGICRESISTANT, 1);
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_EXTRAFAST, 1);
            return;
        }
#endif
        QUESTS_CreateChainRecord(pGame, pUnit, 36);
        D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pUnit, MONUMOD_QUESTMOD, 1);
        return;
    }
}

//1.10: Inlined
//1.14d: 0x005B21B0
//void __fastcall MONSTERSPAWN_SetupNormalMods(D2UnkMonCreateStrc* pMonCreate, D2UnitStrc* pUnit)
//{
//    v64 = v58->nMonsterId;
//    v65 = v58->pGame;
//    if (v64 >= 0)
//    {
//        if (v64 < sgptDataTables->nMonStatsTxtRecordCount)
//        {
//            v66 = &sgptDataTables->pMonStatsTxt[v64];
//            if (v66)
//                v64 = v66->nBaseId;
//        }
//        if (v64 >= 0 && v64 < sgptDataTables->nMonStatsTxtRecordCount)
//        {
//            if (v64 > MONSTER_BONEPRISON4)
//            {
//                switch (v64)
//                {
//                case MONSTER_TRAP_MELEE:
//                    goto LABEL_148;
//                case MONSTER_ANCIENTBARB1:
//                case MONSTER_ANCIENTBARB2:
//                case MONSTER_ANCIENTBARB3:
//                    goto LABEL_150;
//                default:
//                    goto LABEL_152;
//                case MONSTER_SUICIDEMINION1:
//                    v77 = 0;
//                    v75 = MONUMOD_FREEZINDEATH;
//                    break;
//                case MONSTER_REANIMATEDHORDE1:
//                    v77 = 0;
//                    v75 = MONUMOD_SELFRESURRECT;
//                    break;
//                case MONSTER_FROZENHORROR1:
//                    v77 = 0;
//                    v75 = MONUMOD_ICESHATTERDEATH;
//                    break;
//                }
//            }
//            else
//            {
//                if (v64 >= MONSTER_BONEPRISON1)
//                {
//                    v77 = 0;
//                    v75 = MONUMOD_KILLMINIONSDEATH;
//                }
//                else
//                {
//                    if (v64 > MONSTER_MUMMY1)
//                    {
//                        if (v64 == MONSTER_DURIEL)
//                        {
//                            D2GAME_BOSSES_AssignUMod_6FC6FF10(v65, v60, 11, 0);
//LABEL_150:
//                            v77 = 1;
//                            v75 = MONUMOD_QUESTMOD;
//                        }
//                        else
//                        {
//                            if (v64 <= MONSTER_EVILHOLE5 || v64 > MONSTER_TRAP_LIGHTNING)
//                                goto LABEL_152;
//LABEL_148:
//                            v77 = 0;
//                            v75 = MONUMOD_SPCDAMAGE;
//                        }
//                    }
//                    else
//                    {
//                        if (v64 == MONSTER_MUMMY1)
//                        {
//                            v77 = 0;
//                            v75 = MONUMOD_POISDEATH;
//                        }
//                        else
//                        {
//                            if (v64 == MONSTER_BRUTE2)
//                            {
//                                v77 = 0;
//                                v75 = MONUMOD_IGNOREAC;
//                            }
//                            else
//                            {
//                                if (v64 != MONSTER_SCARAB1)
//                                    goto LABEL_152;
//                                v77 = 0;
//                                v75 = MONUMOD_CHARGEDBOLTS;
//                            }
//                        }
//                    }
//                }
//            }
//            D2GAME_BOSSES_AssignUMod_6FC6FF10(v65, v60, v75, v77);
//        }
//    }
//LABEL_152:
//}

//D2Game.0x6FC69C00
void __fastcall sub_6FC69C00(D2UnkMonCreateStrc* pMonCreate, D2UnitStrc* pUnit)
{
    D2MonStatsTxt* pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(pMonCreate->nMonsterId);
    if (!pMonStatsTxtRecord)
    {
        return;
    }

    int32_t nChainId = 0;
    DATATBLS_GetMonsterChainInfo(pMonCreate->nMonsterId, nullptr, &nChainId);
    if (pMonStatsTxtRecord->wMinion[0] < 0 || pMonStatsTxtRecord->wMinion[0] >= sgptDataTables->nMonStatsTxtRecordCount)
    {
        return;
    }

    const int32_t nBaseId = pMonStatsTxtRecord->nBaseId;
    if (nBaseId < 0 || nBaseId >= sgptDataTables->nMonStatsTxtRecordCount || nBaseId != MONSTER_TENTACLEHEAD1)
    {
        if (pMonStatsTxtRecord->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_SETBOSS])
        {
            AIGENERAL_SetOwnerData(pMonCreate->pGame, pUnit, pUnit ? pUnit->dwUnitId : -1, 1, 1, pMonStatsTxtRecord->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_BOSSXFER]);
        }

        int32_t nMinionIndex = 0;
        int32_t nThreshold = 0;
        if (pMonStatsTxtRecord->wMinion[1] >= 0 && pMonStatsTxtRecord->wMinion[1] < sgptDataTables->nMonStatsTxtRecordCount)
        {
            nThreshold = 1;
        }

        int32_t nMinions = pMonStatsTxtRecord->nPartyMin;
        if (nMinions < pMonStatsTxtRecord->nPartyMax)
        {
            nMinions += ITEMS_RollLimitedRandomNumber(&pUnit->pSeed, pMonStatsTxtRecord->nPartyMax - nMinions + 1);
        }

        for (int32_t i = 0; i < nMinions; ++i)
        {
            D2UnkMonCreateStrc monCreate = {};
            monCreate.nUnitGUID = 0;
            monCreate.nAnimMode = 1;
            monCreate.nMonsterId = pMonStatsTxtRecord->wMinion[nMinionIndex];
            monCreate.field_20 = 4;
            monCreate.pRoomCoordList = nullptr;
            monCreate.pRoom = UNITS_GetRoom(pUnit);
            monCreate.pGame = pMonCreate->pGame;
            monCreate.nFlags = 64;
            monCreate.nX = CLIENTS_GetUnitX(pUnit);
            monCreate.nY = CLIENTS_GetUnitY(pUnit);

            D2UnitStrc* pMonster = D2GAME_SpawnNormalMonster_6FC68E30(&monCreate);
            if (pMonster && pMonStatsTxtRecord->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_SETBOSS])
            {
                AIGENERAL_SetOwnerData(pMonCreate->pGame, pMonster, pUnit->dwUnitId, 1, 0, 0);
                AIGENERAL_AllocMinionList(pMonCreate->pGame, pUnit, pMonster);
            }

            ++nMinionIndex;

            if (nMinionIndex > nThreshold)
            {
                nMinionIndex = 0;
            }
        }
    }
    else
    {
        AIGENERAL_SetOwnerData(pMonCreate->pGame, pUnit, pUnit ? pUnit->dwUnitId : -1, 1, 1, pMonStatsTxtRecord->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_BOSSXFER]);
        int32_t nMinions = pMonStatsTxtRecord->nPartyMin;
        if (nMinions < pMonStatsTxtRecord->nPartyMax)
        {
            nMinions += ITEMS_RollLimitedRandomNumber(&pUnit->pSeed, pMonStatsTxtRecord->nPartyMax - nMinions + 1);
        }
        uint32_t nFlags = (uint32_t)pMonCreate->nFlags;
        nFlags &= (0xFFFFFF00 & ~(uint8_t)pMonCreate->nFlags);
        sub_6FC6A8C0(pMonCreate->pGame, pUnit, pMonStatsTxtRecord->wMinion[0], 1, nMinions, ((nFlags >> 2) & 1), 0x40u);
    }
}

//1.10f:D2Game.0x6FC69F10
//1.11: D2Game.0x6FCE80F0
D2UnitStrc* __fastcall D2GAME_SpawnMonster_6FC69F10(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nX, int32_t nY, int32_t nMonsterId, int32_t nAnimMode, int32_t a7, int16_t nFlags)
{
    D2UnkMonCreateStrc monCreate = {};

    monCreate.pRoom = pRoom;
    monCreate.pGame = pGame;
    monCreate.nUnitGUID = 0;
    monCreate.nMonsterId = nMonsterId;
    monCreate.field_20 = a7;
    monCreate.pRoomCoordList = nullptr;
    monCreate.nX = nX;
    monCreate.nAnimMode = nAnimMode;
    monCreate.nFlags = nFlags;
    monCreate.nY = nY;

    return D2GAME_SpawnNormalMonster_6FC68E30(&monCreate);
}

//D2Game.0x6FC69F70
D2UnitStrc* __fastcall sub_6FC69F70(D2GameStrc* pGame, D2RoomCoordListStrc* pRoomCoordList, D2UnitStrc* pUnit, int32_t nMonsterId, int32_t nAnimMode, int32_t a6, int16_t nFlags)
{
    D2UnkMonCreateStrc monCreate = {};

    monCreate.nUnitGUID = 0;
    monCreate.nMonsterId = nMonsterId;
    monCreate.nAnimMode = nAnimMode;
    monCreate.pGame = pGame;
    monCreate.pRoomCoordList = pRoomCoordList;
    monCreate.pRoom = UNITS_GetRoom(pUnit);
    monCreate.field_20 = a6;
    monCreate.nFlags = nFlags;
    monCreate.nX = CLIENTS_GetUnitX(pUnit);
    monCreate.nY = CLIENTS_GetUnitY(pUnit);

    return D2GAME_SpawnNormalMonster_6FC68E30(&monCreate);
}

//D2Game.0x6FC6A030
D2UnitStrc* __fastcall sub_6FC6A030(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, D2RoomCoordListStrc* pRoomCoordList, int32_t nX, int32_t nY, int32_t nMonsterId, int32_t nAnimMode, int32_t a8, int16_t nFlags)
{
    D2UnkMonCreateStrc monCreate = {};

    monCreate.pRoom = pRoom;
    monCreate.nMonsterId = nMonsterId;
    monCreate.pGame = pGame;
    monCreate.nAnimMode = nAnimMode;
    monCreate.field_20 = a8;
    monCreate.pRoomCoordList = pRoomCoordList;
    monCreate.nFlags = nFlags;
    monCreate.nUnitGUID = 0;
    monCreate.nX = nX;
    monCreate.nY = nY;

    return D2GAME_SpawnNormalMonster_6FC68E30(&monCreate);
}

//D2Game.0x6FC6A090
D2UnitStrc* __fastcall sub_6FC6A090(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nX, int32_t nY, int32_t nMonsterId, int32_t nAnimMode, int16_t nFlags)
{
    D2UnkMonCreateStrc monCreate = {};

    monCreate.pGame = pGame;
    monCreate.nUnitGUID = 0;
    monCreate.nMonsterId = nMonsterId;
    monCreate.pRoom = pRoom;
    monCreate.pRoomCoordList = nullptr;
    monCreate.nX = nX;
    monCreate.nAnimMode = nAnimMode;
    monCreate.field_20 = -1;
    monCreate.nFlags = nFlags;
    monCreate.nY = nY;

    return D2GAME_SpawnNormalMonster_6FC68E30(&monCreate);
}

//D2Game.0x6FC6A0F0
D2UnitStrc* __fastcall sub_6FC6A0F0(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nX, int32_t nY, int32_t nMonsterId, int32_t nMode, int32_t nUnitId, int32_t a8, int16_t nFlags)
{
    D2UnkMonCreateStrc monCreate = {};

    monCreate.pRoom = pRoom;
    monCreate.nUnitGUID = nUnitId;
    monCreate.pGame = pGame;
    monCreate.nMonsterId = nMonsterId;
    monCreate.field_20 = a8;
    monCreate.nAnimMode = nMode;
    monCreate.nX = nX;
    monCreate.pRoomCoordList = nullptr;
    monCreate.nFlags = nFlags;
    monCreate.nY = nY;

    return D2GAME_SpawnNormalMonster_6FC68E30(&monCreate);
}

//D2Game.0x6FC6A150
D2UnitStrc* __fastcall sub_6FC6A150(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nMonsterId, int32_t nAnimMode, int32_t a5, int16_t nFlags)
{
    D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
    if (DUNGEON_GetLevelIdFromPopulatedRoom(pRoom))
    {
        D2UnkMonCreateStrc monCreate = {};
        monCreate.nUnitGUID = 0;
        monCreate.nMonsterId = D2Common_11063(pRoom, nMonsterId);
        monCreate.nAnimMode = nAnimMode;
        monCreate.pGame = pGame;
        monCreate.pRoomCoordList = nullptr;
        monCreate.pRoom = pRoom;
        monCreate.field_20 = a5;
        monCreate.nFlags = nFlags;
        monCreate.nX = CLIENTS_GetUnitX(pUnit);
        monCreate.nY = CLIENTS_GetUnitY(pUnit);

        return D2GAME_SpawnNormalMonster_6FC68E30(&monCreate);
    }

    return nullptr;
}

//D2Game.0x6FC6A230
int32_t __fastcall sub_6FC6A230(D2GameStrc* pGame, D2UnitStrc* pOwner, int32_t nMonsterId, int32_t nAnimMode, int32_t a5, int32_t nCount, int16_t nFlags)
{
    if (!pOwner)
    {
        return 0;
    }

    int32_t nResult = 0;
    const int32_t nOwnerId = pOwner->dwUnitId;

    D2UnkMonCreateStrc monCreate = {};

    monCreate.nUnitGUID = 0;
    monCreate.nMonsterId = nMonsterId;
    monCreate.nAnimMode = nAnimMode;
    monCreate.pGame = pGame;
    monCreate.pRoomCoordList = nullptr;
    monCreate.pRoom = UNITS_GetRoom(pOwner);
    monCreate.nFlags = nFlags;
    monCreate.field_20 = a5;
    monCreate.nX = CLIENTS_GetUnitX(pOwner);
    monCreate.nY = CLIENTS_GetUnitY(pOwner);

    for (int32_t i = 0; i < nCount; ++i)
    {
        D2UnitStrc* pMinion = D2GAME_SpawnNormalMonster_6FC68E30(&monCreate);
        if (pMinion)
        {
            nResult = 1;
            AIGENERAL_SetOwnerData(pGame, pMinion, nOwnerId, 1, 0, 0);
            AIGENERAL_AllocMinionList(pGame, pOwner, pMinion);
        }
    }

    return nResult;
}

//D2Game.0x6FC6A350
int32_t __fastcall sub_6FC6A350(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nX, int32_t nY, D2UnitStrc* pOwner, int32_t nClassId, int32_t nAnimMode, D2UnkMonCreateStrc2* a8, int16_t nFlags)
{
    D2UnitStrc* pUnit = nullptr;
    
    int32_t bIncreaseClassId = 0;
    if (a8->unk0x08 & 1)
    {
        bIncreaseClassId = 1;
    }

    int32_t bUseDefaultSpawnType = 0;
    if (nFlags & 1)
    {
        bUseDefaultSpawnType = 1;
    }

    int32_t nResult = 0;
    for (int32_t i = 0; i < a8->nRecordCount; ++i)
    {
        int32_t nSpawnType = 4;
        if (!bUseDefaultSpawnType)
        {
            nSpawnType = a8->records[i].nSpawnType;
        }

        switch (nSpawnType)
        {
        case 0:
        {
            D2UnkMonCreateStrc monCreate = {};
            monCreate.pGame = pGame;
            monCreate.pRoom = pRoom;
            monCreate.pRoomCoordList = nullptr;
            monCreate.nMonsterId = nClassId;
            monCreate.nAnimMode = nAnimMode;
            monCreate.nUnitGUID = 0;
            monCreate.nX = nX + a8->records[i].nXOffset;
            monCreate.nY = nY + a8->records[i].nYOffset;
            monCreate.field_20 = -1;
            monCreate.nFlags = nFlags;

            if (D2GAME_SpawnNormalMonster_6FC68E30(&monCreate))
            {
                nResult = 1;
            }

            break;
        }
        case 1:
        {
            D2UnkMonCreateStrc monCreate = {};
            monCreate.pGame = pGame;
            monCreate.pRoom = pRoom;
            monCreate.pRoomCoordList = nullptr;
            monCreate.nMonsterId = nClassId;
            monCreate.nAnimMode = nAnimMode;
            monCreate.nUnitGUID = 0;
            monCreate.nX = nX + a8->records[i].nXOffset;
            monCreate.nY = nY + a8->records[i].nYOffset;
            monCreate.field_20 = -1;
            monCreate.nFlags = 0;

            pUnit = D2GAME_SpawnNormalMonster_6FC68E30(&monCreate);
            if (pUnit)
            {
                AIGENERAL_SetOwnerData(pGame, pUnit, pUnit->dwUnitId, 1, 1, 0);
                nResult = 1;
            }
            break;
        }
        case 2:
        {
            D2UnkMonCreateStrc monCreate = {};
            monCreate.pGame = pGame;
            monCreate.pRoom = pRoom;
            monCreate.pRoomCoordList = nullptr;
            monCreate.nMonsterId = nClassId;
            monCreate.nAnimMode = nAnimMode;
            monCreate.nUnitGUID = 0;
            monCreate.nX = nX + a8->records[i].nXOffset;
            monCreate.nY = nY + a8->records[i].nYOffset;
            monCreate.field_20 = -1;
            monCreate.nFlags = 0;

            pUnit = D2GAME_SpawnNormalMonster_6FC68E30(&monCreate);
            if (pUnit)
            {
                AIGENERAL_SetOwnerData(pGame, pUnit, pUnit->dwUnitId, 1, 1, 1);
                nResult = 1;
            }
            break;
        }
        case 3:
        {
            if (pUnit)
            {
                D2UnkMonCreateStrc monCreate = {};
                monCreate.pGame = pGame;
                monCreate.pRoom = UNITS_GetRoom(pUnit);
                monCreate.pRoomCoordList = nullptr;
                monCreate.nMonsterId = nClassId;
                monCreate.nAnimMode = nAnimMode;
                monCreate.nUnitGUID = 0;
                monCreate.nX = nX + a8->records[i].nXOffset;
                monCreate.nY = nY + a8->records[i].nYOffset;
                monCreate.field_20 = -1;
                monCreate.nFlags = 0;

                D2UnitStrc* pMonster = D2GAME_SpawnNormalMonster_6FC68E30(&monCreate);
                if (pMonster)
                {
                    AIGENERAL_SetOwnerData(pGame, pMonster, pUnit->dwUnitId, 1, 0, 0);
                    AIGENERAL_AllocMinionList(pGame, pUnit, pMonster);
                    nResult = 1;
                }
            }
            break;
        }
        case 4:
        {
            D2UnkMonCreateStrc monCreate = {};
            monCreate.pGame = pGame;
            monCreate.pRoom = pRoom;
            monCreate.pRoomCoordList = nullptr;
            monCreate.nMonsterId = nClassId;
            monCreate.nAnimMode = nAnimMode;
            monCreate.nUnitGUID = 0;
            monCreate.nX = nX + a8->records[i].nXOffset;
            monCreate.nY = nY + a8->records[i].nYOffset;
            monCreate.field_20 = -1;
            monCreate.nFlags = nFlags;

            nResult = 1;
            
            if (!D2GAME_SpawnNormalMonster_6FC68E30(&monCreate))
            {
                return 0;
            }

            break;
        }
        case 5:
        {
            if (pOwner)
            {
                D2UnkMonCreateStrc monCreate = {};
                monCreate.pGame = pGame;
                monCreate.pRoom = pRoom;
                monCreate.pRoomCoordList = nullptr;
                monCreate.nMonsterId = nClassId;
                monCreate.nAnimMode = nAnimMode;
                monCreate.nUnitGUID = 0;
                monCreate.nX = nX + a8->records[i].nXOffset;
                monCreate.nY = nY + a8->records[i].nYOffset;
                monCreate.field_20 = -1;
                monCreate.nFlags = 0;
                
                D2UnitStrc* pMonster = D2GAME_SpawnNormalMonster_6FC68E30(&monCreate);
                if (pMonster)
                {
                    AIGENERAL_SetOwnerData(pGame, pMonster, pOwner->dwUnitId, pOwner->dwUnitType, 0, 0);
                    nResult = 1;
                }
            }
            break;
        }
        default:
            break;
        }

        if (bIncreaseClassId)
        {
            ++nClassId;
        }
    }

    return nResult;
}

//D2Game.0x6FC6A810
int32_t __fastcall sub_6FC6A810(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t a3, int32_t a4, D2UnitStrc* pTargetUnit, int32_t a6, int16_t a7)
{
    D2UnkMonCreateStrc2 dword_6FD2EEAC =
    {
        4, 0, 1, 0, 0, 0,
        {
            { 1, 1, 1 },
            { 1, -1, 3 },
            { -1, -1, 3 },
            { -1, 1, 3 }
        }
    };

    if (a6 == MONSTER_BONEPRISON1 && pTargetUnit)
    {
        return sub_6FC6A350(pGame, pRoom, CLIENTS_GetUnitX(pTargetUnit), CLIENTS_GetUnitY(pTargetUnit), 0, MONSTER_BONEPRISON1, 8, &dword_6FD2EEAC, a7);
    }

    return 0;
}

//D2Game.0x6FC6A8C0
int32_t __fastcall sub_6FC6A8C0(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nMonsterId, int32_t nAnimMode, int32_t nCount, int32_t bNonWaterSpawn, int16_t nFlags)
{
    static const D2CoordStrc stru_6FD28B68[12] =
    {
        {-1,-4 },
        { 1, 4 },
        { 1,-3 },
        {-1, 3 },
        { 0, 2 },
        { 0,-2 },
        {-3,-1 },
        { 3, 1 },
        { 2,-1 },
        {-2, 1 },
        { 1, 0 },
        {-1, 0 }
    };

    if (!pUnit)
    {
        return 0;
    }

    int32_t v9 = bNonWaterSpawn;
#ifdef NO_BUG_FIX
    // Original game accepts a value of 2 here, which then indexes past the end of stru_6FD28B68 (only 2 sets of 6 coordinates).
    if (bNonWaterSpawn < 0 || bNonWaterSpawn > 2)
#else
    if (bNonWaterSpawn < 0 || bNonWaterSpawn > 1)
#endif
    {
        v9 = 0;
    }

    int32_t nResult = 0;
    D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
    const int32_t nX = CLIENTS_GetUnitX(pUnit);
    const int32_t nY = CLIENTS_GetUnitY(pUnit);

    int32_t nParam1 = ITEMS_RollRandomNumber(&pUnit->pSeed) % 6;
    const int32_t nParam2 = 6 * v9;

    D2UnkMonCreateStrc monCreate = {};
    monCreate.nUnitGUID = 0;
    monCreate.pRoomCoordList = nullptr;
    monCreate.field_20 = -1;
    monCreate.nFlags = nFlags;
    monCreate.nMonsterId = nMonsterId;
    monCreate.nAnimMode = nAnimMode;
    monCreate.pRoom = pRoom;
    monCreate.pGame = pGame;

    for (int32_t i = 0; i < nCount; ++i)
    {
        const int32_t nIndex = nParam1 + nParam2;

        monCreate.nX = nX + stru_6FD28B68[nIndex].nX;
        monCreate.nY = nY + stru_6FD28B68[nIndex].nY;

        nParam1 = (nParam1 + 5) % 6;

        D2UnitStrc* pMinion = D2GAME_SpawnNormalMonster_6FC68E30(&monCreate);
        if (pMinion)
        {
            nResult = 1;
            AIGENERAL_SetOwnerData(pGame, pMinion, pUnit->dwUnitId, 1, 0, 0);
            AIGENERAL_AllocMinionList(pGame, pUnit, pMinion);
        }
    }

    return nResult;
}

//D2Game.0x6FC6AA70
int32_t __fastcall MONSTERSPAWN_SpawnRandomMonsterForLevel(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nX, int32_t nY)
{
    D2LevelsTxt* pLevelsTxtRecord = DATATBLS_GetLevelsTxtRecord(DUNGEON_GetLevelIdFromRoom(pRoom));
    if (!pLevelsTxtRecord->nNumNormalMonsters)
    {
        FOG_DisplayWarning("ptLevelData->bMonTot", __FILE__, __LINE__);
        return 0;
    }

    int32_t nIndex = ITEMS_RollLimitedRandomNumber(pRoom ? &pRoom->pSeed : nullptr, pLevelsTxtRecord->nNumNormalMonsters);

    for (int32_t i = 0; i < pLevelsTxtRecord->nNumNormalMonsters; ++i)
    {
        ++nIndex;
        if (nIndex >= pLevelsTxtRecord->nNumNormalMonsters)
        {
            nIndex = 0;
        }

        const int32_t nMonsterId = pLevelsTxtRecord->wNormalMonsters[nIndex];
        D2MonStatsTxt* pMonStatsTxtRecord = MONSTERMODE_GetMonStatsTxtRecord(nMonsterId);

        if (pMonStatsTxtRecord && pMonStatsTxtRecord->dwMonStatsFlags & gdwBitMasks[MONSTATSFLAGINDEX_ISSPAWN])
        {
            if (nMonsterId != -1)
            {
                return D2GAME_SpawnMonster_6FC69F10(pGame, pRoom, nX, nY, nMonsterId, MONMODE_NEUTRAL, 5, 0) != nullptr;
            }
            break;
        }
    }

    return 0;
}
