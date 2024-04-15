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

int main(int argc, char *args[]) {
  // Collect Your UserName from C:\Users
  LoggerGlobals::UsernameDirectory = std::getenv("USERNAME");

  // Create Log File and folder
  LoggerGlobals::LogFolderPath = "C:\\Users\\" +
                                 LoggerGlobals::UsernameDirectory +
                                 "\\.ThreadedLoggerForCPPTest\\logging\\";
  LoggerGlobals::LogFilePath =
      "C:\\Users\\" + LoggerGlobals::UsernameDirectory +
      "\\.ThreadedLoggerForCPPTest\\logging\\LuaCraftCPP.log";
  LoggerGlobals::LogFolderBackupPath =
      "C:\\Users\\" + LoggerGlobals::UsernameDirectory +
      "\\.ThreadedLoggerForCPPTest\\logging\\LogBackup";
  LoggerGlobals::LogFileBackupPath =
      "C:\\Users\\" + LoggerGlobals::UsernameDirectory +
      "\\.ThreadedLoggerForCPPTest\\logging\\LogBackup\\LuaCraftCPP-";

  CreateGlobalsLoggerInstanceExample::LoggerInstance.StartLoggerThread(
      LoggerGlobals::LogFolderPath, LoggerGlobals::LogFilePath,
      LoggerGlobals::LogFolderBackupPath, LoggerGlobals::LogFileBackupPath);

  // Loop
  while (true) {
    // Log messages
    CreateGlobalsLoggerInstanceExample::LoggerInstance.logMessageAsync(
        LogLevel::INFO, "Test Value: " + std::to_string(test));
    CreateGlobalsLoggerInstanceExample::LoggerInstance.logMessageAsync(
        LogLevel::INFO, "Finish Loop...");

    // Wait for some time before checking again
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  // Note: This call while not work because i don't had exit condition (and so it will don't make log backup and will not reset Backup log)
  // but for you it should work
  CreateGlobalsLoggerInstanceExample::LoggerInstance.ExitLoggerThread();
  return 0;
}
