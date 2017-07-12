#ifndef PARSE_PACKET_H
#define PARSE_PACKET_H

#include <pcap.h>

typedef struct  parse_filter_info_s{
	pcap_t *descr;
	struct bpf_program fp;
	bpf_u_int32 maskp;
	bpf_u_int32 netp;
	const char *filter_str;
}parse_filter_info_t;

parse_filter_info_t *p_filter_info ;
parse_filter_info_t *get_default_parse_filter_info(void);
void parse_filter_str_reg(parse_filter_info_t *info, const char *filter_str);
void parse_packet();

#endif