#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <netinet/if_ether.h>
#include <netinet/tcp.h>        //Provides declarations for tcp header 
#include <netinet/ip.h>     //Provides declarations for ip header 
#include "myglobal.h"
#include "mylogs.h"
#include "my_queue.h"
#include "dpi_modules.h"

void test_out_http_packet(char *packet, int len, void *userdata)
{
	return;
	const struct ethhdr *ethernet;
	const struct iphdr *iph;
	const struct tcphdr *tcph; 
	int size_ethernet = sizeof(struct ethhdr);          
	int i;

	ethernet = (struct ethhdr*)(packet);
	iph = ( struct iphdr * ) ( packet   + size_ethernet) ; 
	tcph = ( struct tcphdr * ) ( packet +  iph -> ihl*4 + size_ethernet) ; 
	
	log_output("start out Payload:\n\r");
	/*
	for (i = 0; i < len; i++) { 
		if(i%16 == 0)
			printf("\n\r");                  
		if(packet[i] == 0)
			printf("00  ");
		else                     
			printf("%02x  ", packet[i]);                    
	}    
	     
	printf("\n\r"); */  
	log_output("Received Packet Size: %d\n\r", len);  
	log_output("eth src addr: 0x"MAC_FMT"\n\r", MACQUAD(ethernet->h_source));
	log_output("eth dst addr: 0x"MAC_FMT"\n\r", MACQUAD(ethernet->h_dest));
	log_output("des ip " NIPQUAD_FMT"\n\r", NIPQUAD(iph->daddr));
	log_output("src ip " NIPQUAD_FMT"\n\r", NIPQUAD(iph->saddr));  
	log_output("ip len %d \n\r", ntohs(iph->ihl));
	log_output("src port:%d \n\r", ntohs(tcph->source));
	log_output("dst port:%d \n\r", ntohs(tcph->dest));
	log_output("seq %u, ack:%u, flags:0x%02x\n\r", ntohl(tcph->seq), ntohl(tcph->ack_seq), (int)*(char *)(tcph+4+offsetof( struct tcphdr, ack_seq)));
	log_output("http payload: ");    
	for(i = (size_ethernet + iph->ihl*4 + tcph->doff*4); i < len; i++){
	    if (isprint(packet[i]))
	        printf("%c", packet[i]);
	    else
	        printf("%2x", packet[i]);           
	} 
	printf("\n\r");  
	
}

dpi_module_t test_dpi_module1 = {
 "test_dpi",
 NULL,
 NULL,
 test_out_http_packet,
 NULL,
};


