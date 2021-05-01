#ifndef PROCESS_H
#define PROCESS_H

#include <map>
#include <queue>
#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid();                               // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  void UpdateCpuUtilization();             // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

  // TODO: Declare any necessary private members
 private:
  int pid_;
  std::string command_;
  std::string user_;
  std::string uid_;
  long upsinceboot_;
  float current_cpu_;
  std::queue<std::map<std::string, float>> prev_stats_;
  long unsigned int procqueuedepth_;
};

#endif