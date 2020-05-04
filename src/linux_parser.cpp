#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

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
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
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
  string key, value;
  string line;
  float UsedMem, TotalMem =0.0, FreeMem = 0.0;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        TotalMem = stof(value);
      }
      if (key == "MemFree:") {
        FreeMem = stof(value);
        break;
      }     
    }
  }
  UsedMem = TotalMem - FreeMem;
  return UsedMem/TotalMem; }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  string sysTime, idleTime, line;
  long UpTime;
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream stream_(line);
    stream_>>sysTime>>idleTime;
    UpTime = std::stol(sysTime);
  }
  return UpTime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
    long jiffies;
    jiffies = sysconf(_SC_CLK_TCK)*UpTime(); 
    return jiffies;
  }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string value, line;
  vector <string> stat_list;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  long total_jiffies;
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream stream_(line);
    while (stream_>>value){
      stat_list.push_back(value);
    }
  }
  long utime{std::stol(stat_list[13])};
  long stime{std::stol(stat_list[14])};
  long cutime{std::stol(stat_list[15])};
  long cstime{std::stol(stat_list[16])};
  total_jiffies = utime + stime + cutime + cstime;
  return total_jiffies;
}


// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  string value, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  long active_jiffies;
  vector <string> stat_list;
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream stream_(line);
    while (stream_>>value){
      stat_list.push_back(value);
    }
  }
  long utime{std::stol(stat_list[0])};
  long ntime{std::stol(stat_list[1])};
  long stime{std::stol(stat_list[2])};
  active_jiffies = utime + stime + ntime;
  return active_jiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  string value, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  vector <string> stat_list;
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream stream_(line);
    while (stream_>>value){
      stat_list.push_back(value);
    }
  }
  long idle_jiffies{std::stol(stat_list[3])};
  return idle_jiffies;
 }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string value, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  string vuser, vnice, vsystem, vidle, viowait, virq, vsoftirq, vsteal, vguest,vguest_nice;
  vector <string> stat_list;
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream stream_(line);
    stream_>>value>>vuser >> vnice >> vsystem >> vidle >> viowait >> virq >> vsoftirq >> vsteal >> vguest >>vguest_nice;
  }
    stat_list.push_back(vuser);
    stat_list.push_back(vnice);
    stat_list.push_back(vsystem);
    stat_list.push_back(vidle);
    stat_list.push_back(viowait);
    stat_list.push_back(virq);
    stat_list.push_back(vsoftirq);
    stat_list.push_back(vsteal);
    stat_list.push_back(vguest);
    stat_list.push_back(vguest_nice);
  return stat_list;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string value, line, key;
  int total_processes;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        total_processes = std::stoi(value);
        break;
      }
    }
  }
  return total_processes; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string value, line, key;
  int running_processes;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        running_processes = std::stoi(value);
        break;
      }
    }
  }
  return running_processes; 
 }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line = ""; 
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);
  if(stream.is_open()){
    std::getline(stream, line);
  }
  return line; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string value, line, key;
  string ram_size;
  int r_size;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if(stream.is_open()){
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize") {
        r_size = std::stoi(value);
        r_size = r_size/1000;
        ram_size = std::to_string(r_size);
        break;
      }
    }
  }
  return ram_size; 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string value, line, key;
  string Uid_;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if(stream.is_open()){
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid") {
        Uid_ = value;
        break;
      }
    }
  }
  return Uid_;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line, usr, x, uid;
  string User_ = "";
  string user_ = Uid(pid);
  std::ifstream stream(kPasswordPath);
  if(stream.is_open()){
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> usr >> x >> uid){
      if (uid == user_) {
        User_ = usr;
        return User_;
      }
      }
    }
  }
  return User_;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string value, line;
  vector <string> stat_list;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream stream_(line);
    while (stream_>>value){
      stat_list.push_back(value);
    }
  }
  long uptime{std::stol(stat_list[21])};
  return uptime;
}