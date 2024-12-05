#include "../libs/include/ThreadedLoggerForCPP/LoggerThread.h"
std::unique_ptr<LoggerThread> LoggerThread::LoggerInstanceT;

void LoggerThread::SetLoggerThread(std::unique_ptr<LoggerThread> newLoggerInstance) {
  LoggerInstanceT = std::move(newLoggerInstance);
}

LoggerThread &LoggerThread::GetLoggerThread() {
  if (LoggerInstanceT == nullptr) {
    LoggerInstanceT = std::unique_ptr<LoggerThread>(new LoggerThread());
  }
  return *LoggerInstanceT;
}

void LoggerThread::logMessageAsync(
    LogLevel level, const std::string &sourceFile, int line, const std::string &message) {
  logMessageAsync(level, sourceFile, line, {message});
}

void LoggerThread::logMessageAsync(
    LogLevel level,
    const std::string &sourceFile,
    int line,
    const std::initializer_list<std::string> &messageParts) {
  if (useFallback) {
    logMessageFallback(level, sourceFile, line, messageParts);
    return;
  }

  std::ostringstream messageStream;
  for (const auto &part : messageParts) {
    messageStream << part;
  }
  std::string message = messageStream.str();

  {
    std::unique_lock<std::mutex> lock(mtx);
    tasks.emplace([=] { logMessage(level, sourceFile, line, message); });
  }
  Unlock_Logger_Thread.notify_one();
}

void LoggerThread::ExitLoggerThread() {
  cleanup();
  TimeStamp = getTimestamp();
  std::string src = LogFilePathForTheThread;
  std::string dst = LogFileBackupPathForTheThread + TimeStamp + ".log";
  copyFile(src, dst);
}
void LoggerThread::StartLoggerThread(
    const std::string &LogFolderPath,
    const std::string &LogFilePath,
    const std::string &LogFolderBackupPath,
    const std::string &LogFileBackupPath) {
  this->LogFolderPathForTheThread = LogFolderPath;
  this->LogFilePathForTheThread = LogFilePath;
  this->logFilePath_ = LogFilePath;
  this->LogFolderBackupPathForTheThread = LogFolderBackupPath;
  this->LogFileBackupPathForTheThread = LogFileBackupPath;
  LoggerFileSystem::createDirectories(LogFolderBackupPathForTheThread);
  LoggerFileSystem::createDirectories(LogFolderPathForTheThread);
  LoggerFileSystem::createFile(LogFilePathForTheThread);
  logFile.open(logFilePath_,
               std::ios::out | std::ios::trunc);  // Open file in truncate mode
  if (!logFile.is_open()) {
    std::cerr << "Error: Unable to open log file.\n";
    useFallback = true;
  }
}

void LoggerThread::logWorker() {
  while (true) {
    std::function<void()> task;
    {
      std::unique_lock<std::mutex> lock(mtx);
      Unlock_Logger_Thread.wait(lock, [this] { return !tasks.empty() || Done_Logger_Thread; });
      if (Done_Logger_Thread && tasks.empty()) {
        break;
      }
      task = std::move(tasks.front());
      tasks.pop();
    }
    task();
  }
}
std::string LoggerThread::formatMessage(
    LogLevel level, const std::string &sourceFile, int line, const std::string &message) {
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
  oss << getTimestamp() << " [" << extractRelativePath(sourceFile) << ":" << line << "] "
      << message;
  return oss.str();
}

template <typename... Args>
void LoggerThread::logMessage(
    LogLevel level, const std::string &sourceFile, int line, const Args &...args) {
  std::ostringstream oss;
  append(oss, args...);
  std::string message = formatMessage(level, sourceFile, line, oss.str());
  std::cout << message << std::endl;
  if (logFile.is_open()) {
    std::lock_guard<std::mutex> lock(mtx);  // Lock before writing
    logFile << message << std::endl;        // Write to file
  } else {
    std::cerr << "Error: Unable to write to log file.\n";
  }
}

void LoggerThread::logMessageFallback(
    LogLevel level,
    const std::string &sourceFile,
    int line,
    const std::initializer_list<std::string> &messageParts) {
  std::ofstream fallbackLogFile(
      "fallback_log.txt",
      std::ios::out | std::ios::app);  // Open in append mode
  if (!fallbackLogFile.is_open()) {
    std::cerr << "Error: Unable to open fallback log file.\n";
    return;
  }
  std::ostringstream oss;
  for (const auto &part : messageParts) {
    oss << part;
  }
  std::string message = formatMessage(level, sourceFile, line, oss.str());
  std::cout << message << std::endl;
  fallbackLogFile << message << std::endl;  // Write to fallback log file
  fallbackLogFile.close();
}

std::string LoggerThread::extractRelativePath(const std::string &filePath) {
  size_t found = filePath.find_last_of("/\\");
  if (found != std::string::npos) {
    size_t srcIndex = filePath.rfind(LoggerGlobals::SrcProjectDirectory, found);
    if (srcIndex != std::string::npos) {
      return filePath.substr(srcIndex);
    }
    return filePath.substr(found);
  }
  return filePath;
}

template <typename T>
void LoggerThread::append(std::ostringstream &oss, const T &arg) {
  oss << arg;
}

template <typename T, typename... Args>
void LoggerThread::append(std::ostringstream &oss, const T &first, const Args &...args) {
  oss << first;
  append(oss, args...);
}

void LoggerThread::copyFile(const std::string &source, const std::string &dest) {
  std::ifstream src(source, std::ios::binary);
  if (!src.is_open()) {
    std::cerr << "Error: Unable to open source file " << source << ".\n";
    return;
  }

  std::ofstream dst(dest, std::ios::binary);
  if (!dst.is_open()) {
    std::cerr << "Error: Unable to create or open destination file " << dest << ".\n";
    src.close();
    return;
  }
  dst << src.rdbuf();  // Efficiently copy file
  src.close();
  dst.close();
}

std::string LoggerThread::getTimestamp() {
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);

  struct tm timeinfo;

#ifdef _WIN32
  localtime_s(&timeinfo, &in_time_t);
#else
  localtime_r(&timeinfo, &in_time_t);
#endif

  char buffer[80];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H-%M-%S", &timeinfo);

  return std::string(buffer);
}
void LoggerThread::cleanup() {
  {
    std::unique_lock<std::mutex> lock(mtx);
    Done_Logger_Thread = true;
    Unlock_Logger_Thread.notify_one();  // Notify worker thread to stop
  }
  if (workerThread.joinable()) {
    workerThread.join();  // Wait for worker thread to finish
  }
  if (logFile.is_open()) {
    logFile.close();
  }
}