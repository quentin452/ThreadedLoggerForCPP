#ifndef LOGGER_THREAD_HPP
#define LOGGER_THREAD_HPP

#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <utility>

#include "LoggerFileSystem.hpp"
#include "LoggerGlobals.hpp"

enum class LogLevel { INFO, WARNING, ERRORING, LOGICERROR };

class LoggerThread {
#if defined(_WIN32) || defined(__linux__) || !defined(__ANDROID__) || !defined(EMSCRIPTEN) || \
    !defined(__NINTENDO__) || !defined(TARGET_OS_IPHONE)

 public:
  LoggerThread() : Done_Logger_Thread(false), useFallback(false) {
    workerThread = std::thread(&LoggerThread::logWorker, this);
  }

  ~LoggerThread() { cleanup(); }

  /*
  Usage example:
  auto newLoggerInstance = std::make_unique<LoggerThread>();
   LoggerThread::SetLoggerThread(std::move(newLoggerInstance));
  */
  static void SetLoggerThread(std::unique_ptr<LoggerThread> newLoggerInstance);

  static LoggerThread &GetLoggerThread();

  void logMessageAsync(
      LogLevel level, const std::string &sourceFile, int line, const std::string &message);

  void logMessageAsync(
      LogLevel level,
      const std::string &sourceFile,
      int line,
      const std::initializer_list<std::string> &messageParts);

  void ExitLoggerThread();

  void StartLoggerThread(
      const std::string &LogFolderPath,
      const std::string &LogFilePath,
      const std::string &LogFolderBackupPath,
      const std::string &LogFileBackupPath);

 private:
  static std::unique_ptr<LoggerThread> LoggerInstanceT;
  std::thread workerThread;
  std::queue<std::function<void()>> tasks;
  std::mutex mtx;
  std::condition_variable Unlock_Logger_Thread;
  std::atomic<bool> Done_Logger_Thread;
  std::ofstream logFile;
  std::string logFilePath_;
  std::string LogFolderPathForTheThread;
  std::string LogFilePathForTheThread;
  std::string LogFolderBackupPathForTheThread;
  std::string LogFileBackupPathForTheThread;
  std::string TimeStamp;
  std::atomic<bool> useFallback;

  void logWorker();

  std::string formatMessage(
      LogLevel level, const std::string &sourceFile, int line, const std::string &message);

  template <typename... Args>
  void logMessage(LogLevel level, const std::string &sourceFile, int line, const Args &...args);

  void logMessageFallback(
      LogLevel level,
      const std::string &sourceFile,
      int line,
      const std::initializer_list<std::string> &messageParts);

  std::string extractRelativePath(const std::string &filePath);

  template <typename T>
  void append(std::ostringstream &oss, const T &arg);

  template <typename T, typename... Args>
  void append(std::ostringstream &oss, const T &first, const Args &...args);

  void copyFile(const std::string &source, const std::string &dest);

  std::string getTimestamp();

  void cleanup();

#else
 public:
  LoggerThread() {}

  ~LoggerThread() {}

  void logMessageAsync(
      LogLevel level, const std::string &sourceFile, int line, const std::string &message) {}

  void logMessageAsync(
      LogLevel level,
      const std::string &sourceFile,
      int line,
      const std::initializer_list<std::string> &messageParts) {}

  void ExitLoggerThread() {}

  void StartLoggerThread(
      const std::string &LogFolderPath,
      const std::string &LogFilePath,
      const std::string &LogFolderBackupPath,
      const std::string &LogFileBackupPath) {}

 private:
  void logWorker() {}

  template <typename... Args>
  void logMessage(LogLevel level, const std::string &sourceFile, int line, const Args &...args) {}

  template <typename T>
  void append(std::ostringstream &oss, const T &arg) {}

  template <typename T, typename... Args>
  void append(std::ostringstream &oss, const T &first, const Args &...args) {}

  void copyFile(const std::string &source, const std::string &dest) {}
#endif
};
#if defined(_WIN32) || defined(__linux__) || !defined(__ANDROID__) || !defined(EMSCRIPTEN) || \
    !defined(__NINTENDO__) || !defined(TARGET_OS_IPHONE)
#define LOGGER_THREAD(loglevel, customString) \
  LoggerThread::GetLoggerThread().logMessageAsync(loglevel, __FILE__, __LINE__, customString);
#else
#define LOGGER_THREAD(loglevel, customString)
#endif
#endif  // LOGGER_THREAD_HPP
