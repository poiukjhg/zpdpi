#ifndef MY_LOGS_H
#define MY_LOGS_H
#include <errno.h>
#include<stdlib.h>
#include<stdio.h>
#include <assert.h>
#include <errno.h>

#define DEBUG
//#undef DEBUG

#define handle_error_exit(msg)  \
	do { \
		perror(msg); \
		printf("exit\n\r"); \
		exit(EXIT_FAILURE); \
	} while (0)

#define handle_error_without_exit(msg)  \
	do { \
		perror(msg); \
		printf("exit\n\r"); \
	} while (0)
		
#ifdef DEBUG	

#define log_output(format, ...)  \
	do { \
		printf (format, ##__VA_ARGS__); \
	} while (0)	
#else
#define log_output(format, ...) \
	do { \
	} while (0)	
#endif

#endif
