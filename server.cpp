#include <SFML/Network.hpp>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <fstream>

#include "constants.hpp"

int main() {
    std::cout << bold_on << "Type an MOTD" << bold_off << " (message of the day,) to be displayed to any visitors: ";
    std::string motd;
    std::getline(std::cin, motd);

    sf::TcpListener listener;
    listener.listen(NET_PORT);

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
                    selector.add(*new_client);
                    std::cout << bold_on << "** " << bold_off << new_client->getRemoteAddress().toString() << " connected." << std::endl;
                    std::cout << "There are now " << clients.size() << " client(s) connected." << std::endl;

                    std::string server_info;
                    server_info += std::to_string(clients.size()-1) + " user(s) connected, excluding yourself:\n";

                    sf::Packet motd_packet;
                    motd_packet << motd << server_info;
                    new_client->send(motd_packet);
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
                                std::string name, body;
                                std::string type = "MSG";
                                packet >> type >> name >> body;

                                client2name[&client] = name;
                                std::cout << bold_on << name << ": " << bold_off << body << std::endl;

                                for (std::list<sf::TcpSocket*>::iterator it2 = clients.begin(); it2 != clients.end(); ++it2) {
                                    sf::TcpSocket& client2send = **it2;
                                    client2send.send(packet);
                                }
                            }
                            break;
                        case sf::Socket::Disconnected:
                            {
                                std::string displayname = client2name.at(&client);
                                std::cout << bold_on << "** " << bold_off << 
                                    client.getRemoteAddress().toString() << " disconnected.\n";
                                selector.remove(client);
                                client.disconnect();
                                delete(&client);
                                clients.erase(it);
                                client2name.erase(&client);
                                it--;
                                std::cout << bold_on << "** " << bold_off << "Removed that client.\n";

                                sf::Packet discon_packet;
                                discon_packet << std::string("DISCON") << displayname << std::string("Left the server.");

                                for (std::list<sf::TcpSocket*>::iterator it2 = clients.begin(); it2 != clients.end(); ++it2) {
                                    sf::TcpSocket& client2send = **it2;
                                    client2send.send(discon_packet);
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