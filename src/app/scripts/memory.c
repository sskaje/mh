/**
 * src/app/scripts/memory.c -- JavaScript API: memory
 *   mh_memory_write
 *   mh_memory_read
 *   mh_memory_dump
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

#include <app/app.h>
#include "../scripts.h"

/**
 * memory write api
 *
 * @param ctx
 * @return
 */
static duk_ret_t native_memory_write(duk_context *ctx)
{
    mach_vm_address_t address = duk_require_address(ctx, 0);
    const char        *buf    = duk_require_string(ctx, 1);

    char *raw = (char *) mh_hex2bytes(buf, strlen(buf));

    MHContext *mh = MHGetGlobalContext();
    mh_write_memory(&mh->process, address, raw, strlen(buf) / 2);

    duk_push_true(ctx);
    return 1;
}
/**
 * memory read api
 *
 * @param ctx
 * @return
 */
static duk_ret_t native_memory_read(duk_context *ctx)
{
    mach_vm_address_t address = duk_require_address(ctx, 0);
    mach_vm_size_t    size    = duk_require_uint(ctx, 1);

    MHContext *mh     = MHGetGlobalContext();
    void      *buffer = mh_read_memory(&mh->process, address, &size);

    // return hex ?
    duk_push_string(ctx, mh_bytes2hex(buffer, size));

    // value returned on top of stack
    return 1;
}

/**
 * memory dump api
 *
 * @param ctx
 * @return
 */
static duk_ret_t native_memory_dump(duk_context *ctx)
{
    mach_vm_address_t address = duk_require_address(ctx, 0);
    mach_vm_size_t    size    = duk_require_uint(ctx, 1);

    MHContext *mh = MHGetGlobalContext();

    mh_dump_memory(&mh->process, address, size);

    duk_push_true(ctx);
    return 1;
}

void script_memory_register(duk_context *ctx)
{
    duk_push_c_function(ctx, native_memory_write, 2);
    duk_put_prop_string(ctx, -2, "mh_memory_write");

    duk_push_c_function(ctx, native_memory_read, 2);
    duk_put_prop_string(ctx, -2, "mh_memory_read");

    duk_push_c_function(ctx, native_memory_dump, 2);
    duk_put_prop_string(ctx, -2, "mh_memory_dump");

}

// EOF
