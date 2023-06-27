#include "pch.h"
#include "vector.hpp"
#include "player.hpp"
#include "globals.hpp"

typedef i_player *( __fastcall *player_interface_functiontype )( c_player * );

namespace hooks
{
	namespace player
	{
		typedef bool( __fastcall *playeronground_function_type )( void *, void * );
		playeronground_function_type original_onground = nullptr;
		bool __fastcall onground( void *ecx, void *edx ){
			return user_interface::options::movement::noclip ? 0 : original_onground( ecx, edx );
		}

		playertick_function_type original_tick = nullptr;
		void __fastcall tick( void *ecx, void *edx, float delta_time ){
			if ( shutdown || in_setup ) return original_tick( ecx, edx, delta_time );

			c_player *player = reinterpret_cast< c_player * >( ecx );
			if ( globals::player_ptr ){
				if ( player->is_pvp_enabled( ) ){
					if ( address( player ) != address( globals::player_ptr ) )
						tick_data::get( ).push_target( player );
				}
			} globals::update_pointers( );

			auto local_player = player->get_localplayer( );
			if ( !local_player ) return original_tick( ecx, edx, delta_time );

			original_tick( ecx, edx, delta_time );

			auto &inventory = player->get_inventory( );
			if ( address( player ) != address( globals::player_ptr ) ){
				globals::player_ptr = player;

				// check if clientworld does not exist (local server) and override the pointer
				if ( !globals::cl_world_ptr ) globals::cl_world_ptr = reinterpret_cast< c_client_world * >( globals::world_ptr );

				/*auto create = MH_CreateHook( ( void* )( *( *( const void*** )local_player + 20 ) ), &onground, reinterpret_cast< void** >( &original_onground ) );
				if ( create != MH_OK )
					utilities::log( "failed to create hook %d", create );

				auto enable = MH_EnableHook( ( void* )( *( *( const void*** )local_player + 20 ) ) );
				if ( enable != MH_OK )
					utilities::log( "failed to enable hook %d", enable );*/
			}

			user_interface::rendering::push_to_graph( delta_time );
			tick_data::get( ).populate( player, delta_time );
			tick_data::get( ).update_targets( );

			vector3	position = tick_data::get( ).position,
				velocity = tick_data::get( ).velocity,
				rotation = tick_data::get( ).rotation,
				look_rotation = tick_data::get( ).look_rotation;

			float	forward_fraction = tick_data::get( ).forward_fraction,
				strafe_fraction = tick_data::get( ).strafe_fraction;

			cheats::movement::get( ).tick( &tick_data::get( ) );

			cheats::refill_ammo::get( ).tick( &tick_data::get( ) );
			cheats::antiaim::get( ).tick( &tick_data::get( ) );
			cheats::aimbot::get( ).tick( &tick_data::get( ) );

			cheats::auto_quest::get( ).tick( &tick_data::get( ) );
			cheats::visuals::get( ).tick( &tick_data::get( ) );

			tick_data::get( ).apply( );
		}

		playercanjump_function_type original_canjump = nullptr;
		bool __fastcall can_jump( void *ecx, void *edx ){
			if ( shutdown || in_setup ) return original_canjump( ecx, edx );
			if ( user_interface::options::movement::fly ) return 1;

			return original_canjump( ecx, edx );
		}

		playerdamage_function_type original_damage = nullptr;
		void __fastcall damage( void *thisptr, void *ecx, void *instigator, void *item, int dmg, int dmg_type ){
			if ( shutdown || in_setup ) return original_damage( thisptr, ecx, instigator, item, dmg, dmg_type );

			original_damage( thisptr, ecx, instigator, item, dmg, dmg_type );
		}

		playergetitemforslot_function_type original_getitemforslot = nullptr;
		i_item *__fastcall get_item_for_slot( void *ecx, void *edx, unsigned int slot ){
			return original_getitemforslot( ecx, edx, slot );
		}

		playercontrollerfov_function_type original_fov = nullptr;
		void __stdcall fov( float fov ){
			original_fov( fov );
		}
	}
}