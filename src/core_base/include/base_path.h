#ifndef BASE_PATH_H
#define BASE_PATH_H

#include "base_types.h"
#include <string>
#include <vector>

class Path
{
public:
    typedef std::vector<std::string> StringVec;

    static const char * sep;
  
    static bool exists(const std::string & path);
    static std::string getParent(const std::string & path);
    static std::string getBasename(const std::string & path);
    static std::string getExtension(const std::string & path);
    static base_size_t  getFileSize(const std::string & path);
    static std::string normalize(const std::string & path);
    static std::string makeAbsolute(const std::string & path);
    static void split(const std::string & path, StringVec & parts);        
    static std::string join(StringVec::iterator begin, StringVec::iterator end);        
    static void copy(const std::string & source, const std::string & destination);
    static void remove(const std::string & path);
    static void rename(const std::string & oldPath, const std::string & newPath);
    static bool isDirectory(const std::string & path);
    static bool isFile(const std::string & path);
    static bool isSymbolicLink(const std::string & path);
    static bool isAbsolute(const std::string & path);
    static bool areEquivalent(const std::string & path1, const std::string & path2);
        
    Path(const std::string & path);
    operator const char*() const;

    Path & operator +=(const Path & path);
    bool exists() const;
    Path getParent() const;
    Path getBasename() const;
    Path getExtension() const;
    base_size_t getFileSize() const;
  
    Path & normalize();
    Path & makeAbsolute();
    void split(StringVec & parts) const;      
  
    void remove() const;
    void copy(const std::string & destination) const;
    void rename(const std::string & newPath);
  
    bool isDirectory() const;
    bool isFile() const;
    bool isAbsolute() const;
    bool isSymbolicLink() const;
    bool isEmpty() const;
	
private:
    Path();
  
private:
    std::string path_;
};

// Global operator  
Path operator+(const Path & p1, const Path & p2);

#endif // BASE_PATH_H