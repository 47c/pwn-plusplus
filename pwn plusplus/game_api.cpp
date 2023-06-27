#include "pch.h"

namespace hooks {
	namespace game_api {
		gameapitick_function_type original_tick = nullptr;
		void __fastcall tick( void* ecx, void* edx, float delta_time ) {
			if ( shutdown || in_setup ) return original_tick( ecx, edx, delta_time );

			packet_constructor::get( ).spawn_queue_thread( );
			if ( address( globals::game_api_ptr ) != address( ecx ) )
				globals::game_api_ptr = ecx;

			original_tick( ecx, edx, delta_time );
		}

		gameapilogin_function_type original_login = nullptr;
		void __fastcall login( void* ecx, void* edx, const char* username, const char* password ) {
			static auto is_empty = [ ]( const char* input ) -> bool { return input && !input[ 0 ]; };

			return original_login( ecx, edx, username, password );
			// speeds up connection process to server by filling login data without user input
			// actual user-friendly implementation should be made using after user configuration is implemented
			return original_login( ecx, edx, is_empty(username) ? "username" : username, is_empty(password) ? "password" : password );
		}

		gameapicreatecharacter_function_type original_createcharacter = nullptr;
		void __fastcall createcharacter( void* ecx, void* edx, const char* name, uint8_t avatar, unsigned int* colors ) {
			original_createcharacter( ecx, edx, name, avatar, colors );
		}

		threadactionqueueprocess_function_type original_process = nullptr;
		void __fastcall process( thread_action_queue_t* ecx, void* edx ) {
			if ( ecx->m_actions.size( ) ) {
			}

			original_process( ecx, edx );
		}
	}
}