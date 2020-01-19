#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
    int                 inFd;
    unsigned char       byte;
    unsigned int        count;

    if (argc != 2)
    {
        fprintf(stderr, "bad arguments\n");
        return 1;
    }

    inFd = open(argv[1], O_RDONLY, 0755);
    if (inFd < 0)
    {
        fprintf(stderr, "error opening input file\n");
        return 2;
    }

	printf("\n");
    printf("\t.globl _binArrayStart\n");
    printf("\t.globl _binArrayEnd\n");
    printf("\t.data\n\n");
    printf("\t.p2align 2\n");
    printf("_binArrayStart:\n");
    printf("\t.byte\t");

    for (count = 0 ; read(inFd, &byte, 1) == 1 ; ++count)
    {
        printf("%04o%s", byte, (count % 16 == 15) ? "\n\t.byte\t" : ",");
    }

	printf("\n\t.p2align 2,0\n");
    printf("\n_binArrayEnd:\n");
    return 0;
}
