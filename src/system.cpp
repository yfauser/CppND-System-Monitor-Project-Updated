#include "system.h"

#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::distance;
using std::find;
using std::find_if;
using std::inserter;
using std::set;
using std::set_difference;
using std::size_t;
using std::sort;
using std::string;
using std::vector;

System::System()
    : kernel_(LinuxParser::Kernel()), osname_(LinuxParser::OperatingSystem()) {}

// DONE: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  vector<int> currentpids = LinuxParser::Pids();
  vector<int> cachedpids;

  // Loop to check if Pids on system are in the processes_ vector.
  // If in vector, update stats. If object is stale, mark it as such.
  for (auto& procobj : processes_) {
    int currentpid = procobj.Pid();
    vector<int>::iterator it =
        find(currentpids.begin(), currentpids.end(), currentpid);
    if (it != currentpids.end()) {
      procobj.UpdateUtilization();
      cachedpids.emplace_back(currentpid);
    } else {
      // Mark object as stale in processes_
      procobj.MarkStale();
    }
  }

  // Delete all stale objects
  processes_.erase(
      remove_if(processes_.begin(), processes_.end(), Process::GetStale),
      processes_.end());
  processes_.shrink_to_fit();

  // create new objects from Pids on system but not in processes_ vector
  vector<int> missingpids{};
  sort(currentpids.begin(), currentpids.end());
  sort(cachedpids.begin(), cachedpids.end());
  set_difference(currentpids.begin(), currentpids.end(), cachedpids.begin(),
                 cachedpids.end(), inserter(missingpids, missingpids.begin()));

  for (int& missingpid : missingpids) {
    Process new_proc(missingpid);
    new_proc.UpdateUtilization();
    processes_.push_back(new_proc);
  }
  // Sort the process_ vector before output
  sort(processes_.begin(), processes_.end());
  return processes_;
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() { return kernel_; }

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// DONE: Return the operating system name
std::string System::OperatingSystem() { return osname_; }

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// DONE: Return the number of seconds since the system started running
long System::UpTime() { return LinuxParser::UpTime(); }