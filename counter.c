#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "my_data_structure.h"
#include "led_output.h"
extern int mode;

int output_counter(msg v_msg) {
    int RET;
    //static int hundreds = 0, tens = 0, ones = 0;
    static int numeral_system = 10;
    
    static int led_count = 0;
    static int led_input = 64;

    int i;

    //printf("output_counter now on working\n");

    /************ FND OUTPUT PART *************/
    if((v_msg.msg_type == SW_TO_FND) || (v_msg.msg_type == SW_TO_LED)) {
        //printf("hours plus ! : %d\n", h_add);
        //printf("mins plus ! : %d\n", m_add);
        //sleep(1);

        if(v_msg.data[0] == 1) {
            switch(numeral_system) {
                case 10: 
                    numeral_system = 8;
                    break;
                case 8:
                    numeral_system = 4;
                    break;
                case 4:
                    numeral_system = 2;
                    break;
                case 2:
                    numeral_system = 10;
                    break;
            }
        }
        //printf("now numeral : %d\n", numeral_system);
        //printf("is this ' v_msg.data[0] == 1 ' true ? %d\n", v_msg.data[0] == 1);
        //printf("v_msg.data[0] is %d\n", v_msg.data[0]);       
    }

    RET = fnd_output_counter(v_msg, numeral_system);
    if(RET != 0) {
        printf("FND OUTPUT failed\n");
    }   

    /************* LED OUTPUT *************/
    
    if((v_msg.msg_type == SW_TO_LED) || (v_msg.msg_type == SW_TO_LED)) {
        if(v_msg.data[0] == 1) {
            //printf("HOXY....\n");
            switch(numeral_system) {
                case 10 :
                    led_input = 64;
                    break;
                case 8 :
                    led_input = 32;
                    break;
                case 4 :
                    led_input = 16;
                    break;
                case 2 :    
                    led_input = 128;
                    break;
            }
            //printf("now numeral_system is %d\n", numeral_system);
            //printf("and led_input is %d\n", led_input);
            
        }
    }
    
    RET = led_output(led_input);

    if(RET != 0) {
        printf("led mmap failed\n");
    }
    
    return 0;
}