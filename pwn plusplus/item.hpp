#pragma once

class i_item {
public:
	VFUNCTION( const char*, get_name, 1 );

	VFUNCTION( bool, can_activate, 8, void* );

	VFUNCTION( double, get_cooldown_time, 15 );

	VFUNCTION( i_item*, get_ammo_type, 17 );
	VFUNCTION( unsigned int, get_clip_size, 18 );
	VFUNCTION( int, get_damage, 19 );

	VFUNCTION( int, get_damage_type, 21 );
	VFUNCTION( int, get_mana_cost, 22 );

	VFUNCTION( unsigned int, get_number_of_projectiles, 25 );
	VFUNCTION( double, get_reload_time, 26, int );

	VFUNCTION( float, get_range, 28 );
	VFUNCTION( int, get_trade_value, 29 );

	VFUNCTION( void, activate, 32, void* );
	VFUNCTION( void, local_activate, 33, void* );

	VFUNCTION( float, get_spread_angle, 36 );
};

struct item_and_count {
public:
	i_item* item; // 0x00
	int count; // 0x04
	int loaded_ammo; // 0x08
};