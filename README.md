# Arduino UDP tuple space communication

This project implements a distributed communication system based on the **tuple space paradigm**, enabling efficient task distribution and result collection in a network of devices. The system leverages **UDP (User Datagram Protocol)** for lightweight and fast data exchange, making it ideal for real-time or resource-constrained environments. 

The core idea is to utilize a shared tuple space to decouple processes, allowing managers and workers to interact asynchronously. Managers can enqueue tasks into the tuple space, and workers can retrieve these tasks to perform computations and return results. This abstraction simplifies coordination in distributed systems by providing a unified data exchange mechanism.

The project is built using **C++** and **C**, with additional integration for Arduino devices to represent the manager and worker roles. The server application handles tuple space management and communication, ensuring tasks and results are properly routed between the entities.

By combining the tuple space concept with UDP communication, this project achieves a highly extensible, scalable, and efficient framework for distributed task management.


## Table of Contents

- [Getting Started](#getting-started)
- [Prerequisites](#prerequisites)
- [Installation and usage](#installation-and-usage)
- [Features](#features)
- [Technologies Used](#technologies-used)

## Getting Started

These instructions will help you set up the project locally and get it running in your environment

### Prerequisites

- **Arduino IDE**: for compiling and uploading `.ino` files to the manager and worker devices.
- **Ethernet Hardware**: Configured with Arduino-compatible Ethernet shield or similar for network connectivity.
- **Unix environemnt with C++ Compiler and ethernet libraries**: for compiling and running server code on your machine

### Installation and usage

1. **Setup server:**:
    ```bash
    # in server directory
    # Compile C++ server files
    gcc -o serverapp serverapp1.c tuple_space_server.cpp udp_manager_server.cpp

    # Start server
    ./serverapp
    ```

2. **Setup arduino devices**:
- Open `manager.ino` and `worker.ino` in Arduino IDE.
- Ensure proper Ethernet configurations (MAC and IP addresses) as per your network.
- Upload the files to the corresponding Arduino devices (manager and worker) and run it.

Now you should be able to monitor communication process via clients (arduinos) and server terminals. **Ensure that the devices are connected to the same network for successful communication.**


## Features
- **Tuple Serialization and Deserialization**: Packs and unpacks tuple data to and from UDP packets for transmission.
- **Tuple Space Management**: Utilizes tuple space as an abstraction for data exchange in a distributed system. It allows for storing and retrieving tuples independently of process locations, enabling seamless collaboration between different components.
- **Asynchronous Communication**: Tuple space facilitates asynchronous sending and receiving of data, reducing the need for direct connections between processes.
- **Flexibility**: Any process can add, remove, and retrieve tuples, making the system easily extendable.
- **Decoupled Processes**: Communication through tuples eliminates the need for time synchronization between processes, promoting independence and robustness in distributed systems.
- **UDP Communication**: Sends and receives packets using UDP, facilitating lightweight, connectionless communication.

## Technologies Used

- **C++ and C**: Core programming languages used for implementing tuple space operations, UDP communication, and server-side logic.
- **Arduino**: Used for managing the manager and worker nodes, including network communication via Ethernet.
- **Ethernet (Arduino Ethernet Library)**: Facilitates UDP communication over an Ethernet connection for Arduino-based devices.
- **UDP Protocol**: Lightweight, connectionless protocol used for fast data transmission between distributed nodes.
- **GCC/G++**: Compiler used for building the server and tuple space logic.
- **POSIX Sockets**: Used in the server application for managing UDP sockets and handling communication between clients and the server.

