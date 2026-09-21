#pragma once

// Retail Fog.dll (MSVC6) allocates gpGameDataTbl. VS2022 tHashTable.NewLock/Lock
// hits the wrong InternalNew (RTC#0).
//
// 1 = mixed mode: New/Ptr/Lock go through retail D2Game thunks (ExtraPatchAction).
// 0 = Fog.dll is VS2022 with this TSHashTable. Use tHashTable.NewLock directly
//     and leave extraPatchActions empty in D2Game.patch.cpp.
#define D2GAME_ROUTE_HASHTABLE_THROUGH_FOG 1
