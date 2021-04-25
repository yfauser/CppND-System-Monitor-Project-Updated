#include "processor.h"

#include <map>
#include <string>

#include "linux_parser.h"

using std::map;
using std::string;

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  // Get the current stats from stats file
  map<string, long> cpustats = LinuxParser::CpuUtilization();
  // Put the current stats to the back of the queue
  this->prev_stats_.emplace(cpustats);
  // Get the oldest stats from the front of the queue
  map<string, long> prevcpustats = this->prev_stats_.front();

  // Get current total CPU
  float total = (float)cpustats["Total"];
  float idle = (float)cpustats["Idle"];

  float prevtotal{0.0};
  float previdle{0.0};
  // Only use prevtotal if queue is longer than 1,
  // Else we run into devision by 0 in return statement
  if (prev_stats_.size() > 1) {
    prevtotal = (float)prevcpustats["Total"];
    previdle = (float)prevcpustats["Idle"];
  }

  // Calculate difference between current and oldest total and idle
  float totald = total - prevtotal;
  float idled = idle - previdle;

  // Pop front element from the queue if its longer than the max time
  if (prev_stats_.size() > this->timedistance_) {
    prev_stats_.pop();
  }

  // Return the CPU usage using difference between current and oldest
  return (totald - idled) / totald;
}