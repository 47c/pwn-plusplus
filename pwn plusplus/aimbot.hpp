#pragma once

namespace cheats {
	class aimbot : public singleton<aimbot> {
	private:
		tick_data* data;

		c_actor* target;
	public:

		void tick( tick_data* d ) {
			data = d;
			find_target( );
			auto_fire( );
		}

		bool has_target( ) { return target != nullptr; }

		void reset_target( ) { target = nullptr; }

		void find_target( ) {
			reset_target( );

			if ( !data ) return;

			float h_distance = std::numeric_limits<float>::infinity( );
			vector3 position;

			for ( auto e : data->target_actors ) {
				e->get_look_position( &position );
				float distance = data->position.distance( position );
				if ( distance < h_distance ) {
					h_distance = distance;
					target = e;
				}
			}
		}

		vector3 get_corrected_position( ) {
			return data->server_position + vector3( 0, 0, 42 );
		}

		vector3 get_target_position( ) {
			if ( !target ) return {};

			vector3 position;
			target->get_look_position( &position );

			return position;
		}

		vector3 get_angle( ) {
			if ( !target ) return {};

			vector3 delta = ( get_target_position( ) - get_corrected_position( ) );

			return math::vector_angle( delta );
		}

		bool can_hit( vector3 p ) {
			if ( !data ) return false;

			auto weapon = data->player_interface->get_item( );
			if ( !weapon ) return false;

			if ( p.distance( data->server_position ) > weapon->get_range( ) ) return false;

			return address( data->local_player_interface->line_trace( p ) ) == address( target );
		}

		void auto_fire( ) {
			if ( !user_interface::options::ragebot::auto_fire ) return;

			if ( !data ) return;
			if ( !target ) return;

			auto weapon = data->player_interface->get_item( );
			if ( !weapon ) return;
			
			if ( weapon->get_mana_cost( ) )
				fire_mana_weapon( weapon );
			else
				fire_weapon( weapon );
		}

		void fire_mana_weapon( i_item* weapon ) {
			if ( data->player_interface->is_item_on_cooldown( weapon ) ) return;

			if ( data->player->get_mana( ) < weapon->get_mana_cost( ) ) return;

			if ( can_hit( get_target_position( ) ) ) {
				globals::cl_world_ptr->activate( data->player, weapon );
				weapon->local_activate( data->player_interface );

				data->player_interface->set_item_cooldown( weapon,
														   weapon->get_cooldown_time( ),
														   true );
			}
		}

		void fire_weapon( i_item* weapon ) {
			if ( data->player_interface->is_item_on_cooldown( weapon ) )
				return;

			if ( data->player_interface->get_loaded_ammo( weapon ) <= 0 ) {
				if ( data->player_interface->can_reload( ) ) {
					data->player_interface->reload( );
					data->player_interface->set_item_cooldown( weapon,
															   weapon->get_reload_time( 1 ),
															   true );
				}
				return;
			}

			if ( !weapon->can_activate( data->player_interface ) )
				return;

			if ( can_hit( get_target_position( ) ) ) {

				globals::cl_world_ptr->activate( data->player, weapon );
				weapon->local_activate( data->player_interface );
				data->player_interface->set_item_cooldown( weapon,
														   weapon->get_cooldown_time( ),
														   true );
			}
		}

		void override_packet( packet_constructor::attack_packet_t& packet ) {
			if ( !data ) return;
			if ( !has_target( ) ) return;
			if ( !data->player_interface->get_item( ) ) return;

			vector3 angle = get_angle( );
			float pitch = -angle.x, 
				yaw = angle.y;

			packet.set_pitch( pitch );
			packet.set_yaw( yaw );
			packet.update( );
		}

		int16_t get_pitch( float val ) {
			float pitch_normalized = std::remainderf( val, 180.f );
			int limit = ( INT16_MAX * 0.5f ) + 1;
			int16_t yaw = ( limit / 90.f ) * pitch_normalized;

			return yaw;
		}

		int16_t get_yaw( float val ) {
			float yaw_normalized = std::remainderf( val, 360.f );
			int limit = ( INT16_MAX * 0.5f ) + 1;
			int16_t yaw = ( limit / 90.f ) * yaw_normalized;

			return yaw;
		}

		int16_t get_roll( float val ) {
			float roll_normalized = std::remainder( val, 360.f );
			int limit = ( INT16_MAX * 0.5f ) + 1;
			int16_t yaw = ( limit / 90.f ) * roll_normalized;

			return yaw;
		}

		void create_attack_packet( packet_constructor::attack_packet_t& packet ) {
			if ( !data ) return;
			if ( !has_target( ) ) return;
			if ( !data->player_interface->get_item( ) ) return;

			auto angle = get_angle( );

			float pitch = -angle.x;
			float yaw = angle.y;

			std::string item_name = data->player_interface->get_item( )->get_name( );
			packet.set_size( item_name.size( ) );
			packet.set_name( item_name );
			packet.set_pitch( pitch );
			packet.set_yaw( yaw );
			packet.set_spread( data->player_interface->get_item( )->get_spread_angle( ) );

			packet.update( );
		}
	};
}