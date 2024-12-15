#ifndef BASE_MODULE_H
#define BASE_MODULE_H

#include "base.h"

BASE_BEGIN_EXTERN_C

/* exporting and importing functions, this is special cased
 * to feature Windows dll stubs.
 */
#if defined(_WIN32) || defined(__CYGWIN__)
// 如果在Windows环境下编译，将BASE_MODULE_EXPORT宏定义为__declspec(dllexport)。__declspec(dllexport)是一个Microsoft特定的关键字，用于指定函数或变量在动态链接库中的导出。
#define BASE_MODULE_EXPORT __declspec(dllexport)
// 如果在Windows环境下编译，将BASE_MODULE_IMPORT宏定义为__declspec(dllimport) extern。__declspec(dllimport)也是一个Microsoft特定的关键字，用于指定函数或变量在动态链接库中的导入。
#define BASE_MODULE_IMPORT __declspec(dllimport) extern
// 如果不在windows环境，且使用GCC编译器，并且GCC版本大于等于4
#elif __GNUC__ >= 4
// 是GCC的一个特性，用于指定函数或变量的默认可见性，也就是导出。
#define BASE_MODULE_EXPORT __attribute__((visibility("default")))
#define BASE_MODULE_IMPORT extern
#else /* !defined(_WIN32) && !defined(__CYGWIN__) && __GNUC__ < 4 */
#  define BASE_MODULE_EXPORT
#  define BASE_MODULE_IMPORT extern
#endif

/**
这段代码定义了一个枚举类型BaseModuleFlags，用于在调用base_module_open()函数时传递标志参数。这些标志用于控制动态加载模块时的行为。
下面是对每个枚举常量的解释：
BASE_MODULE_BIND_LAZY：指定只有在需要时才解析符号。默认情况下，在加载模块时会绑定所有的符号。
BASE_MODULE_BIND_LOCAL：指定模块中的符号不会添加到全局命名空间中。默认情况下，在大多数平台上，模块中的符号会放置在全局命名空间中，可能会与现有的符号冲突。
BASE_MODULE_BIND_MASK：用于掩码操作的标志位。这个掩码用于将标志与其他位进行按位与操作，以过滤出有效的标志位。
这些标志可以通过按位或运算符（|）组合使用，以传递给base_module_open()函数，从而控制模块的加载行为。需要注意的是，这些标志并非在所有平台上都受支持，所以在使用时需要查看文档以了解特定平台的支持情况。
 */
typedef enum
{
  BASE_MODULE_BIND_LAZY	= 1 << 0,
  BASE_MODULE_BIND_LOCAL	= 1 << 1,
  BASE_MODULE_BIND_MASK	= 0x03
} BaseModuleFlags;

typedef struct _BaseModule BaseModule;
typedef const char* (*BaseModuleCheckInit) (BaseModule *module);
typedef void (*BaseModuleUnload) (BaseModule *module);

//#define G_MODULE_ERROR g_module_error_quark () GMODULE_AVAILABLE_MACRO_IN_2_70
//GMODULE_AVAILABLE_IN_2_70
//GQuark g_module_error_quark (void);

BASE_END_EXTERN_C

#endif /* BASE_MODULE_H */