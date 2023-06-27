#pragma once

typedef int( __stdcall* send_function )( SOCKET s, const char* buf, int len, int flags );
typedef void( __fastcall* enqueue_function )( void*, const std::function<void __cdecl( void )>* );

enum damage_type_e {
	physical = 0,
	fire = 1,
	cold = 2,
	damage = 3
};

struct write_stream_t {
public:
	UINT_PTR m_sock;
	std::vector<unsigned char> m_buffer;
};

struct thread_action_queue_t {
public:
	std::mutex* mutex;
	std::vector<std::function<void __cdecl( void )>> m_actions;
};

template<class T>
class actor_ref {
public:
	T* object;
};