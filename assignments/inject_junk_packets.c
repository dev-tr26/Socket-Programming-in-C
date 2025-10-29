// construct ethernet frame and inject using PF_PACKET raw socket 

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <netinet/ether.h>


int get_mac_address(const char *interface_name, unsigned char *mac_buffer){
    int temp_socket = socket(AF_INET,SOCK_DGRAM,0);
    if(temp_socket <0)
        return -1;

    struct ifreq interface_request;
    memset(&interface_request,0,sizeof(interface_request));
    strncpy(interface_request.ifr_name, interface_name, IFNAMSIZ - 1);

    // MAC address
    if (ioctl(temp_socket, SIOCGIFHWADDR, &interface_request) == -1) {
        close(temp_socket);
        return -1;
    }

    memcpy(mac_buffer, interface_request.ifr_hwaddr.sa_data, 6);
    close(temp_socket);
    return 0;
}



int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <interface> <count> [destination_mac]\n", argv[0]);
        fprintf(stderr, "Example: %s eth0 10 11:22:33:44:55:66\n", argv[0]);
        return 1;
    }

    const char *interface_name = argv[1];
    int packet_count = atoi(argv[2]);
    const char *destination_mac_str = (argc >= 4)? argv[3]: "ff:ff:ff:ff:ff:ff"; // default: broadcast

    
    // the source MAC address for this interface
    
    unsigned char source_mac[6];
    if (get_mac_address(interface_name, source_mac) < 0) {
        perror("Error getting source MAC address");
        return 1;
    }

    
    // parsing destination MAC address from user input
    
    unsigned char destination_mac[6];
    if (sscanf(destination_mac_str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
               &destination_mac[0], &destination_mac[1], &destination_mac[2],
               &destination_mac[3], &destination_mac[4], &destination_mac[5]) != 6) {
        fprintf(stderr, "invalid destination MAC address format.\n");
        return 1;
    }


    
    int interface_index = if_nametoindex(interface_name);
    if (!interface_index) {
        perror("error getting interface index");
        return 1;
    }

    
    int raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw_socket < 0) {
        perror("error creating raw socket");
        return 1;
    }

    
    struct sockaddr_ll socket_address;
    memset(&socket_address, 0, sizeof(socket_address));
    socket_address.sll_family = AF_PACKET;
    socket_address.sll_ifindex = interface_index;
    socket_address.sll_halen = ETH_ALEN;
    memcpy(socket_address.sll_addr, destination_mac, 6);

    //building & sending ethernet frames 

    srand(time(NULL));    // random seed for junk data
    unsigned char ethernet_frame[1500];  // max ethernet frame size

    // ethernet header (14 bytes)
    memcpy(ethernet_frame, destination_mac, 6);
    memcpy(ethernet_frame + 6, source_mac, 6);

    // custom ethertype (0x88B5) if ipv4 0x0800
    uint16_t ethertype = htons(0x88B5);
    memcpy(ethernet_frame + 12, &ethertype, 2);

    for (int packet_num = 0; packet_num < packet_count; ++packet_num) {
    
        int payload_length = 46 + (rand() % 55);  // b/w 46 to 100 bytes


        for (int i = 0; i < payload_length; ++i)
            ethernet_frame[14 + i] = rand() & 0xFF;

        // packet sending 
        ssize_t bytes_sent = sendto(
            raw_socket,
            ethernet_frame,
            14 + payload_length,
            0,
            (struct sockaddr *)&socket_address,
            sizeof(socket_address)
        );

        if (bytes_sent == -1) {
            perror("Error sending packet");
            break;
        }

        printf("Sent frame %d (length: %zd bytes)\n", packet_num + 1, bytes_sent);
        usleep(100000);  // sleep 100ms between packets
    }

    close(raw_socket);
    return 0;
}