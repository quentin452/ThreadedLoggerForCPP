#include <ThreadedLoggerForCPP/LoggerFileSystem.h>
#include <ThreadedLoggerForCPP/LoggerGlobals.h>
#include <ThreadedLoggerForCPP/LoggerThread.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

LoggerThread::LoggerThread() : Done_Logger_Thread(false) {
  LoggerGlobals::UsernameDirectory = std::getenv("USERNAME");
  // Create necessary directories and files
  LoggerFileSystem::createDirectories("C:\\Users\\" +
                                      LoggerGlobals::UsernameDirectory +
                                      "\\.ThreadedLoggerForCPPTest\\logging\\");
  LoggerFileSystem::createDirectories(
      "C:\\Users\\" + LoggerGlobals::UsernameDirectory +
      "\\.ThreadedLoggerForCPPTest\\logging\\LogBackup");
  LoggerFileSystem::createFile(
      "C:\\Users\\" + LoggerGlobals::UsernameDirectory +
      "\\.ThreadedLoggerForCPPTest\\logging\\LuaCraftCPP.log");
  std::string logFilePath =
      "C:\\Users\\" + LoggerGlobals::UsernameDirectory +
      "\\.ThreadedLoggerForCPPTest\\logging\\LuaCraftCPP.log";

  logFile.open(logFilePath, std::ios::trunc);
  logFile.close();
  logFilePath_ = logFilePath;
  std::thread workerThread(&LoggerThread::logWorker, this);
  workerThread.detach();
}
LoggerThread::~LoggerThread() {
  {
    std::unique_lock<std::mutex> lock(mtx);
    Done_Logger_Thread = true;
  }
  Unlock_Logger_Thread.notify_one(); // Notify worker thread to stop
}

void LoggerThread::logWorker() {
  while (true) {
    std::function<void()> task;
    {
      std::unique_lock<std::mutex> lock(mtx);
      Unlock_Logger_Thread.wait(
          lock, [this] { return !tasks.empty() || Done_Logger_Thread; });
      if (Done_Logger_Thread && tasks.empty()) {
        break;
      }
      if (tasks.empty()) {
        continue;
      }
      task = std::move(tasks.front());
      tasks.pop();
    }
    task();
  }
}

template <typename... Args>
void LoggerThread::logMessage(LogLevel level, const Args &...args) {
  std::ostringstream oss;
  switch (level) {
  case LogLevel::INFO:
    oss << "[INFO] ";
    break;
  case LogLevel::WARNING:
    oss << "[WARNING] ";
    break;
  case LogLevel::ERROR:
    oss << "[ERROR] ";
    break;
  case LogLevel::LOGICERROR:
    oss << "[LOGIC ERROR] ";
    break;
  }
  append(oss, args...);
  std::string message = oss.str();
  std::cout << message << std::endl;
  std::ofstream logFile(logFilePath_, std::ios::app);
  logFile << message << std::endl;
}

void LoggerThread::logMessageAsync(LogLevel level, const std::string &message) {
  std::unique_lock<std::mutex> lock(mtx);
  tasks.emplace([=] { logMessage(level, message); });
  Unlock_Logger_Thread.notify_one();
}

template <typename T>
void LoggerThread::append(std::ostringstream &oss, const T &arg) {
  oss << arg;
}

template <typename T, typename... Args>
void LoggerThread::append(std::ostringstream &oss, const T &first,
                          const Args &...args) {
  oss << first;
  append(oss, args...);
}

void LoggerThread::copyFile(const std::string &source,
                            const std::string &dest) {
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
  }
  std::string line;
  while (std::getline(src, line)) {
    dst << line << std::endl;
  }
  src.close();
  dst.close();
}

std::string LoggerThread::getTimestamp() {
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%H-%M-%S");
  return ss.str();
}

void LoggerThread::ExitLoggerThread() {
  std::string timestamp = getTimestamp();
  std::string src = "C:\\Users\\" + LoggerGlobals::UsernameDirectory +
                    "\\.ThreadedLoggerForCPPTest\\logging\\LuaCraftCPP.log";

  std::string dst =
      "C:\\Users\\" + LoggerGlobals::UsernameDirectory +
      "\\.ThreadedLoggerForCPPTest\\logging\\LogBackup\\LuaCraftCPP-" +
      timestamp + ".log";
  this->copyFile(src, dst);
  Done_Logger_Thread = true;
  Unlock_Logger_Thread.notify_one();
}

void LoggerThread::StartLoggerThread() {
  LoggerGlobals::LogThread = std::thread(&LoggerThread::logWorker, this);
}