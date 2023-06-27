#pragma once

class i_player{
public:
	VFUNCTION( bool, is_pvp_desired, 7 );
	VFUNCTION( void, set_pvp_desired, 8, bool );

	VFUNCTION( unsigned int, get_loaded_ammo, 11, i_item * );
	VFUNCTION( i_item *, get_item_for_slot, 16, unsigned int );

	VFUNCTION( unsigned int, get_slot, 18 );
	VFUNCTION( void, set_slot, 19, unsigned int );
	VFUNCTION( i_item *, get_item, 20 );

	VFUNCTION( void, set_item_cooldown, 23, i_item *, float, bool );
	VFUNCTION( bool, is_item_on_cooldown, 24, i_item * );
	VFUNCTION( double, get_item_cooldown, 25 );

	VFUNCTION( bool, can_reload, 41 );
	VFUNCTION( void, reload, 42 );

	VFUNCTION( void, set_jump_state, 48, bool );
	VFUNCTION( void, set_fire_request, 50, bool );

	VFUNCTION( void, buy_item, 52, c_actor *npc, i_item *item, unsigned int count );
};

class i_local_player{
public:
	VFUNCTION( vector3 *, get_position, 2, vector3 * );
	VFUNCTION( vector3 *, get_rotation, 3, vector3 * );
	VFUNCTION( vector3 *, get_projectile_position, 4, vector3 * );
	VFUNCTION( vector3 *, get_velocity, 5, vector3 * );
	VFUNCTION( void, set_position, 6, vector3 * );
	VFUNCTION( void, set_rotation, 7, vector3 * );
	VFUNCTION( void, set_velocity, 8, vector3 * );
	VFUNCTION( void, set_movement_fractions, 9, float &, float & );

	VFUNCTION( c_actor *, line_trace, 17, const vector3 & );
	VFUNCTION( void, fire_bullets, 18, i_item *, int, int, const vector3 *, float, unsigned int, float );
	VFUNCTION( bool, on_ground, 20 );

	VFUNCTION( float, get_forward_fraction, 25 );
	VFUNCTION( float, get_strafe_fraction, 26 );

	VFUNCTION( void, display_event, 28, const char *, const char * );
	VFUNCTION( void, display_message, 29, const char *, const char * );
};

class c_player : public c_actor{
public:
	void *Actor; // 0x00
	int m_time; // 0x04
	char pad0x00[ 0x68 ];
	i_player *IPlayer; //0x70
	int m_characterId; // 0x74
	std::string m_name; //0x78
	std::string m_teamName; // 0x90
	int m_avatarIndex; // 0xA8
	char colors[ 0x10 ]; // 0xAC
	std::map<i_item *, item_and_count> m_inventory; //std::map<IItem *,ItemAndCount> // 0xBC
	std::set<std::string> m_pickups; // 0xC4
	std::map<i_item *, float> m_cooldowns; //std::map<IItem *,float> // 0xCC
	std::map<std::string, unsigned int> m_circuitInputs; // 0xD4
	std::map<std::string, std::vector<bool>> m_circuitOutputs; // 0xDC
	bool m_admin; // 0xE4
	bool m_pvpEnabled; // 0xE5
	bool m_pvpDesired; // 0xE6
	char pad0xE6[ 0x1 ];
	float m_pvpChangeTimer; // 0xE8
	float m_pvpChangeReportedTimer; // 0xEC
	int m_changingServerRegion; // 0xF0
	std::string m_currentRegion; // 0xF4
	std::string m_changeRegionDestination; // 0x10C
	std::set<std::string> m_aiZones; // 0x124
	int m_mana; // 0x12C
	int m_manaRegenTimer; // 0x130
	int m_healthRegenCooldown; // 0x134
	int m_healthRegenTimer; // 0x138
	int m_countdown; // 0x13C
	vector3 m_remoteLookPosition; // 0x140
	vector3 m_remoteLookRotation; // 0x14C
	i_item *m_equipped; // 0x158
	char pad0x158[ 0x24 ];
	int m_currentSlot; // 0x180
	void *m_questStates; // std::map<IQuest *,PlayerQuestState> // 0x184
	char pad0x184[ 0x4 ];
	float m_currentQuest; // 0x18C
	float m_walkingSpeed; // 0x190
	float m_jumpSpeed; // 0x194
	float m_jumpHoldTime; // 0x198
	void *m_currentNPC; // ActorRef<NPC> // 0x19C
	std::string m_currentNPCState; // 0x1A0
	i_local_player *m_localPlayer; // 0x1B8
	int m_eventsToSend; // 0x1BC
	int m_itemsUpdated; // 0x1C0
	int m_itemSyncTimer; // 0x1C4
	int m_chatMessageCounter; // 0x1C8
	int m_chatFloodDecayTimer; // 0x1CC
	int m_lastHitByItem; // 0x1D0
	int m_lastHitItemTimeLeft; // 0x1D4
	int m_circuitStateCooldownTimer; // 0x1D8

public:

	void *get_actor( ){
		return *reinterpret_cast< void ** >( this->Actor );
	}

	std::map<std::string, unsigned int> &get_circuit_inputs( ){
		return this->m_circuitInputs;
	}

	std::map<i_item *, float> &get_cooldowns( ){
		return this->m_cooldowns;
	}
	std::map<i_item *, item_and_count> &get_inventory( ){
		return this->m_inventory;
	}

	i_item *get_item( const char *name ){
		for ( auto &entry : get_inventory( ) ){
			if ( !strcmp( entry.first->get_name( ), name ) )
				return entry.first;
		} return nullptr;
	}

	item_and_count get_item_and_count( const char *name ){
		for ( auto &entry : get_inventory( ) ){
			if ( !strcmp( entry.second.item->get_name( ), name ) )
				return entry.second;
		} return item_and_count( );
	}

	i_item *get_equipped( ){
		return this->m_equipped;
	}

	i_player *get_interface( ){
		return reinterpret_cast< i_player * >( address( this ) + 0x70 );
	}

	i_local_player *get_localplayer( ){
		return this->m_localPlayer;
	}

	vector3 &get_remote_look_position( ){
		return this->m_remoteLookPosition;
	}

	vector3 &get_remote_look_rotation( ){
		return this->m_remoteLookRotation;
	}

	std::string &get_name( ){
		return this->m_name;
	}

	std::string &get_team_name( ){
		return this->m_teamName;
	}

	std::string &get_region( ){
		return this->m_currentRegion;
	}

	float &get_pvp_change_timer( ){
		return this->m_pvpChangeTimer;
	}

	bool &is_pvp_enabled( ){
		return this->m_pvpEnabled;
	}

	bool &is_pvp_desired( ){
		return this->m_pvpDesired;
	}

	bool &is_admin( ){
		return this->m_admin;
	}

	bool &is_attacking( ){
		return *reinterpret_cast< bool * >( reinterpret_cast< uintptr_t > ( this ) + 0xD0 );
	}

	int &get_mana( ){
		return this->m_mana;
	}

	int &get_time( ){
		return this->m_time;
	}

	float &get_walkspeed( ){
		return this->m_walkingSpeed;
	}

	float &get_jumpspeed( ){
		return this->m_jumpSpeed;
	}

	float &get_jump_holdtime( ){
		return this->m_jumpHoldTime;
	}

	float get_default_walkspeed( ){
		return 400.0f;
	}

	float get_default_jumpspeed( ){
		return 420.0f;
	}

	float get_default_jumpholdtime( ){
		return 0.200f;
	}
};

class character_info_t{
public:
	int id; // 0x00
	std::string name; // 0x04
	std::string team; // 0x1C
	std::string location; // 0x34
	char avatar; // 0x4C
	char pad_0x4C[ 0x3 ];
	char colors[ 0x10 ]; // 0x50
	int flags; // 0x60
	bool transitioning; // 0x64
	char pad_0x64[ 0x3 ];
	vector3 transition_point; // 0x68
	int transition_health; // 0x74
	int transition_mana; // 0x78
	bool transition_pvp; // 0x7C
	bool admin; // 0x7D
	char pad_0x7D[ 0xB ];
	std::string active_quest; // 0x88
	std::map<std::string, item_and_count> items; // 0xA0
	std::vector<std::string> equipped; // 0xA8
	char active_slot; // 0xB4
	char pad_0xB4[ 0x3 ];
	std::set<std::string> pickups; // 0xB8
};