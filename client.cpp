#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <cstdlib> // stoa

#include "constants.hpp"

bool running = true;

void display_incoming(bool* running, sf::TcpSocket* server, std::string* filter_name) {
    while (running) {
        sf::Packet in_packet;
        if (server->receive(in_packet) == sf::Socket::Done) {
            std::string type, name, body;
            in_packet >> type >> name >> body;
            if (type == "MSG") {
                if (name == *filter_name) continue;
                std::cout << bold_on << name << ": " << bold_off << body << std::endl;
            }
            if (type == "DISCON") {
                std::cout << bold_on << "** " << name << bold_off << " left the server!" << std::endl;
            }
        }
    }
}

int main() {
    sf::TcpSocket socket;

    std::string ip;
    std::cout << "Type the ip of a server, or leave blank for localhost: ";
    std::getline(std::cin, ip);
    if (ip == "") {
        std::cout << bold_on << "** " << bold_off << 
            "No ip specified, connecting to localhost by default.\n";
        ip = "127.0.0.1";
    }

    sf::Socket::Status status = socket.connect(ip, NET_PORT);

    if (status != sf::Socket::Done) {
        std::cout << "Error! Could not connect!" << std::endl;
        return EXIT_FAILURE;
    } else {
        std::cout << bold_on << "** " << bold_off << 
            "Connected!\n";
    }

    // expect an motd
    sf::Packet motd_packet;
    if (socket.receive(motd_packet) == sf::Socket::Done) {
        std::string motd, serv_info;
        motd_packet >> motd >> serv_info;
        std::cout << "Server MOTD:\n\n";
        std::cout << bold_on << motd << bold_off << std::endl << std::endl;
        std::cout << serv_info << std::endl;
    }

    std::string name;
    std::cout << "What's your name? ";
    std::getline(std::cin, name);

    std::thread display_thread(display_incoming, &running, &socket, &name);

    std::cout << "Now you can " << bold_on << "type messages " << bold_off << "to send them to everyone else on the server!" << std::endl;

    while (true) {
        std::string out;

        std::getline(std::cin, out);
        if (out == "") continue;

        sf::Packet packet;
        packet << std::string("MSG") << name << out;
        socket.send(packet);
    }

    running = false;
    display_thread.join();
}