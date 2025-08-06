# CLI chat room

Host a server which connects clients together in a single chat room. Clients chat using a command line based textbox.

The networking logic is written using the Berkeley sockets API. I've implemented a simple application layer protocol on top of TCP that transmits messages and associated information. This is discussed below.

## Building

This project uses the POSIX API. Thus it can only be built on Linux.

No external dependencies are required to build this project.

This project uses Premake5 as its build system. To build using Make, run the following in the root directory of the project :  
`premake5 gmake`  
This will generate the required Makefiles. Then :  
`make config=[release/debug]`  
Alternatively, you may generate any build system files of your choice.

## Usage

The build produces a client app (in client/bin/) and a server app (in server/bin/). It also produces the static library util (in util/lib/) which is linked into the client and server binaries.  

The server can be hosted by the following command :  
`[server_binary_name] [port]`  
Once the server is up, clients can connect to it by running :  
`[client_binary_name] [server_ip] [server_port] [username]`

Clients connected to the same server are in a chat room and can chat with each other. They can send `!exit` to disconnect from the chat.

## How it works

The basic idea is that the server stores all the messages it receives from different clients, and then broadcasts each one to each client. The client doesn't distinguish between its own messages and other messages. It simply sends message+data to the server and prints out whatever the server sends.

### Server-client connection protocol

Immediately after a TCP connection is established, the client sends the length of their username followed by their username for the server to store.

### Client messenging protocol

Each time a user sends a message (via the client), the client sends first the length of the message, and then the message itself. The server stores this along with the username of the client.

### Server broadcast protocol

To each client, the server first sends the number of messages it is going to broadcast. Then, for each message, it broadcasts the length of the username, length of the message, the username, and the message, in that order.