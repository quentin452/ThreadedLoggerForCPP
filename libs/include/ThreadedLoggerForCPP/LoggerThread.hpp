#ifndef LOGGER_THREAD_HPP
#define LOGGER_THREAD_HPP

#include <ThreadedLoggerForCPP/LoggerFileSystem.hpp>
#include <ThreadedLoggerForCPP/LoggerGlobals.hpp>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

enum class LogLevel { INFO, WARNING, ERRORING, LOGICERROR };

class LoggerThread {
public:
  LoggerThread() : Done_Logger_Thread(false), AppClosing(false) {
    std::thread workerThread(&LoggerThread::logWorker, this);
    workerThread.detach();
  }

  ~LoggerThread() {
    {
      std::unique_lock<std::mutex> lock(mtx);
      Done_Logger_Thread = true;
      Unlock_Logger_Thread.notify_one(); // Notify worker thread to stop
    }
    if (logFile.is_open()) {
      logFile.close();
    }
  }

  void logMessageAsync(LogLevel level, const std::string &sourceFile, int line,
                       const std::string &message) {
    std::unique_lock<std::mutex> lock(mtx);
    tasks.emplace([=] { logMessage(level, sourceFile, line, message); });
    Unlock_Logger_Thread.notify_one();
  }

  void ExitLoggerThread() {
    {
      std::unique_lock<std::mutex> lock(mtx);
      Done_Logger_Thread = true;
      AppClosing = true;
      while (!tasks.empty()) {
        auto task = tasks.front();
        tasks.pop();
        task();
      }
      Unlock_Logger_Thread.notify_one(); // Notify worker thread to stop
    }
    logFile.close(); // Close the log file

    // After the thread has finished, perform cleanup tasks
    TimeStamp = getTimestamp();
    std::string src = LogFilePathForTheThread;
    std::string dst = LogFileBackupPathForTheThread + TimeStamp + ".log";
    this->copyFile(src, dst);
    AppClosing = true;
  }
  void StartLoggerThread(const std::string &LogFolderPath,
                         const std::string &LogFilePath,
                         const std::string &LogFolderBackupPath,
                         const std::string &LogFileBackupPath) {
    this->LogFolderPathForTheThread = LogFolderPath;
    this->LogFilePathForTheThread = LogFilePath;
    this->logFilePath_ = LogFilePath;
    this->LogFolderBackupPathForTheThread = LogFolderBackupPath;
    this->LogFileBackupPathForTheThread = LogFileBackupPath;
    std::remove(LogFilePathForTheThread.c_str());
    LoggerFileSystem::createDirectories(LogFolderBackupPathForTheThread);
    LoggerFileSystem::createDirectories(LogFolderPathForTheThread);
    LoggerFileSystem::createFile(LogFilePathForTheThread);
    LogThread = std::thread(&LoggerThread::logWorker, this);
  }

  std::thread &getGlobalLogThread() { return LogThread; }
  std::thread LogThread;

private:
  std::queue<std::function<void()>> tasks;
  std::mutex mtx;
  std::condition_variable Unlock_Logger_Thread;
  bool Done_Logger_Thread;
  bool AppClosing;
  std::ofstream logFile;
  std::string logFilePath_;
  std::string LogFolderPathForTheThread;
  std::string LogFilePathForTheThread;
  std::string LogFolderBackupPathForTheThread;
  std::string LogFileBackupPathForTheThread;
  std::string TimeStamp;

  void logWorker() {
    while (AppClosing == false) {
      std::function<void()> task;
      {
        std::unique_lock<std::mutex> lock(mtx);
        Unlock_Logger_Thread.wait(lock, [this] {
          return !tasks.empty() || Done_Logger_Thread || AppClosing;
        });
        if (Done_Logger_Thread || AppClosing) {
          break;
        }
        if (tasks.empty()) {
          continue;
        }
        task = std::move(tasks.front());
        tasks.pop();
      }
      if (AppClosing) {
        break;
      }
      task();
    }
  }

  template <typename... Args>
  void logMessage(LogLevel level, const std::string &sourceFile, int line,
                  const Args &...args) {
    std::ostringstream oss;
    switch (level) {
    case LogLevel::INFO:
      oss << "[INFO] ";
      break;
    case LogLevel::WARNING:
      oss << "[WARNING] ";
      break;
    case LogLevel::ERRORING:
      oss << "[ERROR] ";
      break;
    case LogLevel::LOGICERROR:
      oss << "[LOGIC ERROR] ";
      break;
    }
    oss << getTimestamp() << " [" << extractRelativePath(sourceFile) << ":"
        << line << "] ";
    append(oss, args...);
    std::string message = oss.str();
    std::cout << message << std::endl;
    std::ofstream logFile(logFilePath_, std::ios::app);
    logFile << message << std::endl; // Write to file
  }

  std::string extractRelativePath(const std::string &filePath) {
    size_t found = filePath.find_last_of("/\\");
    if (found != std::string::npos) {
      size_t srcIndex =
          filePath.rfind(LoggerGlobals::SrcProjectDirectory, found);
      if (srcIndex != std::string::npos) {
        return filePath.substr(srcIndex);
      }
      return filePath.substr(found);
    }
    return filePath;
  }

  template <typename T> void append(std::ostringstream &oss, const T &arg) {
    oss << arg;
  }

  template <typename T, typename... Args>
  void append(std::ostringstream &oss, const T &first, const Args &...args) {
    oss << first;
    append(oss, args...);
  }

  void copyFile(const std::string &source, const std::string &dest) {
    std::ifstream src(source, std::ios::binary);
    if (!src.is_open()) {
      std::cerr << "Error: Unable to open source file " << source << ".\n";
      return;
    }

    std::ofstream dst(dest, std::ios::binary);
    if (!dst.is_open()) {
      std::cerr << "Error: Unable to create or open destination file " << dest
                << ".\n";
      src.close();
      return;
    };
    dst << src.rdbuf(); // Efficiently copy file
    src.close();
    dst.close();
  }

  std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%H-%M-%S");
    return ss.str();
  }
};

#endif // LOGGER_THREAD_HPP
