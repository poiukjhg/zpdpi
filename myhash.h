#ifndef MY_HASH_H
#define MY_HASH_H
#include "myglobal.h"

#define TABLESIZE 0x400UL //1024

struct hash_entry
{
	int32_t key;
	int32_t	used;					
	char	host[HOSTSIZE];	
	char ip[16];
	struct	hash_entry *next;
};



void hashtable_init();
void hashtable_dest();
struct hash_entry* hashtable_create(const char* host, const char *ip);
struct hash_entry* hashtable_get(const char* host);
void hashtable_delete(struct hash_entry *entry);
uint32_t get_all_entry(struct hash_entry ** entry_array);

#endif

