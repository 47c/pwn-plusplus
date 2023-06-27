#pragma once

class tick_data : public singleton<tick_data> {
public:
	c_player* player;
	i_player* player_interface;
	i_local_player* local_player_interface;

	vector3 position, look_position, server_position, rotation, 
		look_rotation, server_rotation, velocity;
	float forward_fraction, strafe_fraction, delta_time;

	std::deque<c_actor*> target_actors;

	tick_data( ) {
		player = nullptr;
		player_interface = nullptr;
		local_player_interface = nullptr;

		position = look_position = server_position 
			= velocity = rotation = server_rotation = vector3( );
		forward_fraction = strafe_fraction = delta_time = 0.f;

		reset_targets( );
	}

	~tick_data( ) { }

	void reset_targets( ) { target_actors.clear( ); target_actors.shrink_to_fit( ); }
	void push_target( c_actor* a ) { 
		if ( std::find( target_actors.begin( ), target_actors.end( ), a ) != target_actors.end( ) ) return;
		if ( !a->is_player( ) && !user_interface::options::ragebot::target_ai ) return;

		target_actors.push_back( a ); 
	}
	void pop_target( c_actor* a ) { 
		auto it = std::find( target_actors.begin( ), target_actors.end( ), a );
		if ( it == target_actors.end( ) ) return;

		target_actors.erase( it );
	}

	void update_targets( ) {
		for ( auto actor : target_actors ) {
			if ( !actor ) pop_target( actor );
			if ( actor->is_player( ) ) {
				if ( !player->is_pvp_enabled( ) ) pop_target( actor );
				if ( !reinterpret_cast< c_player* >( actor )->is_pvp_enabled( ) ) pop_target( actor );
			} else if ( !user_interface::options::ragebot::target_ai ) pop_target( actor );
		}
	}

	void populate( c_player* p, float dt ) {
		delta_time = dt;

		player = p;
		player_interface = p->get_interface( );
		local_player_interface = p->get_localplayer( );

		if ( local_player_interface ) {
			local_player_interface->get_velocity( &velocity );
			local_player_interface->get_position( &position );
			local_player_interface->get_rotation( &rotation );

			forward_fraction = local_player_interface->get_forward_fraction( );
			strafe_fraction = local_player_interface->get_strafe_fraction( );
		}

		p->get_look_rotation( &look_rotation );
		p->get_look_position( &look_position );
	}

	void apply( ) {
		local_player_interface->set_rotation( &rotation );
		local_player_interface->set_position( &position );
		local_player_interface->set_velocity( &velocity );
	}

	void reset( ) {
		reset_targets( );

		auto empty = tick_data( );
		memcpy( this, &empty, sizeof tick_data );
	}

	void set_velocity( vector3& v ) { velocity = v; }
	void set_position( vector3& p ) { position = p; }
};