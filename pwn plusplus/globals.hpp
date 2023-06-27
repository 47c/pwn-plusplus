#pragma once

namespace globals {
	inline c_game_server_connection* game_server_conn_ptr = nullptr;
	inline c_server_connection* server_conn_ptr = nullptr;
	inline c_player* player_ptr = nullptr;
	inline c_client_world* cl_world_ptr = nullptr;
	inline c_world* world_ptr = nullptr;
	inline c_engine* engine_ptr = nullptr;
	inline void* game_api_ptr = nullptr;
	inline void* cl_handler_ptr = nullptr;

	inline HMODULE instance = nullptr;

	inline void update_pointers( ) {
		if ( !world_ptr )
			world_ptr = deref_ptr<c_world>( offsets::map[ "GameWorld::Get" ] );

		if ( !game_server_conn_ptr )
			game_server_conn_ptr = deref_ptr<c_game_server_connection>( offsets::map[ "GameServerConnection::Get" ] );

		if ( !server_conn_ptr )
			server_conn_ptr = deref_ptr<c_server_connection>( offsets::map[ "GameServerConnection::Get" ] );
	}

	inline void invalidate_pointers( ) {
		cl_handler_ptr = nullptr; game_api_ptr = nullptr;
		world_ptr = nullptr; cl_world_ptr = nullptr;
		player_ptr = nullptr; server_conn_ptr = nullptr;
		game_server_conn_ptr = nullptr;
	}
}