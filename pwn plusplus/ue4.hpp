#pragma once

struct movement_component_t {
private:
	char pad_0x00[ 0xF4 ];
	int movement_mode; // 0xF4

public:
	int& get_movement_mode( ) {
		return movement_mode;
	}
};

class c_actor_character {
private:
	char pad_0x00[ 0x55 ];
	byte collision; // 0x55
	char pad_0x56[ 0x223 ];
	movement_component_t* movement_component; // 0x27C

public:
	movement_component_t* get_movement_component( ) {
		return movement_component;
	}

	byte& has_collision( ) {
		return collision;
	}

	friend class c_engine;
};

class c_camera_manager {
private:
	char pad_0x00[ 0x294 ];
	vector3 position;
	vector2 angles;
	char pad_0x2a8[ 0x3 ];
	float field_of_view; // 0x2AC

public:
	vector3& get_position( ) {
		return position;
	}

	vector2& get_angles( ) {
		return angles;
	}

	float& get_field_of_view( ) {
		return field_of_view;
	}

	friend class c_engine;
};

class c_player_controller {
private:
	char pad_0x00[ 0x244 ];
	c_actor_character* actor_character; // 0x244
	char pad_0x248[ 0x4C ];
	c_camera_manager* camera_manager; // 0x294

public:
	c_actor_character* get_character( ) {
		return actor_character;
	}

	c_camera_manager* get_camera_manager( ) {
		return camera_manager;
	}

	friend class c_engine;
};

class c_ue4_local_player {
private:
	char pad_0x00[ 0x20 ];
	c_player_controller* controller; // 0x20

public:
	c_player_controller* get_player_controller( ) {
		return controller;
	}

	friend class c_engine;
};

class c_engine_local_players {
public:
	c_ue4_local_player* local_player; // 0x00
};

class c_game_engine {
private:
	char pad_0x00[ 0x24 ];
	c_engine_local_players* local_players; // 0x24

public:
	c_engine_local_players* get_local_players( ) {
		return local_players;
	}

	friend class c_engine;
};

class c_engine {
private:
	char pad_0x00[ 0x6B0 ];
	c_game_engine* game_engine; // 0x6B0

public:
	c_ue4_local_player* get_local_player( ) {
		return game_engine->local_players->local_player;
	}

	c_player_controller* get_player_controller( ) {
		return get_local_player( )->controller;
	}

	c_actor_character* get_character( ) {
		return get_player_controller( )->actor_character;
	}

	c_camera_manager* get_camera_manager( ) {
		return get_player_controller( )->camera_manager;
	}
};