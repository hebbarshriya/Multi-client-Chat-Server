### Computer Networking Project

## MULTI-CLIENT CHAT SERVER

#### Introduction
In today's interconnected world, efficient and secure communication tools are paramount. This project, a Multi-Client Chat Server with File Transfer capabilities, addresses this need by offering a robust and versatile solution. The application is designed and implemented using the C programming language, leveraging its performance and versatility to create a comprehensive chat server that caters to modern communication demands.

#### Problem Statement
The primary objective of this project is to develop a Multi-Client Chat Server that enables the exchange of messages and files between clients logged into the same network. Each registered user can log in, share text messages, and transfer files with other users available in the chat system.

#### Motivation
The motivation behind this project stems from the necessity for reliable and fast intra-network communication tools. Whether for business collaboration or social interaction, a secure and efficient communication platform is essential. This project aims to create an all-in-one solution that ensures reliable, fast, and secure communications within an organization.

#### Project Overview
The Multi-Client Chat Server facilitates real-time communication between multiple clients over a network. Key features of this project include:

1. Multi-Client Support: The server allows multiple clients to connect simultaneously, enabling numerous users to communicate concurrently.
2. Real-Time Chat: Users can send text messages to each other in real-time, providing a seamless and responsive chatting experience.
3. Document and File Transfer: Users can transfer various types of files, including audio and video files, between clients.
4. Broadcast Functionality: The server includes a broadcast function to relay messages to all connected clients simultaneously.

#### Technical Details
- Socket Programming: Utilizes socket programming to establish connections between the server and clients.
- Multi-threading: Implements multi-threading to handle multiple clients concurrently.
- Authentication: Ensures secure communication through user authentication processes.

#### Prerequisites
To run this project, you will need:
- A C compiler (e.g., GCC)
- Basic understanding of socket programming and multi-threading
- A network environment where multiple clients can connect to a single server

#### Methodology
The application leverages socket programming to establish network connections and multi-threading to handle multiple clients. The server creates a primary socket to await client connections and, upon connecting, spawns a new thread for each client to handle their requests independently. This design ensures concurrent and synchronous operations among multiple clients.

#### Implementation
###### Registration and Login
Users must register and log in to use the chat server. This process ensures that only authenticated users can access the communication platform.
###### Socket Creation
Sockets are created on both the server and client sides to facilitate communication. The server listens for incoming connections while clients initiate connections to the server.
###### Client Operation
Clients can send and receive messages in real-time. They can also transfer files to other clients through the server, which manages the delivery.
###### Client Handling
The server handles each client in a dedicated thread, ensuring that messages and files are correctly relayed between clients.
###### Broadcasting
The server can broadcast messages to all connected clients, which is useful for announcements or notifications.

#### Discussion
During the development of this project, several challenges were encountered, such as handling large messages and ensuring correct message broadcasting. These issues were resolved by refining the message handling processes and verifying client connections before forwarding messages.

#### Conclusion
The Multi-Client Chat Server project successfully establishes a reliable and secure communication platform for intra-network interactions. It supports multiple clients, real-time messaging, and file transfers, making it a versatile tool for organizational communication needs.

#### Future Work
Future enhancements could include:
- Advanced Authentication: Implementing multi-factor authentication (MFA) for added security.
- Graphical User Interface (GUI): Developing a dedicated application with an interactive user interface to enhance accessibility and user-friendliness.
- Enhanced Features: Adding more functionalities such as encrypted communication and support for more file types.

#### References
1. Kurose, J. F., &Kurose K. W. (2012). Computer Networking: A Top-Down Approach (6th ed.). Pearson.
2. Silberschatz, A., Galvin, P. B., & Gagne, G. (2018). Operating System Concepts (9th ed.). Wiley.
