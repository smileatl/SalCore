#include <string>
#include <algorithm>
#include "base.h"
#include "base_directory.h"
#include "base_path.h"
#include <apr_file_io.h>

#ifdef WIN32
  #include <windows.h>
  #include <tchar.h>
#else
  #include <sys/stat.h>
#endif

namespace Directory
{

bool exists(const std::string & path)
{
    return Path::exists(path);
}

std::string getCWD()
  {
    char cwd[APR_PATH_MAX];
  #ifdef WIN32
    DWORD res = ::GetCurrentDirectoryA(APR_PATH_MAX, cwd);
    CHECK(res > 0) << "Couldn't get current working directory. Error code: " 
      << base::getErrorMessage();
  #else
    cwd[0] = '\0';
    char * res = ::getcwd(cwd, APR_PATH_MAX);
    CHECK(res != NULL) << "Couldn't get current working directory. Error code: " << errno;
    
  #endif
    return std::string(cwd);
  }

}

