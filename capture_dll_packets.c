// socket() protocol filed-> ETH_P_ALL ethernet accepts all unicast and broadcast packets addressed to it 
// to get all packets that are not even intended to it but reached somehow to interface , the interface need to be set to promiscuius mode 
// ioctl() req options we get the informations of interface (mac add, broadcast add)
// but one needs to get packets from specified interface so 
// bind() ==> to attach packet socket with interface 


/*
struct sockaddr_ll socket_address;
socket_address.sll_family = PF_PACKET;
socket_address.sll_protocol = htons(ETH_P_ALL)
socket_address.sll_ifindex = req.ifr_ifindex;
b = bind(sockfd, (Struct sockaddr*)&socket_address, sizeof(socket_address))
*/


#include<sys/socket.h>
#include<netpacket/packet.h>
#include<net/ethernet.h>

int main(){
    int sockfd, len;
    char buffer[2048];
    struct sockaddr_ll pla;
    sockfd = socket(PF_PACKET,SOCK_RAW, htons(ETH_P_ALL))
    if(sockfd < 0){
        perror('packet_socket');
        exit(0);
    }
    printf("Types of packets captured are..\n");
    while(1){
        len = sizeof(struct_sockaddr_ll);
        recvfrom(sockfd,buffermsizeof(buffer),0,(struct sockaddr *)&pla, &len);
        switch(pla.sll_pkttype){
            // constant values taken from linux/if_packet.h
            case 0: //PACKET_HOST
                printf("For_Me\n");
                break;
            case 1: //PACKET_BROADCAST
                printf("Broadcast\n");
                break;
            case 2: //PACKET_MUILTICAST
                printf("Muilticast\n");
                break;
            case 3://PACKET_OTHERHOST
                printf("Other_Host\n");
                break;
            case 4://PACKET_OUTGOING
                printf("Outgoing\n");
                break;
            case 5://PACKET_LOOPBACK
                printf("Loop_Back\n");
                break;
            case 6://PACKET_FASTROUTE
                printf("Fast_Route\n");
                break;
        }
    }
}