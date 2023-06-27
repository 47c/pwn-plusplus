#include "pch.h"
#include <color-console/color.hpp>

namespace hooks
{
	bool start( ) {
		auto& hook_manager = hooks::hook_manager::get( ); in_setup = true;

		utilities::log( "hook manager status: {}", hook_manager.interpret( hook_manager.intialize( ) ) );

		utilities::log( "user interface device setup {}", user_interface::device_setup( ) ?
						fmt::styled( "success", fmt::fg( fmt::color::spring_green ) ) :
						fmt::styled( "failed ", fmt::fg( fmt::color::crimson ) ) );

		MH_CreateHookApi( L"ws2_32", "send", &hooks::winsock::send, reinterpret_cast< void** >( &hooks::winsock::original_send ) );
		MH_QueueEnableHook( GetProcAddress( GetModuleHandle( "ws2_32.dll" ), "send" ) );
		{
			globals::engine_ptr = reinterpret_cast< c_engine* >( offsets::map[ "GEngine" ] );
		}
		{
			hook_manager.add_function( "Direct3D11::Present" )
				->set_address( user_interface::directx::virtual_table[ 8 ] )
				->set_original( reinterpret_cast< void** >( &directx::original_present ) )
				->set_detour( &directx::present );

			hook_manager.add_function( "Direct3D11::ResizeBuffers" )
				->set_address( user_interface::directx::virtual_table[ 13 ] )
				->set_original( reinterpret_cast< void** >( &directx::original_resize_buffers ) )
				->set_detour( &directx::resize_buffers );

			hook_manager.add_function( "Direct3D11::PSSetShaderResources" )
				->set_address( user_interface::directx::virtual_table[ 69 ] )
				->set_original( reinterpret_cast< void** >( &directx::original_set_shader_resources ) )
				->set_detour( &directx::set_shader_resources );

			hook_manager.add_function( "Direct3D11::DrawIndexedPrimitve" )
				->set_address( user_interface::directx::virtual_table[ 73 ] )
				->set_original( reinterpret_cast< void** >( &directx::original_draw_indexed_primitive ) )
				->set_detour( &directx::draw_indexed_primitive );
		} {
			hook_manager.add_function( "GameAPI::Tick" )
				->set_original( reinterpret_cast< void** >( &game_api::original_tick ) )
				->set_detour( &game_api::tick );
			hook_manager.add_function( "GameAPI::Login" )
				->set_original( reinterpret_cast< void** >( &game_api::original_login ) )
				->set_detour( &game_api::login );
			hook_manager.add_function( "GameAPI::CreateCharacter" )
				->set_original( reinterpret_cast< void** >( &game_api::original_createcharacter ) )
				->set_detour( &game_api::createcharacter );
			hook_manager.add_function( "ThreadActionQueue::Process" )
				->set_original( reinterpret_cast< void** >( &game_api::original_process ) )
				->set_detour( &game_api::process );
		} {
			hook_manager.add_function( "GameServerConnection::Connect" )
				->set_original( reinterpret_cast< void** >( &game_server_connection::original_connect ) )
				->set_detour( &game_server_connection::connect );

			hook_manager.add_function( "GameServerConnection::~GameServerConnection" )
				->set_original( reinterpret_cast< void** >( &game_server_connection::original_destructor ) )
				->set_detour( &game_server_connection::destructor );

			hook_manager.add_function( "MasterServerConection::CharacterInfo::CharacterInfo" )
				->set_original( reinterpret_cast< void** >( &game_server_connection::original_characterinfo ) )
				->set_detour( &game_server_connection::characterinfo );

			hook_manager.add_function( "ClientHandler::ProcessCommands" )
				->set_original( reinterpret_cast< void** >( &game_server_connection::original_process_commands ) )
				->set_detour( &game_server_connection::process_commands );
		} {
			hook_manager.add_function( "ClientWorld::Tick" )
				->set_original( reinterpret_cast< void** >( &client_world::original_tick ) )
				->set_detour( &client_world::tick );
			hook_manager.add_function( "ClientWorld::Jump" )
				->set_original( reinterpret_cast< void** >( &client_world::original_jump ) )
				->set_detour( &client_world::jump );

			hook_manager.add_function( "World::SpawnActorWithId" )
				->set_original( reinterpret_cast< void** >( &client_world::original_spawnactorwithid ) )
				->set_detour( &client_world::spawnactorwithid );
		} {
			hook_manager.add_function( "Player::Tick" )
				->set_original( reinterpret_cast< void** >( &player::original_tick ) )
				->set_detour( &player::tick );
			hook_manager.add_function( "Player::CanJump" )
				->set_original( reinterpret_cast< void** >( &player::original_canjump ) )
				->set_detour( &player::can_jump );
			hook_manager.add_function( "Player::Damage" )
				->set_original( reinterpret_cast< void** >( &player::original_damage ) )
				->set_detour( &player::damage );
			hook_manager.add_function( "Player::GetItemForSlot" )
				->set_original( reinterpret_cast< void** >( &player::original_getitemforslot ) )
				->set_detour( &player::get_item_for_slot );

			hook_manager.add_function( "PlayerController::FOV" )
				->set_original( reinterpret_cast< void** >( &player::original_fov ) )
				->set_detour( &player::fov );
		} {
			hook_manager.add_function( "Actor::Damage" )
				->set_original( reinterpret_cast< void** >( &actor::original_damage ) )
				->set_detour( &actor::damage );

			hook_manager.add_function( "Actor::FireBullets" )
				->set_original( reinterpret_cast< void** >( &actor::original_firebullets ) )
				->set_detour( &actor::firebullets );

			hook_manager.add_function( "Actor::OnSpawnActor" )
				->set_original( reinterpret_cast< void** >( &actor::original_onspawnactor ) )
				->set_detour( &actor::onspawnactor );

			hook_manager.add_function( "ItemPickup::CanUse" )
				->set_original( reinterpret_cast< void** >( &actor::original_canuse ) )
				->set_detour( &actor::canuse );

			hook_manager.add_function( "Gun::GetReloadTime" )
				->set_original( reinterpret_cast< void** >( &actor::original_getreloadtime ) )
				->set_detour( &actor::getreloadtime );

			hook_manager.add_function( "AIActor::Tick" )
				->set_original( reinterpret_cast< void** >( &actor::original_aiactor_tick ) )
				->set_detour( &actor::aitick );

		} {
			hook_manager.add_function( "Drop::Drop" )
				->set_original( reinterpret_cast< void** >( &drop::original_constructor ) )
				->set_detour( &drop::constructor );
		}

		MH_DisableHook( MH_ALL_HOOKS );

		auto create_hooks_status = hook_manager.create_hooks( );
		auto enable_hooks_status = hook_manager.enable_hooks( );

		utilities::log( "create hooks {}", hook_manager.interpret( create_hooks_status ) );
		utilities::log( "enable hooks {}", hook_manager.interpret( enable_hooks_status ) );

		auto queue_apply_status = MH_ApplyQueued( );
		utilities::log( "apply hook queue: {}", queue_apply_status == MH_OK ?
						fmt::styled( MH_StatusToString( queue_apply_status ), fg( fmt::color::spring_green ) ) :
						fmt::styled( MH_StatusToString( queue_apply_status ), fg( fmt::color::crimson ) ) );

		return in_setup = false;
	}

	bool end( ) {
		auto& hook_manager = hooks::hook_manager::get( );

		user_interface::directx::destroy( );
		SetWindowLongPtr( user_interface::window::handle,
						  GWLP_WNDPROC,
						  reinterpret_cast< LONG_PTR >( hooks::winapi::original_window_proc ) );

		auto status_disable_hooks = hook_manager.disable_hooks( );
		utilities::log( "hooks {}", hook_manager.interpret( status_disable_hooks ) );

		free( user_interface::directx::virtual_table ); user_interface::directx::virtual_table = nullptr;

		MH_DisableHook( MH_ALL_HOOKS );
		MH_RemoveHook( MH_ALL_HOOKS );

		auto status_unintialize = hook_manager.unintialize( );
		utilities::log("hook manager {}", hook_manager.interpret( status_unintialize ) );

		return ( static_cast< int >( status_disable_hooks ) == static_cast< int >( status_unintialize ) )
			&& static_cast< int >( status_unintialize ) == 0x2;
	}
}