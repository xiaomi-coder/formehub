#include <cstdlib>
#include "imgui.h"

// -----------------------------------------------------------------------
// ImGui symbol stubs for the headless skin modules (SkinCore / SkinLoader).
//
// These two projects deliberately do NOT compile the ImGui implementation
// (they render no UI). The shared config code only pulls in ImGui *type*
// definitions, but with optimizations off (/Od) the compiler still emits
// the ImVector / ImDrawListSplitter destructors, which reference
// ImGui::MemFree and ImDrawListSplitter::ClearFreeMemory.
//
// ImGui::MemAlloc is never linked here, so these containers are never grown
// and stay empty: the definitions below only ever run on empty / null input.
// MemFree mirrors ImGui's default allocator (free), so it stays correct even
// if a malloc'd buffer ever reached it.
// -----------------------------------------------------------------------

namespace ImGui
{
    void MemFree(void* ptr) { free(ptr); }
}

void ImDrawListSplitter::ClearFreeMemory()
{
    _Channels.clear(); // empty here; clear() frees only if a buffer was allocated
}
