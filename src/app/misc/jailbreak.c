/**
 * src/app/misc/jailbreak.c -- iOS Jailbreak related functions
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

#include <errno.h>
#include <sysexits.h>
#include <unistd.h>
#include <dlfcn.h>

#include <sys/stat.h>


typedef unsigned int uint32_t;

/* Set platform binary flag */
#define FLAG_PLATFORMIZE (1 << 1)
/**
 * function for jailbroken iOS 11 by Electra
 *
 * @license GPL-3.0 (cydia) https://github.com/ElectraJailbreak/cydia/blob/master/COPYING
 * @see https://github.com/coolstar/electra/blob/master/docs/getting-started.md
 * @see https://github.com/ElectraJailbreak/cydia/blob/master/cydo.cpp
 */
void patch_setuidandplatformize()
{
    void* handle = dlopen("/usr/lib/libjailbreak.dylib", RTLD_LAZY);
    if (!handle) return;

    // Reset errors
    dlerror();

    typedef void (*fix_setuid_prt_t)(pid_t pid);
    fix_setuid_prt_t setuidptr = (fix_setuid_prt_t)dlsym(handle, "jb_oneshot_fix_setuid_now");

    typedef void (*fix_entitle_prt_t)(pid_t pid, uint32_t what);
    fix_entitle_prt_t entitleptr = (fix_entitle_prt_t)dlsym(handle, "jb_oneshot_entitle_now");

    setuidptr(getpid());

    setuid(0);

    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        return;
    }

    entitleptr(getpid(), FLAG_PLATFORMIZE);
}