// ARP sender - responder

#include <unistd.h>
#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include<signal.h>
#include<stdbool.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<linux/if_packet.h>
#include<netinet/in.h>		 
#include<netinet/if_ether.h>    
#include<netinet/ip.h>		
#include<netinet/udp.h>		
#include<netinet/tcp.h>
#include<arpa/inet.h>      

#define ETH_HEADER_SIZE 14



struct sockaddr saddr;


struct L2frame {   
  unsigned long source;           
  unsigned long destination; 
  unsigned short ether_type;
  unsigned char * p_payload;
  unsigned short payload_len;      
}; 

int main()
{

	int sock_r,saddr_len,buflen;
	unsigned char* buffer = (unsigned char *)malloc(65536); 
	memset(buffer,0,65536);
    struct ethhdr *eth = (struct ethhdr *)(buffer);
    struct L2frame mikrotik_frame;
    int i;


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
            mikrotik_frame.source = (unsigned long) (eth->h_source[0]) | (unsigned long) (eth->h_source[1]) << 8;
            mikrotik_frame.source = 0;
            for (i=0; i<6; i++){
                mikrotik_frame.source = mikrotik_frame.source | ((unsigned long) (eth->h_source[5-i])) << (8*i) ;

            }

            mikrotik_frame.destination = (unsigned long) (eth->h_dest[0]) | (unsigned long) (eth->h_dest[1]) << 8;
            mikrotik_frame.destination = 0;
            for (i=0; i<6; i++){
                mikrotik_frame.destination = mikrotik_frame.destination | ((unsigned long) (eth->h_dest[5-i])) << (8*i) ;

            }

            mikrotik_frame.ether_type = (unsigned short) ntohs(eth->h_proto);

            mikrotik_frame.payload_len = (unsigned short) buflen;
            mikrotik_frame.p_payload = (unsigned char *) (buffer + ETH_HEADER_SIZE);

            printf("Source MAC: 0x%.12lX\n", mikrotik_frame.source);
            printf("Destination MAC: 0x%.12lX\n", mikrotik_frame.destination);
            printf("Ethertype: 0x%.4X\n", mikrotik_frame.ether_type);
            if (mikrotik_frame.ether_type == 0x0806 ) {
                printf("Ethertype is ARP\n");
            }

            printf("\n");
 
		}


	}

	close(sock_r);


}
