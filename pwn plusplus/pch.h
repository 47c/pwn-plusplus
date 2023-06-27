#ifndef PCH_H
#define PCH_H

#define FMT_HEADER_ONLY

#include "cppystruct/cppystruct.h"
#include <fmt/format.h>
#include <fmt/color.h>

#include "framework.h"
#include "options.hpp"

using namespace std::chrono_literals;
using namespace std::string_view_literals;

#include "vector.hpp"
#include "matrices.hpp"
#include "math.hpp"

#include "singleton.hpp"
#include "console.hpp"

#include "virtual_calls.hpp"
#include "mem_search.hpp"
#include "offsets.hpp"
#include "hooking.hpp"

#include "user_interface.hpp"

#include "miscellaneous.hpp"
#include "item.hpp"
#include "actor.hpp"
#include "drop.hpp"
#include "player.hpp"
#include "client_world.hpp"
#include "game_server_connection.hpp"
#include "ue4.hpp"

#include "globals.hpp"
#include "hooks.hpp"

#include "tick_data.hpp"
#include "packet_constructor.hpp"
#include "visuals.hpp"
#include "refill_ammo.hpp"
#include "movement.hpp"
#include "aimbot.hpp"
#include "antiaim.hpp"
#include "auto_quest.hpp"

#include "imgui/imgui.h"
#include "imgui/backend/imgui_impl_win32.h"
#include "imgui/backend/imgui_impl_dx11.h"
#endif
