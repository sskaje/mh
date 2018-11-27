/**
 * src/app/scripts/object.c -- javascript MH object
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

/**
 * class MH {
 *   private pid_t      pid;
 *   private MHContext *mhctx;
 * }
 *
 */

#include <app/app.h>
#include "../scripts.h"
#include "script-common.h"



/**
 * MH
 *
 * var mh = new MH();
 */
duk_ret_t mhobj_constructor(duk_context *ctx)
{
    if (!duk_is_constructor_call(ctx)) {
        return DUK_RET_TYPE_ERROR;
    }

    MHContext *mh = MH_new();
    duk_push_this(ctx);
    duk_push_pointer(ctx, mh);
    duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL(MHOBJ_HIDDEN_SYMBOL_CONTEXT));

    duk_push_int(ctx, 0);
    duk_put_prop_string(ctx, -2, "pid");

    return 0;
}

/**
 * MH.prototype.open
 *
 * bool mh.open(int pid)
 */
duk_ret_t mhobj_open(duk_context *ctx)
{
    MHContext *mh = script_get_object_mh_context(ctx);

    pid_t pid = (pid_t) duk_require_uint(ctx, 0);

    int err = mh_open_task(pid, &mh->process);
    if (err) {
        printf("Failed to open task, pid=%d. Error: %d\n", pid, err);
        duk_push_false(ctx);
        return 1;
    }

    mh_read_dyld(&mh->process, &mh->dyld);
    mh->process_id = mh->process.process_id;

    // save to this.pid
    duk_push_this(ctx);
    duk_push_int(ctx, pid);
    duk_put_prop_string(ctx, -2, "pid");

    duk_push_true(ctx);
    return 1;
}

/**
 * MH.prototype.close
 *
 * bool mh.close()
 */
duk_ret_t mhobj_close(duk_context *ctx)
{
    MHContext *mh = script_get_object_mh_context(ctx);

    // reset process
    mh_reset_process(&mh->process);
    // reset dyld

    // reset context
    mh->process_id = 0;

    MH_free(mh);

    duk_push_this(ctx);
    duk_push_int(ctx, 0);
    duk_put_prop_string(ctx, -2, "pid");

    duk_push_true(ctx);
    return 1;
}


/**
 * MH.prototype.get_pid
 *
 * returns current open pid
 *
 * mh.get_pid()
 *
 */
duk_ret_t mhobj_get_pid(duk_context *ctx)
{
    duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "pid");
    pid_t pid = (pid_t) duk_get_int(ctx, -1);
    duk_push_int(ctx, pid);

    return 1;
}


/**
 * MH.prototype.search
 *
 * Start a new search
 * returns result count
 *
 * int mh.search(string needle)
 *
 */
static duk_ret_t mhobj_search(duk_context *ctx)
{
    const char *buf    = duk_require_string(ctx, 0);

    MHContext *mh = script_get_object_mh_context(ctx);

    cmd_search_bytes(mh, (char *)buf, strlen(buf));

    duk_push_uint(ctx, mh->result_count);

    return 1;
}

/**
 * MH.prototype.search_update
 *
 * Search from last result
 * returns result count
 *
 * int mh.search_update(string needle)
 */
static duk_ret_t mhobj_search_update(duk_context *ctx)
{
    const char *buf    = duk_require_string(ctx, 0);

    MHContext *mh = script_get_object_mh_context(ctx);

    cmd_update_search_bytes(mh, (char *)buf, strlen(buf));

    duk_push_uint(ctx, mh->result_count);

    return 1;
}


/**
 * MH.prototype.memory_write
 *
 * Write memory
 *
 * bool mh.memory_write(string hex_address, string data)
 */
static duk_ret_t mhobj_memory_write(duk_context *ctx)
{
    mach_vm_address_t address = duk_require_address(ctx, 0);
    const char        *buf    = duk_require_string(ctx, 1);

    char *raw = (char *) mh_hex2bytes(buf, strlen(buf));

    MHContext *mh = script_get_object_mh_context(ctx);
    mh_write_memory(&mh->process, address, raw, strlen(buf) / 2);

    duk_push_true(ctx);
    return 1;
}

/**
 * MH.prototype.memory_read
 *
 * Read memory
 *
 * string mh.memory_read(string hex_address, int size)
 */
static duk_ret_t mhobj_memory_read(duk_context *ctx)
{
    mach_vm_address_t address = duk_require_address(ctx, 0);
    mach_vm_size_t    size    = duk_require_uint(ctx, 1);

    MHContext *mh = script_get_object_mh_context(ctx);
    void      *buffer = mh_read_memory(&mh->process, address, &size);

    // return hex ?
    duk_push_string(ctx, mh_bytes2hex(buffer, size));

    // value returned on top of stack
    return 1;
}

/**
 * MH.prototype.memory_dump
 *
 * Dump memory
 *
 * bool mh.memory_read(string hex_address, int size)
 */
static duk_ret_t mhobj_memory_dump(duk_context *ctx)
{
    mach_vm_address_t address = duk_require_address(ctx, 0);
    mach_vm_size_t    size    = duk_require_uint(ctx, 1);

    printf("%016llx\t%016llx\n", address, size);

    MHContext *mh = script_get_object_mh_context(ctx);
    mh_dump_memory(&mh->process, address, size);

    duk_push_true(ctx);
    return 1;
}

/**
 * MH.prototype.result_count
 *
 * Result count
 *
 * int mh.result_count()
 */
static duk_ret_t mhobj_result_count(duk_context *ctx)
{
    MHContext *mh = script_get_object_mh_context(ctx);

    // push result count to stack
    duk_push_uint(ctx, (duk_uint_t) mh->result_count);

    return 1;
}

/**
 * MH.prototype.result_reset
 *
 * Result count
 *
 * bool mh.result_reset()
 */
static duk_ret_t mhobj_result_reset(duk_context *ctx)
{
    MHContext *mh = script_get_object_mh_context(ctx);
    mh->result_ptr = STAILQ_FIRST(&mh->results);

    duk_push_true(ctx);

    return 1;
}

/**
 * MH.prototype.result_get
 *
 * get result
 * returns [address, region_address, region_size]
 *
 * array mh.result_get()
 */
static duk_ret_t mhobj_result_get(duk_context *ctx)
{
    // get first result

    MHContext *mh = script_get_object_mh_context(ctx);

    if (mh->result_ptr == NULL) {
        mh->result_ptr = STAILQ_FIRST(&mh->results);
    }

    // push result
    duk_idx_t arr_idx;

    // 0x0123456789abcdef
    char tmp_address[20] = {0};

    arr_idx = duk_push_array(ctx);

    sprintf(tmp_address, "0x%llx", mh->result_ptr->address);
    duk_push_string(ctx, tmp_address);
    duk_put_prop_index(ctx, arr_idx, 0);

    sprintf(tmp_address, "0x%llx", mh->result_ptr->region_address);
    duk_push_string(ctx, tmp_address);
    duk_put_prop_index(ctx, arr_idx, 1);

    duk_push_uint(ctx, mh->result_ptr->region_size);
    duk_put_prop_index(ctx, arr_idx, 2);


    return 1;
}

/**
 * MH.prototype.result_next
 *
 * Move to next
 *
 * bool mh.result_next()
 */
static duk_ret_t mhobj_result_next(duk_context *ctx)
{
    MHContext *mh = script_get_object_mh_context(ctx);

    if (mh->result_ptr == NULL) {
        mh->result_ptr = STAILQ_FIRST(&mh->results);
    } else {
        mh->result_ptr = STAILQ_NEXT(mh->result_ptr, next);
    }

    // push true if element exists
    if (mh->result_ptr != NULL) {
        duk_push_true(ctx);
    } else {
        duk_push_false(ctx);
    }

    return 1;
}

/**
 * MH.prototype.result_free
 *
 * Free result
 *
 * bool mh.result_free()
 */
static duk_ret_t mhobj_result_free(duk_context *ctx)
{
    MHContext *mh = script_get_object_mh_context(ctx);

    mh_result_free(&mh->results);
    mh->result_ptr = NULL;

    duk_push_true(ctx);

    return 1;
}

/**
 * MH.prototype.result_remove
 *
 * Remove result by hex_address
 *
 * bool mh.result_remove(string hex_address)
 */
static duk_ret_t mhobj_result_remove(duk_context *ctx)
{
    mach_vm_address_t address = duk_require_address(ctx, 0);

    MHContext *mh = script_get_object_mh_context(ctx);

    // move to first if current ptr will be removed
    if (mh->result_ptr->address == address) {
        mh->result_ptr = STAILQ_FIRST(&mh->results);
    }
    // remove
    if (mh_result_remove_by_address(&mh->results, address)) {
        mh->result_count --;
    }

    duk_push_true(ctx);

    return 1;
}


void script_object_register(duk_context *ctx) {
    /* Push constructor function; all Duktape/C functions are
     * "constructable" and can be called as 'new Foo()'.
     */
    duk_push_c_function(ctx, mhobj_constructor, 0);

    /* Push MH.prototype object. */
    duk_push_object(ctx);  /* -> stack: [ MH proto ] */

    /* Set MH.prototype.open. */
    duk_push_c_function(ctx, mhobj_open, 1);
    duk_put_prop_string(ctx, -2, "open");

    /* Set MH.prototype.close. */
    duk_push_c_function(ctx, mhobj_close, 0);
    duk_put_prop_string(ctx, -2, "close");

    /* Set MH.prototype.pid. */
    duk_push_c_function(ctx, mhobj_get_pid, 0);
    duk_put_prop_string(ctx, -2, "get_pid");

    /* Set MH.prototype.search. */
    duk_push_c_function(ctx, mhobj_search, 1);
    duk_put_prop_string(ctx, -2, "search");

    /* Set MH.prototype.search_update. */
    duk_push_c_function(ctx, mhobj_search_update, 1);
    duk_put_prop_string(ctx, -2, "search_update");

    /* Set MH.prototype.memory_write. */
    duk_push_c_function(ctx, mhobj_memory_write, 2);
    duk_put_prop_string(ctx, -2, "memory_write");

    /* Set MH.prototype.memory_read. */
    duk_push_c_function(ctx, mhobj_memory_read, 2);
    duk_put_prop_string(ctx, -2, "memory_read");

    /* Set MH.prototype.memory_dump. */
    duk_push_c_function(ctx, mhobj_memory_dump, 2);
    duk_put_prop_string(ctx, -2, "memory_dump");


    /* Set MH.prototype.result_count. */
    duk_push_c_function(ctx, mhobj_result_count, 0);
    duk_put_prop_string(ctx, -2, "result_count");

    /* Set MH.prototype.result_reset. */
    duk_push_c_function(ctx, mhobj_result_reset, 0);
    duk_put_prop_string(ctx, -2, "result_reset");

    /* Set MH.prototype.result_get. */
    duk_push_c_function(ctx, mhobj_result_get, 0);
    duk_put_prop_string(ctx, -2, "result_get");

    /* Set MH.prototype.result_next. */
    duk_push_c_function(ctx, mhobj_result_next, 0);
    duk_put_prop_string(ctx, -2, "result_next");

    /* Set MH.prototype.result_free. */
    duk_push_c_function(ctx, mhobj_result_free, 0);
    duk_put_prop_string(ctx, -2, "result_free");

    /* Set MH.prototype.result_remove. */
    duk_push_c_function(ctx, mhobj_result_remove, 1);
    duk_put_prop_string(ctx, -2, "result_remove");


    /* Set MH.prototype = proto */
    duk_put_prop_string(ctx, -2, "prototype");  /* -> stack: [ MH ] */

    /* Finally, register MH to the global object */
    duk_put_global_string(ctx, "MH");  /* -> stack: [ ] */
}

// EOF
