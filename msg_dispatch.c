#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "mylogs.h"
#include "myglobal.h"
#include "zdpi_handler.h"
#include "msg_dispatch.h"

int send_msg(int fd, char* msg, int m_len)
{
	assert(msg !=NULL);
	assert(m_len >0);
	int write_len = 0;
	log_output("send msg by fd %d len :%d\n\r", fd, m_len);
	log_output("send msg :\n\r");	
	char * tmp_buf = malloc(m_len+1);
	bzero(tmp_buf, m_len+1);
	memcpy(tmp_buf, msg, m_len);
	print_packet((uint8_t *) tmp_buf,0 , m_len , 16);
	while(True){
		log_output("start send fd = %d, pid = %d\n\r", fd, (int)getpid());
		write_len = write(fd, tmp_buf, m_len);
		if(write_len <0){			
			if(errno == EAGAIN || errno ==EWOULDBLOCK){
				free(tmp_buf);
				return write_len;
			}
			if (errno == EINTR)
	      			continue;
			else{
				sleep(1);
				handle_error_exit("write");
			}
		}
		else break;
	}
	log_output("sended len = %d\n\r", write_len);
	free(tmp_buf);
	return write_len;
}

int recv_msg(int fd, char *read_buf)
{
	assert(read_buf);
	int read_len = 0;
	//log_output("start recv\n\r");
	while(True){
		memset(read_buf, '0', BUFFER_SIZE);
		read_len = read(fd, read_buf, BUFFER_SIZE);
		//log_output("recv fd %d len = %d\n\r", fd , read_len);
		if (read_len < 0 ){
			//perror("read looop\n\r");
			if(errno == EAGAIN || errno == EWOULDBLOCK){
				//log_output("read looop EWOULDBLOCK or EAGAIN\n\r");								
				break;
			}
		       if (errno == EINTR){
	                     log_output("recv looop EINTR\n\r");
			      continue;
		       	}
			else{				
				handle_error_exit("recv");
			}
		}
		else {
			break;		
		}
	}
	return read_len;
}

char stop_all_childp()
{
	int index = 0;
	int send_len = 0;
	for (; index<child_pid_num; index++){
		send_len = send_msg(msg_fd[index*2], "stop", strlen("stop"));
		if (send_len<0){
			 send_msg(msg_fd[index*2], "stop", strlen("stop"));
		}
	}
	return 0;
}

char reload_all_childp()
{
	int index = 0;
	int send_len = 0;
	for (; index<child_pid_num; index++){
		send_len = send_msg(msg_fd[index*2], "reload", strlen("reload"));
		if (send_len<0){
			 send_msg(msg_fd[index*2], "reload", strlen("reload"));
		}		
	}
	return 0;	
}
