#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



void init_devices(void) {
    qid = msgget(queue_key , IPC_CREAT|0664);   // parent msg queue allocation.
    if(qid == -1) {
	    printf("Making Message Queue Failed...\n");
    }
    
    sid = shmget(shm_key, SHARE_MEM_SIZE, IPC_CREAT|0664);
    if(sid == -1) {
        printf("Making Shared Memory Failed....\n");
    }
}