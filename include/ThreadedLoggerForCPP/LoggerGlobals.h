#include <ThreadedLoggerForCPP/LoggerThread.h>
#include <atomic>
#include <mutex>

class LoggerGlobals {
public:
    static std::thread &getGlobalLogThread() { return LogThread; }
    static std::thread LogThread;
    static LoggerThread LoggerInstance;
    static std::string UsernameDirectory;
    static std::string LogFilePath;
    static std::string LogFolderPath;
    static std::string LogFolderBackupPath;
    static std::string LogFileBackupPath;
    static std::string TimeStamp;
};