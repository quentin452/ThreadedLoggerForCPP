#ifndef LoggerFileSystem_H
#define LoggerFileSystem_H

#include <string>

class LoggerFileSystem {
public:
  static bool fileExists(const std::string &filename);
  static bool createFile(const std::string &filename);
  static bool directoryExists(const std::string &path);
  static bool createDirectories(const std::string &path);
};

#endif // LoggerFileSystem_H
