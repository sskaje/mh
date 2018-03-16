/*******************************************************

   Test file for DukTape single/multiple line script

 ******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "duktape.h"


static duk_ret_t native_print(duk_context *ctx) {
    duk_push_string(ctx, " ");
    duk_insert(ctx, 0);
    duk_join(ctx, duk_get_top(ctx) - 1);
    printf("%s\n", duk_to_string(ctx, -1));
    return 0;
}

int test_single_line(duk_context *ctx) {
    printf("declare single line\n");
    duk_push_lstring(ctx, "print('hello single line'+\"\\n\")", 31);

    return 0;
}

int test_single_line_stack(duk_context *ctx) {
    printf("declare single line stack\n");
    duk_require_stack(ctx, 1);
    duk_push_lstring(ctx, "print('hello single line require stack'+\"\\n\")", 45);

    return 0;
}

int test_multiple_line(duk_context *ctx) {
    printf("declare multiple line\n");
    size_t sz = 4;
    duk_require_stack(ctx, sz);

    duk_push_lstring(ctx, "print( ", 7);
    duk_push_lstring(ctx, "'a'+'b'", 7);
    duk_push_lstring(ctx, "+(1+31)", 7);
    duk_push_lstring(ctx, ");", 1);

    duk_concat(ctx, sz);
    return 0;
}

int test_multiple_line_segmented(duk_context *ctx) {
    printf("declare multiple line segmented\n");
    size_t sz = 0;
    duk_require_stack(ctx, 1);
    duk_push_lstring(ctx, "print( ", 7);
    ++sz;

    duk_require_stack(ctx, 1);
    duk_push_lstring(ctx, "'a'+'b'", 7);
    ++sz;

    duk_require_stack(ctx, 1);
    duk_push_lstring(ctx, "+(1+62)", 7);
    ++sz;

    duk_require_stack(ctx, 1);
    duk_push_lstring(ctx, ");", 1);
    ++sz;

    duk_concat(ctx, sz);
    return 0;
}

int main(int argc, char *argv[]) {
    duk_context *ctx;

    ctx = duk_create_heap_default();
    if (!ctx) { exit(1); }

    duk_push_global_object(ctx);
    duk_push_c_function(ctx, native_print, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "print");

    test_single_line(ctx);
    if (duk_peval(ctx) != 0) {
        printf("Error running: %s\n", duk_safe_to_string(ctx, -1));
    }
    duk_pop(ctx);

    test_single_line_stack(ctx);
    if (duk_peval(ctx) != 0) {
        printf("Error running: %s\n", duk_safe_to_string(ctx, -1));
    }
    duk_pop(ctx);

    test_multiple_line(ctx);
    if (duk_peval(ctx) != 0) {
        printf("Error running: %s\n", duk_safe_to_string(ctx, -1));
    }
    duk_pop(ctx);

    test_multiple_line_segmented(ctx);
    if (duk_peval(ctx) != 0) {
        printf("Error running: %s\n", duk_safe_to_string(ctx, -1));
    }
    duk_pop(ctx);

    duk_destroy_heap(ctx);
    return 0;
}
