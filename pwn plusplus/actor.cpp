#include "pch.h"

namespace hooks {
	namespace actor {
		actortick_function_type original_tick = nullptr;
		void __fastcall tick( void* ecx, void* edx, float delta_time ) {
			original_tick( ecx, edx, delta_time );
		
		}

		actordamage_function_type original_damage = nullptr;
		void __fastcall damage( void* thisptr, void* ecx, void* instigator, void* item, int dmg, int dmg_type ) {
			if ( shutdown || in_setup ) return original_damage( thisptr, ecx, instigator, item, dmg, dmg_type );
			if ( !globals::player_ptr ) return original_damage( thisptr, ecx, instigator, item, dmg, dmg_type );

			original_damage( thisptr, ecx, instigator, item, dmg, dmg_type );
		}

		actorfirebullets_function_type original_firebullets = nullptr;
		void __fastcall firebullets( void* ecx, void* edx, i_item* item, int dmg, damage_type_e type, vector3* target, float range, unsigned int count, float spread_angle ) {

			original_firebullets( ecx, edx, item, dmg, type, target, range, count, spread_angle );
		}

		itempickupcanuse_function_type original_canuse = nullptr;
		bool __fastcall canuse( void* ecx, void* edx, c_player* player ) {
			return true;
		}

		actoronspawnactor_function_type original_onspawnactor = nullptr;
		void __fastcall onspawnactor( void* ecx, void* edx, void* target ) {
			original_onspawnactor( ecx, edx, target );

			cheats::auto_quest::get( ).push_golden_egg(
				reinterpret_cast< c_actor* >( ecx ) 
			);
		}

		gungetreloadtime_function_type original_getreloadtime = nullptr;
		double __fastcall getreloadtime( void* ecx, void* edx, int additional_ammo ) {
			return user_interface::options::ragebot::no_reload ? 0.f : original_getreloadtime( ecx, edx, additional_ammo );
		}

		pistolgetcooldowntime_function_type original_getcooldowntime = nullptr;
		double __fastcall getcooldowntime( void* ecx, void* edx ) {
			return 0.f;
		}

		aiactortick_function_type original_aiactor_tick = nullptr;
		void __fastcall aitick( void* ecx, void* edx, float delta_time ) {
			auto actor = reinterpret_cast< c_actor* >( ecx );

			// replace string searching by hash-comparing for improved performance
			std::string name = actor->get_blueprint_name( );
			if ( name.find( "Bear" ) == std::string::npos &&
				 name.find( "Rat" ) == std::string::npos &&
				 name.find( "Spider" ) == std::string::npos ) return;

			tick_data::get( ).push_target( actor );

			original_aiactor_tick( ecx, edx, delta_time);
		}
	}

	namespace drop {
		dropconstructor_function_type original_constructor = nullptr;
		void __fastcall constructor( void* ecx, void* edx, const std::map<i_item*, unsigned int>* items, const std::string* blueprint_name ) {
			original_constructor( ecx, edx, items, blueprint_name );
		}
	}
}