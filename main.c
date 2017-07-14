#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>  
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include "mylogs.h"
#include "myglobal.h"
#include "zdpi_handler.h"
#include "parse_packet.h"
#include "msg_dispatch.h"
#include "reg_handler.h"

#define fd_nonblocking(s)  fcntl(s, F_SETFL, fcntl(s, F_GETFL) | O_NONBLOCK)

static void start_daemon(void)
{
	pid_t pid;

	/* Start forking */
	if ((pid = fork()) < 0)
		handle_error_exit("ERROR: fork");
	if (pid > 0)
		exit(0);                  /* parent */

	/* First child process */
	setsid();                   /* become session leader */

	if ((pid = fork()) < 0)
		handle_error_exit( "ERROR: fork");
	if (pid > 0)                /* first child */
		exit(0);
	umask(022);
}
void sig_handler(int sig)
{
	log_output("recv signal\n\r");
	if (sig == SIGINT || sig == SIGQUIT){
		stop_all_childp();
	}
	sleep(4);
	exit(0);
}

void print_packet(uint8_t *packet,
                  uint32_t start, uint32_t end, u_int wrap) {
	int i=0;
	while (i < end - start) {
		/*
		if(isprint(packet[i+start]))
			printf("%c", packet[i+start]);
		else
		*/
			printf("%02x ", packet[i+start]);
		i++;
		if ( i % wrap == 0) 
			printf("\n");
	}
	if ( i % wrap != 0) 
		printf("\n");
}

int main(int argc, char *argv[])
{
	int index = 0;
	int tmp_fd = 0;
	pid_t pid;
	int stat;
	int result;
	int i;
	sigset_t newmask;
	struct sigaction act;
//TODO
//handle args

	char *arg = NULL;
	char check_host_filename[1024] = "/tmp/testfile";
	for (i=1; i<argc; i++) {
		arg=argv[i];  
	    if (strcmp(arg,"-c")==0 || strcmp(arg,"--config-file")==0) {
			if (++i<argc) {
	            		strncpy(check_host_filename, argv[i], strlen(argv[i]));			
			} else {
				log_output("Error: file name expected after %s option.\n", arg);
				exit(-1);
			}
		}     
	}
//TODO
	start_daemon();

//TODO
// signal
	act.sa_handler = sig_handler; 
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, 0);
	sigaction(SIGQUIT, &act, 0);
	
	child_pid_num = get_nprocs();
	child_pid_num = child_pid_num>1?(child_pid_num-1):1;
	msg_fd = (int *)malloc(child_pid_num*sizeof(int));
	assert(msg_fd);
	log_output("child pid = %d\n\r", child_pid_num);
	
	for (index = 0; index < child_pid_num; index ++){
		result = socketpair(AF_UNIX, SOCK_STREAM, 0, &msg_fd[2*index]);
		if (result < 0){
			handle_error_exit("socketpair() ");      
		}  	
		if (fd_nonblocking(msg_fd[2*(index+1)]) == -1) {
			handle_error_exit( "socketpair no blocking");      
		}
		if (fd_nonblocking(msg_fd[2*(index+1)+1]) == -1) {
			handle_error_exit("socketpair no blocking");
		} 	
		log_output("msgfd = %d, %d\n\r", msg_fd[2*index],  msg_fd[2*index+1]);
		tmp_fd = fork();
		if (tmp_fd == 0){	
			log_output("child process %d start\n\r", (int)getpid());
			zdpi_loop(check_host_filename, index, msg_fd[2*index]);
		}
		else if (tmp_fd >0){
			log_output("parent process pid=%d close msgfd %d, fd = %d\n\r",  (int)getpid(), 2*index+1, msg_fd[2*index+1]);
			close(msg_fd[2*index+1]);			
			continue;
		}
		else{
			handle_error_exit("fork error");
		}

	}
	
	parse_packet();
	log_output("out fork %d\n\r", (int)getpid());
	while((pid = wait(&stat)) > 0){ 
		log_output("child %d terminated\n\r", pid); 
		if (WIFEXITED(stat))
			log_output("child exited normal exit status=%d\n", WEXITSTATUS(stat));

		else if (WIFSIGNALED(stat))
			log_output("child exited abnormal signal number=%d\n", WTERMSIG(stat));
		else if (WIFSTOPPED(stat))
			log_output("child stoped signal number=%d\n", WSTOPSIG(stat));			   
       }
	return 0;	

}
