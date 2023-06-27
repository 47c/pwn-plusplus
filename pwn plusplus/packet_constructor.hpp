#pragma once

enum packet_types_e{
	packet_auth,
	packet_spawnpos_1,
	packet_spawnpos_2,
	packet_spawnpos_3,
	packet_spawnpos_4,
	packet_spawnpos_5,
	packet_spawnpos_6,
	packet_chat,
	packet_answer,
	packet_end_dialog,
	packet_send_dialog,
	packet_buy_item,
	packet_attack,
	packet_update_health,
	packet_remove_quest,
	packet_change_region,
	packet_new_item,
	packet_pickup_item,
	packet_unk,
	packet_fast_travel,
	packet_logic_gate,
	packet_jump,
	packet_update_mana,
	packet_entity,
	packet_new_quest,
	packet_position,
	packet_entity_position,
	packet_achievement,
	packet_pvp_state,
	packet_quest_select,
	packet_quest_complete,
	packet_remove_item,
	packet_sneak,
	packet_respawn,
	packet_teleport,
	packet_entity_state,
	packet_attack_state,
	packet_entity_remove,
	packet_slot,
	packet_unk2
};

class packet_constructor : public singleton<packet_constructor>{
private:

	static inline std::map<packet_types_e, const char *> packet_map = {
		{ packet_auth, "0200" },
		{ packet_spawnpos_1, "1600" },
		{ packet_spawnpos_2, "1700"  },
		{ packet_spawnpos_3, "2300" },
		{ packet_chat, "232a" },
		{ packet_answer, "233e" },
		{ packet_end_dialog, "2366" },
		{ packet_send_dialog, "2373" },
		{ packet_buy_item, "2462" },
		{ packet_attack, "2a69" },
		{ packet_update_health, "2b2b" },
		{ packet_spawnpos_4, "3003" },
		{ packet_logic_gate, "3031" },
		{ packet_spawnpos_5, "3206" },
		{ packet_spawnpos_6, "4103" },
		{ packet_remove_quest, "5e64" },
		{ packet_change_region, "6368" },
		{ packet_new_item, "6370" },
		{ packet_pickup_item, "6565" },
		{ packet_unk, "6576" },
		{ packet_fast_travel, "6674" },
		{ packet_jump, "6a70" },
		{ packet_update_mana, "6d61" },
		{ packet_entity, "6d6b" },
		{ packet_position, "6d76" },
		{ packet_new_quest, "6e71"},
		{ packet_entity_position, "7073" },
		{ packet_achievement, "7075" },
		{ packet_pvp_state, "7076" },
		{ packet_quest_select, "713d" },
		{ packet_quest_complete, "713e" },
		{ packet_sneak, "726e" },
		{ packet_respawn, "7273" },
		{ packet_teleport, "7274" },
		{ packet_entity_state, "7374" },
		{ packet_attack_state, "7472" },
		{ packet_entity_remove, "7878" },
		{ packet_slot, "733d" },
		{ packet_unk2, "7665" }
	};

public:

	struct packet_t{
	private:
		std::string buffer,
			identifier,
			payload;

		std::vector<std::string> content;
	public:

		packet_t( ) = default;
		~packet_t( ) = default;

		packet_t( std::string &a, std::vector<std::string> &b ){
			identifier = a;
			content = b;
		}

		packet_t *set_identifier( std::string a ){
			identifier = a;
			return this;
		}

		packet_t *set_identifier( packet_types_e t ){
			identifier = packet_map.at( t );
			return this;
		}

		packet_t *set_content( std::vector<std::string> a ){
			content = a;
			return this;
		}

		packet_t *update_content( std::vector<std::string> a ){
			content.clear( );

			content.push_back( identifier.substr( 0, 2 ) );
			content.push_back( identifier.substr( 2 ) );

			for ( auto &b : a ) content.push_back( b );

			content.shrink_to_fit( );

			return this;
		}

		packet_t *set_payload( std::string a ){
			payload = a;
			return this;
		}

		packet_t *set_payload( std::vector<std::string> &bytes ){
			payload.clear( );
			int counter = 0;
			for ( auto &byte : bytes ){
				//payload += "[" + std::to_string( counter ) + "]: " + byte + "    ";
				payload += byte;
				counter++;
			} return this;
		}

		std::string get_buffer( ){
			return buffer;
		}

		packet_t *set_buffer( std::string buf ){
			buffer = buf;

			return this;
		}

		packet_t *copy_data( packet_t &other ){
			identifier = other.get_identifier( );
			content = other.get_content( );
			buffer = other.get_buffer( );

			return this;
		}

		template< typename T>
		T get_as( ){
			T packet;
			packet.copy_data( *this );

			return packet;
		}

		void update( ){
			set_buffer(
				packet_constructor::get( ).get_buffer( get_content( ) )
			); set_payload( get_content( ) );
		}

		void push_to_queue( ){
			packet_constructor::get( ).buffer_to_queue( get_content( ) );
		}

		int force_send( ){
			utilities::log( "%s\n%s", packet_constructor::get( ).get_buffer( content ).c_str( ), get_buffer( ).c_str( ) );

			return packet_constructor::get( ).send_data(
				packet_constructor::get( ).get_buffer( content )
			);
		}

		std::string					get_payload( ){ return payload; }
		std::string					get_identifier( ){ return identifier; }
		std::vector<std::string> &get_content( ){ return content; }
	};

	struct movement_packet_t : public packet_t{
		enum packet_structure_t{
			identifier_bytes,

			positionx_bytes, positiony_bytes,
			positionz_bytes,

			rotationx_bytes, rotationy_bytes,
			rotationz_bytes,

			forwardfrac_byte, strafefrac_byte
		};

		vector3 position;
		vector3 rotation;
		float forward_fraction, strafe_fraction;

		movement_packet_t( ) = default;
		~movement_packet_t( ) = default;

		movement_packet_t( vector3 a, vector3 b ){
			position = a;
			rotation = b;

			forward_fraction = strafe_fraction = 0;
		}

		uint16_t get_pitch( ){
			static constexpr int limit = ( INT16_MAX + 1 ) * 0.5f;
			uint16_t pitch = ( limit / 90.f ) * std::remainderf( rotation.x, 180.f );

			return pitch;
		}

		uint16_t get_yaw( ){
			static constexpr int limit = ( INT16_MAX + 1 );
			uint16_t yaw = ( limit / 180.f ) * std::remainderf( rotation.y, 360.f );

			return yaw;
		}

		uint16_t get_roll( ){
			static constexpr int limit = ( INT16_MAX + 1 ) * 0.5f;
			uint16_t roll = ( limit / 90.f ) * std::remainder( rotation.z, 360.f );

			return roll;
		}

		movement_packet_t *extract( ){
			auto [identifier, x, y, z, pitch, yaw] = pystruct::unpack( PY_STRING( "=HfffHH" ), get_buffer( ) );

			static constexpr int limit_pitch = ( INT16_MAX + 1 ) * 0.5f;
			static constexpr int limit_yaw = INT16_MAX + 1;

			float pitch_f = std::remainderf( pitch / std::powf( 2, 14 ) * 90.f, 180.f ),
				yaw_f = std::remainderf( yaw / std::powf( 2, 15 ) * 180.f, 360.f );

			position = vector3( x, y, z );
			rotation = vector3( pitch_f, yaw_f );

			return this;
		}

		movement_packet_t *update( ){
			set_identifier( packet_position );

			std::stringstream stream { };
			auto content_pack = pystruct::pack( PY_STRING( "=fffHHH" ),
												position.x, position.y, position.z,
												get_pitch( ), get_yaw( ), get_roll( ) );

			packet_constructor::get( ).stream_append( content_pack, stream );
			update_content(
				packet_constructor::get( ).byte_array( stream.str( ).c_str( ), stream.str( ).size( ), 1 )
			); __super::update( );

			return this;
		}
	};

	struct attack_packet_t : public packet_t{
		enum packet_structure_t{
			identifier_bytes,
			name_size_bytes, name_bytes,

			pitch_bytes, yaw_bytes,

			spread_bytes
		};

		uint16_t size;
		std::string name;
		float pitch, yaw, spread;

		attack_packet_t( ) = default;
		~attack_packet_t( ) = default;

		attack_packet_t( uint16_t a, std::string b, float c, float d, float e ){
			size = a;
			name = b;
			pitch = c;
			yaw = d;
			spread = e;
		}

		attack_packet_t *set_size( uint16_t a ){
			size = a; return this;
		}
		attack_packet_t *set_name( std::string a ){
			name = a; return this;
		}
		attack_packet_t *set_pitch( float a ){
			pitch = a; return this;
		}
		attack_packet_t *set_yaw( float a ){
			yaw = a; return this;
		}
		attack_packet_t *set_spread( float a ){
			spread = a; return this;
		}

		attack_packet_t *extract( ){

			auto [identifier, size] = pystruct::unpack( PY_STRING( "=HH" ), get_buffer( ) );
			auto [pitch, yaw, spread] = pystruct::unpack( PY_STRING( "=fff" ), get_buffer( ).substr( 4 + size ) );

			this->size = size;
			{
				auto bytes = packet_constructor::get( ).byte_array(
					get_buffer( ).substr( 4, size ).c_str( ), size, 1
				); std::vector<uint16_t> byte_array {};

				for ( auto &b : bytes ){
					uint16_t value; sscanf_s( b.c_str( ), "%hx", &value );
					byte_array.push_back( value );
				} std::string name( byte_array.begin( ), byte_array.end( ) );

				this->name = name;
			}
			this->spread = spread;
			this->pitch = pitch;
			this->yaw = yaw;

			return this;
		}

		attack_packet_t *update( ){
			set_identifier( packet_attack );

			std::stringstream stream { };
			auto content_pack_first = pystruct::pack( PY_STRING( "=H" ),
													  size );

			auto content_pack_last = pystruct::pack( PY_STRING( "=fff" ),
													 pitch, yaw, spread );

			packet_constructor::get( ).stream_append( content_pack_first, stream );
			stream << name.c_str( );
			packet_constructor::get( ).stream_append( content_pack_last, stream );

			update_content(
				packet_constructor::get( ).byte_array( stream.str( ).c_str( ), stream.str( ).size( ), 1 )
			); __super::update( );

			return this;
		}
	};

	struct jump_packet_t : public packet_t{
		enum packet_structure_t{
			identifier_bytes,

			state_byte,
		};

		bool state;

		jump_packet_t( ) = default;
		~jump_packet_t( ) = default;

		jump_packet_t( vector3 a, vector3 b, bool c ){
			state = c;
		}

		bool get_state( ){ return state; }
	};

	struct pickup_packet_t : public packet_t{
		enum packet_structure_t{
			identifier_bytes,

			pickup_id_byte,
			empty_bytes,
		};

		unsigned int pickup_id;

		pickup_packet_t( ){
			pickup_id = -1;
		}
		pickup_packet_t( unsigned int a ){
			pickup_id = a;
		} ~pickup_packet_t( ) = default;

		int get_pickup_id( ){ return pickup_id; }
		void set_pickup_id( unsigned int a ){ pickup_id = a; }

		pickup_packet_t *update( ){
			set_identifier( packet_pickup_item );

			std::stringstream stream {};
			std::array id_packed = pystruct::pack( PY_STRING( "I" ), get_pickup_id( ) );

			packet_constructor::get( ).stream_append( id_packed, stream );

			update_content(
				packet_constructor::get( ).byte_array( stream.str( ).c_str( ), stream.str( ).size( ), 1 )
			); __super::update( );

			return this;
		}
	};

private:
	std::shared_timed_mutex buffer_mutex {};
	std::deque<std::string> buffer {};

	UINT_PTR socket;

	void queue_loop( ){
		do{
			if ( hooks::shutdown ) break;
			if ( !globals::game_server_conn_ptr ) continue;
			if ( globals::game_server_conn_ptr->m_tickInProgress ) continue;

			std::unique_lock<decltype( buffer_mutex )> lock( buffer_mutex );

			if ( buffer.empty( ) ) continue;

			send_data( buffer.back( ) );
			buffer.pop_back( );

			std::this_thread::sleep_for( 150ms );
		} while ( true );
	}

public:

	UINT_PTR get_socket( ){ return socket; }

	void update_socket( UINT_PTR s ){ socket = s; }
	void reset_socket( ){ socket = NULL; }

	void spawn_queue_thread( ){
		static auto flag = std::once_flag( );

		std::call_once( flag, [ & ]( ){
			( new std::thread( &packet_constructor::queue_loop, this ) )->detach( );
		} );
	}

	const char *get_identifier( const packet_types_e &type ){
		return packet_map.at( type );
	}

	std::vector<std::string> byte_array( const char *message, const int16_t &length, const byte hex = 0 ){
		std::stringstream ss; std::vector<std::string> bytes;
		for ( int i = 0; i < length; i++ ){
			unsigned char c = message[ i ];
			int16_t ord = static_cast< int16_t >( c );
			hex ? ( ss << std::setw( 2 ) << std::setfill( '0' ) << std::hex << ord ) : ( ss << ord );

			std::string buf { };
			buf += ss.str( ).c_str( )[ ss.str( ).size( ) - 2 ];
			buf += ss.str( ).c_str( )[ ss.str( ).size( ) - 1 ];

			bytes.push_back( buf );
			ss << " ";
		}

		return bytes;
	}

	std::string to_hex( const int32_t &value ){
		std::stringstream ss;
		ss << std::setfill( '0' ) << std::setw( 4 ) << std::hex << value;

		return ss.str( );
	}

	std::string get_buffer( const std::vector<std::string> &content ){
		std::string buffer { };

		for ( auto &byte : content ){
			short val;
			sscanf_s( byte.c_str( ), "%hx", &val );
			buffer += val;
		} return buffer;
	}

	void buffer_to_queue( const std::vector<std::string> &content ){
		buffer.push_front( get_buffer( content ) );
	}

	void buffer_to_queue( const std::string &content ){
		buffer.push_front( content );
	}

	template<typename type, size_t size>
	void stream_append( std::array<type, size> pack, std::stringstream &stream ){
		for ( const auto &a : pack ){
			stream << a;
		}
	}

	packet_t extract_packet_metadata( const char *buf, const int16_t &len ){
		std::vector<std::string> bytes = byte_array( buf, len, 1 );
		packet_t packet { };
		packet.set_identifier( bytes[ 0 ] + bytes[ 1 ] );

		std::string buffer {};
		buffer.assign( buf, buf + len ); // safe copy

		packet.set_content( bytes );
		packet.set_payload( bytes );
		packet.set_buffer( buffer );

		return packet;
	}

	float extract_data_raw( packet_t &packet, const int16_t &end, const int16_t &start, const uint8_t &int_mode = 0 ){
		constexpr float constant_90_ = 0.0054931640625; // (INT16_MAX * 0.5f) / 90.f
		constexpr float constant_180_ = ( INT16_MAX * 0.5f ) / 180.f;

		int32_t integer_ {};
		float out {};

		std::string byte_string = "0x";

		auto content = packet.get_content( );
		for ( int i = end; i > start - 1; i-- ){
			byte_string += content[ i ];
		}

		try{

			if ( int_mode > 0 ){
				sscanf_s( byte_string.c_str( ), "%Ix", &integer_ );

				switch ( int_mode ){
					case 2:
						out = ( integer_ ) *constant_90_;
						break;
					case 3:
						out = ( integer_ ) *constant_180_;
						break;
					default:
						out = integer_;
						break;
				}
			} else sscanf_s( byte_string.c_str( ), "%x", &out );

		} catch ( std::exception &ex ){ utilities::log( "{}", ex.what( ) ); }

		return out;
	}

	pickup_packet_t build_pickup_packet( const uint16_t id ){
		pickup_packet_t packet;

		packet.set_pickup_id( id );
		packet.set_identifier( packet_map.at( packet_pickup_item ) );
		packet.update( );

		return packet;
	}

	int send_data( const std::string &data ){
		static auto send_address = ( send_function ) GetProcAddress( GetModuleHandle( "ws2_32.dll" ), "send" );

		unsigned int length = data.size( );
		std::string buffer = data;

		while ( 1 ){
			int ret = ( *send_address )( packet_constructor::get( ).get_socket( ), buffer.c_str( ), length, 0 );
			if ( ret <= 0 )
				break;

			buffer += ret;
			length -= ret;

			if ( !length )
				return 1;
		}

		return 0;
	}
};