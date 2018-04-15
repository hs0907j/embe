/* FPGA FND Test Application
File : fpga_test_fnd.c
Auth : largest@huins.com */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "clock.h"
#include "readkey_input.h"
#include "my_data_structure.h"

#define SHARE_MEM_SIZE 1024
void shared_memory_sending(void *shm_addr, int add_space, msg v_msg);


int main(void){
    pid_t pid;
    key_t queue_key = 1234321;
    key_t shm_key = 124141251;
    int qid_sw_input;
    int sid, shm_stack = 0;
    void *shm_addr, *ptr;
    int mode = 0;
    int success;
    int i;

    msg v_msg;

    qid_sw_input = msgget(queue_key , IPC_CREAT|0664);   // parent msg queue allocation.
    if(qid_sw_input == -1) {
	    //Exception
    }

    sid = shmget(shm_key, SHARE_MEM_SIZE, IPC_CREAT|0664);
    if(sid == -1) {
        printf("Making Shared Memory Failed....\n");
    }

    shm_addr = (void*)shmat(sid, NULL, 0);
    
    ((int *)shm_addr)[0] = 1;
    printf("shm_addr[0] is %d\n", ((int*)shm_addr)[0]);
 
/*
    printf("sizeof(shm_addr) is %d\n", sizeof(shm_addr));
    for(i=0; i< 50; i++) {
        printf("%d ", shm_addr[i]);
    }
    printf("\n");
    memset(shm_addr, 0, SHARE_MEM_SIZE);
    for(i=0; i< 50; i++) {
        printf("%d ", shm_addr[i]);
    }
    printf("\n");
*/

    pid = fork();
    if(pid < 0)
    {
        printf("Input Process Fork Failed.\n");
        //Fork Fail
    }
    else if(pid == 0) // input process. child 1.
    {
        printf("Input Process Actived.\n");

        qid_sw_input = msgget(queue_key, 0664);
        if(qid_sw_input == -1) {
	        //Exception
        }
        
        while(1) {
            // use switch and read key prog.
            // start with mode 0.

            switch(mode) {
                

                case 0:
                    input_clock(qid_sw_input, mode);
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    break;
            }

            //if read key, then use message queue, and change mode.
            //if key is "BACK", break, and quit.

            if(1) break;
        }
    }
    else    // parent. read key and after that, birth child.
    {
        pid = fork();
        if(pid < 0) 
        {
            printf("Output Process Fork Failed.\n");
            // Fork Fail
        }
        else if(pid == 0) // output process. child 2.
        {
            printf("Output Process Actived.\n");

            while(1) {
                // receive message queue, and change mode.
                // start with mode 0.
                
                switch(mode) {
                    case 0:
                        output_clock(qid_sw_input);
                        break;
                    case 1:
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                }

                if(1) break;
            }
        }
        else { // parent.
            printf("Parent Process Actived.\n");

            while(1) {

                // Message Queue receiving.
                success = msgrcv(qid_sw_input, (void *)(&v_msg), sizeof(msg) - sizeof(long), SW_INPUT, IPC_NOWAIT);
                if(success == -1) {
	              //printf("Receive SW TO FND Message Failed. There is no Message.\n");
                }
                else {
                    printf("receiving SW TO FND data is ");
                    for(i=0; i<9; i++) {
                        printf("%d ", v_msg.data[i]);
                    }
                    printf("\n");

                    // Shared Memory Sending Start.

                    
                }

                
               
                
                shared_memory_sending(shm_addr, 13, v_msg); // data size 9 + sizeof(data_type)

                //printf("memory stacked, now addr is %d\n", ptr);
            }
            printf("parent die\n");
        }
    } 

    /*
    success = msgctl(qid_sw_input, IPC_RMID, (struct msqid_ds *)NULL); // parent msg queue kill
    if(success == -1) {
	    //Exception
    }
    */  
    return 0;
}

void shared_memory_sending(void *shm_addr, int add_space, msg v_msg) {
    void *ptr = ((int*)shm_addr)[0] + shm_addr;

    printf("((int*)shm_addr)[0] is %d\n", ((int*)shm_addr)[0]);
    printf("shm_addr is %d\n", shm_addr);
    printf("ptr is %d\n", ptr);
    if(((int*)shm_addr)[0] + add_space >= SHARE_MEM_SIZE) {
        //printf("Shared Memory Stack Over! Sending Failed...\n");
    }
    else    {
        *((long*)ptr) = v_msg.msg_type;
        ptr++;

        //memcpy(ptr, v_msg.data, 9);
        //for(i=0; i<9; i++) {
        //(char*)shm_addr[stack++] = v_msg.data[i];
        //}


        //((int*)shm_addr)[0] += 9;
    }    
}