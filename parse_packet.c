#include <stdlib.h> 
#include <time.h> 
#include <netinet/if_ether.h>
#include <netinet/udp.h>        //Provides declarations for udp header 
#include <netinet/ip.h>     //Provides declarations for ip header 
#include <assert.h>
#include <unistd.h>
#include "mylogs.h"
#include "myglobal.h"
#include "parse_packet.h"
#include "msg_dispatch.h"
#include "filter_str.h"
parse_filter_info_t *get_default_parse_filter_info(void)
{
	if ( !p_filter_info )
		p_filter_info = (parse_filter_info_t *)malloc(sizeof(parse_filter_info_t));
	return p_filter_info; 
}

void parse_filter_str_reg(parse_filter_info_t *info, const char *filter_str)
{
	assert(info);
	assert(filter_str);
	info->filter_str = filter_str;
}

void my_pcap_callback(u_char *user_data, const struct pcap_pkthdr* pkthdr, const u_char* packet)
{
	static int count = 0;
	int radom_process = -1;
	int send_fd = -1;
	
	assert(packet);
	count++;
	log_output("pcap callpack, packet captured %d\n\r", count);	
	radom_process = (rand() % (child_pid_num)); 	
	send_fd = msg_fd[radom_process*2];
	log_output("send packet to process %d\n\r", radom_process);
	send_msg(send_fd, (char *)packet, pkthdr->len);		
}

void parse_packet()
{
	char *dev;
	char errbuf[PCAP_ERRBUF_SIZE];
	if ( !p_filter_info )
		p_filter_info = (parse_filter_info_t *)malloc(sizeof(parse_filter_info_t));
	assert(p_filter_info);
	/*
	pcap_t* descr;
	struct bpf_program fp;     
	bpf_u_int32 maskp;         
	bpf_u_int32 netp;          
	const char *filter_str = "udp dst port 53";
	*/
	int res = 0;
	parse_filter_str_reg(get_default_parse_filter_info(), FILTER_STR);		
	sleep(3);
	dev = pcap_lookupdev(errbuf);
	srand((unsigned)time(NULL)); 
	if(dev == NULL){ 
		stop_all_childp();
		log_output("%s\n", errbuf);
		exit(-1);
	}
	log_output("dev = %s\n\r", dev);    
	pcap_lookupnet(dev, &p_filter_info->netp, &p_filter_info->maskp, errbuf);   
	p_filter_info->descr = pcap_open_live(dev, SNAP_LEN, 1, 0, errbuf);
	if(p_filter_info->descr == NULL){ 
		stop_all_childp();
		log_output("pcap_open_live(): %s\n", errbuf);
		exit(-1);		
	}
	      
	log_output("pcap loop start, filter str is %s\n\r", p_filter_info->filter_str);
	if(pcap_compile(p_filter_info->descr, &p_filter_info->fp, p_filter_info->filter_str, 0, p_filter_info->netp) == -1){ 
		pcap_perror(p_filter_info->descr, "Error calling pcap_compile");
		pcap_close(p_filter_info->descr);
		exit(-1);
	}
	log_output("start to install filter str: %s\n\r", p_filter_info->filter_str);
	if(pcap_setfilter(p_filter_info->descr, &p_filter_info->fp) == -1){
		pcap_close(p_filter_info->descr);
		stop_all_childp();
		log_output("Error setting filter\n");
		exit(-1);				 
	} 
	log_output("start pcap_loop\n\r"); 
	res = pcap_loop(p_filter_info->descr, 0, my_pcap_callback, NULL);    

	if (res<0){
		pcap_perror(p_filter_info->descr, "pcap_loop");      
	}
	pcap_close(p_filter_info->descr); 
	stop_all_childp();
}
