# t
## A very simple chat client and server

### Why's it called *t*?
Because *talk* begins with *t*.

### Features
 - Hosting a server on a port of your choice.
 - Server motd.
 - Easy to use.
 - Named clients.
 - As many clients as your computer can handle.
 - Can read the motd from a file.

### Requirements
 - CMake >= 3.4 (for building/installing)
 - SFML >= 2.0 (as a sockets library)
 - A Unix system (linux, mac,) or Windows with the WSL.

### Installation
Once you're sure your system meets all of the requirements, run the following commands:

```
git clone https://github.com/j4cobgarby/t-chat-server-client
cd t-chat-server-client
mkdir build && cd build
cmake ..
sudo make install

# Quick test

t --help
t-server --help

# Should print a help message
```

### Usage
#### Starting a server
```
t-server
```
And then follow the instructions, *or*
```
t-server -m <An motd string - whatever you want> -p <A valid port, if you don't want to use the default>
```
To set the motd and port in one go.
#### Joining a server
```
t
```
And then follow the instructions, *or*
```
t -h <Server ip> -p <Server port, if you don't want to use the default> -n <What you want to be called>
```