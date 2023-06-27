#pragma once

#define DEPTH_BIAS_D32_FLOAT(d) (d/(1/pow(2,23)))

namespace user_interface {
	inline bool visible = false;

	inline float dt_array[ 90 ] = {};

	inline ID3D11Device* device = nullptr;
	inline IDXGISwapChain* swapchain = nullptr;
	inline ID3D11DeviceContext* context = nullptr;
	inline ID3D11RenderTargetView* main_render_target_view = nullptr;

	inline ID3D11Buffer* veBuffer;
	inline UINT Stride;
	inline UINT veBufferOffset;
	inline D3D11_BUFFER_DESC vedesc;

	inline ID3D11Buffer* inBuffer;
	inline DXGI_FORMAT inFormat;
	inline UINT        inOffset;
	inline D3D11_BUFFER_DESC indesc;

	inline UINT pscStartSlot;
	inline ID3D11Buffer* pscBuffer;
	inline D3D11_BUFFER_DESC pscdesc;

	inline UINT vscStartSlot;
	inline ID3D11Buffer* vscBuffer;
	inline D3D11_BUFFER_DESC vscdesc;

	inline UINT pssrStartSlot;

	inline HWND window_handle = nullptr;

	namespace window {
		inline HWND handle = nullptr;
		inline WNDCLASSEX window_class { };
		inline WNDPROC process = nullptr;

		extern bool setup( ) noexcept;
		extern void destroy( ) noexcept;
	}

	namespace directx {
		namespace viewport {
			inline float width, height;
		}

		inline const D3D_FEATURE_LEVEL feature_levels[ ] = { D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0 };
		inline uint32_t* virtual_table;

		extern DXGI_SAMPLE_DESC populate_sameple_desc( );
		extern DXGI_MODE_DESC populate_mode_desc( );
		extern DXGI_SWAP_CHAIN_DESC populate_swapchain_desc( );
		extern DXGI_RATIONAL populate_rational( );

		extern bool setup( ) noexcept;
		extern void destroy( ) noexcept;

		extern long generate_shader( ID3D11Device* device, ID3D11PixelShader** shader, float r, float g, float b, float a );
		extern void generate_texture( uint32_t pixelcolor, DXGI_FORMAT format );
	}

	namespace rendering {
		struct render_queue_t {
			vector3 origin;
			const char* content;

			render_queue_t( ) {
				origin = vector3();
				content = nullptr;
			}

			render_queue_t( vector3 a, std::string b ) {
				origin = a;
				content = b.c_str( );
			}
		};

		inline int framerate;
		inline std::vector<render_queue_t> queue;

		inline void push_to_graph( float& dt ) {
			static int counter = 90;
			if ( counter >= 0 ) {
				user_interface::dt_array[ counter ] = dt;
				counter--;
			} else counter = 90;
		}

		extern vector3 world_to_screen( vector3 v );

		extern void update_framerate( );

		extern void main_menu( );
		extern void overlay( );
		extern void screen( );
		extern void tick_interval( );

		extern void apply_style( );
	};

	extern bool device_setup( );
};