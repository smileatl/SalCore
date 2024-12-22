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


