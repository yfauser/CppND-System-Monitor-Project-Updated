#include "process.h"

#include <unistd.h>

#include <cctype>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::map;
using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid)
    : pid_(pid),
      command_(LinuxParser::Command(pid)),
      user_(LinuxParser::User(pid)),
      uid_(LinuxParser::Uid(pid)),
      upsinceboot_(LinuxParser::UpTime(pid)),
      procqueuedepth_(10),
      stale_(false) {}

// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() { return current_cpu_; }

// DONE: Return the command that generated this process
string Process::Command() { return command_; }

// DONE: Return this process's memory utilization
string Process::Ram() { return current_ram_; }

// DONE: Return the user (name) that generated this process
string Process::User() { return user_; }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime() - upsinceboot_; }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return current_cpu_ > a.current_cpu_;
}

// Update the Process CPU and RAM utilization
void Process::UpdateUtilization() {
  // Get the current stats from stats file
  map<string, float> procstats = LinuxParser::CpuUtilization(pid_);
  // Put the current stats to the back of the queue
  this->prev_stats_.emplace(procstats);
  // Get the oldest stats from the front of the queue
  map<string, float> prevprocstats = this->prev_stats_.front();

  float prevtotal{0.0};
  float prevproctime{0.0};
  // Only use prevtotal if queue is longer than 1,
  // Else we run into devision by 0 in return statement
  if (prev_stats_.size() > 1) {
    prevtotal = prevprocstats["total_time"];
    prevproctime = prevprocstats["proc_time"];
  }

  // Calculate difference between current and oldest total and idle
  float totald = procstats["total_time"] - prevtotal;
  float proctimed = procstats["proc_time"] - prevproctime;

  // Pop front element from the queue if its longer than the max time
  if (prev_stats_.size() > this->procqueuedepth_) {
    prev_stats_.pop();
  }
  // Store the CPU usage using difference between current and oldest
  this->current_cpu_ = (totald / procstats["mhz"]) / proctimed;

  // Store the RAM usage of current process
  this->current_ram_ = LinuxParser::Ram(pid_);
}

void Process::MarkStale() { stale_ = true; }

bool Process::GetStale(const Process& proc_) { return proc_.stale_; }