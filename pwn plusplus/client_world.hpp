#pragma once

class c_world {
public:
	void* __vftable;
	std::set<actor_ref<c_player>> m_players;
	std::set<actor_ref<c_actor>> m_actors;
	std::map<unsigned int, actor_ref<c_actor>> m_actorsById;
	void* m_localPlayer;
	int m_nextId;
	std::map<std::string, void*> m_aiZones;
	// 0x2C

public:
	VFUNCTION( void, use, 8, c_player* invoker, c_actor* target );

	VFUNCTION( void, activate, 9, void* player, void* item );
	VFUNCTION( void, jump, 11, bool state );
	VFUNCTION( void, fire_request, 13, bool state );
	VFUNCTION( void, chat, 19, c_player* player, const std::string* message );

	VFUNCTION( void, send_mana_update_event, 38, void*, int );
	VFUNCTION( void, send_pvp_enable_event, 41, c_player*, bool );
	VFUNCTION( void, send_state_event, 42, c_actor* actor, const std::string* name, bool state );
	VFUNCTION( void, send_fire_bullets, 44, c_actor* actor, i_item* item, const vector3* target, unsigned int count, float spread_angle );

public:
	c_actor* get_actor_by_blueprint( const char* name ) {
		for ( auto& actor : this->m_actors ) {
			if ( !strcmp( actor.object->get_blueprint_name( ), name ) )
				return actor.object;
		}

		return nullptr;
	}
};

class c_client_world : public c_world {
	actor_ref<c_player> m_activePlayer;
	float m_timeUntilNextTick;
};