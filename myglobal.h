#ifndef MY_GLOBAL_H
#define MY_GLOBAL_H
#include <stdint.h>

#define LOCAL static
#define RES_OK 0
#define RES_ERROR -1
#ifndef True
#define True 1
#define False 0
#endif

#ifndef u8
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#endif


#define NIPQUAD_FMT "%u.%u.%u.%u"
#define NIPQUAD(addr) \
((unsigned char *)&addr)[0],  ((unsigned char *)&addr)[1],  ((unsigned char *)&addr)[2],  ((unsigned char *)&addr)[3]

#define MAC_FMT "%02x%02x%02x%02x%02x%02x"
#define MACQUAD(addr) \
((unsigned char *)&addr)[0],  ((unsigned char *)&addr)[1],  ((unsigned char *)&addr)[2],  ((unsigned char *)&addr)[3], ((unsigned char *)&addr)[4], ((unsigned char *)&addr)[5]

#define SEND_BUF_LEN 65535
#define BUFFER_SIZE 65535
#define HOSTSIZE 1024
#define SNAP_LEN 1518

enum REG_RES{
	REGEX_MISS,
	REGEX_HIT
};
 
int *msg_fd;
int child_pid_num;

void print_packet(uint8_t *packet,
                  uint32_t start, uint32_t end, u_int wrap) ;

#endif
