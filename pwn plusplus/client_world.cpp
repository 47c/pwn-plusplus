#include "pch.h"

namespace hooks {
	namespace client_world {
		clientworldtick_function_type original_tick = nullptr;
		void __fastcall tick( void* ecx, void* edx, float delta_time ) {
			if ( shutdown || in_setup ) return original_tick( ecx, edx, delta_time );

			if ( address( ecx ) != address( globals::cl_world_ptr ) ) {
				globals::cl_world_ptr = reinterpret_cast< c_client_world* >( ecx );
			}

			original_tick( ecx, edx, delta_time );
		}

		clientworldjump_function_type original_jump = nullptr;
		void __fastcall jump( void* ecx, void* edx, bool state ) {
			uintptr_t return_address = address( _ReturnAddress( ) );

			return original_jump( ecx, edx, state );
		}

		worldspawnactorwithid_function_type original_spawnactorwithid = nullptr;
		void __fastcall spawnactorwithid( void* ecx, void* edx, unsigned int id, void* object, const vector3* position, const vector3* rotation ) {
			original_spawnactorwithid( ecx, edx, id, object, position, rotation );

			c_actor* actor = reinterpret_cast< c_actor* >( object );

			if ( !actor ) return;
			if ( !globals::player_ptr ) return;
			
			std::string blueprint_name = std::string( actor->get_blueprint_name( ) );
			if ( blueprint_name.find( "Drop" ) == std::string::npos ) return;

			c_drop* drop = reinterpret_cast< c_drop* >( object );

			// replace string searching by hash-comparing for improved performance
			if ( blueprint_name.find( "White" ) != std::string::npos
				 && !user_interface::options::misc::auto_loot[ user_interface::options::misc::drop_white ]
				 || blueprint_name.find( "Green" ) != std::string::npos
				 && !user_interface::options::misc::auto_loot[ user_interface::options::misc::drop_green ]
				 || blueprint_name.find( "Blue" ) != std::string::npos
				 && !user_interface::options::misc::auto_loot[ user_interface::options::misc::drop_blue ]
				 || blueprint_name.find( "Purple" ) != std::string::npos
				 && !user_interface::options::misc::auto_loot[ user_interface::options::misc::drop_purple ] ) return;

			packet_constructor::get( ).build_pickup_packet( id ).push_to_queue( );
		}
	}
}