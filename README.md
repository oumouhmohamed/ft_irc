*This project has been created as part of the 42 curriculum by mooumouh, arahhab.*

# ft_irc

## Description

ft_irc is a 42 project consisting of implementing an IRC (Internet Relay Chat) server in C++98.

The server allows multiple clients to connect through TCP/IP, authenticate with a password, register with a nickname and username, join channels, and communicate with other users.

The server uses non-blocking I/O and a single `poll()` mechanism to handle multiple clients simultaneously without blocking the server.

## Features

### Client management

- `PASS` — authenticate with the server password.
- `NICK` — set or change a nickname.
- `USER` — register a username.
- Multiple simultaneous client connections.
- Client disconnection handling.

### Channel management

- `JOIN` — create or join a channel.
- `PART` — leave a channel.
- Channel members and operators management.

### Messaging

- `PRIVMSG` — send private messages to users.
- Send messages to all members of a channel.

### Operator commands

- `KICK` — remove a user from a channel.
- `INVITE` — invite a user to a channel.
- `TOPIC` — display or change a channel topic.
- `MODE` — manage channel modes and privileges.

### Supported channel modes

- `i` — invite-only channel.
- `t` — restrict topic changes to operators.
- `k` — channel password/key.
- `o` — give or remove operator privileges.
- `l` — set or remove the channel user limit.

## Requirements

- C++98
- Compilation with `-Wall -Wextra -Werror`
- TCP/IP communication
- Non-blocking I/O
- A single `poll()` for I/O multiplexing
- No `fork()`
- No external libraries

## Instructions

### Compilation

make

### Running the server

./ircserv <port> <password>

Example:

./ircserv 6667 password

### Testing with netcat

You can test the server using:

nc -C 127.0.0.1 6667

Then register the client:

PASS password
NICK atmane
USER atmane 0 * :Atmane

Example IRC commands:

JOIN #42
TOPIC #42 :Welcome to #42
PRIVMSG #42 :Hello everyone
PART #42

For private messages, connect another client and use:

PRIVMSG <nickname> :Hello

## Technical Overview

The server is based on a TCP socket architecture:

Client
   |
   | TCP
   v
Server Socket
   |
 accept()
   |
   v
Client Sockets
   |
 poll()
   |
   +---- POLLIN  -> recv()
   |
   +---- POLLOUT -> send()

Received data is stored in a client input buffer because TCP data can arrive in several parts. Complete IRC lines are then extracted, parsed, and processed as commands.

The server maintains clients and channels using C++ containers such as `std::map` and `std::vector`.

## Testing

Testing covers normal behavior and error cases, including:

- Wrong or missing passwords.
- Missing command parameters.
- Duplicate nicknames.
- Multiple clients.
- Joining and leaving channels.
- Invite-only channels.
- Password-protected channels.
- Channel user limits.
- Operator permissions.
- Private and channel messages.
- Client disconnections.
- Partial TCP messages.
- Invalid commands and modes.

## Resources

The main resources used for the project are:
-https://www.geeksforgeeks.org/cpp/socket-programming-in-cpp/
-https://www.youtube.com/watch?v=dEHZb9JsmOU&t=429s
-https://www.youtube.com/watch?v=Gg_WABtPKdc
- IRC protocol documentation and RFCs.
- TCP/IP and socket documentation.
- C++98 standard library documentation.

### AI Usage

AI tools were used as a learning and development aid to understand networking, TCP/IP, sockets, `poll()`, C++98 concepts, IRC commands, debugging, and test cases.

The generated information was reviewed and tested before being used in the project.

