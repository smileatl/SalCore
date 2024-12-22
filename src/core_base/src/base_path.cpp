#include "base.h"
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <iterator>
#include <apr_pools.h>
#include <apr_file_info.h>

#ifdef WIN32
  #include <windows.h>

  const char * Path::sep = "\\";
#else
  #include <s#inys/stat.h>
  #include <fstream>

  const char * Path::sep = "/";
#endif

Path::Path(const std::string & path) : path_(path)
{
}

static base_status_t getInfo(const std::string& path, base_int32_t wanted, base_finfo_t& info)
{
    CHECK(!path.empty()) << "Can't get the info of an empty path";

    base_status_t res;
    apr_pool_t * pool = NULL;
#ifdef WIN32
    res = base_pool_create(&pool, NULL);
#endif

    res = base_stat(&info, path.c_str(), wanted, pool);

#ifdef WIN32
    base_pool_destroy(pool);
#endif

    return res;
}

bool Path::exists(const std::string & path)
{
  if (path.empty())
    return false;
  
  base_finfo_t st;      
  base_status_t res  = getInfo(path, APR_FINFO_TYPE, st);
  return res == BASE_STATUS_SUCCESS;
}

static apr_filetype_e getType(const std::string & path)
{
    apr_finfo_t st;
    base_status_t res = getInfo(path, APR_FINFO_TYPE, st);
    CHECK(res == BASE_STATUS_SUCCESS) 
        << "Can't get info for '" << path << "', " << getErrorMessage();
  
    return st.filetype;
}

bool Path::isFile(const std::string & path)
{
  return getType(path) == APR_REG;
}

bool Path::isDirectory(const std::string & path)
{
    return getType(path) == APR_DIR;
}

bool Path::isSymbolicLink(const std::string & path)
{
    return getType(path) == APR_LNK;
}

bool Path::isAbsolute(const std::string & path)
{
  CHECK(!path.empty()) << "Empty path is invalid";
#ifdef WIN32
  if (path.size() < 2)
    return false;
  else
  return path[1] == ':';
#else
  return path[0] == '/';
#endif
}

bool Path::areEquivalent(const std::string & path1, const std::string & path2)
{
  apr_finfo_t st1;
  apr_finfo_t st2;
  apr_int32_t wanted = APR_FINFO_IDENT;
  getInfo(path1.c_str(), wanted, st1);
  getInfo(path2.c_str(), wanted, st2);
  bool res = true;
  res &= st1.device == st2.device;
  res &= st1.inode == st2.inode;
  res &= std::string(st1.fname) == std::string(st2.fname);
  
  return res;
}

