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
#include "button_input.h"
#include "my_data_structure.h"

#define SHARE_MEM_SIZE 1024
void shared_memory_sending(void *shm_addr, msg v_msg);

key_t queue_key = 1234321;
key_t shm_key = 124141251;
key_t shared_mode_key = 65878335;


int main(void){
    pid_t pid;
    
    int qid;
    int sid, shared_mode_id;
    void *shm_addr, *shm_mode_addr, *ptr;
    int success;
    int i;
    int h_add, m_add;
    int mode = 0;

    int clock_change = 1;

    msg v_msg, mode_msg;

    qid = msgget(queue_key , IPC_CREAT|0664);   // parent msg queue allocation.
    if(qid == -1) {
	    printf("Making Message Queue Failed...\n");
    }
    
    //////// shared memory for switch data.//////////
    sid = shmget(shm_key, SHARE_MEM_SIZE, IPC_CREAT|0664);
    if(sid == -1) {
        printf("Making Shared Memory Failed....\n");
    }
    
    shm_addr = (void*)shmat(sid, NULL, 0);
    //////////////////////////////////////////////////

    //////// shared memory for MODE data./////////////
    shared_mode_id = shmget(shared_mode_key, SHARE_MEM_SIZE, IPC_CREAT|0664);
    if(shared_mode_id == -1) {
        printf("Making Shared Memory Failed....\n");
    }
    
    shm_mode_addr = (void*)shmat(shared_mode_id, NULL, 0);
    //////////////////////////////////////////////////



    led_setup();

    pid = fork();
    if(pid < 0)
    {
        printf("Input Process Fork Failed.\n");
        //Fork Fail
    }
    /*************************** INPUT PROCESS CHILD 1 **************************/
    else if(pid == 0) // input process. child 1.
    {
        printf("Input Process Actived.\n");

        qid = msgget(queue_key, 0664);
        if(qid == -1) {
	        printf("child process queue get failed...\n");
        }
        
        button_input(qid);
    }
    else
    {
        pid = fork();
        if(pid < 0) 
        {
            printf("Output Process Fork Failed.\n");
            // Fork Fail
        }
        //****************** output process. child 2 **********************************/
        else if(pid == 0) 
        {
            h_add = m_add = 0;

            
            // Accessing shared memory.
            sid = shmget(shm_key, SHARE_MEM_SIZE, 0664);
            if(sid == -1) {
                printf("Accessing Parent Shared Memory Failed....\n");
            }
            // fatch shared memory data.
            shm_addr = (void*)shmat(sid, NULL, 0);

            //////// shared memory for MODE data./////////////
            shared_mode_id = shmget(shared_mode_key, SHARE_MEM_SIZE, 0664);
            if(shared_mode_id == -1) {
                printf("Accessing Parent Shared MODE Memory Failed....\n");
            }
    
            shm_mode_addr = (void*)shmat(shared_mode_id, NULL, 0);
            //////////////////////////////////////////////////

            

            void *shm_ptr;
            int time_change_flag = 0; // 0 don't work, 1 work.
            
            printf("Output Process Actived.\n");

            while(1) {
                /************* shared memory reading start. **********/
                shm_ptr = shm_addr;
                v_msg.msg_type = *((char *)shm_ptr);
                if((v_msg.msg_type != -1) || time_change_flag) {
                    //printf("v_msg.msg_type is %ld\n", v_msg.msg_type);

                    shm_ptr++;

                    if((v_msg.msg_type == SW_TO_FND) || (v_msg.msg_type == SW_TO_LED)) {
                        //printf("input data is switch\n");
                        for(i=0; i<9; i++) {
                            v_msg.data[i] = *((char*)shm_ptr);
                            shm_ptr++;
                        }

                        if(v_msg.data[0]==1) {
                            if(v_msg.msg_type != 0) { // by new push [0] key.
                                if(time_change_flag) time_change_flag = 0;
                                else time_change_flag = 1;
                            }
                        }
                    }

                    //printf("time_change_flag is %d\n", time_change_flag);

                    *((long *)shm_addr) = -1;
                    /************* shared memory reading done. **********/

                    // receive shared memory, and change mode.
                    // start with mode 0.
                    
                    shm_ptr = shm_mode_addr;
                    shm_ptr++;
                    
                    mode_msg.data[0] = *((char*)shm_ptr);
                    mode = mode_msg.data[0];
                    
                    for(i =0; i<10; i++);
                    //printf("OUTPUT PROC now mode is %d\n", mode);
                    switch(mode) {
                        case 0:
                            output_clock(v_msg, time_change_flag);
                            break;
                        case 1:
                            output_counter(v_msg);
                            break;
                        case 2:
                            break;
                        case 3:
                            break;
                    }
                }
            }
        }
        /*************************** Parent Process !! Calculator !!!!******************/
        else { // parent. Receive Child1's Message Queue.
            printf("Parent Process Actived.\n");

            memset(v_msg.data, 0, sizeof(v_msg.data));
            v_msg.msg_type = -1;
            while(1) {
                // 1. Switch message receive.
                
                success = msgrcv(qid, (void *)(&v_msg), sizeof(msg) - sizeof(long), SW_INPUT, IPC_NOWAIT);
                if(success == -1) {
	              //printf("Receive SW Message Failed. There is no Message.\n");
                }
                else {
                    printf("receiving SW message queue is ");
                    for(i=0; i<9; i++) {
                        printf("%d ", v_msg.data[i]);
                    }
                    printf("\n");

                    // do it ! switch()
                    for(i=0; i<9; i++) {
                        if(v_msg.data[i] == 1){
                            switch(i){
                                case 0: // switch 1
                                    clock_change *= -1;
                                    
                                    v_msg.msg_type = SW_TO_LED;
                                    break;

                                case 1: // switch 2
                                    v_msg.msg_type = SW_TO_FND;
                                    break;  

                                case 2: // switch 3
                                    if(clock_change) {
                                        v_msg.msg_type = SW_TO_FND;
                                    }
                                    break;
                                case 3: // switch 4
                                    if(clock_change) {
                                        v_msg.msg_type = SW_TO_FND;
                                    }
                                    break;
                            }
                        }
                    }
                    if(v_msg.msg_type != SW_INPUT) {
                        shared_memory_sending(shm_addr, v_msg); 
                    }
                }
                
                /****************** button message receive is not need....? **********/
                // 2. Button message Receive.

                success = msgrcv(qid, (void *)(&v_msg), sizeof(msg) - sizeof(long), BUTTON_INPUT, IPC_NOWAIT);
                if(success == -1) {
	              //printf("Receive SW Message Failed. There is no Message.\n");
                }
                else {
                    printf("receiving BUTTON data is ");
                    printf("%d ", v_msg.data[0]);
                    printf("\n");

                    switch(v_msg.data[0]) {
                        case 158: // back button. turn off.

                            break;

                        case 115: // volumn + button. mode change.
                            break;

                        case 114: // volumn - button. mode change.
                            break;  
                    }
                    //shared_memory_sending(shm_addr, v_msg); 
                }
            }
            printf("parent die\n");
        }
    } 

    return 0;
}

void shared_memory_sending(void *shm_addr, msg v_msg) {
    int CPY_SIZE;

    *((long *)shm_addr) = v_msg.msg_type;

    shm_addr++;

    

    if(v_msg.msg_type == SW_TO_FND || v_msg.msg_type == SW_TO_LED) {
        CPY_SIZE = 9;
    }
    else if(v_msg.msg_type == BUTTON_INPUT) {
        CPY_SIZE = 3;
    }
    else {
        CPY_SIZE = 5;
    }
    memcpy(shm_addr, v_msg.data, CPY_SIZE);    

    
    /*
    printf("%d ", *((char*)shm_addr));
    shm_addr++;
    printf("%d ", *((char*)shm_addr));
    shm_addr++;
    printf("%d ", *((char*)shm_addr));
    shm_addr++;
    printf("%d ", *((char*)shm_addr));
    shm_addr++;
    printf("%d ", *((char*)shm_addr));
    shm_addr++;
    printf("%d ", *((char*)shm_addr));
    shm_addr++;
    */
}