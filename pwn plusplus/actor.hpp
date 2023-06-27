#pragma once

class i_ue4_actor {
public:
	VFUNCTION( vector3*, get_position, 2, vector3* result );
};

class c_actor {
public:
	int get_id( ) {
		return *reinterpret_cast< int* >( reinterpret_cast< uintptr_t >( this ) + 0x04 );
	}

public:
	VFUNCTION( i_ue4_actor*, get_ue4_actor, 1 );
	VFUNCTION( bool, is_npc, 2 );
	VFUNCTION( bool, is_player, 3 );

	VFUNCTION( const char*, get_blueprint_name, 9 );

	VFUNCTION( int, get_health, 12 );

	VFUNCTION( vector3*, get_look_position, 27, vector3* );
	VFUNCTION( vector3*, get_look_rotation, 28, vector3* );
	VFUNCTION( void, trigger_event, 38, const std::string*, void*, bool );
};