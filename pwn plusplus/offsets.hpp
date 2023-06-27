#pragma once

#define GAMELOGIC "GameLogic.dll"
#define BASEMOD "PwnAdventure3-Win32-Shipping.exe"

namespace offsets {
	inline std::map<const char*, uintptr_t> map;

	extern void populate( );
}