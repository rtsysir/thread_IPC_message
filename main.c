/*
 * main.c
 *
 *  Created on: Feb 26, 2017
 *      Author: saeed.r.64@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "rtos_abstract/rtos_abstract.h"

#define PRIMES 4

typedef struct msgbuf {
	long    mtype;
	char    mtext[128];
} message_buf;


void loop1(uint8_t *arg){
	int key, mask,msgid;
	message_buf sbuf;
	size_t buf_length;
	sbuf.mtype = 1;
	(void) strcpy(sbuf.mtext, "Did you get this?");
	buf_length = strlen(sbuf.mtext) + 1 ;

	key = getuid();
	mask = 0666;
	printf("key1:%d\n",key);
	msgid = msgget(key, mask);
	if (msgid == -1) {
		printf( "Could not create message queue loop1.\n");
	}
	// Send messages ...
//	int ret, prime, j, i = 3, msg[2] = {PRIMES, 0};
	int ret;
	while (1) {

		ret = msgsnd(msgid, &sbuf, buf_length, IPC_NOWAIT);
		if (ret == -1) {
			if (errno != EAGAIN) {
				printf( "Message could not be sended.\n");
				continue;
			}
			usleep(5000);
			if (msgsnd(msgid, &sbuf, buf_length, 0) == -1) {
				printf( "Message could not be sended2.\n");
				continue;
			}
			usleep(5000);
		}
		printf("send mesg lopp1 success\n");


	}

}

void loop2(uint8_t *arg){
	int key, mask,msgid;
	message_buf  rbuf;

	key = getuid();
	mask = 0666;
	printf("key2:%d\n",key);
	usleep(5000);
	msgid = msgget(key, mask);
	if (msgid == -1) {
		printf( "Could not create message queue loop2.\n");
	}

	while (1)
	{
		if (msgrcv(msgid, &rbuf, 128, 1, IPC_NOWAIT) == -1) {
			if (errno != ENOMSG) {
				printf( "Message could not be received ENOMSG.\n");
				usleep(5000);
				continue;
			}
			usleep(50000);
			if (msgrcv(msgid, &rbuf, 128, 0, 0) == -1) {
				printf( "Message could not be received.\n");
				continue;
			}
		}
		usleep(5000);
		printf("%s\n", rbuf.mtext);
	}
}


int main ( void )
{
	int key, mask,msgid;
	key = getuid();
	mask = 0666;
	printf("key main:%d\n",key);
	msgid = msgget(key, mask);
	if (msgid == -1) {
		msgid = msgget(key, mask | IPC_CREAT|IPC_EXCL);
		if (msgid == -1) {
			printf( "Could not create message queue loop1.\n");
			exit(EXIT_FAILURE);
		}
	}


	void *thread1;
	void *thread_attrib;

	thread1 = calloc(sizeof(pthread_t),1);
	thread_attrib = calloc(sizeof(pthread_attr_t),1);

	pthread_attr_init(thread_attrib);
	pthread_attr_setdetachstate(thread_attrib,
			PTHREAD_CREATE_JOINABLE);
	/* create a second thread which executes inc_x(&x) */
	if (pthread_create(thread1, NULL, loop1, NULL)) {
		printf("Error creating thread\n");
	}



	void *thread2;
	void *thread_attrib2;
	thread2 = calloc(sizeof(pthread_t),1);

	thread_attrib2 = calloc(sizeof(pthread_attr_t),1);
	pthread_attr_init(thread_attrib2);
	pthread_attr_setdetachstate(thread_attrib2,
			PTHREAD_CREATE_JOINABLE);
	/* create a second thread which executes inc_x(&x) */
	if (pthread_create(thread2, NULL, loop2, NULL)) {
		printf("Error creating thread\n");
	}


//	BSP_Task_t task1;
//	BSP_InitTask(&task1,loop1,NULL);
//	BSP_CreateTask(&task1);

//	BSP_Task_t task2;
//	BSP_InitTask(&task2,loop2,NULL);
//	BSP_CreateTask(&task2);

	while(1);

	return EXIT_SUCCESS;
}
