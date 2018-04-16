/* FPGA FND Test Application
File : fpga_test_fnd.c
Auth : largest@huins.com */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "my_data_structure.h"
#include "led_output.h"
extern int mode;

int output_clock(msg v_msg, int time_change_flag) {
    int RET;
    static int h_add = 0, m_add = 0;
    void *shm_addr;
    static int led_count = 0;
    static int led_input = 128;

    int i;

    //printf("output delay test\n");

    /************ FND OUTPUT PART *************/
    if((v_msg.msg_type == SW_TO_FND) && time_change_flag) {
        //printf("hours plus ! : %d\n", h_add);
        //printf("mins plus ! : %d\n", m_add);

        if(v_msg.data[1] == 1) {
            h_add = 0;
            m_add = 0;
        }
        if(v_msg.data[2] == 1) h_add++;
        if(v_msg.data[3] == 1) m_add++;
    }

    RET = fnd_output_clock(h_add, m_add);
    if(RET != 0) {
        printf("FND OUTPUT failed\n");
    }

    /************* LED OUTPUT *************/
    if(v_msg.msg_type == SW_TO_LED) {
       if(v_msg.data[0]==1) {
           if(!time_change_flag) {
               led_input = 128;
           }
           else {
               led_input = 64;
           }

       }
    }

    if(time_change_flag) {
        led_count++;
        if(led_count >= 12000) {
            led_count = 0;

            if(led_input == 64) led_input = 32;
            else led_input = 64;
        }
    }
    else {
        led_input = 128;
    }

    RET = led_output(led_input);

    if(RET != 0) {
        printf("led mmap failed\n");
    }
    return 0;
}



