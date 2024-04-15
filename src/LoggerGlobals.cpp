
#include <ThreadedLoggerForCPP/LoggerGlobals.h>
#include <string>

std::thread LoggerGlobals::LogThread;
LoggerThread LoggerGlobals::LoggerInstance;
std::string LoggerGlobals::UsernameDirectory;
std::string LoggerGlobals::LogFilePath;
std::string LoggerGlobals::LogFolderPath;
std::string LoggerGlobals::LogFolderBackupPath;
std::string LoggerGlobals::LogFileBackupPath;
std::string LoggerGlobals::TimeStamp;