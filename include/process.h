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
  void MarkStale();
  static bool GetStale(const Process& proc);
  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  void UpdateUtilization();
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;

  // DONE: Declare any necessary private members
 private:
  int pid_;
  std::string command_;
  std::string user_;
  std::string uid_;
  long upsinceboot_;
  float current_cpu_;
  std::string current_ram_;
  std::queue<std::map<std::string, float>> prev_stats_;
  long unsigned int procqueuedepth_;
  bool stale_;
};

#endif