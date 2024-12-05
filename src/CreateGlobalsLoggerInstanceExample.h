#ifndef CreateGlobalsLoggerInstanceExample_H
#define CreateGlobalsLoggerInstanceExample_H
#include <ThreadedLoggerForCPP/LoggerThread.h>

#include <string>
#include <thread>

class CreateGlobalsLoggerInstanceExample {
 public:
  static LoggerThread LoggerInstance;
};
#endif  // CreateGlobalsLoggerInstanceExample_H