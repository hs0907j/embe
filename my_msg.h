#define MSG_SIZE 100

// msg_type 1 : int, 2 : string, 3: char.
enum m_type {
    msg_INT, msg_STRING, msg_CHAR
};

typedef struct _msg
{
    long msg_type;
    char data[MSG_SIZE];
} msg;