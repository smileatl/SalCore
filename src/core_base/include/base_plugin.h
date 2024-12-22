/*
 * 这是一个 C 头文件。这就允许我们的插件框架能够被纯 C 系统编译和使用，并且能够编写纯 C 插件
 * 但是，这么做并不会限定必须使用 C，实际上，它已经被设计为更常用 C++ 来实现。
 */

#ifndef BASE_PLUGIN_H
#define BASE_PLUGIN_H

#include "base.h"

BASE_BEGIN_EXTERN_C

// 该枚举类型允许插件告诉插件管理器，它本身是由 C 还是 C++ 实现的。
typedef enum Base_ProgrammingLanguage
{
    Base_ProgrammingLanguage_C,
    Base_ProgrammingLanguage_CPP,
} Base_ProgrammingLanguage;

struct Base_PlatformServices_;

// 一个抽象结构，在创建插件对象时被传入。
typedef struct Base_ObjectParams
{
    const base_byte_t * objectType;
    const struct Base_PlatformServices_ * platformServices;
} Base_ObjectParams;

// 指明版本信息。这有助于插件管理器只加载兼容版本的插件。
typedef struct Base_PluginAPI_Version
{
    base_int32_t major;
    base_int32_t minor;
} Base_PluginAPI_Version;

// 函数指针PF_CreateFunc和PF_DestroyFunc必须由插件实现，用于插件管理器创建和销毁插件对象。
typedef void * (*Base_CreateFunc)(Base_ObjectParams *);
typedef base_int32_t (*Base_DestroyFunc)(void *);

// 包含了插件必须提供给插件管理器的所有信息，以便插件管理器初始化插件（版本，创建、销毁函数以及开发语言）。
typedef struct Base_RegisterParams
{
    Base_PluginAPI_Version version;
    Base_CreateFunc createFunc;
    Base_DestroyFunc destroyFunc;
    Base_ProgrammingLanguage programmingLanguage;
} Base_RegisterParams;

// 该函数指针（由插件管理器实现）允许每个插件将其支持的对象类型以Base_RegisterParams结构的形式注册给插件管理器。
// 注意，这种实现允许插件注册不同版本的对象，以及注册多个对象类型。
typedef base_int32_t (*Base_RegisterFunc)(const base_byte_t * nodeType,
                                         const Base_RegisterParams * params);
// 该函数指针是一个通用函数，允许插件调用主系统提供的各种服务，例如日志、事件处理或者错误报告等。
// 该函数要求有一个服务名称以及一个指向参数结构的不透明的指针。
// 插件应当知道可用的服务以及如何调用它们（或者实现一种服务发现机制）。
typedef base_int32_t (*Base_InvokeServiceFunc)(const base_byte_t * serviceName,
                                              void * serviceParams);

// 用于表示平台提供的所有服务（版本、已注册对象和调用函数）。该结构会在插件初始化的时候传给每一个插件。
typedef struct Base_PlatformServices
{
    Base_PluginAPI_Version version;
    Base_RegisterFunc registerObject;
    Base_InvokeServiceFunc invokeService;
} Base_PlatformServices;

// 插件退出函数的指针，由插件实现。
typedef base_int32_t (*Base_ExitFunc)();

// 插件初始化的函数指针。
typedef Base_ExitFunc (*Base_InitFunc)(const Base_PlatformServices *);

// 动态插件（也就是通过动态链接库或者共享库部署的插件）初始化函数的实际声明。
// 它由动态插件暴露出，所以插件管理器可以在加载插件时进行调用。
// 它有一个指向Base_PlatformServices结构的指针，所以在插件初始化时，这些服务都是可以调用的（这正是注册对象的理想时机），函数返回退出函数的指针。
extern
#ifdef __cplusplus
"C"
#endif
PLUGIN_API Base_ExitFunc Base_initPlugin(const Base_PlatformServices * params);
// 对于静态插件（由静态链接库实现，并且直接与主应用程序链接的插件）应该实现init函数，但是不能命名为Base_initPlugin。
// 原因是，如果有多个静态插件，它们不能有相同的名字的函数。
// 静态插件的初始化过程有所不同。它们必须由主程序显式地进行初始化，也就是通过Base_InitFunc调用其初始化函数。
// 这实际是不好的设计，因为如果要新增或者删除静态插件，主应用的代码都必须修改，并且那些不同名字的init函数都必须能够找到。


BASE_END_EXTERN_C

#endif // BASE_PLUGIN_H