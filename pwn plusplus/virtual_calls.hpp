#pragma once

#include <cstdint>

#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
#define PAD(size) char MACRO_CONCAT(_pad, __COUNTER__)[size];

#define VFUNCTION(x, y, z, ...) x y##(__VA_ARGS__)\
	{ \
	    _asm mov eax, dword ptr[ecx]\
	    _asm mov edx, z\
	    _asm imul edx, 4\
	    _asm mov eax, dword ptr[eax + edx]\
	    _asm pop ebp\
	    _asm pop ebp\
	    _asm jmp eax\
	}

inline uintptr_t address( void* ptr ) {
	return reinterpret_cast< uintptr_t >( ptr );
}

template<typename T>
T deref( uintptr_t address, uint16_t t = 1 ) {
	T ptr = static_cast<T>( address );

	for ( int i = 0; i < t; i++ )
		ptr = *reinterpret_cast< T* >( ptr );

	return ptr;
}

template<typename T>
T* deref_ptr( uintptr_t address, uint16_t t = 1 ) {
	T* ptr = reinterpret_cast< T* >( address );
	for ( int i = 0; i < t; i++ )
		ptr = *reinterpret_cast< T** >( ptr );

	return ptr;
}

template<typename T>
inline void write( uintptr_t address, T value ) {
	DWORD access_rights {};

	VirtualProtect( reinterpret_cast< void* >( address ), sizeof( T ), PAGE_READWRITE, &access_rights );
	*(reinterpret_cast< T* >( address )) = value;
	VirtualProtect( reinterpret_cast< void* >( address ), sizeof( T ), access_rights, &access_rights );
}

inline void nop_address(uintptr_t address, int bytes ) {
	DWORD access_rights {};

	VirtualProtect( reinterpret_cast< void* >( address ), bytes, PAGE_EXECUTE_READWRITE, &access_rights );
	memset( reinterpret_cast< void* >( address ), 0x90, bytes );
	VirtualProtect( reinterpret_cast< void* >( address ), bytes, access_rights, &access_rights );
}

//Kinda manual calls with needed manual identifiers, *if* you are that kinda person
template<typename FuncType>
inline FuncType call_virtual_function( void* ppClass, const uintptr_t index )
{
	auto vtable = *static_cast< std::uintptr_t** >( ppClass );
	return FuncType( vtable[ index ] );
}

template<typename Fn>
inline Fn vfunc( const void* virtual_func, int index ) {
	return ( Fn ) * ( *( const void*** )virtual_func + index );
}

template <typename ReturnType, std::uint64_t Index, typename... Args>
__forceinline ReturnType call_virtual( void* instance, Args... args )
{
	using Fn = ReturnType( __thiscall* )( void*, Args... );

	auto function = ( *reinterpret_cast< Fn** >( instance ) )[ Index ];
	return function( instance, args... );
}

