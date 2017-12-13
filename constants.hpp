#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <iostream>

#define NET_MSG_SIZE 1024 // bytes
#define NET_PORT 53000
#define NET_MAX_CLIENTS 32

std::ostream& bold_on(std::ostream& os) { return os << "\e[1m"; }
std::ostream& bold_off(std::ostream& os) { return os << "\e[0m"; }

#endif