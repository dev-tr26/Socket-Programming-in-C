# Socket-Programming-in-C

1. **Socket (socket function call)**

* creating a socket descriptor
* to perform network i/o 1st thing a process must do is call socket function to create specific type of socket by specifying type of communication, protocol, protocol family
* type can be streaming packets, raw datagram packets, stream circuits
* protocol tcp/ udp
* phone is needed to communicate analogy

---

2. **Bind (bind function call)**

* binding the socket

* registering phone number

* sockets do not have complete address in beginning to start transferring data so we bind a socket to a port

* process of allocating a port number to socket ==> BINDING

* socket is combination of ip addr finds device and port identifies services /process running on pc

* sys call binds circuit to an addr of host and port num on which server will run

---

3. **Listen (listen function call)**

* identifying a bound , unconnected socket == means identifying a connection request
* process of converting an unconnected socket into a passive socket indicating kernel that should accept incoming connection requests directing to this socket

---

4. **Accept (accept function call)**

* accepting a connection request
* transition from request listen() method to an actual socket means -> "accept connection request"
* after accepting the request (and automatic creation of socket) the data can finally be transferred between nodes

---

5. **Connect (requesting to connect)**

* dialing specific phone num
* to make connection req specify address to connect to and the socket descriptor to attach the socket to

---

## **Common Function Declarations**

```c
#include <sys/socket.h>
#include <bits/stdc++.h>
int socket(int domain, int type, int protocol);

int bind(int sockfd, struct sockaddr *myaddr, socketlen_t addrlen);

int listen(int sockfd, int backlog);

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int connect(int sockfd, struct sockaddr * serv_addr, socklen_t addrlen);
```

---

## **Compile and Test for Linux-based**

**Compile:**

```bash
gcc hello_server.c -o hserver
gcc hello_client.c -o hclient
```

**Run:**

```bash
./hserver 9190
./hclient 127.0.0.1 9190
```

* port num == 9190
* 127.0.0.1 is port num of same pc only coz same device running
