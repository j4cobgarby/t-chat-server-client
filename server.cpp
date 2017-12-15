#include <SFML/Network.hpp>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <fstream>

#include "constants.hpp"

int main(int argc, char* argv[]) {
    std::string motd;
    int port = NET_PORT_DEFAULT;
    bool motd_set = false;
    if (argc > 1) { // some command line arguments, but not --help
        for (int i = 0; i < argc; i++) {
            std::string flag(argv[i]);
            if (flag == "-?" || flag == "--help") {
                std::cout << 
                    "Hosts a server for the simple chat server/client protocol." << std::endl << std::endl <<
                    "  -?, --help\tShows this help." << std::endl << std::endl <<
                    "  -m, --motd\tFollow this flag by the desired motd." << std::endl <<
                    "\t\tAlternatively, you can create a file at the path /etc/tserver/motd.txt." << std::endl << std::endl <<
                    "  -p, --port\tSets the port for the server to run on." << std::endl;
                return EXIT_SUCCESS;
            }
            if (i != argc-1) { // if not the last argument
                if (flag == "-p" || flag == "--port") {
                    port = atoi(argv[i+1]);
                } else if (flag == "-m" || flag == "--motd") {
                    motd = argv[i+1];
                    motd_set = true;
                }
            }
        }
    }

    if (!motd_set) {
        if (file_exists("/etc/tserver/motd.txt")) {
            std::ifstream motdfile("/etc/tserver/motd.txt");
            motd = std::string((std::istreambuf_iterator<char>(motdfile)), std::istreambuf_iterator<char>());
        } else {
            std::cout << bold_on << "Type an MOTD" << bold_off << " (message of the day,) to be displayed to any visitors: ";
            std::getline(std::cin, motd);
        }
    }

    sf::TcpListener listener;
    listener.listen(port);

    std::cout << "Listening for clients...\n";

    std::list<sf::TcpSocket*> clients;
    std::map<sf::TcpSocket*, std::string> client2name;
    sf::SocketSelector selector;

    selector.add(listener);

    while (true) {
        if (selector.wait()) {

            // if the listener is ready then a client's trying to connect
            if (selector.isReady(listener)) {
                sf::TcpSocket* new_client = new sf::TcpSocket;
                if (listener.accept(*new_client) == sf::Socket::Done) {
                    clients.push_back(new_client);
                    client2name[new_client] = new_client->getRemoteAddress().toString();
                    selector.add(*new_client);
                    std::cout << bold_on << "** " << bold_off << new_client->getRemoteAddress().toString() << " connected." << std::endl;
                    std::cout << "There are now " << clients.size() << " client(s) connected." << std::endl;

                    std::string server_info;
                    server_info += std::to_string(clients.size()-1) + " user(s) connected, excluding yourself:\n";

                    send_packet("MOTD", motd, server_info, new_client);
                } else {
                    std::cout << "A client tried to join but didn't quite manage.\n";
                    delete new_client;
                }
            } else {
                for (std::list<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); ++it) {
                    sf::TcpSocket& client = **it;
                    if (selector.isReady(client)) {
                        // this client sent data, so receive it
                        sf::Packet packet;
                        switch (client.receive(packet)) {
                        case sf::Socket::Done:
                            {
                                std::string type, name, body;
                                packet >> type >> name >> body;

                                if (type == "MSG") {
                                    client2name[&client] = name;
                                    std::cout << bold_on << name << ": " << bold_off << body << std::endl;

                                    for (std::list<sf::TcpSocket*>::iterator it2 = clients.begin(); it2 != clients.end(); ++it2) {
                                        sf::TcpSocket& client2send = **it2;
                                        client2send.send(packet);
                                    }
                                } else if (type == "NAME") {
                                    std::cout << client2name.at(&client) << " (" << client.getRemoteAddress() << ") changed their name to " << name << std::endl;
                                    client2name[&client] = name;
                                }
                            }
                            break;
                        case sf::Socket::Disconnected:
                            {
                                std::string displayname = "test";
                                displayname = client2name.at(&client);
            
                                std::cout << bold_on << "** " << bold_off << 
                                    displayname << " disconnected.\n";
                                selector.remove(client);
                                client.disconnect();
                                delete(&client);
                                clients.erase(it);
                                client2name.erase(&client);
                                it--;

                                for (std::list<sf::TcpSocket*>::iterator it2 = clients.begin(); it2 != clients.end(); ++it2) {
                                    sf::TcpSocket& client2send = **it2;
                                    send_packet("DISCON", displayname, "Left the server", &client2send);
                                }
                            }
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }
    }
}