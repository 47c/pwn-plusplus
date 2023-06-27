#include "pch.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

namespace hooks {
	namespace winapi {
		WNDPROC original_window_proc = nullptr;

		LRESULT CALLBACK window_proc( const HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param ) {

			if ( user_interface::window_handle ) {
				POINT coordinates;

				ImGuiIO& io = ImGui::GetIO( );
				GetCursorPos( &coordinates ); ScreenToClient( user_interface::window_handle, &coordinates );
				ImGui::GetIO( ).MousePos.x = coordinates.x;
				ImGui::GetIO( ).MousePos.y = coordinates.y;
			}

			if ( msg == WM_KEYUP ) {
				if ( w_param == VK_INSERT ) {
					user_interface::visible = !user_interface::visible;
				}
			}

			if ( user_interface::visible ) {
				ImGui_ImplWin32_WndProcHandler( hwnd, msg, w_param, l_param );
				return true;
			}

			return CallWindowProcA( original_window_proc, hwnd, msg, w_param, l_param );
		}
	}
}