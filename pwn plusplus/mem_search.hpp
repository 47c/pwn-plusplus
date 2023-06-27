#pragma once

namespace core
{
#define INRANGE(x,a,b)  (x >= a && x <= b)
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

	extern std::uintptr_t find_signature( const char* szModule, const char* szSignature );
	extern std::uintptr_t find_signature( MODULEINFO& module_info, const char* szSignature );

	inline int wait_for_modules( std::int32_t timeout, std::initializer_list<std::wstring> modules ) {
		bool signaled[ 32 ] = { 0 };
		bool success = false;

		std::uint32_t totalSlept = 0;

		if ( timeout == 0 ) {
			for ( auto& mod : modules ) {
				if ( GetModuleHandleW( std::data( mod ) ) == NULL )
					return WAIT_TIMEOUT;
			}
			return WAIT_OBJECT_0;
		}

		if ( timeout < 0 )
			timeout = INT32_MAX;

		while ( true ) {
			for ( auto i = 0u; i < modules.size( ); ++i ) {
				auto& module = *( modules.begin( ) + i );
				if ( !signaled[ i ] && GetModuleHandleW( std::data( module ) ) != NULL ) {
					signaled[ i ] = true;

					//
					// Checks if all modules are signaled
					//
					bool done = true;
					for ( auto j = 0u; j < modules.size( ); ++j ) {
						if ( !signaled[ j ] ) {
							done = false;
							break;
						}
					}
					if ( done ) {
						success = true;
						goto exit;
					}
				}
			}
			if ( totalSlept > std::uint32_t( timeout ) ) {
				break;
			}
			Sleep( 10 );
			totalSlept += 10;
		}

	exit:
		return success ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
	}

	inline uintptr_t relative_call_address( const char* module, const char* pattern ) {
		uintptr_t call_opcode_address = find_signature( module, pattern );

		uintptr_t relative_call_address = deref<uintptr_t>( call_opcode_address + 1 );

		return call_opcode_address + 1 /*Skip E8 opcode*/ + sizeof( std::uintptr_t ) /*Skip bytes of relative address*/ + relative_call_address;
	}

}