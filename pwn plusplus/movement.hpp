#pragma once

namespace cheats {
	class movement : public singleton<movement> {
	private:
		tick_data* data;

	public:
		void tick( tick_data* d ) {
			data = d;

			do_fly( ); do_noclip( );
			do_movespeed( ); do_jumpspeed( );
			do_quickstop( );
		}

		void correct_movement_rotation( vector3& v ) {
			if ( fabsf( data->forward_fraction ) > 0 || fabsf( data->strafe_fraction ) > 0 ) {
				v += vector3( 0, 
							  RAD2DEG( atan2f( data->forward_fraction, -data->strafe_fraction ) ) - 90.f, 
							  0 );
			}

		}

		void do_noclip( ) {
			static vector3 noclip_position { },
				empty { };

			auto character = globals::engine_ptr->get_character( );

			if ( !user_interface::options::movement::noclip 
				 || hooks::shutdown ) {
				noclip_position = data->position;
				character->has_collision( ) = 197;
				return;
			}

			character->has_collision( ) = 0;
			character->get_movement_component( )->get_movement_mode( ) = 2;

			vector3 rotation = data->look_rotation;
			vector3 position = data->position;

			data->set_velocity( empty );
			if ( fabs( data->forward_fraction ) > 0 || fabsf( data->strafe_fraction ) > 0 ) {
				correct_movement_rotation( rotation );

				vector3 forward = math::angle_vector( rotation );
				forward.normalize( );
				forward.z *= -fabs( data->forward_fraction );

				forward *= 10 * user_interface::options::movement::movespeed_multiplier;

				position += forward;

				noclip_position = position;
			} else noclip_position = position;

			data->set_position( noclip_position );
		}

		void do_fly( ) {
			if ( !user_interface::options::movement::fly) {
				data->player->get_jump_holdtime( ) = data->player->get_default_jumpholdtime( );
				return;
			}

			data->player->get_jump_holdtime( ) = std::numeric_limits<float>::infinity( );

			vector3 rotation = data->look_rotation;
			static float accel_modifier = 1.f;
			if ( GetAsyncKeyState( VK_SPACE ) ) {
				correct_movement_rotation( rotation );

				vector3 forward = math::angle_vector( rotation );
				forward.normalize( );

				forward *= min( data->velocity.length( ) * accel_modifier, 5000.f );

				data->set_velocity( forward );

				accel_modifier += 0.05f;
			} else accel_modifier = 1.f;
		}

		void do_movespeed( ) {
			if ( user_interface::options::movement::movespeed_multiplier == 1.f ) return;
			if ( user_interface::options::movement::noclip ) {
				data->player->get_walkspeed( ) = data->player->get_default_walkspeed( );
				return;
			}

			float goal_speed = data->player->get_default_walkspeed( ) * user_interface::options::movement::movespeed_multiplier;

			if ( data->player->get_walkspeed( ) != goal_speed ) {
				data->player->get_walkspeed( ) = goal_speed;
			}
		}

		void do_jumpspeed( ) {
			if ( user_interface::options::movement::jumpspeed_multiplier == 1.f ) return;

			float goal_speed = data->player->get_default_jumpspeed( ) * user_interface::options::movement::jumpspeed_multiplier;

			if ( data->player->get_jumpspeed( ) != goal_speed ) {
				data->player->get_jumpspeed( ) = goal_speed;
			}
		}

		void do_quickstop( ) {
			if ( !user_interface::options::movement::quickstop ) return;
			if ( !data->local_player_interface->on_ground( ) ) return;

			static vector3 empty {};
			if ( fabsf( data->forward_fraction ) + fabsf( data->strafe_fraction ) < 1 ) {
				data->set_velocity( empty );
			}
		}

		void do_fakelag( packet_constructor::movement_packet_t* packet ) {
			static int tick_counter = 0;
			if ( tick_counter >= user_interface::options::ragebot::fakelag_interval ) {
				data->server_position = packet->position;
				data->server_rotation = packet->rotation;

				tick_counter = 0;
			}

			packet->rotation = data->server_rotation;
			packet->position = data->server_position;

			tick_counter++;
		}
	};
}