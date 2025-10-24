// To receive packets from the datalink layer in Linux, a socket of type SOCK RAW is created.
// packet socket method used for linux systems to access Data link layer
// packet sockets used to send/receive raw packets at device driver (dll)
// They allow the user to implement protocol modules in user space on top of the physical layer.

#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/Ethernet.h>            // for DLL protocols

// socket descriptor
packet_socket = socket(PF_PACKET, int socket_type, int protocol)

// socket family - send/receive link-layer frames 
PF_PACKET or AF_PACKET 


// socket_types 

- SOCK_RAW (for raw packets Receive full Ethernet frames (including headers).) 
  packets are passed to and from the device driver without any changes in the packet data

  - SOCK_DGRAM (for only payloads no ethernet header )

// protocol 

htons() or htonl() => used to convert host byte to network byte order 

htons(ETH_P_ALL) to receive packets of all protocols to which Ethernet forwards data

// to reveive packets

int revecfrom(int packet_socket, void*buf, unsigned int len , int flags, struct sockaddr*from, unsigned int *fromlen);