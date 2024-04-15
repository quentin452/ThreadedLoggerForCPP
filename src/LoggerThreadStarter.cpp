#include <ThreadedLoggerForCPP/LoggerGlobals.h>
#include <ThreadedLoggerForCPP/LoggerThread.h>
#include <ThreadedLoggerForCPP/LoggerThreadStarter.h>

void LoggerThreadStarter::LuaCraftStartAllThreads() {
  LoggerGlobals::LoggerInstance.StartLoggerThread();
}