#ifndef MYLOCK_H
#define MYLOCK_H

int my_lock_init(void **lock);
int my_lock_trylock(void* lock);
int my_lock_tryunlock(void* lock);
void my_lock_destroy(void* lock);
#endif