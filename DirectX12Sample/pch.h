#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#define NOMINMAX
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <queue>
#include <random>
#include <stack>
#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <variant>
#include <vector>
#include <windows.h>
#include <wrl.h>
#include <set>
#include <any>
#include <format>
#include <concurrent_queue.h>
#include <functional>
#include <ranges>

#include "directx/d3dcommon.h"
#include "directx/d3dx12.h"
#include <d3dcompiler.h>
#include <dbghelp.h>
#include <dxcapi.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

using namespace DirectX;
using namespace Microsoft::WRL;


#include "Util.h"
#include "Enums.h"
#include "Graphics_Structs.h"
#include "Graphics_Header.h"


#include "Externs.h"
#include "GraphicsCore.h"

#endif //PCH_H
