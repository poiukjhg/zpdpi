PWD = $(shell pwd)
SRC_PATH = $(PWD)
CC= gcc
OBJS = zpdpi
INCFILE = 
MY_LIBS = 
MY_FLAGS = -lpthread -lm -lpcap
CFLAGS += -Wall -pg -fPIC $(INCFILE) $(MY_LIBS) $(MY_FLAGS) 
OBJECTS = mylock.o zdpi_handler.o reg_handler.o  parse_packet.o myhash.o msg_dispatch.o test_dpi_module.o main.o 

.PHONY: all clean 
all: $(OBJS) 
$(OBJS):$(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(CFLAGS)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@			
clean:
	rm -rf $(OBJS)
	rm -rf *.o 
	rm -rf *.out
