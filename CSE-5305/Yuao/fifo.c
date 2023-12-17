#include<stdio.h>


int main(int argc, char *argv[])
{
    for(int i=1; i<argc; i++) {
        printf("job %d printing %s\n", i, argv[i]);
    }
}
