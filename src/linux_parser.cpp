#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <map>
#include <string>
#include <vector>

using std::map;
using std::stof;
using std::stoi;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// This function reads files line by line and pushes every item in the line
// to a vector. This vector is then pushed onto another vector.
vector<vector<string>> LinuxParser::GetSpacedContent(string const filepath,
                                                     char const separator) {
  string line, item;
  vector<string> innervec;
  vector<vector<string>> externalvec;
  std::ifstream filestream(filepath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::stringstream linestream(line);
      while (std::getline(linestream, item, separator)) {
        innervec.push_back(item);
      }
      externalvec.push_back(innervec);
      innervec = {};
    }
  }
  return externalvec;
}

// This function reads files line by line and extracts Key/Values from the lines
// it can also remove unwanted characters from the line
map<string, string> LinuxParser::GetKVContent(string const filepath,
                                              char const separator,
                                              vector<char> const removechars) {
  string line, key, value;
  map<string, string> dict;
  std::ifstream filestream(filepath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::stringstream linestream(line);
      std::getline(linestream, key, separator);
      std::getline(linestream, value, separator);
      for (char toremove : removechars) {
        key.erase(remove(key.begin(), key.end(), toremove), key.end());
        value.erase(remove(value.begin(), value.end(), toremove), value.end());
      }
      dict.insert({key, value});
    }
  }
  return dict;
}

// DONE: Refactored OperatorSystem function using external function for
// modularity
//       Modified from original Udacity example
string LinuxParser::OperatingSystem() {
  vector<char> removechars{'"'};
  map<string, string> osinfo = GetKVContent(kOSPath, '=', removechars);
  return osinfo["PRETTY_NAME"];
}

// DONE: Modified from original Udacity example
string LinuxParser::Kernel() {
  string filepath = kProcDirectory + kVersionFilename;
  vector<vector<string>> filecontent = GetSpacedContent(filepath, ' ');
  // in the kernel version file the kernel name is in the first
  // line and third position [0][2]
  return filecontent[0][2];
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  vector<char> removechars{' ', 'k', 'B'};
  string filepath = kProcDirectory + kMeminfoFilename;
  map<string, string> meminfo = GetKVContent(filepath, ':', removechars);
  float memuse_kb = stof(meminfo["MemTotal"]) - stof(meminfo["MemAvailable"]);
  return memuse_kb / stof(meminfo["MemTotal"]);
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string filepath = kProcDirectory + kUptimeFilename;
  vector<vector<string>> filecontent = GetSpacedContent(filepath, ' ');
  // in the proc/uptime file the uptime in seconds is in the first
  // line and first position [0][0]
  return stol(filecontent[0][0]);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
map<string, long> LinuxParser::CpuUtilization() {
  map<string, long> cpustats;
  string filepath = kProcDirectory + kStatFilename;
  vector<vector<string>> filecontent = GetSpacedContent(filepath, ' ');
  long user = stol(filecontent[0][2]);
  long nice = stol(filecontent[0][3]);
  long system = stol(filecontent[0][4]);
  long idle = stol(filecontent[0][5]);
  long iowait = stol(filecontent[0][6]);
  long irq = stol(filecontent[0][7]);
  long softirq = stol(filecontent[0][8]);
  long steal = stol(filecontent[0][9]);
  cpustats["Idle"] = idle + iowait;
  cpustats["NonIdle"] = user + nice + system + irq + softirq + steal;
  cpustats["Total"] = cpustats["Idle"] + cpustats["NonIdle"];
  return cpustats;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string filepath = kProcDirectory + kStatFilename;
  vector<vector<string>> filecontent = GetSpacedContent(filepath, ' ');
  // in the proc/stats file the processes count is in the seventh
  // line and second position [6][1]
  return stoi(filecontent[6][1]);
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string filepath = kProcDirectory + kStatFilename;
  vector<vector<string>> filecontent = GetSpacedContent(filepath, ' ');
  // in the proc/stats file the running processes count is in the seventh
  // line and second position [7][1]
  return stoi(filecontent[7][1]);
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid [[maybe_unused]]) { return 0; }
