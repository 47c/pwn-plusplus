#pragma once

class c_client_handler {
public:
	UINT_PTR m_sock;
	write_stream_t write_stream;
	int m_character_id;
	actor_ref<i_player> reference;
};

class c_server_connection {
public:
	void* __vftable;
	bool m_running; // 0x04
	bool m_valid; // 0x05
	bool m_readyToDisconnect; // 0x06
	char pad_0x06[ 0x1 ];
	std::thread m_thread;
	std::mutex* m_mutex;
	std::queue<std::function<void __cdecl( void )>> m_serverQueue;
	std::condition_variable m_serverQueueEvent;
	thread_action_queue_t m_gameThreadActions;
};

class c_game_server_connection {
public:
	char pad_0x00[ 0x3C ];
	UINT_PTR m_sock;
	write_stream_t m_writeStream;
	bool m_tickInProgress;
};