#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  string line, key, mem;
  float mem_total, mem_free;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream ss(line);
      ss >> key >> mem;
      if (key == "MemTotal:") {
        mem_total = std::stof(mem);
      } else if (key == "MemFree:") {
        mem_free = std::stof(mem);
        ;
        return (mem_total - mem_free) / (mem_total);
      }
    }
  }
  return 0.0;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  string str_time;
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    std::istringstream ss(line);
    ss >> str_time;
    return (long)std::stod(str_time);
  }
  return 0.0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return UpTime() * sysconf(_SC_CLK_TCK);  // ActiveJiffies() + IdleJiffies();//
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    string line, val;
    long utime, stime, cutime, cstime;

    while (std::getline(stream, line)) {
      std::istringstream ss(line);
      for (int i = 0; i < 13; i++) {
        ss >> val;
      }
      ss >> val;
      utime = std::stol(val);
      ss >> val;
      stime = std::stol(val);
      ss >> val;
      cutime = std::stol(val);
      ss >> val;
      cstime = std::stol(val);
    }
    return utime + stime + cutime + cstime;
  }

  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  long active_jiffies = 0;
  auto jiffies = CpuUtilization();

  active_jiffies += std::stol(jiffies[kUser_]);
  active_jiffies += std::stol(jiffies[kNice_]);
  active_jiffies += std::stol(jiffies[kSystem_]);
  active_jiffies += std::stol(jiffies[kIRQ_]);
  active_jiffies += std::stol(jiffies[kSoftIRQ_]);
  active_jiffies += std::stol(jiffies[kSteal_]);

  return active_jiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long idle_jiffies = 0;
  auto jiffies = CpuUtilization();
  idle_jiffies += std::stol(jiffies[kIdle_]);
  idle_jiffies += std::stol(jiffies[kIOwait_]);
  return idle_jiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  return GetValuesFromFile(stream, "cpu");
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  return std::stoi(GetValueFromFile(stream, "processes"));
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  return std::stoi(GetValueFromFile(stream, "procs_running"));
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  string cmd = "";
  if (stream.is_open()) {
    std::getline(stream, cmd);
  }
  return cmd;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  long ram_mb = 0;
  auto value = GetValueFromFile(stream, "VmSize:");
  try {
    ram_mb = std::stol(value) / 1024;
  } catch (...) {
  }
  return std::to_string(ram_mb);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  return GetValueFromFile(stream, "Uid:");
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::ifstream stream(kPasswordPath);
  string target_uid = Uid(pid);

  string line, user, uid, pwd;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream ss(line);
      ss >> user >> pwd >> uid;
      if (uid == target_uid) {
        return user;
      }
    }
  }

  return "";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    string line, val;
    while (std::getline(stream, line)) {
      std::istringstream ss(line);
      for (int i = 0; i < 22; i++) {
        ss >> val;
      }
      return std::stol(val) / sysconf(_SC_CLK_TCK);
    }
  }
  return 0;
}

std::string LinuxParser::GetValueFromFile(std::ifstream& stream,
                                          std::string target_key) {
  if (stream.is_open()) {
    string line, key, val;
    while (std::getline(stream, line)) {
      std::istringstream ss(line);
      ss >> key >> val;
      if (key == target_key) {
        return val;
      }
    }
  }
  return "";
}
vector<string> LinuxParser::GetValuesFromFile(std::ifstream& stream,
                                              string target_key) {
  vector<string> vals;
  if (stream.is_open()) {
    string line, key, val;
    while (std::getline(stream, line)) {
      std::istringstream ss(line);
      ss >> key;
      if (key == target_key) {
        while (ss >> val) {
          vals.push_back(val);
        }
        break;
      }
    }
  }
  return vals;
}
