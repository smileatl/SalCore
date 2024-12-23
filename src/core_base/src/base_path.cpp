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
        << "Can't get info for '" << path << "', " << base::getErrorMessage();
  
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

std::string Path::getParent(const std::string & path)
{
  Path::StringVec sv;
  Path::split(path, sv);
  std::string root;
  if (path[0] == '/') {
      root = "/";
  }
  return root + Path::join(sv.begin(), sv.end()-1);
}

std::string Path::getBasename(const std::string & path)
{
  std::string::size_type index = path.find_last_of(Path::sep);
  
  if (index == std::string::npos)
    return path;
  
  return std::string(path.c_str() + index + 1, index);
}

std::string Path::getExtension(const std::string & path)
{
  std::string filename = Path::getBasename(path);
  std::string::size_type index = filename.find_last_of('.');
  
  // If its a  regular or hidden filenames with no extension
  // return an empty string
  if (index == std::string::npos ||  // regular filename with no ext 
      index == 0                 ||  // hidden file (starts with a '.')
      index == path.size() -1)       // filename ends with a dot
    return "";
  
  // Don't include the dot, just the extension itself (unlike Python)
  return filename.substr(index + 1);
}

apr_size_t Path::getFileSize(const std::string & path)
{
  apr_finfo_t st;
  apr_int32_t wanted = APR_FINFO_TYPE | APR_FINFO_SIZE;
  getInfo(path.c_str(), wanted, st);
  CHECK(st.filetype == APR_REG) << "Can't get the size of a non-file object";
  
  return (apr_size_t)st.size;
}

std::string Path::normalize(const std::string & path)
{
  return path;
}

void Path::split(const std::string& path, StringVec& parts)
{
    std::istringstream iss(path);
    std::string token;
    char sep_char = Path::sep[0]; // ת��Ϊ�����ַ�
    while (std::getline(iss, token, sep_char)) {
        parts.push_back(token);
    }
}

std::string Path::makeAbsolute(const std::string & path)
{
  if (Path::isAbsolute(path))
    return path;
    
  std::string cwd = Directory::getCWD();
  // If its already absolute just return the original path
  if (::strncmp(cwd.c_str(), path.c_str(), cwd.length()) == 0) 
    return path;
  
  // Get rid of trailing separators if any
  if (path.find_last_of(Path::sep) == path.length() - 1)
  {
    cwd = std::string(cwd.c_str(), cwd.length()-1);
  }
  // join the cwd to the path and return it (handle duplicate separators) 
  std::string result = cwd;
  if (path.find_first_of(Path::sep) == 0)
  {
    return cwd + path;
  }
  else
  {
    return cwd + Path::sep + path;
  }
  
  return "";
}

std::string Path::join(StringVec::iterator begin, StringVec::iterator end)
{
  // Need to get rid of redundant separators

  if (begin == end)
    return "";
    
  std::string path(*begin++);
  
  while (begin != end)
  {
    path += Path::sep;
    path += *begin++;
  };
  
  return path;
}