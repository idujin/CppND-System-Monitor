#include "format.h"

#include <iomanip>
#include <iostream>
#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  seconds = seconds % 60;
  std::ostringstream ss;
  ss << std::setw(2) << std::setfill('0') << std::to_string(hours) << ":";
  ss << std::setw(2) << std::setfill('0') << std::to_string(minutes) << ":";
  ss << std::setw(2) << std::setfill('0') << std::to_string(seconds);
  return ss.str();
}