#include "pch.h"

HANDLE _out = 0, _old_out = 0;
HANDLE _err = 0, _old_err = 0;
HANDLE _in = 0, _old_in = 0;
FILE* fptrin = stdin, * fptrout = stdout, * fptrerr = stderr;

namespace utilities
{
	void setup_console( ) {
		_old_out = GetStdHandle( STD_OUTPUT_HANDLE );
		_old_err = GetStdHandle( STD_ERROR_HANDLE );
		_old_in = GetStdHandle( STD_INPUT_HANDLE );

		::AllocConsole( ) && ::AttachConsole( GetCurrentProcessId( ) );

		_out = GetStdHandle( STD_OUTPUT_HANDLE );
		_err = GetStdHandle( STD_ERROR_HANDLE );
		_in = GetStdHandle( STD_INPUT_HANDLE );

		_wfreopen_s( &fptrin, L"CONIN$", L"r", stdin );
		_wfreopen_s( &fptrout, L"CONOUT$", L"w", stdout );
		_wfreopen_s( &fptrerr, L"CONOUT$", L"w", stderr );

		/*SetConsoleMode( _out,
						ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS |
						ENABLE_PROCESSED_OUTPUT | ENABLE_QUICK_EDIT_MODE | ENABLE_WRAP_AT_EOL_OUTPUT |
						ENABLE_VIRTUAL_TERMINAL_PROCESSING );*/

		DWORD flags = 0;
		GetConsoleMode( _out, &flags );
		flags |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode( _out, flags );
	}

	void destroy_console( ) {
		if ( _out && _err && _in ) {

			if ( _old_out )
				SetStdHandle( STD_OUTPUT_HANDLE, _old_out );
			if ( _old_err )
				SetStdHandle( STD_ERROR_HANDLE, _old_err );
			if ( _old_in )
				SetStdHandle( STD_INPUT_HANDLE, _old_in );

			fclose( stdin ); fclose( stdout ); fclose( stderr );

			FreeConsole( );
		}
	}

	/*bool log( const char* fmt, ... )
	{
		std::unique_lock<decltype( mutex )> lock( mutex );

		if ( !_out )
			return false;

		char buf[ 1024 ];
		va_list va;

		va_start( va, fmt );
		_vsnprintf_s( buf, 1024, fmt, va );
		va_end( va );

		fmt::print( buf );

		return true;
		//return !!WriteConsoleA( _out, buf, static_cast< uintptr_t >( strlen( buf ) ), nullptr, nullptr ) 
		//	&& WriteConsoleA( _out, "\n", static_cast< uintptr_t >( strlen( "\n" ) ), nullptr, nullptr );
	}*/
}