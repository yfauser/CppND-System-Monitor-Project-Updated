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

// Helper function to read lines from files and put them into a vector
vector<string> LinuxParser::GetLines(string const &filepath) {
  string line;
  vector<string> linevec;
  std::ifstream filestream(filepath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      linevec.push_back(line);
    }
  }
  return linevec;
}

// Helper function to split line elements into a vector given a separator
vector<string> LinuxParser::GetLineElements(string const &line,
                                            char const separator) {
  string item;
  vector<string> elements;
  std::stringstream linestream(line);
  while (std::getline(linestream, item, separator)) {
    elements.push_back(item);
  }
  return elements;
}

// Helper function to read files line by line and to push every item in the line
// to a vector. This vector is then pushed onto another vector.
vector<vector<string>> LinuxParser::GetSpacedContent(string const filepath,
                                                     char const separator) {
  vector<vector<string>> contentvec;
  vector<string> linevector = GetLines(filepath);
  for (string &line : linevector) {
    contentvec.push_back(GetLineElements(line, separator));
  }
  return contentvec;
}

// This Helper function reads files line by line and extracts Key/Values
// from the lines it can also remove unwanted characters from the line
map<string, string> LinuxParser::GetKVContent(string const filepath,
                                              char const separator,
                                              vector<char> const removechars) {
  string key, value;
  map<string, string> dict;
  vector<string> linevector = GetLines(filepath);
  for (string &line : linevector) {
    std::stringstream linestream(line);
    std::getline(linestream, key, separator);
    std::getline(linestream, value, separator);
    for (char toremove : removechars) {
      key.erase(remove(key.begin(), key.end(), toremove), key.end());
      value.erase(remove(value.begin(), value.end(), toremove), value.end());
    }
    dict.insert({key, value});
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
  DIR *directory = opendir(kProcDirectory.c_str());
  struct dirent *file;
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

// DONE: Read and return CPU utilization
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

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string filepath = kProcDirectory + kStatFilename;
  vector<vector<string>> filecontent = GetSpacedContent(filepath, ' ');
  // in the proc/stats file the processes count is in the seventh
  // line and second position [6][1]
  return stoi(filecontent[6][1]);
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string filepath = kProcDirectory + kStatFilename;
  vector<vector<string>> filecontent = GetSpacedContent(filepath, ' ');
  // in the proc/stats file the running processes count is in the seventh
  // line and second position [7][1]
  return stoi(filecontent[7][1]);
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string filepath = kProcDirectory + to_string(pid) + kCmdlineFilename;
  vector<string> linevect = GetLines(filepath);
  if (linevect.size() == 0)
    return string();
  else
    return linevect[0];
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  vector<char> removechars{' ', 'k', 'B', '\t'};
  string filepath = kProcDirectory + to_string(pid) + kStatusFilename;
  map<string, string> procstatus = GetKVContent(filepath, ':', removechars);
  float mem_mb{0.0};
  if (procstatus.count("VmSize")) {
    mem_mb = stof(procstatus["VmSize"]) / 1000;
  }
  string mem_string = to_string(mem_mb);
  return mem_string.substr(0, mem_string.find(".") + 3);
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  vector<char> removechars{' '};
  string filepath = kProcDirectory + to_string(pid) + kStatusFilename;
  map<string, string> procstatus = GetKVContent(filepath, ':', removechars);
  // The Line in which the Uid is found has tab separated content
  vector<string> linevect = GetLineElements(procstatus["Uid"], '\t');
  // The Uid is the second element of the created vector
  return linevect[1];
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid_str = Uid(pid);
  // The lines in the /etc/passwd file are ':' separated
  vector<vector<string>> filecontent = GetSpacedContent(kPasswordPath, ':');
  // iterating through the lines in the file to find the userid (3rd pos)
  // and returning the username (1st pos)
  for (vector<string> &line : filecontent) {
    if (line[2] == uid_str) {
      // Truncating the string for better display
      int strsize = line[0].size();
      if (strsize >= 14) {
        return line[0].substr(0, 11) + "..";
      } else {
        return line[0].append(14 - strsize, ' ');
      }
    }
  }
  return string();
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string filepath = kProcDirectory + to_string(pid) + kStatFilename;
  vector<vector<string>> filecontent = GetSpacedContent(filepath, ' ');
  // in the proc/<pid>/stat file the uptime since boot in seconds
  // is in the first line and 22nd position [0][21]
  long uptime_ticks = stol(filecontent[0][21]);
  long uptime_seconds = uptime_ticks / sysconf(_SC_CLK_TCK);
  return uptime_seconds;
}

// DONE: Read and return per process CPU utilization
map<string, float> LinuxParser::CpuUtilization(int pid) {
  string filepath = kProcDirectory + to_string(pid) + kStatFilename;
  vector<vector<string>> filecontent = GetSpacedContent(filepath, ' ');
  map<string, float> procstats;
  float utime = stof(filecontent[0][13]);
  float stime = stof(filecontent[0][14]);
  float cutime = stof(filecontent[0][15]);
  float cstime = stof(filecontent[0][16]);
  float starttime = stof(filecontent[0][21]);
  // Calculation taken from
  // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  procstats["total_time"] = utime + stime + cutime + cstime;
  procstats["proc_time"] = UpTime() - (starttime / (float)sysconf(_SC_CLK_TCK));
  procstats["cpu_usage"] =
      ((procstats["total_time"] / (float)sysconf(_SC_CLK_TCK)) /
       procstats["proc_time"]);
  if (procstats["cpu_usage"] < 0.0) {
    ;
  }
  return procstats;
}