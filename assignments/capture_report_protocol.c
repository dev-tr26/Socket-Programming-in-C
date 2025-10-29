// capture_report_proto.c
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

const char *ethertype_name(uint16_t eth) {
    switch (eth) {
        case 0x0800: return "IPv4";
        case 0x86DD: return "IPv6";
        case 0x0806: return "ARP";
        case 0x8847: return "MPLS";
        case 0x8100: return "VLAN";
        default: return "OTHER";
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <network_interface>\n", argv[0]);
        return 1;
    }
    const char *interface_name = argv[1];


    int raw_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    
    if (raw_sock < 0){ 
        perror("error creating socket"); 
        return 1; 
    }
    
    int interface_index = if_nametoindex(interface_name);
    if (!interface_index){ 
        perror("error getting interface index"); 
        close(raw_sock); 
        return 1; 
    }

    struct sockaddr_ll socket_address;
    memset(&socket_address, 0, sizeof(socket_address));
    socket_address.sll_family = AF_PACKET;
    socket_address.sll_protocol = htons(ETH_P_ALL);
    socket_address.sll_ifindex = interface_index;

    if (bind(sock, (struct sockaddr*)&socket_address, sizeof(socket_address)) < 0){ 
        perror("error binding sockets"); 
        close(raw_sock); 
        return 1; 
    }
    
    
    struct packet_mreq promiscuous_mode;
    memset(&promiscuous_mode, 0, sizeof(promiscuous_mode));
    promiscuous_mode.mr_interface_index = interface_index;
    promiscuous_mode.mr_type = PACKET_MR_PROMISC;
    
    if (setsockopt(raw_sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &promiscuous_mode, sizeof(promiscuous_mode)) < 0) {
        perror("error enabling promiscuous mode");
        close(raw_sock); 
        return 1;
    }
    printf("Listening on interface: %s (promiscuous mode enabled)\n", interface_name);
    printf("Reporting protocol (EtherType) for each packet\n\n");


    // caputring packets 
    unsigned char packet_buf[65536];
    for (;;) {
        struct sockaddr_ll packet_addr;
        socklen_t addrlen = sizeof(packet_addr);

        ssize_t bytes_received = recvfrom(
            raw_sock, 
            packet_buf, 
            sizeof(packet_buf), 
            0, 
            (struct sockaddr*)&packet_addr, &addrlen
        );
        
        if (bytes_received < 0) {
            if (errno == EINTR) continue;
            perror("error receiving packets"); 
            break;
        }
        
        // Extracting protocol type (ethertype)

        uint16_t ethertype = ntohs(packet_addr.sll_protocol);
        
        printf("Packet length: %zd bytes, Protocol: 0x%04x (%s), Interface index: %d\n",
            bytes_received, 
            ethertype, 
            get_ethertype_name(ethertype), 
            packet_addr.sll_ifindex);


        // print source and destination MAC addresses (first 14 bytes = Ethernet header)
        if (bytes_received >= 14) {
            printf("  destination MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
                   packet_buffer[0], packet_buffer[1], packet_buffer[2],
                   packet_buffer[3], packet_buffer[4], packet_buffer[5]);

            printf("  source MAC:      %02x:%02x:%02x:%02x:%02x:%02x\n",
                   packet_buffer[6], packet_buffer[7], packet_buffer[8],
                   packet_buffer[9], packet_buffer[10], packet_buffer[11]);
        }
    }

    promiscuous_mode.mr_type = PACKET_MR_PROMISC;
    setsockopt(raw_sock, SOL_PACKET, PACKET_DROP_MEMBERSHIP, &promiscuous_mode, sizeof(promiscuous_mode));
    close(raw_sock);
    return 0;
}
