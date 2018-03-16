/**
 * src/app/scripts.c -- source file for mh app javascript functions
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

#include "scripts.h"
#include "scripts/script-functions.h"

/**
 * Initialize duktape
 *
 * @return
 */
duk_context *script_new()
{
    duk_context *ctx = NULL;

    ctx = duk_create_heap_default();
    if (!ctx) {
        printf("Failed to create a Duktape heap.\n");
        return NULL;
    }

    duk_console_init(ctx, DUK_CONSOLE_PROXY_WRAPPER /*flags*/);

    duk_push_global_object(ctx);

    SCRIPT_LOAD()

    return ctx;
}

/**
 * Destroy duktape
 *
 * @param ctx
 */
void script_free(duk_context *ctx)
{
    duk_destroy_heap(ctx);
}


#define SCRIPT_LOAD_BUF_SIZE 16384

/**
 * Load script file and run
 *
 * @param ctx
 * @param filename
 */
void script_load(duk_context *ctx, const char *filename)
{
    FILE   *fp;
    size_t len;
    char   buf[SCRIPT_LOAD_BUF_SIZE];

    fp = fopen(filename, "rb");
    if (fp) {
        size_t size = 0;

        while (!feof(fp)) {
            duk_require_stack(ctx, 1);
            len = fread((void *) buf, 1, SCRIPT_LOAD_BUF_SIZE, fp);
            duk_push_lstring(ctx, (const char *) buf, (duk_size_t) len);
            ++size;
        }

        if (size > 1) {
            duk_concat(ctx, (duk_idx_t) size);
        }

        fclose(fp);
    } else {
        duk_push_undefined(ctx);

        perror("Error");
    }
}

/**
 * Convert hex string to vm_address
 * Used in script functions
 *
 * @param ctx
 * @param idx
 * @return
 */
mach_vm_address_t duk_require_address(duk_context *ctx, duk_idx_t idx)
{
    size_t size;
    const char *addr = duk_require_lstring(ctx, idx, &size);
    return (mach_vm_address_t) strtoll(addr, NULL, 16);
}

// EOF