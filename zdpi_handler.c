#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <strings.h> 
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/udp.h>        //Provides declarations for udp header 
#include <netinet/ip.h>     //Provides declarations for ip header
#include <netdb.h>            // struct addrinfo  
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t, uint32_t  
#include <sys/ioctl.h>        // macro ioctl is defined  
#include <bits/ioctls.h>      // defines values for argument "request" of ioctl.  
#include <errno.h>  
#include "mylogs.h"
#include "myglobal.h"
#include "zdpi_handler.h"
#include "myhash.h"
#include "msg_dispatch.h"
#include "dpi_modules_list.h"

void zdpi_loop(char *check_host_filename, int fd_num, int close_fd)
{
	log_output("start child process %d pid=%d\n\r", fd_num, (int)getpid());
	char *recv_buf = (char *)malloc(BUFFER_SIZE);
	int recv_len = 0;
	log_output("child process %d pid=%d close msgfd %d, fd =%d\n\r", fd_num, (int)getpid(), 2*fd_num, close_fd);
	close(close_fd);	
	int i;
	int dpi_module_list_len = sizeof(dpi_module_list)/sizeof(dpi_module_t*);
	
	log_output("moduel list len = %d\n\r", dpi_module_list_len);
	for (i = 0; i<dpi_module_list_len; i++) {
		log_output("moduel %s init func \n\r", dpi_module_list[i]->dpi_name);		
		if(dpi_module_list[i]->init_func){			
			dpi_module_list[i]->init_func(dpi_module_list[i]->init_userdata);
		}		
 	}
 	 	
	log_output("child process %d init end\n\r", fd_num);
	while(True){
		memset(recv_buf, 0, BUFFER_SIZE);
		recv_len = recv_msg(msg_fd[fd_num*2+1], recv_buf);
		if (recv_len <= 0)
			continue;
		log_output("recv msg from %d\n\r", msg_fd[fd_num*2+1]);
		if (strncmp(recv_buf, "stop", 4) == 0){
			log_output("recv exit\n\r");
			sleep(3);
			exit(0);
		}
		for (i = 0; i<dpi_module_list_len; i++) {
			if(dpi_module_list[i]->helper_func);
				dpi_module_list[i]->helper_func(recv_buf, recv_len, dpi_module_list[i]->helper_userdata);
	 	}
	 	
	}
	
	if(recv_buf)
		free(recv_buf);
}


