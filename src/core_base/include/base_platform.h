#ifdef BASELIB_EXPORTS
#define BASELIB_API __declspec(dllexport)
#else
#define BASELIB_API __declspec(dllimport)
#endif

#ifdef WIN32
#ifdef PLUGIN_EXPORTS
#define PLUGIN_API __declspec(dllexport)
#else
#define PLUGIN_API __declspec(dllimport)
#endif
#endif

#ifdef __cplusplus
#define BASE_BEGIN_EXTERN_C       extern "C" {
#define BASE_END_EXTERN_C         }
#else
#define BASE_BEGIN_EXTERN_C
#define BASE_END_EXTERN_C
#endif