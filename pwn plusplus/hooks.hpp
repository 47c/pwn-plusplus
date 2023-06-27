#pragma once

namespace hooks
{
	class hook_manager : public singleton<hook_manager> {
	private:
		std::vector<std::string> status_codes = {
			"initialized",
			"success",
			"disabled successfully",
			"running partially",
			"failed initialization",
			"failed to disable",
		};

		enum hook_manager_status_t {
			STATUS_INITIALIZED,
			STATUS_SUCCESS,
			STATUS_DISABLED,
			STATUS_PARTIAL,
			STATUS_FAILED_INITIALIZATION,
			STATUS_FAILED_DISABLE
		};

		struct hook_data_t {
		private:
			void** function_pointer;

			LPVOID* original_call;
			LPVOID detour;

			const char* name;

		public:
			hook_data_t( const char* function_name ) { 
				name = function_name;
				function_pointer = nullptr;
				original_call = nullptr;
				detour = nullptr;
			}

			hook_data_t* set_address( uintptr_t address ) { 
				function_pointer = reinterpret_cast< void** >( address );
				return this; 
			}
			hook_data_t* set_original( LPVOID* original ) {
				original_call = original;
				return this;
			}
			hook_data_t* set_detour( LPVOID func ) {
				detour = func;
				return this;
			}

			void** get_function_pointer( ) { return function_pointer; }

			LPVOID* get_original_call( ) { return original_call; }
			LPVOID get_detour( ) { return detour; }

			const char* get_name( ) { return name; }
		};

		std::vector<hook_data_t> hook_queue;
	public:

		hook_data_t* add_function( const char* function_name ) {
			hook_queue.emplace_back( function_name );
			hook_data_t* entry = &hook_queue.back( );

			if ( offsets::map.find( function_name ) == offsets::map.end( ) ) return entry;

			entry->set_address( offsets::map[ function_name ] );

			return entry;
		}

		hook_manager_status_t intialize( ) {
			if ( MH_Initialize( ) != MH_OK ) return STATUS_FAILED_INITIALIZATION;

			return STATUS_INITIALIZED;
		}

		hook_manager_status_t unintialize( ) {
			if ( MH_Uninitialize( ) != MH_OK ) return STATUS_FAILED_DISABLE;

			return STATUS_DISABLED;
		}

		hook_manager_status_t create_hooks( ) {
			hook_manager_status_t status = STATUS_SUCCESS;

			for ( hook_data_t& data : hook_queue ) {
				auto hook_status = MH_CreateHook( data.get_function_pointer( ), data.get_detour( ),
												  data.get_original_call( ) );
				if ( hook_status != MH_OK ) {
					utilities::log( "{1} [{0}]", MH_StatusToString( hook_status ),
									fmt::styled( data.get_name( ), fg( fmt::color::crimson ) ) );
					status = STATUS_PARTIAL;
				}
			}

			return status;
		}

		hook_manager_status_t enable_hooks( ) {
			hook_manager_status_t status = STATUS_SUCCESS;

			for ( hook_data_t& data : hook_queue ) {
				auto hook_status = MH_QueueEnableHook( data.get_function_pointer( ) );
				if ( hook_status != MH_OK ) {
					utilities::log( "{1} [{0}]", MH_StatusToString( hook_status ),
									fmt::styled( data.get_name( ), fg( fmt::color::orange ) ) );
					status = STATUS_PARTIAL;
				} else utilities::log( "{}",
									   fmt::styled( data.get_name( ), fg( fmt::color::spring_green ) ) );
			}

			return status;
		}

		hook_manager_status_t disable_hooks( ) {
			hook_manager_status_t status = STATUS_DISABLED;

			for ( hook_data_t& data : hook_queue ) {
				if ( MH_DisableHook( data.get_function_pointer( ) ) != MH_OK ) status = STATUS_FAILED_DISABLE;
			}

			return status;
		}

		fmt::detail::styled_arg<std::string> interpret( hook_manager_status_t code ) {
			if ( code <= STATUS_DISABLED )
				return fmt::styled( status_codes[ code ], fg( fmt::color::spring_green ) );
			else if ( code == STATUS_PARTIAL )
				return fmt::styled( status_codes[ code ], fg( fmt::color::orange ) );
			else 
				return fmt::styled( status_codes[ code ], fg( fmt::color::crimson ) );
		}
	};

	extern bool start( );
	extern bool end( );

	inline bool shutdown = false;
	inline bool in_setup = false;
}

namespace hooks {
	namespace winapi {
		extern WNDPROC original_window_proc;
		extern LRESULT CALLBACK window_proc( const HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	}

	namespace winsock {
		typedef int( __stdcall* ws2_32send_function_type )( SOCKET s, const char* buf, int len, int flags );
		extern ws2_32send_function_type original_send;
		extern int __stdcall send( SOCKET s, const char* buf, int len, int flags );
	}

	namespace directx {
		typedef long( __stdcall* d3d11present_function_type )( IDXGISwapChain*, UINT, UINT );
		extern d3d11present_function_type original_present;
		extern long __stdcall present( IDXGISwapChain* swapchain, UINT sync_interval, UINT flags );

		typedef void( __stdcall* d3d11dip_function_type )( ID3D11DeviceContext*, UINT, UINT, INT );
		extern d3d11dip_function_type original_draw_indexed_primitive;
		extern void __stdcall draw_indexed_primitive( ID3D11DeviceContext* context, UINT index_count, UINT start_index_location, INT base_vertex_location );

		typedef void( __stdcall* d3d11ssr_function_type )( ID3D11DeviceContext*, UINT, UINT, ID3D11ShaderResourceView* const* );
		extern d3d11ssr_function_type original_set_shader_resources;
		extern void __stdcall set_shader_resources( ID3D11DeviceContext* context, UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* shader_resource_views );

		typedef long( __stdcall* d3d11resizebuffers_function_type )( IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT );
		extern d3d11resizebuffers_function_type original_resize_buffers;
		extern long __stdcall resize_buffers( IDXGISwapChain* swapchain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT flags );
	}

	namespace game_api {
		typedef void( __fastcall* gameapitick_function_type )( void*, void*, float );
		extern gameapitick_function_type original_tick;
		extern void __fastcall tick( void* ecx, void* edx, float delta_time );
	
		typedef void( __fastcall* gameapilogin_function_type )( void*, void*, const char*, const char* );
		extern gameapilogin_function_type original_login;
		extern void __fastcall login( void* ecx, void* edx, const char* username, const char* password );

		typedef void( __fastcall* gameapicreatecharacter_function_type )( void*, void*, const char*, uint8_t, unsigned int* );
		extern gameapicreatecharacter_function_type original_createcharacter;
		extern void __fastcall createcharacter( void* ecx, void* edx, const char* name, uint8_t avatar, unsigned int* colors );

		typedef void( __fastcall* threadactionqueueprocess_function_type )( void*, void* );
		extern threadactionqueueprocess_function_type original_process;
		extern void __fastcall process( thread_action_queue_t* ecx, void* edx );
	}

	namespace game_server_connection {
		typedef void( __fastcall* gameserverconnectionconnect_function_type )( void*, void*, const std::string*, uint16_t, int, const std::string*, void* );
		extern gameserverconnectionconnect_function_type original_connect;
		extern void __fastcall connect( void* ecx, void* edx, const std::string* host, uint16_t port, int id, const std::string* token, void* completion );

		typedef void( __fastcall* gameserverconnectiondesctructor_function_type ) ( void*, void* );
		extern gameserverconnectiondesctructor_function_type original_destructor;
		extern void __fastcall destructor( void* ecx, void* edx );

		typedef void( __fastcall* masterserveconnectioncharacterinfo_function_type )( character_info_t*, void*, const character_info_t* );
		extern masterserveconnectioncharacterinfo_function_type original_characterinfo;
		extern void __fastcall characterinfo( character_info_t* ecx, void* edx, character_info_t* __that );

		typedef void( __fastcall* clienthandlerprocesscommands_function_type )( void*, void* );
		extern clienthandlerprocesscommands_function_type original_process_commands;
		extern void __fastcall process_commands( c_client_handler* ecx, void* edx );
	}

	namespace client_world {
		typedef void( __fastcall* clientworldtick_function_type )( void*, void*, float );
		extern clientworldtick_function_type original_tick;
		extern void __fastcall tick( void* ecx, void* edx, float delta_time );

		typedef void( __fastcall* clientworldjump_function_type )( void*, void*, bool );
		extern clientworldjump_function_type original_jump;
		extern void __fastcall jump( void* ecx, void* edx, bool state );

		typedef void( __fastcall* worldspawnactorwithid_function_type )( void*, void*, unsigned int, void*, const vector3*, const vector3* );
		extern worldspawnactorwithid_function_type original_spawnactorwithid;
		extern void __fastcall spawnactorwithid( void* ecx, void* edx, unsigned int id, void* object, const vector3* position, const vector3* rotation );
	}

	namespace player {
		typedef void( __fastcall* playertick_function_type )( void*, void*, float );
		extern playertick_function_type original_tick;
		extern void __fastcall tick( void* ecx, void* edx, float delta_time );

		typedef bool( __fastcall* playercanjump_function_type )( void*, void* );
		extern playercanjump_function_type original_canjump;
		extern bool __fastcall can_jump( void* ecx, void* edx );

		typedef void( __fastcall* playerdamage_function_type )( void*, void*, void*, void*, int, int );
		extern playerdamage_function_type original_damage;
		extern void __fastcall damage( void* ecx, void* edx, void* instigator, void* item, int dmg, int dmg_type );

		typedef i_item* ( __fastcall* playergetitemforslot_function_type )( void*, void*, unsigned int );
		extern playergetitemforslot_function_type original_getitemforslot;
		extern i_item* __fastcall get_item_for_slot( void* ecx, void* edx, unsigned int slot );

		typedef void( __stdcall* playercontrollerfov_function_type )( float );
		extern playercontrollerfov_function_type original_fov;
		extern void __stdcall fov( float fov );
	}

	namespace actor {
		typedef void( __fastcall* actortick_function_type )( void*, void*, float );
		extern actortick_function_type original_tick;
		extern void __fastcall tick( void* ecx, void* edx, float delta_time );

		typedef void( __fastcall* actordamage_function_type )( void*, void*, void*, void*, int, int );
		extern actordamage_function_type original_damage;
		extern void __fastcall damage( void* ecx, void* edx, void* instigator, void* item, int dmg, int dmg_type );

		typedef void( __fastcall* actorfirebullets_function_type ) ( void*, void*, i_item*, int, damage_type_e, vector3*, float, unsigned int, float );
		extern actorfirebullets_function_type original_firebullets;
		extern void __fastcall firebullets( void* ecx, void* edx, i_item* item, int dmg, damage_type_e type, vector3* target, float range, unsigned int count, float spread_angle );
		
		typedef bool( __fastcall* itempickupcanuse_function_type )( void*, void*, c_player* );
		extern itempickupcanuse_function_type original_canuse;
		extern bool __fastcall canuse( void* ecx, void* edx, c_player* player );

		typedef void( __fastcall* actoronspawnactor_function_type )( void*, void*, void* );
		extern actoronspawnactor_function_type original_onspawnactor;
		extern void __fastcall onspawnactor( void* ecx, void* edx, void* target );

		typedef double( __fastcall* gungetreloadtime_function_type )( void*, void*, int );
		extern gungetreloadtime_function_type original_getreloadtime;
		extern double __fastcall getreloadtime( void* ecx, void* edx, int additional_ammo );

		typedef double( __fastcall* pistolgetcooldowntime_function_type )( void*, void* );
		extern pistolgetcooldowntime_function_type original_getcooldowntime;
		extern double __fastcall getcooldowntime( void* ecx, void* edx );

		typedef void( __fastcall* aiactortick_function_type )( void*, void*, float );
		extern aiactortick_function_type original_aiactor_tick;
		extern void __fastcall aitick( void* ecx, void* edx, float delta_time );
	}

	namespace drop {
		typedef void( __fastcall* dropconstructor_function_type )( void*, void*, const std::map<i_item*, unsigned int>*, const std::string* );
		extern dropconstructor_function_type original_constructor;
		extern void __fastcall constructor( void* ecx, void* edx, const std::map<i_item*, unsigned int>* items, const std::string* blueprint_name );
	}
}