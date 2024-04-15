#ifndef LOGGER_THREAD_H
#define LOGGER_THREAD_H

#include <condition_variable>
#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

enum class LogLevel { INFO, WARNING, ERROR, LOGICERROR };

class LoggerThread {
public:
  LoggerThread();
  ~LoggerThread();

  void logMessageAsync(LogLevel level, const std::string &message);
  void ExitLoggerThread();
  void StartLoggerThread(const std::string &LogFolderPath,
                         const std::string &LogFilePath,
                         const std::string &LogFolderBackupPath,
                         const std::string &LogFileBackupPath);
  void flushToFile();

  template <typename... Args>
  void logMessage(LogLevel level, const Args &...args);
  void logWorker();
  void copyAndCompressLogFile();
  void copyFile(const std::string &source, const std::string &dest);
  std::string getTimestamp();

private:
  std::queue<std::function<void()>> tasks;
  std::mutex mtx;
  std::condition_variable Unlock_Logger_Thread;
  bool Done_Logger_Thread;
  std::ofstream logFile;
  std::string logFilePath_;
  std::string LogFolderPathForTheThread;
  std::string LogFilePathForTheThread;
  std::string LogFolderBackupPathForTheThread;
  std::string LogFileBackupPathForTheThread;
  template <typename T> void append(std::ostringstream &oss, const T &arg);
  template <typename T, typename... Args>
  void append(std::ostringstream &oss, const T &first, const Args &...args);
};

#endif // LOGGER_THREAD_H
