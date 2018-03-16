
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <app/app.h>


int test(const char* args) {
    char **argv;
    int  argc;

    printf("===  TEST  ===\n%s\n", args);
    argv = args_parse(args, &argc);
    printf("=== RESULT ===\n");
    printf("argc=%d\n", argc);

    for (int i = 0; i < argc; i++)
        printf("[%d]{%zu} %s\n", i, strlen(argv[i]), argv[i]);

    printf("\n");

    free(argv);

    return 0;
}


int main()
{
    test("");
    test("a");
    test("hello a b c \"aaa  bbb\" ccc \\x31\\x32\\x33");
    test("\'hello\'");
    test("\'hello'");
    test("\'hello");
    test("\'hello\"");
    test("\"hello'");
    test("\"hello");
    test("\"hello\"");
    test("\\ a\\ ");
    test("\\ a \\ ");
    test("    ");
    test("    a    b    c    ");
    test("'aaa'bbb'ccc'ddd");
    test("'aaa' bbb'ccc'ddd");
    test("'aaa'bbb 'ccc'ddd");
    test("'aaa' bbb 'ccc'ddd");


}