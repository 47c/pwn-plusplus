#include "pch.h"

namespace offsets {
	void populate( ) {
		map.emplace( "GEngine",
					 deref<uintptr_t>( core::find_signature( BASEMOD, "8B 0D ? ? ? ? E8 ? ? ? ? 8B 0D ? ? ? ? F2 0F 10 05 ? ? ? ? 66 0F 5A C0 51 8B 01 F3 0F 11 04 24 FF 90 ? ? ? ? E8 ? ? ? ? 8B 0D ? ? ? ? 8B 01 FF 50 18" ) + 0x2, 2 ) );
		map.emplace( "CameraCache_FOV",
					 core::find_signature( BASEMOD, "89 86 ? ? ? ? 8B 41 1C" ) );

		// GameAPI
		{
			map.emplace( "GameAPI::Get",
						 core::find_signature( GAMELOGIC, "C3 CC CC CC CC CC CC CC CC CC CC 55 8B EC FF 15 ?? ?? ?? ??" ) - 0x4 );
			map.emplace( "GameAPI::Tick",
						 core::find_signature( GAMELOGIC, "83 EC 28 A1 ? ? ? ? 33 C4 89 44 24 20 53 56 57 A1 ? ? ? ? 33 C4 50 8D 44 24 38 64 A3 ? ? ? ? 89 4C 24 10 " ) - 0x14 );
			map.emplace( "GameAPI::Login",
						 core::find_signature( GAMELOGIC, "55 8B EC 83 E4 F8 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 ? ? ? ? 53 56 57 A1 ? ? ? ? 33 C4 50 8D 84 24 ? ? ? ? 64 A3 ? ? ? ? 8B F9 83 3D ? ? ? ? ? 8B 75 08 " ) );
			map.emplace( "GameAPI::GetGoldenEggList",
						 core::find_signature( GAMELOGIC, "55 8B EC 51 51 8B 4D 08 C7 45 ? ? ? ? ? E8 ? ? ? ? 8B 45 08 8B E5 5D C2 04 00" ) );
			map.emplace( "GameAPI::CreateCharacter",
						 core::find_signature( GAMELOGIC, "55 8B EC 83 E4 F8 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 40 A1 ? ? ? ? 33 C4 89 44 24 38 53 56 57 A1 ? ? ? ? 33 C4 50 8D 44 24 50 64 A3 ? ? ? ? 8B F1" ) );
		}

		{
			map.emplace( "ThreadActionQueue::Process",
						 core::find_signature( GAMELOGIC, "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 28 A1 ? ? ? ? 33 C5 89 45 F0 56 57 50 8D 45 F4 64 A3 ? ? ? ? 8D 45 CC 50" ) );
			map.emplace( "ThreadActionQueue::GetActionsForThisTick",
						 core::find_signature( GAMELOGIC, "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 0C 56 57 A1 ? ? ? ? 33 C5 50 8D 45 F4 64 A3 ? ? ? ? 8B F9 57 C7 45 ? ? ? ? ? 89 7D E8 FF 15 ? ? ? ? 83 C4 04 85 C0 74 0A 50 FF 15 ? ? ? ? 83 C4 04" ) );
		}

		{
			map.emplace( "ClientHandler::ProcessCommands",
						 core::relative_call_address( GAMELOGIC, "E8 ? ? ? ? 85 F6 74 08" ) );
		}

		{
			map.emplace( "GameServerConnection::~GameServerConnection",
						 core::find_signature( GAMELOGIC, "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 08 53 56 57 A1 ? ? ? ? 33 C5 50 8D 45 F4 64 A3 ? ? ? ? 89 65 F0 8B F1 89 75 EC C7 06 ? ? ? ? C7 45 ? ? ? ? ? 80 7E 05 00 74 22 C6 45 FC 02" ) );
			map.emplace( "GameServerConnection::Connect",
						 core::find_signature( GAMELOGIC, "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 81 EC ? ? ? ? A1 ? ? ? ? 33 C5 89 45 EC 53 56 57 50 8D 45 F4 64 A3 ? ? ? ? A1 ? ? ? ?" ) );
			map.emplace( "MasterServerConection::CharacterInfo::CharacterInfo",
						 (core::find_signature( GAMELOGIC, "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 51 56 57 A1 ? ? ? ? 33 C5 50 8D 45 F4 64 A3 ? ? ? ? 8B F9 89 7D F0 8B 75 08 8D 4F 04 6A FF 6A 00 8B 06 89 07 8D 46 04 C7 41 ? ? ? ? ? C7 41 ? ? ? ? ? 50 C6 01 00 E8 ? ? ? ? 6A FF 8D 4F 1C C7 45 ? ? ? ? ? 6A 00 8D 46 1C C7 41 ? ? ? ? ? C7 41 ? ? ? ? ? 50 C6 01 00 E8 ? ? ? ? 6A FF 8D 4F 34 C6 45 FC 01 6A 00 8D 46 34 C7 41 ? ? ? ? ? C7 41 ? ? ? ? ? 50 C6 01 00 E8 ? ? ? ? C6 45 FC 02 8D 8F ? ? ? ? 0F B6 46 4C 88 47 4C F3 0F 6F 46 ? 51 F3 0F 7F 47 ? 8B 46 60 89 47 60 0F B6 46 64 88 47 64 F3 0F 7E 46 ? 66 0F D6 47 ? 8B 46 70 89 47 70 8B 46 74 89 47 74 8B 46 78 89 47 78 0F B6 46 7C 88 47 7C 0F B6 46 7D 88 47 7D 8D 86 ? ? ? ? 50 E8 ? ? ? ? 6A FF 8D 8F ? ? ? ? C6 45 FC 03 6A 00 8D 86 ? ? ? ? C7 41 ? ? ? ? ? C7 41 ? ? ? ? ? 50 C6 01 00 E8 ? ? ? ? 8D 8F ? ? ? ? C6 45 FC 04 51 8D 86 ? ? ? ? 50 E8 ? ? ? ? C6 45 FC 05 8D 86 ? ? ? ? 50 8D 8F ? ? ? ? E8 ? ? ? ?" ) ) );
			map.emplace( "GameServerConnection::Get",
						deref<uintptr_t>( core::find_signature( GAMELOGIC, "8B 0D ? ? ? ? 8B 5D 08 8B 7D 14 85 C9 74 0F E8 ? ? ? ? C7 05 ? ? ? ? ? ? ? ?" ) + 0x2 ) );
			map.emplace( "ServerConnection::ServerEnqueue",
						 core::relative_call_address( GAMELOGIC, "E8 ? ? ? ? 8D 45 A0 C6 45 FC 01" ) );
		}

		// ClientWorld
		{
			map.emplace( "ClientWorld::Tick", 
						 core::find_signature( GAMELOGIC, "55 8B EC 51 57 8B F9 8B 4F 2C 85 C9 0F 84 ? ? ? ?" ) );
			map.emplace( "ClientWorld::Jump",
						 core::find_signature( GAMELOGIC, "55 8B EC 83 E4 F8 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 2C A1 ? ? ? ? 33 C4 89 44 24 24 A1 ? ? ? ? 33 C4 50 8D 44 24 30 64 A3 ? ? ? ? A1 ? ? ? ? 85 C0 74 67 8A 40 05 84 C0 74 60 8B 0D ? ? ? ? E8 ? ? ? ? 8B 4D 08 88 4C 24 08 8B 4C 24 08 89 4C 24 14 8D 4C 24 0C C7 44 24 ? ? ? ? ? 89 44 24 10 89 4C 24 1C 51 8B C8 C7 44 24 ? ? ? ? ? E8 ? ? ? ? C7 44 24 ? ? ? ? ? 8B 4C 24 1C 85 C9 74 12 8B 11 8D 44 24 0C 3B C8 0F 95 C0 0F B6 C0 50 FF 52 10 8B 4C 24 30 64 89 0D ? ? ? ? 59 8B 4C 24 24 33 CC E8 ? ? ? ? 8B E5 5D C2 04 00 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 55 8B EC 83 E4 F8 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 2C A1 ? ? ? ? 33 C4 89 44 24 24 A1 ? ? ? ? 33 C4 50 8D 44 24 30 64 A3 ? ? ? ? A1 ? ? ? ? 85 C0 74 67 8A 40 05 84 C0 74 60 8B 0D ? ? ? ? E8 ? ? ? ? 8B 4D 08 88 4C 24 08 8B 4C 24 08 89 4C 24 14 8D 4C 24 0C C7 44 24 ? ? ? ? ? 89 44 24 10 89 4C 24 1C 51 8B C8 C7 44 24 ? ? ? ? ? E8 ? ? ? ? C7 44 24 ? ? ? ? ? 8B 4C 24 1C 85 C9 74 12 8B 11 8D 44 24 0C 3B C8 0F 95 C0 0F B6 C0 50 FF 52 10 8B 4C 24 30 64 89 0D ? ? ? ? 59 8B 4C 24 24 33 CC E8 ? ? ? ? 8B E5 5D C2 04 00 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 55 8B EC 83 E4 F8 6A FF " ) );
			map.emplace( "World::SpawnActorWithId",
						 core::find_signature( GAMELOGIC, "55 8B EC 83 EC 08 8B 0D ? ? ? ? 56 8B 75 0C 57 8B 01 8B 3D ? ? ? ? 6A 01 FF 75 14 FF 75 10 56 FF 10 8B 45 08 89 45 10" ) );
			map.emplace( "GameWorld::Get",
						 deref<uintptr_t>( core::find_signature( GAMELOGIC, "8B 0D ? ? ? ? 85 C9 74 11 8B 01 8B 40 0C FF D0 84 C0 74 11 8B 0D ? ? ? ? 8B 01 53 56 57 FF 90 ? ? ? ? 56 8D 4F 34 E8 ? ? ? ? 88 18 8B 4F 0C" ) + 0x2 ) );
		}

		// Player
		{
			map.emplace( "Player::Tick",
						 core::find_signature( GAMELOGIC, "55 8B EC 83 E4 C0 6A FF 68" ) );
			map.emplace( "Player::CanJump",
						 core::find_signature( GAMELOGIC, "8B 49 9C 85 C9 74 07 8B 01 8B 40 50 FF E0" ) );
			map.emplace( "Player::Damage",
						 core::find_signature( GAMELOGIC, "55 8B EC 57 8B F9 8B 0D ? ? ? ? 85 C9 74 0F" ) );

			map.emplace( "Player::MarkAsPickedUp",
						 core::find_signature( GAMELOGIC, "55 8B EC 83 E4 F8 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 40 A1 ? ? ? ? 33 C4 89 44 24 38 53 56 57 A1 ? ? ? ? 33 C4 50 8D 44 24 50 64 A3 ? ? ? ? 8B F9 8B 0D ? ? ? ? 8B 5D 08 85 C9 74 0F 8B 01 8B 40 0C FF D0 84 C0 0F 84 ? ? ? ?" ) );
			map.emplace( "Player::PerformSetPvPEnabled",
						 core::find_signature( GAMELOGIC, " 55 8B EC 83 E4 F8 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 24 A1 ? ? ? ? 33 C4 89 44 24 1C 53 56 A1 ? ? ? ? 33 C4 50 8D 44 24 30 64 A3 ? ? ? ? 8B F1 8B 5D 08 8B 4E 0C 88 9E ? ? ? ? 88 9E ? ? ? ? 85 C9 74 06 8B 01 53 FF 50 40" ) );
			map.emplace( "Player::GetItemForSlot",
						 core::find_signature( GAMELOGIC, "55 8B EC 8B 45 08 83 F8 0A 72 06 33 C0 5D C2 04 00" ) );
		}

		// Actor
		{
			map.emplace( "Actor::Actor",
						 core::find_signature( GAMELOGIC, "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 51 56 A1 ? ? ? ? 33 C5 50 8D 45 F4 64 A3 ? ? ? ? 8B F1 89 75 F0 C7 45 ? ? ? ? ? 8D 4E 14 6A FF C7 06 ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? 6A 00 FF 75 08 C7 41 ? ? ? ? ? C7 41 ? ? ? ? ? C6 01 00 E8 ? ? ? ?" ) );

			map.emplace( "Actor::Damage",
						 core::find_signature( GAMELOGIC, "55 8B EC 83 EC 24 57 8B F9 8B 0D ? ? ? ? 85 C9 74 0F " ) );

			map.emplace( "Actor::FireBullets",
						 core::find_signature( GAMELOGIC, "55 8B EC 83 E4 F8 83 EC 34 56" ) );

			map.emplace( "Actor::OnSpawnActor",
						 core::find_signature( GAMELOGIC, "55 8B EC 8B 45 08 89 41 0C 5D C2 04 00" ) );

			map.emplace( "Actor::Tick",
						 core::find_signature( GAMELOGIC, "55 8B EC 8B 41 10 85 C0 74 0D F3 0F 10 55 ? 51 8B C8 E8 ? ? ? ?" ) );

			map.emplace( "Gun::GetReloadTime",
						 core::find_signature( GAMELOGIC, "D9 05 ? ? ? ? C2 04 00 CC CC CC CC CC CC CC B8 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC B8 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC B8 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC B8 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC B8 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC B8 ? ? ? ?" ) );

			map.emplace( "Pistol::GetCooldownTime",
						 core::find_signature( GAMELOGIC, "D9 05 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC B8 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC D9 05 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC 8B 0D ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC B8 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC B8 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC B8 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC B8 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC B8 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC B8 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC B8 ? ? ? ?" ) );
		
			map.emplace( "AIActor::Tick",
						 core::find_signature( GAMELOGIC, "55 8B EC 56 8B F1 8B 4E 10 85 C9 74 0B F3 0F 10 55 ? 56 E8 ? ? ? ? 8B 0D ? ? ? ? 85 C9 74 0B 8B 01 8B 40 0C FF D0 84 C0 74 6C 8B 8E ? ? ? ? 85 C9 74 36 8B 01 8B 80 ? ? ? ? FF D0 84 C0 74 0F 8B 8E ? ? ? ? 8B 01 FF 50 30 85 C0 7F 19" ) );
		
			map.emplace( "PlayerController::FOV",
						 core::find_signature( BASEMOD, "51 56 8B 74 24 0C 57 8B F9 C7 44 24 08 ?? ?? ?? ?? 83 7E 10 00 74 11 8D 44 24 08 8B CE 50 FF 76 0C E8 7A B5 38 FF" ) );
			
			map.emplace( "PlayerController::ProjectWorldLocationToScreen",
						 core::find_signature( BASEMOD, "83 EC 0C 56 8B 74 24 14 57 8B F9 83 7E 10 00 74 11 8D 44 24 08 8B CE 50 FF 76 0C E8 F0 02 37 FF EB" ) );
		}

		// ItemPickup
		{
			map.emplace( "ItemPickup::CanUse",
						 core::find_signature( GAMELOGIC, "55 8B EC 53 56 8B 75 08 8B D9 8B CE 8B 06 FF 10 8B C8 8B 10 FF 52 30 85 C0 7F 08" ) );

			map.emplace( "ItemPickup::_insert",
						 core::find_signature( GAMELOGIC, "55 8B EC 53 56 8B 75 08 8B D9 8B CE 8B 06 FF 10 8B C8 8B 10 FF 52 30 85 C0 7F 08" ) );
		}

		// Drop
		{
			map.emplace( "Drop::Drop",
						 core::find_signature( GAMELOGIC, "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 40 A1 ? ? ? ? 33 C5 89 45 F0 56 57 50 8D 45 F4 64 A3 ? ? ? ? 8B F9 8B 45 0C 8B 75 08 50 89 7D B8 E8 ? ? ? ? 8D 4F 70" ) );
		
			map.emplace( "Drop::PerformUse",
						 core::find_signature( GAMELOGIC, "55 8B EC 53 8B 5D 08 57 8B F9 53 8B 07 8B 40 40 FF" ) );
		}

		{
			map.emplace( "APlayerController",
						 deref< uintptr_t >( core::find_signature( BASEMOD, "89 44 24 3C 56 E8 ? ? ? ? 68 ? ? ? ? 6A 00 68 ? ? ? ? 68 ? ? ? ? 8B CE E8 ? ? ? ? E8 ? ? ? ? 8B 4C 24 30 " ) ) );
		}
	}
}