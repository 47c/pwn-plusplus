#pragma once

#pragma warning (disable : 4731 4244 6387)

#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
#define _ITERATOR_DEBUG_LEVEL 1
#endif

#include <windows.h>
#include <cstdint>
#include <psapi.h>
#include <processthreadsapi.h>
#include <libloaderapi.h>
#include <thread>
#include <chrono>
#include <map>
#include <vector>
#include <deque>
#include <queue>
#include <functional>
#include <set>
#include <memory>
#include <io.h>
#include <shared_mutex>
#include <any>
#include <intrin.h>
#include <iostream>
#include <bitset>
#include <winsock.h>

#include <MinHook.h>
#include <d3d11.h>
#include <dxgi.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>