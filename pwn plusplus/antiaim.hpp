#pragma once

namespace cheats {
	class antiaim : public singleton<antiaim> {
	private:
		tick_data* data;

	public:
		void tick( tick_data* d ) {
			data = d;

			apply_yaw( );
		}

		void apply_yaw( ) {
			auto yaw_modifier_mode = user_interface::options::ragebot::yaw_modifier_mode;
			if ( !yaw_modifier_mode ) return;

			if ( yaw_modifier_mode == 1
				 || yaw_modifier_mode == 3 ) {

				static float base_yaw = data->look_rotation.y + 180.f;
				float temp_base_yaw = data->look_rotation.y;
				yaw_at_target( temp_base_yaw );

				base_yaw = math::interpolate( std::remainderf( base_yaw, 360.f ), std::remainderf( temp_base_yaw + 180.f, 360.f ), 0.05f );

				static float c = 0;
				if ( data->player->get_time( ) % 13 )
					c += 0.1f;

				float range = 65.f;
				data->rotation.y = base_yaw + ( cos( c ) * range );
			}
		}

		void apply_on_packet( packet_constructor::movement_packet_t& mov_packet ) {
			if ( user_interface::options::ragebot::yaw_modifier_mode >= 2 ) {
				static float base_yaw = tick_data::get( ).look_rotation.y + 180.f;
				float temp_base_yaw = tick_data::get( ).look_rotation.y;
				yaw_at_target( temp_base_yaw );

				base_yaw = math::interpolate( std::remainderf( base_yaw, 360.f ), std::remainderf( temp_base_yaw + 180.f, 360.f ), 0.01f );

				static float c = 0;
				float range = 65.f;
				mov_packet.rotation.y = base_yaw + ( cos( c ) * range );
				c += 1;
			}
		}

		void yaw_at_target( float& yaw ) {
			if ( !data ) return;
			
			auto &aimbot = aimbot::get( );
			if ( !aimbot.has_target( ) ) return;

			yaw = std::remainderf( aimbot.get_angle( ).y, 360.f );
		}
	};
}