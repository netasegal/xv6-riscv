#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    printf("Hi\n");
    int cd = channel_create();
    if (cd < 0) {
        printf("Failed to create channel\n");
        exit(1);
    }
    if (fork() == 0) {
        if (channel_put(cd, 42) < 0) {
            printf("Failed to put data in channel\n");
            exit(1);
        }
        printf("Mew\n");
        printf("%d\n", channel_put(cd, 43)); // Sleeps until cleared
        printf("Bye\n");
        // Handle error
        channel_destroy(cd);
        // Handle error
        
    } else {
        int data;
        printf("%d\n", data);
        if ((channel_take(cd, &data)) < 0) { // 42
            printf("Failed to take data from channel\n");
            exit(1);
        }
        printf("%d\n", data);
        channel_take(cd, &data); // 43
        printf("%d\n", data);
        // Handle error
        data=1;
        data = channel_take(cd, &data); // Sleep until child destroys channel
        // Handle error
        printf("%d\n", data);
    }
    exit(0);
}
