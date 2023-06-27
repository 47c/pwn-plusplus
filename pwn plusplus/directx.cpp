#include "pch.h"

namespace hooks {
	namespace directx {
		HRESULT device_context_from_swapchain( IDXGISwapChain* swapchain, ID3D11Device** device, ID3D11DeviceContext** context ) {
			HRESULT ret = swapchain->GetDevice( __uuidof( ID3D11Device ), ( PVOID* )device );

			if ( SUCCEEDED( ret ) )
				( *device )->GetImmediateContext( context );

			return ret;
		}


		d3d11present_function_type original_present = nullptr;
		long __stdcall present( IDXGISwapChain* swapchain, UINT sync_interval, UINT flags ) {
			if ( shutdown || in_setup ) return original_present( swapchain, sync_interval, flags );

			static bool init = false;

			if ( !init ) {
				user_interface::context = nullptr;
				user_interface::device = nullptr;
				user_interface::swapchain = nullptr;

				if ( FAILED( device_context_from_swapchain( swapchain, &user_interface::device, &user_interface::context ) ) ) return original_present( swapchain, sync_interval, flags );

				DXGI_SWAP_CHAIN_DESC desc;
				swapchain->GetDesc( &desc );

				user_interface::window::handle = desc.OutputWindow;
				winapi::original_window_proc = reinterpret_cast< WNDPROC >( SetWindowLongPtr( user_interface::window::handle,
																							  GWLP_WNDPROC,
																							  reinterpret_cast< LONG_PTR >( winapi::window_proc ) ) );
				ImGui::CreateContext( );
				ImGui_ImplWin32_Init( desc.OutputWindow );
				ImGui_ImplDX11_Init( user_interface::device, user_interface::context );
				ImGui::GetIO( ).ImeWindowHandle = user_interface::window_handle;

				user_interface::rendering::apply_style( );

				init = true;
			} if ( user_interface::main_render_target_view == NULL ) {
				ID3D11Texture2D* back_buffer = nullptr;

				swapchain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&back_buffer );
				if ( back_buffer ) {
					user_interface::device->CreateRenderTargetView( back_buffer, NULL, &user_interface::main_render_target_view );
				} back_buffer->Release( );
			} else user_interface::context->OMSetRenderTargets( 1, &user_interface::main_render_target_view, 0 );

			ImGui_ImplDX11_NewFrame( );
			ImGui_ImplWin32_NewFrame( );
			ImGui::NewFrame( );

			ImGuiIO& io = ImGui::GetIO( );
			user_interface::directx::viewport::width = io.DisplaySize.x;
			user_interface::directx::viewport::height = io.DisplaySize.y;

			if ( user_interface::visible ) {
				//ImGui::ShowDemoWindow( );
				user_interface::rendering::main_menu( );
			}io.MouseDrawCursor = user_interface::visible;

			user_interface::rendering::overlay( );
			user_interface::rendering::tick_interval( );
			user_interface::rendering::screen( );

			ImGui::EndFrame( );
			ImGui::Render( );

			ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );
			
			user_interface::rendering::update_framerate( );

			return original_present( swapchain, sync_interval, flags );
		}

		d3d11resizebuffers_function_type original_resize_buffers = nullptr;
		long __stdcall resize_buffers( IDXGISwapChain* swapchain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT flags ) {
			if ( shutdown || in_setup ) return original_resize_buffers( swapchain, buffer_count, width, height, new_format, flags );
			
			ImGui_ImplDX11_InvalidateDeviceObjects( );
			if ( nullptr != user_interface::main_render_target_view ) { 
				user_interface::main_render_target_view->Release( ); 
				user_interface::main_render_target_view = nullptr; 
			}

			auto original = original_resize_buffers( swapchain, buffer_count, width, height, new_format, flags );

			ImGui_ImplDX11_CreateDeviceObjects( );

			return original;
		}

		d3d11ssr_function_type original_set_shader_resources = nullptr;
		void __stdcall set_shader_resources( ID3D11DeviceContext* context, UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* shader_resource_views ) {
			user_interface::pssrStartSlot = start_slot;

			original_set_shader_resources( context, start_slot, num_views, shader_resource_views );
		}

		d3d11dip_function_type original_draw_indexed_primitive = nullptr;
		void __stdcall draw_indexed_primitive( ID3D11DeviceContext* context, UINT index_count, UINT start_index_location, INT base_vertex_location ) {
			
			// Stride == 40 && IndexCount == 35304 && indesc.ByteWidth == 129126 && vedesc.ByteWidth == 2582520 && pscdesc.ByteWidth == 2048 
			// && vscdesc.ByteWidth == 256 && pssrStartSlot == 0 && vscStartSlot == 0

			static ID3D11RasterizerState* depth_bias_false = nullptr;
			static ID3D11RasterizerState* depth_bias_true = nullptr;

			static ID3D11DepthStencilState* depth_stencil_original = nullptr;
			static ID3D11DepthStencilState* depth_stencil_false = nullptr;
			static ID3D11ShaderResourceView* texture_view = nullptr;
			static ID3D11SamplerState* sampler_state = nullptr;

			static ID3D11PixelShader* shader_green = NULL;
			static ID3D11PixelShader* shader_magenta = NULL;

			if ( !user_interface::device ) return original_draw_indexed_primitive( context, index_count, start_index_location, base_vertex_location );

			static bool setup = true;
			if ( setup ) {
				D3D11_RASTERIZER_DESC rasterizer_desc;
				ZeroMemory( &rasterizer_desc, sizeof( rasterizer_desc ) );
				rasterizer_desc.FillMode = D3D11_FILL_SOLID;
				rasterizer_desc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_FRONT;
				rasterizer_desc.FrontCounterClockwise = false;
				float bias = 1000.0f;
				float bias_float = static_cast< float >( -bias );
				bias_float /= 10000.0f;
				rasterizer_desc.DepthBias = DEPTH_BIAS_D32_FLOAT( *( DWORD* )&bias_float );
				rasterizer_desc.SlopeScaledDepthBias = 0.0f;
				rasterizer_desc.DepthBiasClamp = 0.0f;
				rasterizer_desc.DepthClipEnable = true;
				rasterizer_desc.ScissorEnable = false;
				rasterizer_desc.MultisampleEnable = false;
				rasterizer_desc.AntialiasedLineEnable = false;
				user_interface::device->CreateRasterizerState( &rasterizer_desc, &depth_bias_false );

				D3D11_RASTERIZER_DESC nrasterizer_desc;
				ZeroMemory( &nrasterizer_desc, sizeof( nrasterizer_desc ) );
				nrasterizer_desc.FillMode = D3D11_FILL_SOLID;
				//nrasterizer_desc.CullMode = D3D11_CULL_BACK; //flickering
				nrasterizer_desc.CullMode = D3D11_CULL_NONE;
				nrasterizer_desc.FrontCounterClockwise = false;
				nrasterizer_desc.DepthBias = 0.0f;
				nrasterizer_desc.SlopeScaledDepthBias = 0.0f;
				nrasterizer_desc.DepthBiasClamp = 0.0f;
				nrasterizer_desc.DepthClipEnable = true;
				nrasterizer_desc.ScissorEnable = false;
				nrasterizer_desc.MultisampleEnable = false;
				nrasterizer_desc.AntialiasedLineEnable = false;
				user_interface::device->CreateRasterizerState( &nrasterizer_desc, &depth_bias_true );

				D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
				depthStencilDesc.DepthEnable = TRUE;
				depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
				depthStencilDesc.StencilEnable = FALSE;
				depthStencilDesc.StencilReadMask = 0xFF;
				depthStencilDesc.StencilWriteMask = 0xFF;
				// Stencil operations if pixel is front-facing
				depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
				depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				// Stencil operations if pixel is back-facing
				depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
				depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				user_interface::device->CreateDepthStencilState( &depthStencilDesc, &depth_stencil_false );

				uint32_t pixel_color = 0xFFFFFF;
				D3D11_SUBRESOURCE_DATA initData = { &pixel_color, sizeof( uint32_t ), 0 };

				D3D11_TEXTURE2D_DESC desc;
				desc.Width = 1;
				desc.Height = 1;
				desc.MipLevels = desc.ArraySize = 1;
				desc.Format = DXGI_FORMAT_R8_UNORM;
				desc.SampleDesc.Count = 1;
				desc.Usage = D3D11_USAGE_DYNAMIC;
				desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				desc.MiscFlags = 0;

				ID3D11Texture2D* pTexture = nullptr;
				user_interface::device->CreateTexture2D( &desc, &initData, &pTexture );

				D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc;
				ZeroMemory( &textureViewDesc, sizeof( textureViewDesc ) );
				textureViewDesc.Format = desc.Format;
				textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				textureViewDesc.Texture2D.MipLevels = desc.MipLevels;
				textureViewDesc.Texture2D.MostDetailedMip = 0;

				user_interface::device->CreateShaderResourceView( pTexture, &textureViewDesc, &texture_view );

				D3D11_SAMPLER_DESC samplerDesc;
				ZeroMemory( &samplerDesc, sizeof( samplerDesc ) );
				samplerDesc.Filter = D3D11_FILTER_MAXIMUM_ANISOTROPIC;
				samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
				samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
				samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
				samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
				samplerDesc.BorderColor[ 0 ] = 0.f;
				samplerDesc.BorderColor[ 1 ] = 0.f;
				samplerDesc.BorderColor[ 2 ] = 0.f;
				samplerDesc.BorderColor[ 3 ] = 0.f;
				samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
				samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

				user_interface::device->CreateSamplerState( &samplerDesc, &sampler_state );

				setup = false;
			}

			if ( !shader_green )
				user_interface::directx::generate_shader( user_interface::device, &shader_green, 1.0f, 1.0f, 1.0f, 1.0f );
			
			bool should_update_shader = false;
			static float previous_color[ 4 ];
			for ( int i = 0; i < 4; i++ ) {
				if ( previous_color[ i ] != user_interface::options::visuals::chams_color[ i ] ) {
					should_update_shader = true;
				} previous_color[ i ] = user_interface::options::visuals::chams_color[ i ];
			}

			if ( !shader_magenta || should_update_shader )
				user_interface::directx::generate_shader( user_interface::device, &shader_magenta,
														  user_interface::options::visuals::chams_color[ 0 ],
														  user_interface::options::visuals::chams_color[ 1 ],
														  user_interface::options::visuals::chams_color[ 2 ],
														  1.0f );



			context->IAGetVertexBuffers( 0, 1, &user_interface::veBuffer, &user_interface::Stride, &user_interface::veBufferOffset );
			if ( user_interface::veBuffer != NULL )
				user_interface::veBuffer->GetDesc( &user_interface::vedesc );
			if ( user_interface::veBuffer != NULL ) { user_interface::veBuffer->Release( ); user_interface::veBuffer = NULL; }

			//get indesc.ByteWidth (comment out if not used)
			context->IAGetIndexBuffer( &user_interface::inBuffer, &user_interface::inFormat, &user_interface::inOffset );
			if ( user_interface::inBuffer != NULL )
				user_interface::inBuffer->GetDesc( &user_interface::indesc );
			if ( user_interface::inBuffer != NULL ) { user_interface::inBuffer->Release( ); user_interface::inBuffer = NULL; }

			//get pscdesc.ByteWidth (comment out if not used)
			context->PSGetConstantBuffers( user_interface::pscStartSlot, 1, &user_interface::pscBuffer );
			if ( user_interface::pscBuffer != NULL )
				user_interface::pscBuffer->GetDesc( &user_interface::pscdesc );
			if ( user_interface::pscBuffer != NULL ) { user_interface::pscBuffer->Release( ); user_interface::pscBuffer = NULL; }

			//get vscdesc.ByteWidth (comment out if not used)
			context->VSGetConstantBuffers( user_interface::vscStartSlot, 1, &user_interface::vscBuffer );
			if ( user_interface::vscBuffer != NULL )
				user_interface::vscBuffer->GetDesc( &user_interface::vscdesc );
			if ( user_interface::vscBuffer != NULL ) { user_interface::vscBuffer->Release( ); user_interface::vscBuffer = NULL; }
			
			if ( user_interface::Stride == 12 ) {
				//context->PSSetShader( shader_green, NULL, NULL );
				//original_draw_indexed_primitive( context, index_count, start_index_location, base_vertex_location );
			}

			if ( user_interface::Stride == 16 ) {
				//context->PSSetSamplers( 0, 1, &sampler_state );
				
				//original_draw_indexed_primitive( context, index_count, start_index_location, base_vertex_location );
				//context->PSSetShaderResources( 0, 1, &texture_view );
				//context->PSSetShader( shader_green, NULL, NULL );

			}

			// stride 16 = world
			// stride 12 = trees
			// stride 36 = building
			if ( user_interface::Stride == 40 && user_interface::options::visuals::z_buffer ) {
				UINT stencilValue = 0;

				context->OMGetDepthStencilState( &depth_stencil_original, &stencilValue );
				context->OMSetDepthStencilState( depth_stencil_false, 0 );

				context->PSSetShader( shader_magenta, NULL, NULL );

				original_draw_indexed_primitive( context, index_count, start_index_location, base_vertex_location );

				//context->PSSetShader( shader_green, NULL, NULL );

				context->OMSetDepthStencilState( depth_stencil_original, stencilValue );
				depth_stencil_original->Release( );
			}

			original_draw_indexed_primitive( context, index_count, start_index_location, base_vertex_location );
		}
	}
}