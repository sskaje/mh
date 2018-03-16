/**
 * src/app/scripts/search.c -- JavaScript API: search
 *   mh_search
 *   mh_search_update
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

static duk_ret_t native_search(duk_context *ctx)
{
    const char *buf    = duk_require_string(ctx, 0);

    MHContext *mh = MHGetGlobalContext();

    cmd_search_bytes(mh, (char *)buf, strlen(buf));

    duk_push_uint(ctx, mh->result_count);

    return 1;
}

static duk_ret_t native_search_update(duk_context *ctx)
{
    const char *buf    = duk_require_string(ctx, 0);

    MHContext *mh = MHGetGlobalContext();

    cmd_update_search_bytes(mh, (char *)buf, strlen(buf));

    duk_push_uint(ctx, mh->result_count);

    return 1;
}

void script_search_register(duk_context *ctx)
{
    duk_push_c_function(ctx, native_search, 1);
    duk_put_prop_string(ctx, -2, "mh_search");

    duk_push_c_function(ctx, native_search_update, 1);
    duk_put_prop_string(ctx, -2, "mh_search_update");
}

// EOF
