#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <cstdlib> // stoa, atoi

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

int main(int argc, char* argv[]) {
    bool host_set = false, name_set = false;
    std::string ip, name;
    int port = NET_PORT_DEFAULT;
    if (argc > 1) { // some command line arguments, but not --help
        for (int i = 0; i < argc; i++) {
            std::string flag(argv[i]);
            if (flag == "-?" || flag == "--help") {
                std::cout << 
                    "Starts a client to join a server using this protocol." << std::endl << std::endl <<
                    "  -?, --help\tShows this help." << std::endl << std::endl <<
                    "  -h, --host\tSets the ip of the host to connect to." << std::endl << std::endl <<
                    "  -p, --port\tSets the port of the server to connect to." << std::endl << std::endl <<
                    "  -n, --name\tSets your name, so that the program doesn't ask you when you join." << std::endl;
                return EXIT_SUCCESS;
            }
            if (i != argc-1) { // if not the last argument
                if (flag == "-h" || flag == "--host") {
                    ip = argv[i+1];
                    host_set = true;
                } else if (flag == "-p" || flag == "--port") {
                    port = atoi(argv[i+1]);
                } else if (flag == "-n" || flag == "--name") {
                    name = argv[i+1];
                    name_set = true;
                }
            }
        }
    }

    sf::TcpSocket socket;

    if (!host_set) {
        std::cout << "Type the ip of a server, or leave blank for localhost: ";
        std::getline(std::cin, ip);
        if (ip == "") {
            std::cout << bold_on << "** " << bold_off << 
                "No ip specified, connecting to localhost by default.\n";
            ip = "127.0.0.1";
        }
    }

    sf::Socket::Status status = socket.connect(ip, port);

    if (status != sf::Socket::Done) {
        std::cout << "Could not connect!" << std::endl;
        return EXIT_FAILURE;
    } else {
        std::cout << bold_on << "** " << bold_off << 
            "Connected!\n";
    }

    // expect an motd
    sf::Packet motd_packet;
    if (socket.receive(motd_packet) == sf::Socket::Done) {
        std::string type, motd, serv_info;
        motd_packet >> type >> motd >> serv_info;
        std::cout << "Server MOTD:\n\n";
        std::cout << motd << std::endl << std::endl;
        std::cout << serv_info << std::endl;
    }

    if (!name_set) {
        std::cout << "What's your name? ";
        std::getline(std::cin, name);
    }

    send_packet("NAME", name, "Set by user", &socket);

    std::thread display_thread(display_incoming, &running, &socket, &name);

    std::cout << "Now you can " << bold_on << "type messages " << bold_off << "to send them to everyone else on the server!" << std::endl <<
        std::string(10, '_') << std::endl << std::endl;

    while (true) {
        std::string out;

        std::getline(std::cin, out);
        if (out == "") {
            std::cout << "\033[F";
            continue;
        }

        send_packet("MSG", name, out, &socket);
    }

    running = false;
    display_thread.join();
}