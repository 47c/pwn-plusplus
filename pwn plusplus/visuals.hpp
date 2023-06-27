#pragma once

namespace cheats {
	class visuals : public singleton<visuals> {
	private:
		tick_data* data;

	public:
		void tick( tick_data* d ) {
			data = d;

			modifier_fov( );
		}

		void modifier_fov( ) {
			if ( !globals::engine_ptr->get_camera_manager( ) ) return;
			
			// patch this to change fov (use more efficient method later)
			if ( offsets::map[ "CameraCache_FOV" ] ) nop_address( offsets::map[ "CameraCache_FOV" ], 6 );

			float goal_value = user_interface::options::visuals::field_of_view;
			static float current_value = 90.f;

			if ( fabsf( goal_value - current_value ) < 0.1f ) return;

			globals::engine_ptr->get_camera_manager( )->get_field_of_view( ) = current_value = math::interpolate( current_value, goal_value, 0.05f );
		}
	};
}