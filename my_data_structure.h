#ifndef MY_DATA_STRUCTURE
#define MY_DATA_STRUCTURE

#define MSG_SIZE 100

enum m_type {
    SW_INPUT = 1,
    SW_TO_FND = 2,
    SW_TO_LED = 3,
    BUTTON_INPUT = 4,
};

typedef struct _msg
{
    long msg_type;
    char data[MSG_SIZE];
} msg; // message structure.

/* shared memory data structure
 | ---------------------------- 1024 (SHARE_MEM_SIZE) --------------------------|
 |   4(int)  |  4(long)  | by data_type |    4      |  by_data_type  |          |
 |stack_size | data_type |    data      | data_type |      data      | ........ |
 */// now don't use.

 /* shared memory data structure.
 |---------------------------- 1024 (SHARE_MEM_SIZE) --------------------------|
 | 4 (long) |   data-----------------------------------------------------------|
 | data_type|   data-----------------------------------------------------------|


 */


#endif