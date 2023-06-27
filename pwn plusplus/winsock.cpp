#include "pch.h"

namespace hooks
{
	namespace winsock
	{
		ws2_32send_function_type original_send = nullptr;
		int __stdcall send( SOCKET s, const char *buf, int len, int flags ){
			if ( shutdown || in_setup ) return original_send( s, buf, len, flags );
			if ( !globals::player_ptr ) return original_send( s, buf, len, flags );

			if ( s != packet_constructor::get( ).get_socket( ) ){ packet_constructor::get( ).update_socket( s ); }

			auto packet = packet_constructor::get( ).extract_packet_metadata( buf, len );
			if ( packet.get_identifier( ) == packet_constructor::get( ).get_identifier( packet_position ) ){
				auto mov_packet = *packet.get_as<packet_constructor::movement_packet_t>( ).extract( );

				cheats::movement::get( ).do_fakelag( &mov_packet ); mov_packet.update( );

				return original_send( s, mov_packet.get_buffer( ).c_str( ), mov_packet.get_buffer( ).size( ), flags );
			} else if ( packet.get_identifier( ) == packet_constructor::get( ).get_identifier( packet_attack ) ){
				auto atk_packet = *packet.get_as<packet_constructor::attack_packet_t>( ).extract( );

				if ( cheats::aimbot::get( ).can_hit( cheats::aimbot::get( ).get_target_position( ) ) )
					cheats::aimbot::get( ).override_packet( atk_packet );

				return original_send( s, atk_packet.get_buffer( ).c_str( ), atk_packet.get_buffer( ).size( ), flags );
			}

			return original_send( s, buf, len, flags );
		}
	}
}