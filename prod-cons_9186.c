/*
 *	File	: pc.c
 *
 *	Title	: Demo Producer/Consumer.
 *
 *	Short	: A solution to the producer consumer problem using
 *		pthreads.
 *
 *	Long 	:
 *
 *	Author	: Andrae Muys
 *
 *	Date	: 18 September 1997
 *
 *	Revised	:
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#define QUEUESIZE 20
#define LOOP 40
#define P 64
#define Q 128

void *producer (void *args);
void *consumer (void *args);

int get_time() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (tv.tv_sec*1000000 + tv.tv_usec) ;
}

void *workfunc(void * a)
{
  int* b = a;
  //printf ("Work in Function. ");
  //printf ("a= %d from %ld\n",*b, pthread_self());

  //usleep(2000);

  for (double i = 0; i < 10+(*b); i++) {
    sin(i*(3.14+(*b)));
  }

}

typedef struct {
 void * (*work)(void *);
 void * arg;
}workFunction;


typedef struct {
  workFunction buf[QUEUESIZE];
  long head, tail;
  int full, empty;
  pthread_mutex_t *mut;
  pthread_cond_t *notFull, *notEmpty;
} queue;

int *times;
int myindex=0;
pthread_mutex_t *time_mux;
int flag=0;

queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q, int in);
void queueDel (queue *q, int *out);

int main ( int argc, char *argv[] )
{


  int p,q;
  if( argc == 3 && atoi(argv[1])>0 && atoi(argv[2])>0 ) {
   p=atoi(argv[1]);
   q=atoi(argv[2]);
   //printf("p=%d q=%d\n",p,q );
  }
else {
  p=P;
  q=Q;
  printf("p=%d q=%d\n",p,q );
}

  time_mux=(pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  if (pthread_mutex_init(time_mux, NULL) != 0) {
      printf("\n mutex init has failed\n");
      return 1;
  }
  times=(int *)malloc(sizeof(int)*LOOP*p);

  queue *fifo;
  pthread_t *pro=(pthread_t *)malloc(p*sizeof(pthread_t));
  pthread_t *con=(pthread_t *)malloc(q*sizeof(pthread_t));

  fifo = queueInit ();
  if (fifo ==  NULL) {
    fprintf (stderr, "main: Queue Init failed.\n");
    exit (1);
  }

  for (int i = 0; i < p; i++)
      pthread_create (&pro[i], NULL, producer, fifo);


  for (int i = 0; i < q; i++)
      pthread_create (&con[i], NULL, consumer, fifo);


  for (int i = 0; i < p; i++)
      pthread_join (pro[i], NULL);


  while (fifo->empty!=1)
    usleep(1000);



  flag=1; //exit pthreads consumers

  pthread_mutex_lock (fifo->mut);

    fifo->empty=0;

  pthread_mutex_unlock (fifo->mut);

  pthread_cond_broadcast (fifo->notEmpty); //signals for all consumers



  for (int i = 0; i < q; i++)
    pthread_join (con[i],NULL);

  pthread_mutex_destroy (time_mux);
  queueDelete (fifo);

  float mean=0;
  for (int i = 0; i < LOOP*p; i++) {
    //printf("%d\n",times[i] );
    mean+=times[i];

  }

  printf("%f ",mean/(LOOP * p) );
  free(pro);
  free(con);
  free(times);
  free(time_mux);
  return 0;
}

void *producer (void *q)
{

  queue *fifo;
  int i;

  fifo = (queue *)q;

  for (i = 0; i < LOOP; i++) {
    pthread_mutex_lock (fifo->mut);
    while (fifo->full) {
//      printf ("producer: queue FULL.\n");
      pthread_cond_wait (fifo->notFull, fifo->mut);
    }
    queueAdd (fifo, i );
    pthread_mutex_unlock (fifo->mut);
    pthread_cond_signal (fifo->notEmpty);

    //usleep (100000);
  }
  return (NULL);
  pthread_exit(0);
}

void *consumer (void *q)
{



  queue *fifo;
  int i, d;

  fifo = (queue *)q;

  while (1) {
    pthread_mutex_lock (fifo->mut);
    while (fifo->empty) {
//      printf ("consumer: queue EMPTY.\n");
      pthread_cond_wait (fifo->notEmpty, fifo->mut);
    }
    if (flag==1) {
          pthread_mutex_unlock (fifo->mut);
          pthread_cond_signal (fifo->notEmpty); //signals for all consumers
          pthread_exit(0);
    }
    queueDel (fifo, &d);
//    printf ("consumer: recieved %d .\n", d);
    pthread_mutex_unlock (fifo->mut);
    pthread_cond_signal (fifo->notFull);


    //usleep (50000);
  }
  return (NULL);
  pthread_exit(0);
}

/*
  typedef struct {
  int buf[QUEUESIZE];
  long head, tail;
  int full, empty;
  pthread_mutex_t *mut;
  pthread_cond_t *notFull, *notEmpty;
  } queue;
*/

queue *queueInit (void)
{
  queue *q;

  q = (queue *)malloc (sizeof (queue));
  if (q == NULL) return (NULL);

  q->empty = 1;
  q->full = 0;
  q->head = 0;
  q->tail = 0;
  q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (q->mut, NULL);
  q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notFull, NULL);
  q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notEmpty, NULL);

  return (q);
}

void queueDelete (queue *q)
{
  pthread_mutex_destroy (q->mut);

  free (q->mut);
  pthread_cond_destroy (q->notFull);
  free (q->notFull);
  pthread_cond_destroy (q->notEmpty);
  free (q->notEmpty);
  free (q);
}

void queueAdd (queue *q, int in)
{
  q->buf[q->tail].work = workfunc;
  q->buf[q->tail].arg=(int *)malloc(sizeof(int)*2);
  if ((q->buf[q->tail].arg) == NULL) return;

  ((int *)q->buf[q->tail].arg)[0] = in;
  ((int *)q->buf[q->tail].arg)[1] = get_time();

  //q->buf[q->tail].work(q->buf[q->tail].arg);
  //printf("%d\n", ((int *)q->buf[q->tail].arg)[0] );

  q->tail++;
  if (q->tail == QUEUESIZE)
    q->tail = 0;
  if (q->tail == q->head)
    q->full = 1;
  q->empty = 0;

  return;
}

void queueDel (queue *q, int *out)
{

  *out = ((int *)q->buf[q->head].arg)[0];
  q->buf[q->head].work( &( ((int *)q->buf[q->head].arg)[0] ) );

  pthread_mutex_lock (time_mux);

  times[myindex++]=get_time()- ((int *)q->buf[q->head].arg)[1];

  pthread_mutex_unlock (time_mux);

  free(q->buf[q->head].arg);

  q->head++;
  if (q->head == QUEUESIZE)
    q->head = 0;
  if (q->head == q->tail)
    q->empty = 1;
  q->full = 0;

  return;
}
