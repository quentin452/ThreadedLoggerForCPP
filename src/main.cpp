#include "CreateGlobalsLoggerInstanceExample.h"

#include <ThreadedLoggerForCPP/LoggerFileSystem.hpp>
#include <ThreadedLoggerForCPP/LoggerGlobals.hpp>
#include <ThreadedLoggerForCPP/LoggerThread.hpp>

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

// Declaration of test as int
int test = 99;
void testLoggingSpeed() {
  const int numIterations = 10000;

  auto startLogAsync = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < numIterations; ++i) {
    CreateGlobalsLoggerInstanceExample::LoggerInstance.logMessageAsync(
        LogLevel::INFO, __FILE__, __LINE__, "logger test");
  }
  auto endLogAsync = std::chrono::high_resolution_clock::now();
  auto durationLogAsync = std::chrono::duration_cast<std::chrono::milliseconds>(
      endLogAsync - startLogAsync);

  auto startStdCout = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < numIterations; ++i) {
    std::cout << "logger test" << std::endl;
  }
  auto endStdCout = std::chrono::high_resolution_clock::now();
  auto durationStdCout = std::chrono::duration_cast<std::chrono::milliseconds>(
      endStdCout - startStdCout);
  std::string durationStdCoutStr =
      "Time taken by std::cout: " + std::to_string(durationStdCout.count()) +
      " milliseconds\n";
  CreateGlobalsLoggerInstanceExample::LoggerInstance.logMessageAsync(
      LogLevel::INFO, __FILE__, __LINE__, durationStdCoutStr);
  std::string durationLogAsyncStr = "Time taken by logMessageAsync: " +
                                    std::to_string(durationLogAsync.count()) +
                                    " milliseconds\n";
  CreateGlobalsLoggerInstanceExample::LoggerInstance.logMessageAsync(
      LogLevel::INFO, __FILE__, __LINE__, durationLogAsyncStr);
}
int main(int argc, char *args[]) {
  // Collect Your UserName from C:\Users
#ifdef _WIN32
  LoggerGlobals::UsernameDirectory = std::getenv("USERNAME");
#else
  LoggerGlobals::UsernameDirectory = std::getenv("USER");
#endif

  // this is the folder that contain your src files like main.cpp
  LoggerGlobals::SrcProjectDirectory = "src";
  // Create Log File and folder
  LoggerGlobals::LogFolderPath = "C:\\Users\\" +
                                 LoggerGlobals::UsernameDirectory +
                                 "\\.ThreadedLoggerForCPPTest\\logging\\";
  LoggerGlobals::LogFilePath =
      "C:\\Users\\" + LoggerGlobals::UsernameDirectory +
      "\\.ThreadedLoggerForCPPTest\\logging\\ThreadedLoggerForCPP.log";
  LoggerGlobals::LogFolderBackupPath =
      "C:\\Users\\" + LoggerGlobals::UsernameDirectory +
      "\\.ThreadedLoggerForCPPTest\\logging\\LogBackup";
  LoggerGlobals::LogFileBackupPath =
      "C:\\Users\\" + LoggerGlobals::UsernameDirectory +
      "\\.ThreadedLoggerForCPPTest\\logging\\LogBackup\\ThreadedLoggerForCPP-";

  CreateGlobalsLoggerInstanceExample::LoggerInstance.StartLoggerThread(
      LoggerGlobals::LogFolderPath, LoggerGlobals::LogFilePath,
      LoggerGlobals::LogFolderBackupPath, LoggerGlobals::LogFileBackupPath);
  testLoggingSpeed();
  // Loop
  while (true) {
    // Log messages
    CreateGlobalsLoggerInstanceExample::LoggerInstance.logMessageAsync(
        LogLevel::INFO, __FILE__, __LINE__,
        "Test Value: " + std::to_string(test));
    CreateGlobalsLoggerInstanceExample::LoggerInstance.logMessageAsync(
        LogLevel::INFO, __FILE__, __LINE__, "Finish Loop...");

    // Wait for some time before checking again
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  // Note: This call while not work because i don't had exit condition (and so
  // it will don't make log backup and will not reset Backup log) but for you it
  // should work
  CreateGlobalsLoggerInstanceExample::LoggerInstance.ExitLoggerThread();
  return 0;
}
