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
AF_INET  // Ipv4 add family


// socket_types 

- SOCK_RAW (for raw packets Receive full Ethernet frames (including headers).) 
  packets are passed to and from the device driver without any changes in the packet data

  - SOCK_DGRAM (for only payloads no ethernet header )

// protocol 

htons() or htonl() => used to convert host byte to network byte order 
htons(ETH_P_ALL) to receive packets of all protocols to which Ethernet forwards data
htons(INADDR_ANY) bind to all available network interfaces (localhost, eth0)

// to reveive packets

int revecfrom(int packet_socket, void*buf, unsigned int len , int flags, struct sockaddr*from, unsigned int *fromlen);

/*
- packet recieved is stored in buf of size len 
- If from is not NULL , source add is in proper format f rom is always typecast to the generic socket structure struct sockaddr
-  fromlen : initialized to size of buffer associated with it 
- modified on return to indicate the actual size of the address stored there
- When receiving a packet from packet socket the address is passed in a standard sockaddr ll address structure

*/
struct socckaddr_ll{
    unsigned short sll_family;               // always AF_PACKET
    unsigned short sll_protocol;             // protocol
    int sll_ifindex;                         //Interface number 
    unsigned short sll_hatype;               // Header type
    unsigned char sll_pkttype;               //Packet type
    unsigned char sll_halen;                 // Length of add
    unsigned char sll_addr[8];               //Physical layer add
};

// it is device independent add structure 