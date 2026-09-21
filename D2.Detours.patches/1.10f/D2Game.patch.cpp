#include <DetoursPatch.h>
#include <cstdint>
#include "../../source/D2Game/include/GAME/GameHashTableFog.h"

// D2Game.patch.cpp is compiled into the D2Game shared target (not D2GameObjects),
// so it does not see D2Common includes. Do not pull GAME/Game.h here; keep the
// thiscall thunk declarations in sync with Game.h instead.
struct D2GameStrc;
struct D2GameDataTableStrc;
class HASHKEY_NONE;
typedef struct HGAMEDATA__ { int unused; }* HGAMEDATA;
typedef struct GAMEDATALOCKEDHANDLE__ { int unused; }* GAMEDATALOCKEDHANDLE;
using D2GameGUID = uint32_t;

D2GameStrc* __fastcall D2GameDataTable_Lock(D2GameDataTableStrc* pGameDataTable, int32_t nUnused, HGAMEDATA hGame, GAMEDATALOCKEDHANDLE* pLockedHandle, int32_t forWriting);
void __fastcall D2GameDataTable_SyncEnterLock(D2GameDataTableStrc* pGameDataTable, int32_t nUnused, int32_t* pLockHandle, int32_t bForWriting);
void __fastcall D2GameDataTable_SyncLeaveLock(D2GameDataTableStrc* pGameDataTable, int32_t nUnused, int32_t tLockHandle);
D2GameStrc* __fastcall D2GameDataTable_New(D2GameDataTableStrc* pGameDataTable, int32_t nUnused, D2GameGUID nGameGUID, HASHKEY_NONE* pKey, int32_t extrabytes, int32_t flags);
D2GameStrc* __fastcall D2GameDataTable_Ptr(D2GameDataTableStrc* pGameDataTable, int32_t nUnused, D2GameGUID nGameGUID, const HASHKEY_NONE* pHashKey);

//#define DISABLE_ALL_PATCHES

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wmicrosoft-cast"
#endif

extern "C" {
    __declspec(dllexport)
    constexpr int __cdecl GetBaseOrdinal() { return 10'001; }
    __declspec(dllexport)
    constexpr int __cdecl GetLastOrdinal() { return 10'062; }
    constexpr int GetOrdinalCount() { return GetLastOrdinal() - GetBaseOrdinal() + 1; }
}

// We are using the following letters:
// /*C*/ to mean that the function has been "Checked" and works
// /*B*/ to mean wean know it is "Broken" and needs fixing.
//`     ` (spaces) to mean that it is "Unknown" wether it works or not

// If not explicit stated, then the check was done by code review only
// *Code Change* means D2MOO code change and we want to test this ingame
static PatchAction patchActions[GetOrdinalCount()] = {
	
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   D2Game_10001_Return0		Only code review												@10001
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_InitGameDataTable											@10002
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_ProcessNetworkMessages										@10003
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_UpdateGamesProgress											@10004
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_UpdateClients												@10005
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_CloseAllGames												@10006
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_ReceiveDatabaseCharacter										@10007
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_SetTargetFrameRate											@10008
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_SetGlobalAct												@10009
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_SetInitSeed												@10010
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_GetFirstGameInitSeed	(D2GS)									@10011
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_GetGamesCount												@10012
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_GetGameClientCount											@10013
    PatchAction::FunctionReplacePatchByOriginal, /*B*/ //   GAME_GetGameInformation											@10014
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_GetGameServerGameIds	(D2GS)										@10015
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_GetPlayerUnitsCount											@10016
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_GetPlayerUnitsInfo											@10017
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_GetUnitsInfo														@10018
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_GetUnitsDescriptions											@10019
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_GetStatistics	(D2GS)												@10020
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_SendMessageToAllClients	(D2GS)									@10021
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_SendMessageToGameClients										@10022
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_SetServerCallbackFunctions									@10023
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_RemoveClientFromGame									@10024
    PatchAction::FunctionReplacePatchByOriginal,     //   DEBUG_GetUnitFromTypeAndGUID										@10025
    PatchAction::FunctionReplacePatchByOriginal,     //   DEBUG_GetAct														@10026
    PatchAction::FunctionReplacePatchByOriginal,     //   DEBUG_GetFreeMonsterIndex											@10027
    PatchAction::FunctionReplacePatchByOriginal,     //   DEBUG_GetRoomBySubtileCoordinates									@10028
    PatchAction::FunctionReplacePatchByOriginal,     //   D2GAME_10029_SAVE_WriteFileInterface								@10029
    PatchAction::Ignore,                             //   ; Not exported
    PatchAction::Ignore,                             //   ; Not exported
    PatchAction::Ignore,                             //   ; Not exported
    PatchAction::FunctionReplacePatchByOriginal,     //   D2Game_10033														@10033
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   D2Game_10034_Return												@10034
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   QUESTS_DisableSequences											@10035
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   D2GAME_10036_PLRSAVE_EnableSaveFileWriting						@10036
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   QUESTS_CheckFirstPickedSet										@10037
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   QUESTS_CheckNotIntroQuest										@10038
    PatchAction::FunctionReplacePatchByOriginal,     //   D2Game_10039														@10039
    PatchAction::FunctionReplacePatchByOriginal,     //   D2Game_10040														@10040
    PatchAction::FunctionReplacePatchByOriginal,     //   D2Game_10041														@10041
    PatchAction::FunctionReplacePatchByOriginal,     //   D2Game_10042														@10042
    PatchAction::FunctionReplacePatchByOriginal,     //   D2Game_10043														@10043
    PatchAction::FunctionReplacePatchByOriginal,     //   D2Game_10044														@10044
    PatchAction::FunctionReplacePatchByOriginal,     //   TASK_ProcessGame													@10045
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_Initialize														@10046
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_CreateNewEmptyGame											@10047
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   D2Game_10048_Return1											@10048
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   D2Game_10049_Return1											@10049
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_Shutdown														@10050
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_GetFrameRate												@10051
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_GetMemoryUsage	(D2GS)											@10052
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_CountGamesByClientCount	*Code Change*	(D2GS)				@10053
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_CountGamesByDuration		*Code Change*	(D2GS)													@10054
    PatchAction::FunctionReplacePatchByOriginal,     //   GAME_DisconnectClientByName										@10055
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   D2Game_10056													@10056
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_GetExpansionGamesCount		(D2GS)								@10057
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   CLIENTS_GetExpansionClientCount	(D2GS)								@10058
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   PLAYER_SetVirtualPlayerCount									@10059
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   D2Game_10060_Return	Only code review							@10060
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //	D2Game_10061_Return	Only code review							@10061
    PatchAction::FunctionReplaceOriginalByPatch, /*C*/ //   GAME_ReturnArgument												@10062
};

extern "C" {

__declspec(dllexport)
PatchAction __cdecl GetPatchAction(int ordinal)
{
#ifdef DISABLE_ALL_PATCHES
    return PatchAction::Ignore;
#else
    if (ordinal < GetBaseOrdinal() || ordinal > GetLastOrdinal())
        return PatchAction::FunctionReplacePatchByOriginal;
    
    static_assert(GetOrdinalCount() == (sizeof(patchActions) / sizeof(*patchActions)), "Make sure we have the right number of ordinal patch entries");
    return ::patchActions[ordinal - GetBaseOrdinal()];
#endif
}

static const int D2GameImageBase = 0x6FC30000;

static ExtraPatchAction extraPatchActions[] = {
#if D2GAME_ROUTE_HASHTABLE_THROUGH_FOG
    { 0x6FC3B480 - D2GameImageBase, &D2GameDataTable_Lock, PatchAction::FunctionReplacePatchByOriginal },
    { 0x6FC3B510 - D2GameImageBase, &D2GameDataTable_SyncEnterLock, PatchAction::FunctionReplacePatchByOriginal },
    { 0x6FC3B540 - D2GameImageBase, &D2GameDataTable_SyncLeaveLock, PatchAction::FunctionReplacePatchByOriginal },
    { 0x6FC3B590 - D2GameImageBase, &D2GameDataTable_New, PatchAction::FunctionReplacePatchByOriginal },
    { 0x6FC3B6A0 - D2GameImageBase, &D2GameDataTable_Ptr, PatchAction::FunctionReplacePatchByOriginal },
#endif
    { 0, 0, PatchAction::Ignore},
};

__declspec(dllexport)
constexpr int __cdecl GetExtraPatchActionsCount() { 

#ifdef DISABLE_ALL_PATCHES
    return 0;
#else
    return sizeof(extraPatchActions) / sizeof(ExtraPatchAction); 
#endif
}

__declspec(dllexport)
ExtraPatchAction* __cdecl GetExtraPatchAction(int index)
{
    return &extraPatchActions[index];
}

}

#include <type_traits>
static_assert(std::is_same<decltype(GetBaseOrdinal)*, GetIntegerFunctionType>::value, "Ensure calling convention doesn't change");
static_assert(std::is_same<decltype(GetLastOrdinal)*, GetIntegerFunctionType>::value, "Ensure calling convention doesn't change");
static_assert(std::is_same<decltype(GetPatchAction)*, GetPatchActionType>::value, "Ensure calling convention doesn't change");


static_assert(std::is_same<decltype(GetExtraPatchActionsCount)*, GetIntegerFunctionType>::value, "Ensure calling convention doesn't change");
static_assert(std::is_same<decltype(GetExtraPatchAction)*, GetExtraPatchActionType>::value, "Ensure calling convention doesn't change");
