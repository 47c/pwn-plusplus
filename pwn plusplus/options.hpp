#pragma once

namespace user_interface {
	namespace options {
		// work on actual implementation using hash-maps
		namespace ragebot {
			inline bool auto_fire = false;
			inline bool target_ai = true;

			inline bool no_reload = false;
			inline bool refill_ammo = false;

			inline int fakelag_interval = 0;
			inline int yaw_modifier_mode = 0;
		}

		namespace movement {
			inline bool noclip = false;
			inline bool fly = true;
			inline bool quickstop = true;
			inline float movespeed_multiplier = 2.f;
			inline float jumpspeed_multiplier = 2.f;
		}

		namespace visuals {
			inline bool z_buffer = false;
			inline float field_of_view = 90.f;
			inline float chams_color[ 4 ];
		}

		namespace misc {
			enum drop_type_e {
				drop_white,
				drop_green,
				drop_blue,
				drop_purple,
				drop_max_size
			};

			inline bool auto_loot[ drop_max_size ] = { true, true, true, true };
		}
	}
}