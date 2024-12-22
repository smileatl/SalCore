#ifndef BASE_PLUGIN_MANAGER_H
#define BASE_PLUGIN_MANAGER_H

#include <vector>
#include <string>
#include <map>
#include <memory>

#include "base_plugin.h"

class DynamicLibrary;
struct IObjectAdapter;

class PluginManager
{
    typedef std::map<std::string, std::shared_ptr<DynamicLibrary>> DynamicLibraryMap;
    typedef std::vector<Base_ExitFunc> ExitFuncVec;
    typedef std::vector<Base_RegisterParams> RegistrationVec;

public:
    typedef std::map<std::string, Base_RegisterParams> RegistrationMap;

    static PluginManager & getInstance();
    static base_int32_t initializePlugin(Base_InitFunc initFunc);
    base_int32_t loadAll(const std::string & pluginDirectory, Base_InvokeServiceFunc func = NULL);
    base_int32_t loadByPath(const std::string & path);

    void * createObject(const std::string & objectType, IObjectAdapter & adapter);

    base_int32_t shutdown();
    static base_int32_t registerObject(const base_byte_t * nodeType, 
                                       const Base_RegisterParams * params);
    const RegistrationMap & getRegistrationMap();
    Base_PlatformServices & getPlatformServices();

private:
    ~PluginManager();
    PluginManager();
    PluginManager(const PluginManager &);

    DynamicLibrary * loadLibrary(const std::string & path, std::string & errorString);

private:
    bool                inInitializePlugin_;
    Base_PlatformServices platformServices_;
    DynamicLibraryMap   dynamicLibraryMap_;
    ExitFuncVec         exitFuncVec_;

    // 注册精确匹配的对象类型
    RegistrationMap     tempExactMatchMap_;   // register exact-match object types 
    // 通配符（'*'）对象类型
    RegistrationVec     tempWildCardVec_;     // wild card ('*') object types

    RegistrationMap     exactMatchMap_;   // register exact-match object types 
    RegistrationVec     wildCardVec_;     // wild card ('*') object types
};

#endif // BASE_PLUGIN_MANAGER_H
