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
}