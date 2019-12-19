#include <stdlib.h>
#include <pthread.h>
#define BUFFER_SIZE 20
#define NUMITEMS 30
typedef struct {
int item[BUFFER_SIZE];
int totalitems;
int in, out;
pthread_mutex_t mutex;
pthread_cond_t full;
pthread_cond_t empty;
} buffer_t;
buffer_t bound = { {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER};
int produce_item ()
{
int item = (int) (100.0*rand()/(RAND_MAX+1.0));
sleep((unsigned long) (5.0*rand()/(RAND_MAX+1.0)));
printf("생산자_아이템: 아이템=%d\n", item);
return item;
}
insert_item (int item)
{
int status;
status = pthread_mutex_lock (&bound.mutex);
if (status != 0)
return status;
while (bound.totalitems >= BUFFER_SIZE && status == NULL)
status = pthread_cond_wait (&bound.empty, &bound.mutex);
if (status != 0) {
pthread_mutex_unlock(&bound.mutex);
return status; }

bound.item[bound.in] = item;
bound.in = (bound.in + 1) % BUFFER_SIZE;
bound.totalitems++;
if (status = pthread_cond_signal(&bound.full)) {
pthread_mutex_unlock (&bound.mutex);
return status;
}
return pthread_mutex_unlock (&bound.mutex);
}
consume_item (int item)
{
sleep((unsigned long)
(5.0*rand()/(RAND_MAX+1.0)));
printf("\t\t소비자_아이템: 아이템=%d\n", item);
}
remove_item (int *temp)
{
int status;
status = pthread_mutex_lock (&bound.mutex);
if (status != 0)
return status;
while (bound.totalitems <= 0 && status == NULL)
status = pthread_cond_wait (&bound.full, &bound.mutex);
if (status != 0) {
pthread_mutex_unlock(&bound.mutex);
return status;
}
*temp = bound.item[bound.out];
bound.out = (bound.out + 1) % BUFFER_SIZE;
bound.totalitems--;
if (status = pthread_cond_signal(&bound.empty)) {
pthread_mutex_unlock (&bound.mutex);
return status;
}
return pthread_mutex_unlock (&bound.mutex);
}
void * producer(void *arg)
{
int item;
while (1) {
item = produce_item ();
insert_item(item);
}
}
void * consumer(void *arg)
{
int item;
while (1) {
remove_item (&item);
consume_item (item);
}
}
main ()
{
int status;
void *result;
pthread_t producer_tid, consumer_tid;
/* 쓰레드 생성 */
status = pthread_create (&producer_tid, NULL, producer, NULL);
if (status != 0)
perror ("생산자 생성");
status = pthread_create (&consumer_tid, NULL, consumer, NULL);
if (status != 0)
perror ("소비자 생성");
status = pthread_join (producer_tid, NULL);
if (status != 0)
perror ("생산자");
status = pthread_join (consumer_tid, NULL);
if (status != 0)
perror ("소비자");
}

