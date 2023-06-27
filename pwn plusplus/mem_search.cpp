#include "pch.h"

namespace core
{
	std::uintptr_t find_signature( const char* szModule, const char* szSignature )
	{
		MODULEINFO modInfo;
		GetModuleInformation( GetCurrentProcess( ), GetModuleHandleA( szModule ), &modInfo, sizeof( MODULEINFO ) );
		uintptr_t startAddress = ( uintptr_t )modInfo.lpBaseOfDll;
		uintptr_t endAddress = startAddress + modInfo.SizeOfImage;
		const char* pat = szSignature;
		uintptr_t firstMatch = 0;
		for ( uintptr_t pCur = startAddress; pCur < endAddress; pCur++ ) {
			if ( !*pat ) return firstMatch;
			if ( *( PBYTE )pat == '\?' || *( BYTE* )pCur == getByte( pat ) ) {
				if ( !firstMatch ) firstMatch = pCur;
				if ( !pat[ 2 ] ) return firstMatch;
				if ( *( PWORD )pat == '\?\?' || *( PBYTE )pat != '\?' ) pat += 3;
				else pat += 2;    //one ?
			}
			else {
				pat = szSignature;
				firstMatch = 0;
			}
		}
		return NULL;
	}

	std::uintptr_t find_signature( MODULEINFO& module_info, const char* szSignature )
	{
		uintptr_t startAddress = ( uintptr_t )module_info.lpBaseOfDll;
		uintptr_t endAddress = startAddress + module_info.SizeOfImage;
		const char* pat = szSignature;
		uintptr_t firstMatch = 0;
		for ( uintptr_t pCur = startAddress; pCur < endAddress; pCur++ ) {
			if ( !*pat ) return firstMatch;
			if ( *( PBYTE )pat == '\?' || *( BYTE* )pCur == getByte( pat ) )
			{
				if ( !firstMatch )
					firstMatch = pCur;

				if ( !pat[ 2 ] )
					return firstMatch;

				if ( *( PWORD )pat == '\?\?' || *( PBYTE )pat != '\?' )
					pat += 3;
				else
					pat += 2;    //one ?
			}
			else
			{
				pat = szSignature;
				firstMatch = 0;
			}
		}
		return NULL;
	}
}