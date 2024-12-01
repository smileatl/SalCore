#pragma once
#include <stdio.h>

#ifdef WIN32
#if defined(BASE_CORE_DECLARE_STATIC)
#define BASE_DECLARE(type)			type __stdcall
#define BASE_DECLARE_NONSTD(type)		type __cdecl
#define BASE_DECLARE_DATA
#elif defined(FREEBASECORE_EXPORTS)
#define BASE_DECLARE(type)			__declspec(dllexport) type __stdcall
#define BASE_DECLARE_NONSTD(type)		__declspec(dllexport) type __cdecl
#define BASE_DECLARE_DATA				__declspec(dllexport)
#else
#define BASE_DECLARE(type)			__declspec(dllimport) type __stdcall
#define BASE_DECLARE_NONSTD(type)		__declspec(dllimport) type __cdecl
#define BASE_DECLARE_DATA				__declspec(dllimport)
#endif
#endif

typedef int pid_t;
typedef int uid_t;
typedef int gid_t;
typedef uintptr_t base_size_t;