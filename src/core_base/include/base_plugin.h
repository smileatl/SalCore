/*
 * ����һ�� C ͷ�ļ�������������ǵĲ������ܹ����� C ϵͳ�����ʹ�ã������ܹ���д�� C ���
 * ���ǣ���ô���������޶�����ʹ�� C��ʵ���ϣ����Ѿ������Ϊ������ C++ ��ʵ�֡�
 */

#ifndef BASE_PLUGIN_H
#define BASE_PLUGIN_H

#include "base.h"

BASE_BEGIN_EXTERN_C

// ��ö���������������߲�������������������� C ���� C++ ʵ�ֵġ�
typedef enum Base_ProgrammingLanguage
{
    Base_ProgrammingLanguage_C,
    Base_ProgrammingLanguage_CPP,
} Base_ProgrammingLanguage;

struct Base_PlatformServices_;

// һ������ṹ���ڴ����������ʱ�����롣
typedef struct Base_ObjectParams
{
    const base_byte_t * objectType;
    const struct Base_PlatformServices_ * platformServices;
} Base_ObjectParams;

// ָ���汾��Ϣ���������ڲ��������ֻ���ؼ��ݰ汾�Ĳ����
typedef struct Base_PluginAPI_Version
{
    base_int32_t major;
    base_int32_t minor;
} Base_PluginAPI_Version;

// ����ָ��PF_CreateFunc��PF_DestroyFunc�����ɲ��ʵ�֣����ڲ�����������������ٲ������
typedef void * (*Base_CreateFunc)(Base_ObjectParams *);
typedef base_int32_t (*Base_DestroyFunc)(void *);

// �����˲�������ṩ�������������������Ϣ���Ա�����������ʼ��������汾�����������ٺ����Լ��������ԣ���
typedef struct Base_RegisterParams
{
    Base_PluginAPI_Version version;
    Base_CreateFunc createFunc;
    Base_DestroyFunc destroyFunc;
    Base_ProgrammingLanguage programmingLanguage;
} Base_RegisterParams;

// �ú���ָ�루�ɲ��������ʵ�֣�����ÿ���������֧�ֵĶ���������Base_RegisterParams�ṹ����ʽע��������������
// ע�⣬����ʵ��������ע�᲻ͬ�汾�Ķ����Լ�ע�����������͡�
typedef base_int32_t (*Base_RegisterFunc)(const base_byte_t * nodeType,
                                         const Base_RegisterParams * params);
// �ú���ָ����һ��ͨ�ú�����������������ϵͳ�ṩ�ĸ��ַ���������־���¼�������ߴ��󱨸�ȡ�
// �ú���Ҫ����һ�����������Լ�һ��ָ������ṹ�Ĳ�͸����ָ�롣
// ���Ӧ��֪�����õķ����Լ���ε������ǣ�����ʵ��һ�ַ����ֻ��ƣ���
typedef base_int32_t (*Base_InvokeServiceFunc)(const base_byte_t * serviceName,
                                              void * serviceParams);

// ���ڱ�ʾƽ̨�ṩ�����з��񣨰汾����ע�����͵��ú��������ýṹ���ڲ����ʼ����ʱ�򴫸�ÿһ�������
typedef struct Base_PlatformServices
{
    Base_PluginAPI_Version version;
    Base_RegisterFunc registerObject;
    Base_InvokeServiceFunc invokeService;
} Base_PlatformServices;

// ����˳�������ָ�룬�ɲ��ʵ�֡�
typedef base_int32_t (*Base_ExitFunc)();

// �����ʼ���ĺ���ָ�롣
typedef Base_ExitFunc (*Base_InitFunc)(const Base_PlatformServices *);

// ��̬�����Ҳ����ͨ����̬���ӿ���߹���ⲿ��Ĳ������ʼ��������ʵ��������
// ���ɶ�̬�����¶�������Բ�������������ڼ��ز��ʱ���е��á�
// ����һ��ָ��Base_PlatformServices�ṹ��ָ�룬�����ڲ����ʼ��ʱ����Щ�����ǿ��Ե��õģ�������ע����������ʱ���������������˳�������ָ�롣
extern
#ifdef __cplusplus
"C"
#endif
PLUGIN_API Base_ExitFunc Base_initPlugin(const Base_PlatformServices * params);
// ���ھ�̬������ɾ�̬���ӿ�ʵ�֣�����ֱ������Ӧ�ó������ӵĲ����Ӧ��ʵ��init���������ǲ�������ΪBase_initPlugin��
// ԭ���ǣ�����ж����̬��������ǲ�������ͬ�����ֵĺ�����
// ��̬����ĳ�ʼ������������ͬ�����Ǳ�������������ʽ�ؽ��г�ʼ����Ҳ����ͨ��Base_InitFunc�������ʼ��������
// ��ʵ���ǲ��õ���ƣ���Ϊ���Ҫ��������ɾ����̬�������Ӧ�õĴ��붼�����޸ģ�������Щ��ͬ���ֵ�init�����������ܹ��ҵ���


BASE_END_EXTERN_C

#endif // BASE_PLUGIN_H