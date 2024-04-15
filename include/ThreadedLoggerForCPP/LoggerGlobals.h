#include "LoggerThread.h"
#ifndef LoggerGlobals_H
#define LoggerGlobals_H

#include <thread>

#include <string>
class LoggerGlobals {
public:
  static std::thread &getGlobalLogThread() { return LogThread; }
  static std::thread LogThread;
  static LoggerThread LoggerInstance;
  static std::string UsernameDirectory;
  static std::string LogFilePath;
};
#endif // LoggerGlobals_H