/**
 * src/app/script-main.c -- mh_script main file
 *
 * @author sskaje
 * @license MIT
 * ------------------------------------------------------------------------
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 - , sskaje (https://sskaje.me/)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * ------------------------------------------------------------------------
 */


#include "app.h"

#include "scripts.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s SCRIPT_FILE [EXTRA ARGS]\n", argv[0]);
        exit(-2);
    }

#if IOS_JAILBREAK_ELECTRA
    patch_setuidandplatformize();
#endif

    if (getuid() != 0) {
        printf("Error: Please run as 'root'\n\n");
        printf("Usage: %s SCRIPT_FILE [EXTRA ARGS]\n", argv[0]);
        exit(-1);
    }

#if IOS_JAILBREAK_ELECTRA
    setuid(0);
    setgid(0);
#endif

#if JAVASCRIPT_SUPPORT

    duk_context *ctx = script_new();

    // register argv to args
    duk_idx_t arr_idx;
    arr_idx = duk_push_array(ctx);
    for (int i = 1; i < argc; i++) {
        duk_push_string(ctx, argv[i]);
        duk_put_prop_index(ctx, arr_idx, (duk_uarridx_t) (i - 1));
    }
    duk_put_global_string(ctx, "args");

    // run script
    script_load(ctx, argv[1]);
    if (duk_peval(ctx) == 0) {
        duk_pop(ctx);/* ignore result */
    } else {
        printf("Error running: %s\n", duk_safe_to_string(ctx, -1));
    }

    script_free(ctx);

#else
    printf("JAVASCRIPT not supported. Please run duktape_prepare.sh under tools.\n");
#endif //JAVASCRIPT_SUPPORT

    return 0;
}

// EOF