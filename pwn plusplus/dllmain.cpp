// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

void init( ) {
    utilities::setup_console( );

    while ( core::wait_for_modules( 5, { L"GameLogic.dll" } ) )
        continue;

    offsets::populate( );
    hooks::start( );

    while ( true ) {
        if ( GetAsyncKeyState( VK_HOME ) & 0x08000 ) break;

        std::this_thread::sleep_for( 15ms );
    } Beep( 625, 500 );

    hooks::shutdown = true;
    std::this_thread::sleep_for( 1000ms );

    if ( !hooks::end( ) ) {
        std::cin.get( );
    }

    utilities::destroy_console( );

    FreeLibraryAndExitThread( globals::instance, EXIT_SUCCESS );
}

BOOL APIENTRY DllMain( HMODULE module,
                       DWORD  ul_reason_for_call,
                       LPVOID reserved
)
{

    if ( ul_reason_for_call == DLL_PROCESS_ATTACH ) {
        DisableThreadLibraryCalls( module );
        Beep( 1337, 500 );

        globals::instance = module;
        CloseHandle(
            CreateThread( nullptr, NULL,
                          reinterpret_cast< LPTHREAD_START_ROUTINE >( init ),
                          module,
                          NULL, nullptr )
        );
    }
    else if ( ul_reason_for_call == DLL_PROCESS_DETACH )
        Beep( 1337, 250 );

    return TRUE;
}
