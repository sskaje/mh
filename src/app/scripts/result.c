/**
 * src/app/scripts/result.c -- JavaScript API: result
 *   mh_result_count
 *   mh_result_get
 *   mh_result_next
 *   mh_result_remove
 *   mh_result_reset
 *   mh_result_free
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
#include <mh/mh.h>
#include "../scripts.h"

static struct result_entry * result_ptr;

/**
 * Get result count from context
 *
 * @param ctx
 * @return
 */
static duk_ret_t native_result_count(duk_context *ctx)
{
    MHContext *mh = MHGetGlobalContext();

    // push result count to stack
    duk_push_uint(ctx, (duk_uint_t) mh->result_count);

    return 1;
}

/**
 * Initialize result ptr
 *
 * @param ctx
 * @return
 */
static duk_ret_t native_result_reset(duk_context *ctx)
{
    MHContext *mh = MHGetGlobalContext();

    result_ptr = STAILQ_FIRST(&mh->results);

    duk_push_true(ctx);

    return 1;
}

/**
 * Get current result
 *
 * @param ctx
 * @return
 */
static duk_ret_t native_result_get(duk_context *ctx)
{
    // get first result

    MHContext *mh = MHGetGlobalContext();

    if (result_ptr == NULL) {
        result_ptr = STAILQ_FIRST(&mh->results);
    }

    // push result
//    (struct result_entry *) result_ptr;
    duk_idx_t arr_idx;

    // 0x0123456789abcdef
    char tmp_address[20] = {0};

    arr_idx = duk_push_array(ctx);

    sprintf(tmp_address, "0x%llx", result_ptr->address);
    duk_push_string(ctx, tmp_address);
    duk_put_prop_index(ctx, arr_idx, 0);

    sprintf(tmp_address, "0x%llx", result_ptr->region_address);
    duk_push_string(ctx, tmp_address);
    duk_put_prop_index(ctx, arr_idx, 1);

    duk_push_uint(ctx, result_ptr->region_size);
    duk_put_prop_index(ctx, arr_idx, 2);


    return 1;
}

/**
 * Move result ptr to next element
 *
 * @param ctx
 * @return
 */
static duk_ret_t native_result_next(duk_context *ctx)
{
    MHContext *mh = MHGetGlobalContext();

    if (result_ptr == NULL) {
        result_ptr = STAILQ_FIRST(&mh->results);
    } else {
        result_ptr = STAILQ_NEXT(result_ptr, next);
    }

    // push true if element exists
    if (result_ptr != NULL) {
        duk_push_true(ctx);
    } else {
        duk_push_false(ctx);
    }

    return 1;
}

/**
 * Free element
 *
 * @param ctx
 * @return
 */
static duk_ret_t native_result_free(duk_context *ctx)
{
    MHContext *mh = MHGetGlobalContext();

    mh_result_free(&mh->results);

    duk_push_true(ctx);

    return 1;
}

/**
 * Remove element from result by address
 *
 * @param ctx
 * @return
 */
static duk_ret_t native_result_remove(duk_context *ctx)
{
    mach_vm_address_t address = duk_require_address(ctx, 0);

    MHContext *mh = MHGetGlobalContext();

    // move to first if current ptr will be removed
    if (result_ptr->address == address) {
        result_ptr = STAILQ_FIRST(&mh->results);
    }
    // remove
    if (mh_result_remove_by_address(&mh->results, address)) {
        mh->result_count --;
    }

    duk_push_true(ctx);

    return 1;
}


/**
 * Register result functions
 *
 * @param ctx
 */
void script_result_register(duk_context *ctx)
{
    duk_push_c_function(ctx, native_result_count, 0);
    duk_put_prop_string(ctx, -2, "mh_result_count");

    duk_push_c_function(ctx, native_result_get, 0);
    duk_put_prop_string(ctx, -2, "mh_result_get");

    duk_push_c_function(ctx, native_result_next, 0);
    duk_put_prop_string(ctx, -2, "mh_result_next");

    duk_push_c_function(ctx, native_result_remove, 1);
    duk_put_prop_string(ctx, -2, "mh_result_remove");

    duk_push_c_function(ctx, native_result_reset, 0);
    duk_put_prop_string(ctx, -2, "mh_result_reset");

    duk_push_c_function(ctx, native_result_free, 0);
    duk_put_prop_string(ctx, -2, "mh_result_free");

}

// EOF