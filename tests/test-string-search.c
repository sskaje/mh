//
// Created by sskaje on 2017/8/8.
//

#include <mh/search.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Missing needle\n");
        return -1;
    }

    char* haystack = "hi, this is an example, hi, this is an example, hi, this is an example, hi, this is an example\0";
    //char* needle = "examp\0";
    char* needle = argv[1];

    printf("haystack:[%d] %s\n", (int) strlen(haystack), haystack);
    printf("needle:[%d] %s\n", (int) strlen(needle), needle);

    int* results = malloc(sizeof(int) * 1024);

    int result_size = bm_search(
            (unsigned char *)haystack, (int) strlen(haystack),
            (unsigned char *)needle, (int) strlen(needle),
            results
    );

    printf("%d result(s) found\n", result_size);


    for (int i = 0; i < result_size; i ++) {
        printf(
                "[%d] pos=%d, data=%s\n",
                i,
                results[i],
                haystack + results[i]
        );
    }

    return 0;
}


