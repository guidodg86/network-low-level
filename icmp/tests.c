// to compile gcc -g tests.c -o tests.out


#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include<signal.h>
#include<stdbool.h>
#include<sys/socket.h>
#include<sys/types.h>

#include<linux/if_packet.h>
#include<netinet/in.h>		 
#include<netinet/if_ether.h>    // for ethernet header
#include<netinet/ip.h>		// for ip header
#include<netinet/udp.h>		// for udp header
#include<netinet/tcp.h>
#include<arpa/inet.h>           // to avoid warning at inet_ntoa


int total,tcp,udp,icmp,igmp,other,iphdrlen;

struct sockaddr saddr;
struct sockaddr_in source,dest;


int main()
{

	int sock_r,saddr_len,buflen;
	unsigned char* buffer = (unsigned char *)malloc(65536); 
	memset(buffer,0,65536);
    struct ethhdr *eth = (struct ethhdr *)(buffer);


	printf("Listening to Mirkotik MAC address .... \n");

	sock_r=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL)); 
	if(sock_r<0)
	{
		printf("error in socket\n");
		return -1;
	}

	while(1)
	{
		saddr_len=sizeof saddr;
		buflen=recvfrom(sock_r,buffer,65536,0,&saddr,(socklen_t *)&saddr_len);
		if(buflen<0)
		{
			printf("error in reading recvfrom function\n");
			return -1;
		}

        // Microtik Mac address on ether4 -> 18:FD:74:C1:8E:4D
        if ( (eth->h_source[0]==0x18) && (eth->h_source[1]==0xFD) && (eth->h_source[2]==0x74) && (eth->h_source[3]==0xC1) && (eth->h_source[4]==0x8E) && (eth->h_source[5]==0x4D)) {
            printf("\t|-Source Address	: %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
	        printf("\t|-Destination Address	: %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
        }


	}

	close(sock_r);// use signals to close socket 
	printf("DONE!!!!\n");

}
