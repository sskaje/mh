#include <mh/mh.h>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Usage: %s STRING\n", argv[0]);
        return -1;
    }

    fprintf(stderr, "Input: %s\nOutput:\n", argv[1]);
    fprintf(stdout, "%s\n", mh_bytes2hex((uint8_t *) argv[1], strlen(argv[1])));

    return 0;
}