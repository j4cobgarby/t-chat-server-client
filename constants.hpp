#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <iostream>
#include <fstream>

#define NET_MSG_SIZE 1024 // bytes
#define NET_PORT_DEFAULT 53000

std::ostream& bold_on(std::ostream& os) { return os << "\e[1m"; }
std::ostream& bold_off(std::ostream& os) { return os << "\e[0m"; }

inline bool file_exists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

#endif