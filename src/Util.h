#ifndef __GPUAUDIO_RT3S_PLUGIN_UTILS__
#define __GPUAUDIO_RT3S_PLUGIN_UTILS__

#include <filesystem>

#ifdef __APPLE__
#include <dlfcn.h>
#endif // __APPLE__

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif // _WIN32

std::filesystem::path GetCurrentModulePath() {
#ifdef _WIN32
    WCHAR tmp[MAX_PATH];
    HMODULE hm {};

    if (::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPWSTR)&GetCurrentModulePath, &hm) == 0) {
        throw std::exception("GetModuleHandleExW function failed");
    }

    if (::GetModuleFileNameW(hm, tmp, MAX_PATH) == 0) {
        throw std::exception("GetModuleFileNameW function failed");
    }

    return {tmp};
#elif defined __APPLE__
    Dl_info info;
    if (::dladdr(reinterpret_cast<void*>(&GetCurrentModulePath), &info) == 0) {
        throw std::runtime_error("dladdr function failed");
    }
    return std::filesystem::u8path(info.dli_fname);
#else
    static_assert(false, "GetCurrentModulePath() is not implemented for this target.");
#error Platform is not supported.
#endif // WIN32
}

#endif // __GPUAUDIO_RT3S_PLUGIN_UTILS__