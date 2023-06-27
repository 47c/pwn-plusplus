#pragma once

class c_drop : public c_actor {
public:
	bool& get_used( ) {
		return *reinterpret_cast< bool* >( reinterpret_cast< uintptr_t >( this ) + 0x78 );
	}

	std::map<i_item*, unsigned int>& get_items( ) {
		return *reinterpret_cast< std::map<i_item*, unsigned int>* >( reinterpret_cast< uintptr_t >( this ) + 0x70 );
	}

public:
	VFUNCTION( void, perform_use, 40, void* );
};