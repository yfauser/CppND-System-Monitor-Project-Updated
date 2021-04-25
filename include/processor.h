#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <map>
#include <queue>
#include <string>

class Processor {
 public:
  float Utilization();  // DONE: See src/processor.cpp

  // DONE: Declare any necessary private members
 private:
  std::queue<std::map<std::string, long>> prev_stats_;
  const long unsigned int timedistance_{3};
};

#endif