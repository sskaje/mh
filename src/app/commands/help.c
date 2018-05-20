/**
 * src/app/commands/help.c -- mh_cli: help command
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

#include "commands.h"

void cmd_help()
{
    printf(
            "=== Memory Hacker ===\n"
            "Author: sskaje\n\n"
            "Command                                 Description\n"
            " process-list                            Print process list\n"
            " open PID                                Open a task\n"
            " close                                   Close current task\n"
            " [update-]search-hex HEX                 Search HEX bytes, like search-hex 1a2b3c4d\n"
            " [update-]search-string STR              Search string, like search-string hello\n"
            " memory-read ADDRESS SIZE                Read memory data at ADDRESS, both ADDRESS and SIZE are in HEX\n"
            " memory-write ADDRESS STR                Write data to ADDRESS, ADDRESS in HEX, STR in BYTES\n"
            " memory-write-hex ADDRESS HEX            Write hex data to ADDRESS, both ADDRESS and HEXSTR are in HEX\n"
            " result                                  Print result list, last round by default\n"
            " dyld                                    Print dyld info\n"
            " vm-region                               Print mach_vm_region()\n"
            " bytes2hex STR                           Bytes to hex string\n"
            " hex2bytes HEX                           Hex string to bytes\n"
            " float2hex FLOAT                         Float number to hex\n"
            " double2hex DOUBLE                       Double number to hex\n"
            " int2hex INTEGER                         Integer to hex\n"
            " clear                                   Clear screen\n"
            " help                                    Print this message\n"
            "\n"
    );
}

// EOF
