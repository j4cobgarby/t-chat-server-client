#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <iostream>
#include <fstream>

#define NET_MSG_SIZE 1024 // bytes
#define NET_PORT_DEFAULT 53000

std::ostream& bold_on(std::ostream& os) { return os << "\033[1m"; }
std::ostream& bold_off(std::ostream& os) { return os << "\033[0m"; }

inline bool file_exists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

inline void send_packet(const std::string type, const std::string name, 
        const std::string body, sf::TcpSocket* socket) {
    sf::Packet pack;
    pack << type << name << body;
    socket->send(pack);
}

#endif