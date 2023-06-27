#pragma once

namespace cheats {
	class auto_quest : public singleton<auto_quest> {
	private:
		tick_data* data;

		bool do_quest;

		std::vector<c_actor*> golden_eggs;
	public:
		void tick( tick_data* d ) {
			data = d;

			do_golden_egg( );
		}

		void start( ) {
			do_quest = true;
		}

		void reset( ) {
			golden_eggs.clear( ); golden_eggs.shrink_to_fit( );
		}

		void push_golden_egg( c_actor* a ) {
			std::string blueprint_name = a->get_blueprint_name( );

			if ( blueprint_name.find( "Egg" ) != std::string::npos ) {
				golden_eggs.push_back( a );
			}
		}

		void do_golden_egg( ) {
			if ( !data ) return;
			if ( !do_quest ) return;

			static int egg_index = 0;
			static bool teleport_to_egg = true;

			if ( egg_index >= golden_eggs.size( ) ) {
				do_quest = false; egg_index = 0;
				return;
			}

			auto egg = golden_eggs[ egg_index ];
			std::string blueprint_name = egg->get_blueprint_name( );

			if ( blueprint_name.find( "Ballmer" ) != std::string::npos ) {
				egg_index++;
			}

			static int counter = 0;
			if ( teleport_to_egg ) {
				vector3 position; egg->get_look_position( &position );
				data->set_position( position );
				counter++;

				if ( counter > 100 ) {
					teleport_to_egg = false;
					counter = 0;
				}
			} else {
				globals::cl_world_ptr->use( reinterpret_cast<c_player*>(data->player), egg );

				counter++;
				if ( counter > 50 ) {
					egg_index++;
					teleport_to_egg = true;
					counter = 0;
				}
			}

			// teleport to egg
			// wait 1 tick
			// use egg
		}
	};
}