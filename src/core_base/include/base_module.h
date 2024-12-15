#ifndef BASE_MODULE_H
#define BASE_MODULE_H

#include "base.h"

BASE_BEGIN_EXTERN_C

/* exporting and importing functions, this is special cased
 * to feature Windows dll stubs.
 */
#if defined(_WIN32) || defined(__CYGWIN__)
// �����Windows�����±��룬��BASE_MODULE_EXPORT�궨��Ϊ__declspec(dllexport)��__declspec(dllexport)��һ��Microsoft�ض��Ĺؼ��֣�����ָ������������ڶ�̬���ӿ��еĵ�����
#define BASE_MODULE_EXPORT __declspec(dllexport)
// �����Windows�����±��룬��BASE_MODULE_IMPORT�궨��Ϊ__declspec(dllimport) extern��__declspec(dllimport)Ҳ��һ��Microsoft�ض��Ĺؼ��֣�����ָ������������ڶ�̬���ӿ��еĵ��롣
#define BASE_MODULE_IMPORT __declspec(dllimport) extern
// �������windows��������ʹ��GCC������������GCC�汾���ڵ���4
#elif __GNUC__ >= 4
// ��GCC��һ�����ԣ�����ָ�������������Ĭ�Ͽɼ��ԣ�Ҳ���ǵ�����
#define BASE_MODULE_EXPORT __attribute__((visibility("default")))
#define BASE_MODULE_IMPORT extern
#else /* !defined(_WIN32) && !defined(__CYGWIN__) && __GNUC__ < 4 */
#  define BASE_MODULE_EXPORT
#  define BASE_MODULE_IMPORT extern
#endif

/**
��δ��붨����һ��ö������BaseModuleFlags�������ڵ���base_module_open()����ʱ���ݱ�־��������Щ��־���ڿ��ƶ�̬����ģ��ʱ����Ϊ��
�����Ƕ�ÿ��ö�ٳ����Ľ��ͣ�
BASE_MODULE_BIND_LAZY��ָ��ֻ������Ҫʱ�Ž������š�Ĭ������£��ڼ���ģ��ʱ������еķ��š�
BASE_MODULE_BIND_LOCAL��ָ��ģ���еķ��Ų�����ӵ�ȫ�������ռ��С�Ĭ������£��ڴ����ƽ̨�ϣ�ģ���еķ��Ż������ȫ�������ռ��У����ܻ������еķ��ų�ͻ��
BASE_MODULE_BIND_MASK��������������ı�־λ������������ڽ���־������λ���а�λ��������Թ��˳���Ч�ı�־λ��
��Щ��־����ͨ����λ���������|�����ʹ�ã��Դ��ݸ�base_module_open()�������Ӷ�����ģ��ļ�����Ϊ����Ҫע����ǣ���Щ��־����������ƽ̨�϶���֧�֣�������ʹ��ʱ��Ҫ�鿴�ĵ����˽��ض�ƽ̨��֧�������
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