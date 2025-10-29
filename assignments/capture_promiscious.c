#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <netinet/ether.h>
#include <sys/ioctl.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <network_interface>\n", argv[0]);
        return 1;
    }
    const char *interface_name = argv[1];

    // 1. creating raw socket to capture all ethernet packets

    int raw_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw_sock < 0){ 
        perror("socket"); 
        return 1; 
    }

    //2. get interface index
    int interface_index = if_nametoindex(interface_name);
    if (!interface_index){ 
        perror("Error creating raw socket"); 
        close(sock); 
        return 1; 
    }

    //3. bind socket to interface 

    struct sockaddr_ll socket_address;
    memset(&socket_address, 0, sizeof(socket_address));
    socket_address.sll_family = AF_PACKET;
    socket_address.sll_protocol = htons(ETH_P_ALL);
    socket_address.sll_ifindex = ifindex;
    
    if(bind(raw_sock,(struct sockaddr*)&socket_address, sizeof(socket_address)) < 0){ 
        perror("error in binding socket to interface");
        close(raw_sock); 
        return 1; 
    }

    // 4 .enable promiscuous mode via PACKET_ADD_MEMBERSHIP
    struct packet_mreq promescuous_mode;
    memset(&promescuous_mode, 0, sizeof(promescuous_mode));
    promescuous_mode.mr_ifindex = interface_index;
    promescuous_mode.mr_type = PACKET_MR_PROMISC;
    
    if(setsockopt(raw_sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &promescuous_mode, sizeof(promescuous_mode))<0){
        perror("error enabling promiscous mode");
        close(raw_sock);
        return 1;
    }

    printf("Listening on interface: %s (promiscuous mode enabled)\n", interface_name);
    printf("Press Ctrl+C to stop...\n");

     // 5. reading packets in an infinite loop
    unsigned char packet_buffer[65536];
    while (1) {
        ssize_t packet_length = recvfrom(raw_socket, packet_buffer, sizeof(packet_buffer), 0, NULL, NULL);
        if (packet_length < 0) {
            if (errno == EINTR) continue;  
            perror("Error receiving packet");
            break;
        }

        printf("Received packet of length: %zd bytes\n", packet_length);

        // Print the first 32 bytes in hexa format
        printf("First 32 bytes: ");
        for (ssize_t i = 0; i < packet_length && i < 32; ++i) {
            printf("%02x ", packet_buffer[i]);
        }
        printf("\n");
    }

    promescuous_mode.mr_type = PACKET_MR_PROMISC;
    setsockopt(raw_sock, SOL_PACKET, PACKET_DROP_MEMBERSHIP, &promescuous_mode, sizeof(promescuous_mode));
    close(raw_sock);
    return 0;
}
