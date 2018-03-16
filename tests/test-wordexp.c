
#include <stdlib.h>
#include <stdio.h>

#include <wordexp.h>


int main()
{
    const char* args = "hello a b c \"aaa  bbb\" ccc";

    wordexp_t p;
    char **w;
    int i;

    wordexp(args, &p, 0);

    for (i = 0; i < p.we_wordc; i++)
        printf("%s\n", p.we_wordv[i]);

    wordfree(&p);
    exit(EXIT_SUCCESS);
}
