#pragma once

class registers {
public:
	void* _ecx;
	void* _edx;

	registers( ) {
		
	}
};

namespace core {
	class protect_guard {
	public:
		protect_guard( void* base, size_t len, std::uint32_t flags ) {
			_base = base;
			_length = len;
			VirtualProtect( base, len, flags, reinterpret_cast< PDWORD >( &_old ) );
		}
		~protect_guard( ) {
			VirtualProtect( _base, _length, _old, reinterpret_cast< PDWORD >( &_old ) );
		}

	private:
		void* _base;
		size_t        _length;
		std::uint32_t _old;
	};

	class vmt {
	public:

		vmt( ) : m_class_base( nullptr ), vftbl_len( 0 ), new_vftbl( nullptr ), old_vftbl( nullptr ) {
		}

		vmt( void* base ) : m_class_base( base ), vftbl_len( 0 ), new_vftbl( nullptr ), old_vftbl( nullptr ) {
		}
		~vmt( ) {
			unhook_all( );

			//delete new_vftbl;
		}

		uintptr_t* search_free_data_page( void* module_address, const std::size_t vmt_size, std::uint8_t null_byte = 0x0 ) //Modified code from exphck https://www.unknowncheats.me/forum/2128832-post43.html
		{
			auto check_data_section = [ & ]( LPCVOID address, const std::size_t vmt_size ) {
				const DWORD DataProtection = ( PAGE_READWRITE | PAGE_EXECUTE_READWRITE );
				MEMORY_BASIC_INFORMATION mbi = { 0 };

				if ( VirtualQuery( address, &mbi, sizeof( mbi ) ) == sizeof( mbi ) && mbi.AllocationBase && mbi.BaseAddress &&
					 mbi.State == MEM_COMMIT && !( mbi.Protect & PAGE_GUARD ) && mbi.Protect != PAGE_NOACCESS ) {
					if ( ( mbi.Protect & DataProtection ) && mbi.RegionSize >= vmt_size ) {
						return true;
					}
				}
				return false;
			};

			auto module_addr = module_address;

			if ( module_addr == nullptr )
				return nullptr;

			const auto dos_header = reinterpret_cast< PIMAGE_DOS_HEADER > ( module_addr );
			const auto nt_headers = reinterpret_cast< PIMAGE_NT_HEADERS > ( reinterpret_cast< std::uint8_t* >( module_addr ) + dos_header->e_lfanew );

			const auto module_end = reinterpret_cast< std::uintptr_t >( module_addr ) + nt_headers->OptionalHeader.SizeOfImage - sizeof( std::uintptr_t );

			for ( auto current_address = module_end; current_address > reinterpret_cast< std::uintptr_t >( module_addr ); current_address -= sizeof( unsigned char ) ) {
				if ( *reinterpret_cast< unsigned char* >( current_address ) == null_byte && check_data_section( reinterpret_cast< LPCVOID >( current_address ), vmt_size ) ) {
					bool is_good_vmt = true;
					auto page_count = 0u;

					for ( ; page_count < vmt_size && is_good_vmt; page_count += sizeof( unsigned char ) ) {
						if ( *reinterpret_cast< unsigned char* >( current_address + page_count ) != null_byte )
							is_good_vmt = false;
					}

					if ( is_good_vmt && page_count >= vmt_size )
						return ( uintptr_t* )current_address;
				}
			}
			return nullptr;
		}

		void hook_index( void* new_function, int index ) {
			new_vftbl[ index ] = reinterpret_cast< std::uintptr_t >( new_function );
		}

		void cleanup( bool unhook = true ) {
			if ( !m_class_base || !new_vftbl || !old_vftbl || !vftbl_len )
				return;

			if ( unhook )
				unhook_all( );

			//delete new_vftbl;
		}

		bool setup( void* base, const char* module_name = nullptr ) {
			if ( base != nullptr )
				m_class_base = base;

			if ( m_class_base == nullptr )
				return false;

			old_vftbl = *static_cast< std::uintptr_t** >( m_class_base );
			vftbl_len = estimate_vftbl_length( old_vftbl );

			if ( vftbl_len == 0 )
				return false;


			new_vftbl = new std::uintptr_t[ vftbl_len + 1 ]( );

			new_vftbl = new_vftbl + 1;
			std::memcpy( new_vftbl, old_vftbl, vftbl_len * sizeof( std::uintptr_t ) );
			new_vftbl[ -1 ] = old_vftbl[ -1 ];

			try {
				//auto guard = protect_guard{ m_class_base, sizeof(std::uintptr_t), PAGE_READWRITE };
				DWORD old_protection;
				auto orginal_func = *static_cast< std::uintptr_t* >( m_class_base );
				VirtualProtect( reinterpret_cast< void* >( orginal_func ), sizeof( std::uintptr_t ), PAGE_READWRITE, &old_protection );
				*static_cast< std::uintptr_t** >( m_class_base ) = new_vftbl;
				VirtualProtect( reinterpret_cast< void* >( orginal_func ), sizeof( std::uintptr_t ), old_protection, &old_protection );
			} catch ( ... ) {
				delete[ ] new_vftbl;
				return false;
			}

			return true;
		}

		void unhook_index( int index ) {
			new_vftbl[ index ] = old_vftbl[ index ];
		}
		void unhook_all( ) {

			try {
				if ( old_vftbl != nullptr ) {
					//auto guard = protect_guard{ m_class_base, sizeof(std::uintptr_t), PAGE_READWRITE };
					DWORD old_protection;
					auto orginal_func = *static_cast< std::uintptr_t* >( m_class_base );
					VirtualProtect( reinterpret_cast< void* >( orginal_func ), sizeof( std::uintptr_t ), PAGE_READWRITE, &old_protection );
					*static_cast< std::uintptr_t** >( m_class_base ) = old_vftbl;
					VirtualProtect( reinterpret_cast< void* >( orginal_func ), sizeof( std::uintptr_t ), old_protection, &old_protection );
					old_vftbl = nullptr;
				}
			} catch ( ... ) {
			}


		}

		template<typename T>
		T get_original( int index ) {


			return ( T )old_vftbl[ index ];
		}

		std::size_t     vftbl_len;
		std::uintptr_t* new_vftbl;
		std::uintptr_t* old_vftbl;

		void* m_class_base = nullptr;
		void* module_base_ = nullptr;

	private:
		static std::size_t estimate_vftbl_length( std::uintptr_t* vftbl_start ) {
			auto len = std::size_t {};

			while ( vftbl_start[ len ] >= 0x00010000 &&
					vftbl_start[ len ] < 0xFFF00000 &&
					len < 512 /* Hard coded value. Can cause problems, beware.*/ ) {
				len++;
			}

			return len;
		}


	};

	class vmt_hidden {
	public:

		vmt_hidden( ) : m_class_base( nullptr ), vftbl_len( 0 ), new_vftbl( nullptr ), old_vftbl( nullptr ) {
		}

		vmt_hidden( void* base ) : m_class_base( base ), vftbl_len( 0 ), new_vftbl( nullptr ), old_vftbl( nullptr ) {
		}
		~vmt_hidden( ) {
			unhook_all( );

			//delete new_vftbl;
		}

		uintptr_t* search_free_data_page( void* module_address, const std::size_t vmt_size, std::uint8_t null_byte = 0x0 ) //Modified code from exphck https://www.unknowncheats.me/forum/2128832-post43.html
		{
			auto check_data_section = [ & ]( LPCVOID address, const std::size_t vmt_size ) {
				const DWORD DataProtection = ( PAGE_READWRITE | PAGE_EXECUTE_READWRITE );
				MEMORY_BASIC_INFORMATION mbi = { 0 };

				if ( VirtualQuery( address, &mbi, sizeof( mbi ) ) == sizeof( mbi ) && mbi.AllocationBase && mbi.BaseAddress &&
					 mbi.State == MEM_COMMIT && !( mbi.Protect & PAGE_GUARD ) && mbi.Protect != PAGE_NOACCESS ) {
					if ( ( mbi.Protect & DataProtection ) && mbi.RegionSize >= vmt_size ) {
						return true;
					}
				}
				return false;
			};

			auto module_addr = module_address;

			if ( module_addr == nullptr )
				return nullptr;

			const auto dos_header = reinterpret_cast< PIMAGE_DOS_HEADER > ( module_addr );
			const auto nt_headers = reinterpret_cast< PIMAGE_NT_HEADERS > ( reinterpret_cast< std::uint8_t* >( module_addr ) + dos_header->e_lfanew );

			const auto module_end = reinterpret_cast< std::uintptr_t >( module_addr ) + nt_headers->OptionalHeader.SizeOfImage - sizeof( std::uintptr_t );

			for ( auto current_address = module_end; current_address > ( std::uintptr_t )module_addr; current_address -= sizeof( unsigned char ) ) {
				if ( *reinterpret_cast< unsigned char* >( current_address ) == null_byte && check_data_section( reinterpret_cast< LPCVOID >( current_address ), vmt_size ) ) {
					bool is_good_vmt = true;
					auto page_count = 0u;

					for ( ; page_count < vmt_size && is_good_vmt; page_count += sizeof( unsigned char ) ) {
						if ( *reinterpret_cast< unsigned char* >( current_address + page_count ) != null_byte )
							is_good_vmt = false;
					}

					if ( is_good_vmt && page_count >= vmt_size )
						return ( uintptr_t* )current_address;
				}
			}
			return nullptr;
		}

		void hook_index( void* new_function, int index ) {
			if ( this->m_class_base == nullptr || new_function == nullptr )
				return;

			//normal vmt hooking, just replacing the damn fucking function entries
			if ( !this->module_base_ ) {
				new_vftbl[ index ] = reinterpret_cast< std::uintptr_t >( new_function );
				return;
			}


			//the trampoline vmt hook

			//jmp hook_address
			unsigned char jump[ 5 ] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };

			//Can we allocate the shellcode in the function paddings?
			auto trampoline_shellcode_location = search_free_data_page( this->module_base_, sizeof( jump ), 0x0 );
			if ( !trampoline_shellcode_location ) {

				if ( !trampoline_shellcode_location )
					ExitProcess( 0x3 );
			}

			// Calculate the "distance" we're gonna have to jump - the size of the JMP instruction
			const auto relative_address = ( std::uintptr_t( new_function ) - std::uintptr_t( trampoline_shellcode_location ) - sizeof( jump ) );
			*reinterpret_cast< std::uintptr_t* >( jump + 1 ) = relative_address;

			DWORD old_trampoline_protection;
			//Make region RWX
			VirtualProtect( reinterpret_cast< void* >( trampoline_shellcode_location ), sizeof( jump ), PAGE_EXECUTE_READWRITE, &old_trampoline_protection );
			//Write jump to that memory region
			memcpy( trampoline_shellcode_location, jump, sizeof( jump ) );
			//Restore old protection
			//Don't restore it as we use these 5 bytes in a rw section to also execute, we literally create a 4kb page at the end of some region and make it rwx, fuck you vac
			//VirtualProtect(reinterpret_cast<void*>(trampoline_shellcode_location), 5, old_trampoline_protection, &old_trampoline_protection);

			/*DWORD old_protection;
			const auto original_func = *static_cast<std::uintptr_t*>(this->m_class_base) + 4 * index;
			const auto backup_func = reinterpret_cast<void*>(*reinterpret_cast<std::uintptr_t*>(original_func));
			//Save original function to map to call it later
			this->m_original_functions[index] = backup_func;

			//Overwrite the vtable function address
			//Make it RWX
			VirtualProtect(reinterpret_cast<void*>(original_func), 4, PAGE_EXECUTE_READWRITE, &old_protection);
			//Set the original func to point to our new trampoline
			*reinterpret_cast<std::uintptr_t*>(original_func) = reinterpret_cast<std::uintptr_t>(trampoline_shellcode_location);
			//Restore org
			VirtualProtect(reinterpret_cast<void*>(original_func), 4, old_protection, &old_protection);*/

			//write the address of the shellcode into the vtable entry
			new_vftbl[ index ] = reinterpret_cast< std::uintptr_t >( trampoline_shellcode_location );
		}

		void cleanup( bool unhook = true ) {
			if ( !m_class_base || !new_vftbl || !old_vftbl || !vftbl_len )
				return;

			if ( unhook )
				unhook_all( );

			//delete new_vftbl;
		}

		bool setup( void* base, const char* module_name = nullptr ) {
			if ( base != nullptr )
				m_class_base = base;

			if ( m_class_base == nullptr )
				return false;

			old_vftbl = *static_cast< std::uintptr_t** >( m_class_base );
			vftbl_len = estimate_vftbl_length( old_vftbl );

			if ( vftbl_len == 0 )
				return false;

			if ( module_name ) {
				this->module_base_ = GetModuleHandleA( module_name );

				//Search free data page and literally just use it uff yaa
				new_vftbl = search_free_data_page( this->module_base_, sizeof( std::uintptr_t ) * ( vftbl_len + 1 ) );//
				if ( !new_vftbl ) {
					new_vftbl = new std::uintptr_t[ vftbl_len + 1 ]( );
					this->module_base_ = 0;
					MessageBox( 0, "Error", module_name, 0 );
				}
			} else
				new_vftbl = new std::uintptr_t[ vftbl_len + 1 ]( );

			new_vftbl = new_vftbl + 1;
			std::memcpy( new_vftbl, old_vftbl, vftbl_len * sizeof( std::uintptr_t ) );
			new_vftbl[ -1 ] = old_vftbl[ -1 ];

			try {
				//auto guard = protect_guard{ m_class_base, sizeof(std::uintptr_t), PAGE_READWRITE };
				DWORD old_protection;
				auto orginal_func = *static_cast< std::uintptr_t* >( m_class_base );
				VirtualProtect( reinterpret_cast< void* >( orginal_func ), sizeof( std::uintptr_t ), PAGE_READWRITE, &old_protection );
				*static_cast< std::uintptr_t** >( m_class_base ) = new_vftbl;
				VirtualProtect( reinterpret_cast< void* >( orginal_func ), sizeof( std::uintptr_t ), old_protection, &old_protection );
			} catch ( ... ) {
				delete[ ] new_vftbl;
				return false;
			}

			return true;
		}

		void unhook_index( int index ) {
			new_vftbl[ index ] = old_vftbl[ index ];
		}
		void unhook_all( ) {

			try {
				if ( old_vftbl != nullptr ) {
					//auto guard = protect_guard{ m_class_base, sizeof(std::uintptr_t), PAGE_READWRITE };
					DWORD old_protection;
					auto orginal_func = *static_cast< std::uintptr_t* >( m_class_base );
					VirtualProtect( reinterpret_cast< void* >( orginal_func ), sizeof( std::uintptr_t ), PAGE_READWRITE, &old_protection );
					*static_cast< std::uintptr_t** >( m_class_base ) = old_vftbl;
					VirtualProtect( reinterpret_cast< void* >( orginal_func ), sizeof( std::uintptr_t ), old_protection, &old_protection );
					old_vftbl = nullptr;
				}
			} catch ( ... ) {
			}


		}

		template<typename T>
		T get_original( int index ) {


			return ( T )old_vftbl[ index ];
		}

		std::size_t     vftbl_len;
		std::uintptr_t* new_vftbl;
		std::uintptr_t* old_vftbl;

		void* m_class_base = nullptr;
		void* module_base_ = nullptr;

	private:
		static std::size_t estimate_vftbl_length( std::uintptr_t* vftbl_start ) {
			auto len = std::size_t {};

			while ( vftbl_start[ len ] >= 0x00010000 &&
					vftbl_start[ len ] < 0xFFF00000 &&
					len < 512 /* Hard coded value. Can cause problems, beware.*/ ) {
				len++;
			}

			return len;
		}


	};
}