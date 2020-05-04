#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    std::vector<std::string> cu = LinuxParser::CpuUtilization();
    float idle = std::stol(cu[3]) + std::stol(cu[4]);
    float nonidle = std::stol(cu[0]) + std::stol(cu[1]) + std::stol(cu[2]) + std::stol(cu[5]) + std::stol(cu[6]) + std::stol(cu[7]);
    float total = idle + nonidle;
    return (total - idle)/total;
}