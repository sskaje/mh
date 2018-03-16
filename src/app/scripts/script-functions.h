/**
 * src/app/scripts/script-functions.h -- JavaScript API common header files
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

#ifndef MH_SCRIPT_FUNCTIONS_H
#define MH_SCRIPT_FUNCTIONS_H


#define SCRIPT_REGISTER(name) \
    script_##name##_register(ctx);

#define SCRIPT_LOAD()          \
    SCRIPT_REGISTER(utils)     \
    SCRIPT_REGISTER(memory)    \
    SCRIPT_REGISTER(result)    \
    SCRIPT_REGISTER(search)

void script_utils_register(duk_context *ctx);

void script_memory_register(duk_context *ctx);

void script_result_register(duk_context *ctx);

void script_search_register(duk_context *ctx);
/*
The function can return one of the following:

Return value 1 indicates that the value on the stack top is to be interpreted as a return value.
Return value 0 indicates that there is no explicit return value on the value stack; an undefined is returned to caller.
A negative return value indicates that an error is to be automatically thrown. Error codes named DUK_RET_xxx map to specific kinds of errors (do not confuse these with DUK_ERR_xxx which are positive values).
A return value higher than 1 is currently undefined, as Ecmascript doesn't support multiple return values in Edition 5.1. (Values higher than 1 may be taken into to support multiple return values in Ecmascript Edition 6.)

 */

#endif //MH_SCRIPT_FUNCTIONS_H
