#include <string>
#include <iostream>

#include "processor.h"
#include "linux_parser.h"
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  int total_idle = LinuxParser::IdleJiffies();
  int total_non_idle =  LinuxParser::ActiveJiffies();

  int total_prev = total_prev_idle_ + total_prev_non_idle_;
  int total = total_idle + total_non_idle;

  // differiate: actual value minus the preus one
  int totald = total - total_prev;
  int idled = total_idle - total_prev_idle_;

  float cpu_utilization = (totald - idled)/(float)totald;
  total_prev_idle_ = total_idle;
  total_prev_non_idle_ = total_non_idle;

  return cpu_utilization;
}