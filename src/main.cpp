#include "ThreadedLoggerForCPP/LoggerFileSystem.h"
#include "ThreadedLoggerForCPP/LoggerGlobals.h"
#include "ThreadedLoggerForCPP/LoggerThread.h"
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

  LoggerGlobals::LoggerInstance.StartLoggerThread(
      LoggerGlobals::LogFolderPath, LoggerGlobals::LogFilePath,
      LoggerGlobals::LogFolderBackupPath, LoggerGlobals::LogFileBackupPath);

  // Loop
  while (true) {
    // Log messages
    LoggerGlobals::LoggerInstance.logMessageAsync(
        LogLevel::INFO, "Test Value: " + std::to_string(test));
    LoggerGlobals::LoggerInstance.logMessageAsync(LogLevel::INFO,
                                                  "Finish Loop...");

    // Wait for some time before checking again
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Note: I call ExitLoggerThread within the while loop because I don't have
    // another exit condition, but you can call ExitLoggerThread when you exit
    // your window created by SDL or another library to stop the logging thread
    // and save a copy of the logs
    LoggerGlobals::LoggerInstance.ExitLoggerThread();
  }

  return 0;
}
