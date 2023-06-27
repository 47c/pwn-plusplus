#pragma once

namespace cheats {
	class refill_ammo : public singleton<refill_ammo> {
	private: 
		tick_data* data;

		int last_buy_time;
	public:
		void tick( tick_data* d ) {
			data = d;

			do_refill( );
		}

		void do_refill( ) {
			if ( !user_interface::options::ragebot::refill_ammo ) return;

			if ( data->player->get_time( ) - last_buy_time <= 200 ) return;

			auto item = data->player_interface->get_item( );
			if ( item->get_mana_cost( ) ) return;
			if ( !item->get_ammo_type( ) ) return;

			int ammo_inventory = data->player->get_item_and_count( item->get_ammo_type( )->get_name( ) ).count;
			int clip_size = item->get_clip_size( );

			if ( ammo_inventory >= clip_size ) return;

			data->player_interface->buy_item( globals::world_ptr->get_actor_by_blueprint( "GunShopOwner" ),
											  item->get_ammo_type( ), clip_size - ammo_inventory );

			last_buy_time = data->player->get_time( );
		}
	};
}