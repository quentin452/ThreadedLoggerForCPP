
#include <string>
#include <ThreadedLoggerForCPP/LoggerGlobals.h>

std::thread LoggerGlobals::LogThread;
LoggerThread LoggerGlobals::LoggerInstance;
std::string LoggerGlobals::UsernameDirectory;

std::string LoggerGlobals::LogFilePath;
