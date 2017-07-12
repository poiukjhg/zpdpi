#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mylogs.h"
#include "myglobal.h"
#include "myhash.h"


struct hash_entry*	hash_table[TABLESIZE];
uint32_t hashtable_entry_count = 0;
LOCAL inline int32_t ELFhash(char *host)
{
    uint32_t h = 0;
    uint32_t g;

    while( *host ) {
        h =( h<< 4) + *host++;
        g = h & 0xf0000000L;
        if( g ) h ^= g >> 24;
        h &= ~g;
    }

    return h;
}

LOCAL inline void entry_init(struct hash_entry *entry)
{
	memset(entry, 0, sizeof(struct hash_entry));
}

void hashtable_init()
{
	uint32_t i;
	for( i = 0 ; i < TABLESIZE ; i++ )
		hash_table[i] = NULL;
}

void hashtable_dest()
{
	assert(hashtable_entry_count == 0);
}


struct hash_entry* hashtable_create(const char* host, const char *ip)
{
	hashtable_entry_count++;
	assert(host);

	struct hash_entry *entry = (struct hash_entry *)malloc(sizeof(struct hash_entry));
	entry_init(entry);
	entry->key = ELFhash((char*)host);
	assert(strlen(host) < 4095);
	strcpy(entry->host, host);
	strcpy(entry->ip, ip);
	const uint32_t val = entry->key & (TABLESIZE - 1);

	struct hash_entry *ptr = hash_table[val];

	//������Ϊ�գ������½�ڣ�����
	if(ptr == NULL) {
		hash_table[val] = entry;
	//	hash_table[val]->next = NULL;	//��ʼ����ΪNULL
		return entry;
	}

	struct hash_entry *prev = NULL;

	while( ptr != NULL ) { //��С��������

		int ret = strcmp(ptr->host, entry->host);

		if( ret > 0 ) {	//��ǰ�ڵ��key���ڲ���ڵ㣬���ڴ˴����룬break
			break;
		} else if( ret < 0 ) {	//δ�ҵ�λ�ã���������
			prev = ptr;
			ptr = ptr->next;
		} else {
			//�½�ǰ������ң��Ҳ������½����������host��ײ
			assert(0);
		}
	}

	entry->next = ptr;	// ptr�п���Ϊ��

	if (prev == NULL)	//newPtr��С��������ǰ��
		hash_table[val] = entry;
	else
		prev->next = entry;

	return entry;
}

struct hash_entry* hashtable_get(const char* host)
{
	int32_t key = ELFhash((char*)host);

	const uint32_t val = key & (TABLESIZE-1);

	if( hash_table[val] == NULL )
		return NULL;

	struct hash_entry *ptr = hash_table[val];

		//����hashtable�ϵ�����
	while( ptr != NULL ) {

		int ret = strcmp(ptr->host, host);

		if(ret == 0) {
			//�ҵ�
            return ptr;
		} else if ( ret < 0 ) {
            ptr = ptr->next;
		} else {
			//δ�ҵ�
            return NULL;
		}
	}

	return NULL;
}

void hashtable_delete(struct hash_entry *entry)
{
	hashtable_entry_count--;

	const uint32_t val = entry->key & (TABLESIZE-1);

	struct hash_entry *ptr = hash_table[val], *prev = NULL;

	assert(ptr);

	while(ptr != NULL) {

		int ret = strcmp(ptr->host, entry->host);

		if( ret == 0 ) {	//�ҵ��ýڵ�

			if( prev == NULL ) {
				hash_table[val] = ptr->next;
				free(ptr);
				return;
			}


			if( prev != NULL ) {
				prev->next = ptr->next;
				free(ptr);
				return;
			}

			assert(0);

		} else if ( ret < 0 ) {
			prev = ptr;
			ptr = ptr->next;
		} else {
			break;
		}
	}

	//������δ�ҵ�
	assert(0);
	return;
}

uint32_t get_all_entry(struct hash_entry ** entry_array)
{
	uint32_t i;
	uint32_t offset = 0;
	
	struct hash_entry* entry = NULL;
	for( i = 0 ; i < TABLESIZE ; i++ ) {

		if( hash_table[i] == NULL )
			continue;

		
		entry = hash_table[i];
		hash_table[i] = NULL;
		
		while( entry != NULL ) {
			entry_array[offset] = entry;
			offset++;
			entry = entry->next;
		}
	}

	assert( offset == hashtable_entry_count );
//diffrent as mod_billing;when writelog in billingd, clear the hashtable.	
	hashtable_entry_count = 0;

	return hashtable_entry_count;
}

