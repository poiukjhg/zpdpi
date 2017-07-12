#ifndef MSG_DISPATCH_H
#define MSG_DISPATCH_H

int send_msg(int fd, char* msg, int m_len);
int recv_msg(int fd, char *buf);
char stop_all_childp();
char reload_all_childp();
#endif
