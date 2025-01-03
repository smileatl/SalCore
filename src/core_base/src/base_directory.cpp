#include <string>
#include <algorithm>
#include "base.h"
//#include "base_path.h"
//#include "base_directory.h"
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

  static void createSingleDir(const char * path)
  {
    bool success;
  #ifdef WIN32
      success = ::CreateDirectoryA(path, NULL) != FALSE;
  #else
      int res = ::mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      success = res == 0;
  #endif
    CHECK(success) << "Faile to create directory. " << base::getErrorMessage();
  }

// Create directory recursively (creates parent if doesn't exist)
  void create(const std::string & path)
  {
    CHECK(!path.empty()) << "Can't create directory with no name";
    std::string p = Path::makeAbsolute(path);
    
    if (Path::exists(p))
    {
      CHECK(Path::isDirectory(p)) 
        << path << " already exists but it's not a directory";
        
      return;
    }

    std::string parent = Path::getParent(p);
    
    if (!Directory::exists(parent))
      create(parent);
      
    createSingleDir(p.c_str());
  }


void copyTree(const std::string& source, const std::string& destination)
{
    CHECK(Path::isDirectory(source));
    std::string baseSource(Path::getBasename(source));
    std::string dest(destination);
    dest += baseSource;
    if (!Path::exists(dest)) {
      Directory::create(dest);
    }
    CHECK(Path::isDirectory(dest));
    //Iterator i(source);
    Entry e;
}


}

