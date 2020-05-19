
#define MAX_USERS 10
#define MAX_LOGIN_SIZE 50

struct user{
    int socket;
    char login[MAX_LOGIN_SIZE];
};
