#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int isPrime(int n){
    if(n < 2){
        return 0;
    }
    for(int i = 2; i < n; i++){
        if(n % i == 0){
            return 0;
        }
    }
    return 1;
}


void
main(int argc, char **argv)
{

    int checkers = 3;
    if (argc > 1) {
        checkers = atoi(argv[1]);
    }

    char c = 'y';
    while(c == 'y'){
        int channelGenerate = channel_create();
        int channelCheck = channel_create();
        if (channelGenerate < 0 || channelCheck < 0) {
            printf("Failed to create channel\n");
            exit(1);
        }

        if(fork() == 0){
            //PRINTER
            printf("c\n");
            int check;
            int count = 0;
            while(channel_take(channelCheck, &check) >= 0){
                count++;
                printf("%d\n", check);
                if(count >= 100){
                    channel_destroy(channelCheck);
                    printf("printer exiting - %d\n", getpid());
                    exit(0);
                }
            }
            printf("Failed to take data from channel check\n");
            exit(1);

        } else {
            int temp = checkers;
            while(temp > 0){
                //CHECKERS
                if(fork() == 0){
                    int num;
                    while(channel_take(channelGenerate, &num) >= 0){
                        if(isPrime(num)){
                            if (channel_put(channelCheck, num) < 0) {
                                channel_destroy(channelGenerate);
                                printf("checker %d exiting - %d\n", temp, getpid());
                                exit(0); //exit checker
                            }
                        }
                    }
                    //someone already exited
                    printf("checker %d exiting - %d\n", temp, getpid());
                    exit(0); //exit checker
                }
                temp--;
            }
            //GENERATOR - parent
            int num = 2;
            while(1){
                if (channel_put(channelGenerate, num++) < 0) {
                    int status;
                    while (wait(&status) > 0); //wait for all checkers to exit
                    printf("would you like to start the sequence again? (y/n)\n");
                    read(0, &c, 1);
                    char backslash;
                    read(0, &backslash, 1);
                    break;
                }
            }
        }
    }

    printf("generator exiting - %d\n", getpid());
    exit(0);

}
