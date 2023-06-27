#include "pch.h"

namespace user_interface
{
	namespace window
	{
		bool setup( ) noexcept{
			window_class.cbSize = sizeof( WNDCLASSEX );
			window_class.style = CS_HREDRAW | CS_VREDRAW;
			window_class.lpfnWndProc = DefWindowProc;
			window_class.cbClsExtra = 0;
			window_class.cbWndExtra = 0;
			window_class.hInstance = ::GetModuleHandle( NULL );
			window_class.hIcon = NULL;
			window_class.hCursor = NULL;
			window_class.hbrBackground = NULL;
			window_class.lpszMenuName = NULL;
			window_class.lpszClassName = "window class";
			window_class.hIconSm = NULL;

			::RegisterClassEx( &window_class );

			handle = ::CreateWindow( window_class.lpszClassName, "window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, window_class.hInstance, NULL );

			return true;
		}

		void destroy( ) noexcept{
			::DestroyWindow( handle );
			::UnregisterClass( window_class.lpszClassName, window_class.hInstance );
		}
	}

	namespace directx
	{
		bool setup( ) noexcept{
			const auto handle = ::GetModuleHandle( "d3d11.dll" );
			if ( !handle )
				return false;

			void *create_device_swapchain = ::GetProcAddress( handle, "D3D11CreateDeviceAndSwapChain" );
			if ( !create_device_swapchain )
				return false;

			D3D_FEATURE_LEVEL feature_level;

			auto swapchain_desc = populate_swapchain_desc( );

			auto create_ret = ( ( long( __stdcall * )(
				IDXGIAdapter *,
				D3D_DRIVER_TYPE,
				HMODULE,
				UINT,
				const D3D_FEATURE_LEVEL *,
				UINT,
				UINT,
				const DXGI_SWAP_CHAIN_DESC *,
				IDXGISwapChain **,
				ID3D11Device **,
				D3D_FEATURE_LEVEL *,
				ID3D11DeviceContext ** ) )( create_device_swapchain ) )( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
																		 NULL, feature_levels, 2,
																		 D3D11_SDK_VERSION, &swapchain_desc, &swapchain,
																		 &device, &feature_level, &context );

			if ( create_ret < 0 )
				return false;

			virtual_table = ( uint32_t * )::calloc( 205, sizeof( uint32_t ) );
			if ( virtual_table ){
				memcpy( virtual_table, *( uint32_t ** ) swapchain, 18 * sizeof( uint32_t ) );
				memcpy( virtual_table + 18, *( uint32_t ** ) device, 43 * sizeof( uint32_t ) );
				memcpy( virtual_table + 18 + 43, *( uint32_t ** ) context, 144 * sizeof( uint32_t ) );
			}

			destroy( );
			window::destroy( );

			return true;
		}

		void destroy( ) noexcept{
			if ( device ){
				device->Release( );
				device = nullptr;
			}if ( swapchain ){
				swapchain->Release( );
				swapchain = nullptr;
			}if ( context ){
				context->Release( );
				context = nullptr;
			}
		}

		DXGI_SAMPLE_DESC populate_sameple_desc( ){
			DXGI_SAMPLE_DESC sample_desc;
			sample_desc.Count = 1;
			sample_desc.Quality = 0;

			return sample_desc;
		}

		DXGI_MODE_DESC populate_mode_desc( ){
			DXGI_MODE_DESC mode_desc;
			mode_desc.Width = 100;
			mode_desc.Height = 100;
			mode_desc.RefreshRate = populate_rational( );
			mode_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			mode_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			mode_desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

			return mode_desc;
		}

		DXGI_SWAP_CHAIN_DESC populate_swapchain_desc( ){
			DXGI_SWAP_CHAIN_DESC swapchain_desc;
			swapchain_desc.BufferDesc = populate_mode_desc( );
			swapchain_desc.SampleDesc = populate_sameple_desc( );
			swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapchain_desc.BufferCount = 1;
			swapchain_desc.OutputWindow = window::handle;
			swapchain_desc.Windowed = 1;
			swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			return swapchain_desc;
		}

		DXGI_RATIONAL populate_rational( ){
			DXGI_RATIONAL rational;
			rational.Numerator = 60;
			rational.Denominator = 1;

			return rational;
		}

		long generate_shader( ID3D11Device *device, ID3D11PixelShader **shader, float r, float g, float b, float a ){
			char szCast[ ] = "struct VS_OUT"
				"{"
				" float4 Position : SV_Position;"
				" float4 Color : COLOR0;"
				"};"
				"float4 main( VS_OUT input ) : SV_Target"
				"{"
				" float4 col;"
				" col.a = %f;"
				" col.r = %f;"
				" col.g = %f;"
				" col.b = %f;"
				" return col;"
				"}";

			ID3D10Blob *pBlob;
			char szPixelShader[ 1000 ];

			sprintf_s( szPixelShader, szCast, a, r, g, b );

			ID3DBlob *error;

			HRESULT hr = D3DCompile( szPixelShader, sizeof( szPixelShader ), "shader", NULL, NULL, "main", "ps_4_0", NULL, NULL, &pBlob, &error );

			if ( FAILED( hr ) )
				return hr;

			hr = device->CreatePixelShader( ( DWORD * ) pBlob->GetBufferPointer( ), pBlob->GetBufferSize( ), NULL, shader );

			if ( FAILED( hr ) )
				return hr;

			return S_OK;
		}

		void generate_texture( uint32_t pixelcolor, DXGI_FORMAT format ){
			ID3D11Texture2D *texc = nullptr;
			ID3D11ShaderResourceView *textureColor = nullptr;

			D3D11_SUBRESOURCE_DATA initData = { &pixelcolor, sizeof( uint32_t ), 0 };

			D3D11_TEXTURE2D_DESC desc;
			desc.Width = 1;
			desc.Height = 1;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = format;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			device->CreateTexture2D( &desc, &initData, &texc );

			D3D11_SHADER_RESOURCE_VIEW_DESC srdes;
			memset( &srdes, 0, sizeof( srdes ) );
			srdes.Format = format;
			srdes.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srdes.Texture2D.MostDetailedMip = 0;
			srdes.Texture2D.MipLevels = 1;
			device->CreateShaderResourceView( texc, &srdes, &textureColor );
		}
	}

	bool device_setup( ){
		if ( !window::setup( ) )
			return false;

		if ( !directx::setup( ) ){
			window::destroy( );
			return false;
		}

		return true;
	}

	namespace rendering
	{
		void menu_misc( ImGuiWindowFlags &flags ){
			ImGui::SetNextWindowSize( ImVec2( 120, 110 ), ImGuiCond_Once );
			ImGui::SetNextWindowPos( ImVec2( directx::viewport::width * 0.5f + 205, directx::viewport::height * 0.5f ), ImGuiCond_Appearing );
			if ( ImGui::Begin( "miscellaneous##window", ( bool * ) NULL, ImGuiWindowFlags_NoResize ) ){
				if ( ImGui::Button( "auto loot" ) )
					ImGui::OpenPopup( "rarity filter" );
				if ( ImGui::BeginPopup( "rarity filter" ) ){
					static const char *drop_rarity_name[ ] = { "white", "green", "blue", "purple" };
					for ( int i = 0; i < options::misc::drop_max_size; i++ )
						ImGui::MenuItem( drop_rarity_name[ i ], "", &options::misc::auto_loot[ i ] );

					ImGui::EndPopup( );
				} ImGui::Spacing( );

				static bool pvp_toggle = 1;
				if ( ImGui::Button( "toggle pvp" )
					 && globals::player_ptr ){
					globals::player_ptr->get_interface( )->set_pvp_desired( pvp_toggle );

					pvp_toggle = !pvp_toggle;
				}
				ImGui::Spacing( );

				if ( ImGui::Button( "auto quest" ) ) cheats::auto_quest::get( ).start( );
			} ImGui::End( );
		}

		void menu_ragebot( ImGuiWindowFlags &flags ){
			ImGui::SetNextWindowSize( ImVec2( 310, 120 ), ImGuiCond_Once );
			ImGui::SetNextWindowPos( ImVec2( directx::viewport::width * 0.5f - 165, directx::viewport::height * 0.5f ), ImGuiCond_Appearing );
			if ( ImGui::Begin( "ragebot##window", ( bool * ) NULL, flags ) ){
				ImGui::Columns( 2, 0, false );
				ImGui::Checkbox( "no reload", &options::ragebot::no_reload );
				ImGui::Checkbox( "refill ammo", &options::ragebot::refill_ammo );

				ImGui::Spacing( ); ImGui::Spacing( );
				ImGui::Checkbox( "auto fire", &options::ragebot::auto_fire );

				if ( options::ragebot::auto_fire )
					ImGui::Checkbox( "target ai", &options::ragebot::target_ai );

				ImGui::NextColumn( );
				ImGui::Text( "yaw modifier" ); ImGui::SetNextItemWidth( 140 );
				const char *items[ ] = { "disabled", "client", "server", "both" };
				const char *combo_preview_value = items[ options::ragebot::yaw_modifier_mode ];  // Pass in the preview value visible before opening the combo (it could be anything)
				if ( ImGui::BeginCombo( "##yaw_modifier", combo_preview_value, 0 ) ){
					for ( int n = 0; n < IM_ARRAYSIZE( items ); n++ ){
						const bool is_selected = ( options::ragebot::yaw_modifier_mode == n );
						if ( ImGui::Selectable( items[ n ], is_selected ) )
							options::ragebot::yaw_modifier_mode = n;

						if ( is_selected )
							ImGui::SetItemDefaultFocus( );
					}
					ImGui::EndCombo( );
				}

				ImGui::Text( "fakelag interval" ); ImGui::SetNextItemWidth( 140 );
				ImGui::SliderInt( "##fakelag_interval", &options::ragebot::fakelag_interval, 0, 64, "%d" );
			} ImGui::End( );
		}

		void menu_visuals( ImGuiWindowFlags &flags ){
			ImGui::SetNextWindowSize( ImVec2( 310, 110 ), ImGuiCond_Once );
			ImGui::SetNextWindowPos( ImVec2( directx::viewport::width * 0.5f - 165, directx::viewport::height * 0.5f ), ImGuiCond_Appearing );
			if ( ImGui::Begin( "visuals##window", ( bool * ) NULL, flags ) ){
				ImGui::Columns( 2, 0, false );
				ImGui::Spacing( ); ImGui::Spacing( ); ImGui::Spacing( ); ImGui::Spacing( );
				ImGui::Checkbox( "chams", &options::visuals::z_buffer );
				ImGui::NextColumn( );

				ImGui::Text( "field of view" ); ImGui::SetNextItemWidth( 140 );
				ImGui::SliderFloat( "##field_of_view", &options::visuals::field_of_view, 0.0f, 120.0f, "%.f" );
				ImGui::Columns( 1 );

				ImGui::Spacing( ); ImGui::SetNextItemWidth( 290 );
				ImGui::ColorEdit3( "##chams_color", options::visuals::chams_color, 0 );

			} ImGui::End( );
		}

		void menu_movement( ImGuiWindowFlags &flags ){
			ImGui::SetNextWindowSize( ImVec2( 310, 120 ), ImGuiCond_Once );
			ImGui::SetNextWindowPos( ImVec2( directx::viewport::width * 0.5f - 165, directx::viewport::height * 0.5f ), ImGuiCond_Appearing );
			if ( ImGui::Begin( "movement##window", ( bool * ) NULL, flags ) ){
				ImGui::Columns( 2, 0, false );
				ImGui::Checkbox( "fly", &options::movement::fly );
				ImGui::Checkbox( "noclip", &options::movement::noclip );
				ImGui::Checkbox( "quickstop", &options::movement::quickstop );
				ImGui::NextColumn( );

				ImGui::Text( "movement speed" ); ImGui::SetNextItemWidth( 140 );
				ImGui::SliderFloat( "##movespeed", &options::movement::movespeed_multiplier, 0.0f, 5.0f, "%.2f" );

				ImGui::Text( "jump speed" ); ImGui::SetNextItemWidth( 140 );
				ImGui::SliderFloat( "##jumpspeed", &options::movement::jumpspeed_multiplier, 0.0f, 5.0f, "%.2f" );

			} ImGui::End( );
		}

		enum menu_tab_e{
			tab_ragebot,
			tab_visuals,
			tab_movement
		};

		void main_menu( ){
			static menu_tab_e selected_tab = tab_ragebot;
			if ( ImGui::BeginMainMenuBar( ) ){
				if ( ImGui::BeginMenu( "ragebot##menu" ) ){
					selected_tab = tab_ragebot;

					ImGui::EndMenu( );
				}if ( ImGui::BeginMenu( "visuals##menu" ) ){
					selected_tab = tab_visuals;

					ImGui::EndMenu( );
				}if ( ImGui::BeginMenu( "movement##menu" ) ){
					selected_tab = tab_movement;

					ImGui::EndMenu( );
				}
			}ImGui::EndMainMenuBar( );

			static ImGuiWindowFlags flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;

			switch ( selected_tab ){
				case tab_ragebot:
					menu_ragebot( flags );
					break;
				case tab_visuals:
					menu_visuals( flags );
					break;
				case tab_movement:
					menu_movement( flags );
					break;
			} menu_misc( flags );
		}

		void overlay( ){
			static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
			static ImVec2 window_pos, window_pos_pivot;

			static byte predefinitions = 0;
			if ( !predefinitions ){
				ImGuiIO &io = ImGui::GetIO( );
				const float PAD = 10.0f;
				const ImGuiViewport *viewport = ImGui::GetMainViewport( );
				ImVec2 work_pos = viewport->WorkPos;
				ImVec2 work_size = viewport->WorkSize;

				window_pos.x = ( 0 & 1 ) ? ( work_pos.x + work_size.x - PAD ) : ( work_pos.x + PAD );
				window_pos.y = ( 2 & 2 ) ? ( work_pos.y + work_size.y - PAD ) : ( work_pos.y + PAD );
				window_pos_pivot.x = ( 0 & 1 ) ? 1.0f : 0.0f;
				window_pos_pivot.y = ( 2 & 2 ) ? 1.0f : 0.0f;

				predefinitions = 1;
			}

			ImGui::SetNextWindowPos( window_pos, ImGuiCond_Always, window_pos_pivot );

			ImGui::SetNextWindowBgAlpha( 0.85f );
			if ( globals::player_ptr ){
				if ( ImGui::Begin( "overlay", ( bool * ) 0, window_flags ) ){

					ImGui::Text( "name: %s", globals::player_ptr->get_name( ).c_str( ) );
					ImGui::Text( "team: %s", globals::player_ptr->get_team_name( ).c_str( ) );
					ImGui::Text( "%s", globals::player_ptr->get_region( ).c_str( ) );
				}ImGui::End( );
			}
		}

		void update_framerate( ){
			static auto frames = 0;
			static auto then = std::chrono::high_resolution_clock::now( );
			auto now = std::chrono::high_resolution_clock::now( );
			auto duration = duration_cast< std::chrono::duration<double> >( now - then ).count( );

			if ( duration >= 1 ){
				framerate = frames;

				frames = 0;
				then = now;
			} else ++frames;
		}

		void tick_interval( ){

			static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
			static ImVec2 window_pos, window_pos_pivot;

			static byte predefinitions = 0;
			if ( !predefinitions ){
				ImGuiIO &io = ImGui::GetIO( );
				const float PAD = 10.0f;
				const ImGuiViewport *viewport = ImGui::GetMainViewport( );
				ImVec2 work_pos = viewport->WorkPos;
				ImVec2 work_size = viewport->WorkSize;

				window_pos.x = ( 1 & 1 ) ? ( work_pos.x + work_size.x - PAD ) : ( work_pos.x + PAD );
				window_pos.y = ( 2 & 2 ) ? ( work_pos.y + work_size.y - PAD ) : ( work_pos.y + PAD );
				window_pos_pivot.x = ( 1 & 1 ) ? 1.0f : 0.0f;
				window_pos_pivot.y = ( 2 & 2 ) ? 1.0f : 0.0f;

				predefinitions = 1;
			}

			static int values_offset = 0;
			values_offset = ( values_offset + 1 ) % IM_ARRAYSIZE( dt_array );

			ImGui::SetNextWindowPos( window_pos, ImGuiCond_Always, window_pos_pivot );

			ImGui::SetNextWindowBgAlpha( 0.85f );
			if ( globals::player_ptr ){
				if ( ImGui::Begin( "overlay2", ( bool * ) 0, window_flags ) ){
					float average = 0.0f;
					for ( int n = 0; n < IM_ARRAYSIZE( dt_array ); n++ )
						average += dt_array[ n ];
					average /= ( float ) IM_ARRAYSIZE( dt_array );
					char overlay[ 32 ];
					sprintf_s( overlay, "avg %f", average );
					ImGui::PlotLines( "##delta time", dt_array, IM_ARRAYSIZE( dt_array ), values_offset, overlay, 0.001f, 0.008f, ImVec2( 0, 80.0f ) );
				}
			}
		}

		void screen( ){
			ImGuiIO &io = ImGui::GetIO( );

			ImGui::SetNextWindowPos( ImVec2( -10, -10 ) );
			ImGui::SetNextWindowSize( ImVec2( io.DisplaySize.x + 10, io.DisplaySize.y + 10 ) );
			if ( ImGui::Begin( "screen", nullptr,
				 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground ) ){
				ImDrawList *draw_list = ImGui::GetWindowDrawList( );

				for ( const auto &entry : queue ){
					draw_list->AddText( ImVec2( entry.origin.x, entry.origin.y ), 0xFFFFFFFF, entry.content );
				}queue.clear( );

				draw_list->AddText( ImVec2( 20, 20 ), ImColor( 255, 255, 255, 255 ), std::to_string( framerate ).c_str( ) );

				ImGui::End( );
			}
			ImGui::Render( );
		}

		void apply_style( ){
			ImVec4 *colors = ImGui::GetStyle( ).Colors;
			colors[ ImGuiCol_Text ] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
			colors[ ImGuiCol_TextDisabled ] = ImVec4( 0.50f, 0.50f, 0.50f, 1.00f );
			colors[ ImGuiCol_WindowBg ] = ImVec4( 0.29f, 0.34f, 0.26f, 1.00f );
			colors[ ImGuiCol_ChildBg ] = ImVec4( 0.29f, 0.34f, 0.26f, 1.00f );
			colors[ ImGuiCol_PopupBg ] = ImVec4( 0.24f, 0.27f, 0.20f, 1.00f );
			colors[ ImGuiCol_Border ] = ImVec4( 0.54f, 0.57f, 0.51f, 0.50f );
			colors[ ImGuiCol_BorderShadow ] = ImVec4( 0.14f, 0.16f, 0.11f, 0.52f );
			colors[ ImGuiCol_FrameBg ] = ImVec4( 0.24f, 0.27f, 0.20f, 1.00f );
			colors[ ImGuiCol_FrameBgHovered ] = ImVec4( 0.27f, 0.30f, 0.23f, 1.00f );
			colors[ ImGuiCol_FrameBgActive ] = ImVec4( 0.30f, 0.34f, 0.26f, 1.00f );
			colors[ ImGuiCol_TitleBg ] = ImVec4( 0.24f, 0.27f, 0.20f, 1.00f );
			colors[ ImGuiCol_TitleBgActive ] = ImVec4( 0.29f, 0.34f, 0.26f, 1.00f );
			colors[ ImGuiCol_TitleBgCollapsed ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.51f );
			colors[ ImGuiCol_MenuBarBg ] = ImVec4( 0.24f, 0.27f, 0.20f, 1.00f );
			colors[ ImGuiCol_ScrollbarBg ] = ImVec4( 0.35f, 0.42f, 0.31f, 1.00f );
			colors[ ImGuiCol_ScrollbarGrab ] = ImVec4( 0.28f, 0.32f, 0.24f, 1.00f );
			colors[ ImGuiCol_ScrollbarGrabHovered ] = ImVec4( 0.25f, 0.30f, 0.22f, 1.00f );
			colors[ ImGuiCol_ScrollbarGrabActive ] = ImVec4( 0.23f, 0.27f, 0.21f, 1.00f );
			colors[ ImGuiCol_CheckMark ] = ImVec4( 0.59f, 0.54f, 0.18f, 1.00f );
			colors[ ImGuiCol_SliderGrab ] = ImVec4( 0.35f, 0.42f, 0.31f, 1.00f );
			colors[ ImGuiCol_SliderGrabActive ] = ImVec4( 0.54f, 0.57f, 0.51f, 0.50f );
			colors[ ImGuiCol_Button ] = ImVec4( 0.29f, 0.34f, 0.26f, 0.40f );
			colors[ ImGuiCol_ButtonHovered ] = ImVec4( 0.35f, 0.42f, 0.31f, 1.00f );
			colors[ ImGuiCol_ButtonActive ] = ImVec4( 0.54f, 0.57f, 0.51f, 0.50f );
			colors[ ImGuiCol_Header ] = ImVec4( 0.35f, 0.42f, 0.31f, 1.00f );
			colors[ ImGuiCol_HeaderHovered ] = ImVec4( 0.35f, 0.42f, 0.31f, 0.6f );
			colors[ ImGuiCol_HeaderActive ] = ImVec4( 0.54f, 0.57f, 0.51f, 0.50f );
			colors[ ImGuiCol_Separator ] = ImVec4( 0.14f, 0.16f, 0.11f, 1.00f );
			colors[ ImGuiCol_SeparatorHovered ] = ImVec4( 0.54f, 0.57f, 0.51f, 1.00f );
			colors[ ImGuiCol_SeparatorActive ] = ImVec4( 0.59f, 0.54f, 0.18f, 1.00f );
			colors[ ImGuiCol_ResizeGrip ] = ImVec4( 0.19f, 0.23f, 0.18f, 0.00f ); // grip invis
			colors[ ImGuiCol_ResizeGripHovered ] = ImVec4( 0.54f, 0.57f, 0.51f, 1.00f );
			colors[ ImGuiCol_ResizeGripActive ] = ImVec4( 0.59f, 0.54f, 0.18f, 1.00f );
			colors[ ImGuiCol_Tab ] = ImVec4( 0.35f, 0.42f, 0.31f, 1.00f );
			colors[ ImGuiCol_TabHovered ] = ImVec4( 0.54f, 0.57f, 0.51f, 0.78f );
			colors[ ImGuiCol_TabActive ] = ImVec4( 0.59f, 0.54f, 0.18f, 1.00f );
			colors[ ImGuiCol_TabUnfocused ] = ImVec4( 0.24f, 0.27f, 0.20f, 1.00f );
			colors[ ImGuiCol_TabUnfocusedActive ] = ImVec4( 0.35f, 0.42f, 0.31f, 1.00f );
			colors[ ImGuiCol_PlotLines ] = ImVec4( 0.61f, 0.61f, 0.61f, 1.00f );
			colors[ ImGuiCol_PlotLinesHovered ] = ImVec4( 0.59f, 0.54f, 0.18f, 1.00f );
			colors[ ImGuiCol_PlotHistogram ] = ImVec4( 1.00f, 0.78f, 0.28f, 1.00f );
			colors[ ImGuiCol_PlotHistogramHovered ] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
			colors[ ImGuiCol_TextSelectedBg ] = ImVec4( 0.59f, 0.54f, 0.18f, 1.00f );
			colors[ ImGuiCol_DragDropTarget ] = ImVec4( 0.73f, 0.67f, 0.24f, 1.00f );
			colors[ ImGuiCol_NavHighlight ] = ImVec4( 0.59f, 0.54f, 0.18f, 1.00f );
			colors[ ImGuiCol_NavWindowingHighlight ] = ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
			colors[ ImGuiCol_NavWindowingDimBg ] = ImVec4( 0.80f, 0.80f, 0.80f, 0.20f );
			colors[ ImGuiCol_ModalWindowDimBg ] = ImVec4( 0.80f, 0.80f, 0.80f, 0.35f );

			ImGuiStyle &style = ImGui::GetStyle( );
			style.FrameBorderSize = 1.0f;
			style.WindowRounding = 0.0f;
			style.ChildRounding = 0.0f;
			style.FrameRounding = 0.0f;
			style.PopupRounding = 0.0f;
			style.ScrollbarRounding = 0.0f;
			style.GrabRounding = 0.0f;
			style.TabRounding = 0.0f;
		}
	};
}