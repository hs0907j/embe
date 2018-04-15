#define MSG_SIZE 100

enum m_type {
    SW_INPUT
};

enum shm_type {
    SHM_SW_INPUT
};

typedef struct _msg
{
    long msg_type;
    char data[MSG_SIZE];
} msg;

/* shared memory data structure
 | ---------------------------- 1024 (SHARE_MEM_SIZE) --------------------------|
 |   4(int)  |  4(long)  | by data_type |    4      |  by_data_type  |          |
 |stack_size | data_type |    data      | data_type |      data      | ........ |
 */