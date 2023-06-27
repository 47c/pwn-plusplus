#include "pch.h"

namespace hooks {
	namespace game_server_connection {
		gameserverconnectiondesctructor_function_type original_destructor = nullptr;
		void __fastcall destructor( void* ecx, void* edx ) {
			original_destructor( ecx, edx );

			packet_constructor::get( ).reset_socket( );
			tick_data::get( ).reset_targets( );

			cheats::auto_quest::get( ).reset( );

			globals::invalidate_pointers( );
		}

		gameserverconnectionconnect_function_type original_connect = nullptr;;
		void __fastcall connect( void* ecx, void* edx, const std::string* host, uint16_t port, int id, const std::string* token, void* completion ) {
			original_connect( ecx, edx, host, port, id, token, completion );
		}

		masterserveconnectioncharacterinfo_function_type original_characterinfo = nullptr;
		void __fastcall characterinfo( character_info_t* ecx, void* edx, character_info_t* __that ) {
			original_characterinfo( ecx, edx, __that );
		}

		clienthandlerprocesscommands_function_type original_process_commands = nullptr;
		void __fastcall process_commands( c_client_handler* ecx, void* edx ) {
			original_process_commands( ecx, edx );
		}
	}
}