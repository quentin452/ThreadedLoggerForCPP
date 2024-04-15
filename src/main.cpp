#include "ThreadedLoggerForCPP/LoggerGlobals.h"
#include "ThreadedLoggerForCPP/LoggerThreadStarter.h"
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

// Declaration of test as int
int test = 99;

// Declaration of a variable to store the signal status
volatile std::sig_atomic_t g_signal_status;

// Function to handle signals
void signal_handler(int signal) { g_signal_status = signal; }

int main(int argc, char *args[]) {
  // Start LuaCraft threads
  LoggerThreadStarter::LuaCraftStartAllThreads();

  // Capture SIGINT (Ctrl+C) and SIGTERM (termination request) signals
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  // Loop until a signal is received
  while (g_signal_status == 0) {
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
  // LoggerGlobals::LoggerInstance.ExitLoggerThread();
  return 0;
}

/* Simplified example for clarity

// Declaration of test as int
int test = 99;

int main(int argc, char *args[]) {
  // Start LuaCraft threads
  LoggerThreadStarter::LuaCraftStartAllThreads();
  // Loop
  while (true) {
    // Log messages
    LoggerGlobals::LoggerInstance.logMessageAsync(
        LogLevel::INFO, "Test Value: " + std::to_string(test));
    LoggerGlobals::LoggerInstance.logMessageAsync(LogLevel::INFO,
                                                  "Finish Loop...");
  }
  // Call ExitLoggerThread after destroying Window or Somewhere to exit the logger thread and save a log backup
  // LoggerGlobals::LoggerInstance.ExitLoggerThread();
  return 0;
}
*/