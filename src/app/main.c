/**
 * src/app/main.c -- mh_cli main file
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

char *cmds[] = {
        "process-list",

        "open", "close",
        "exit", "quit",

        "search-string", "search-hex",
        "update-search-string", "update-search-hex",
        "result",

        "memory-read", "memory-write",

        "script-run",

        "dyld",
        "vm-region",

        "bytes2hex", "hex2bytes", "float2hex", "double2hex", "int2hex",

        "clear",

        "help",
        NULL
};


void completion(const char *buf, linenoiseCompletions *lc)
{
    size_t i;

    for (i = 0; cmds[i] != NULL; ++i) {
        if (strncmp(buf, cmds[i], strlen(buf)) == 0) {
            linenoiseAddCompletion(lc, cmds[i]);
        }
    }
}

static MHContext *mh_global_context = NULL;

int MHSaveGlobalContext(MHContext *context)
{
    mh_global_context = context;
    return 0;
}

MHContext *MHGetGlobalContext()
{
    return mh_global_context;
}

void prepareLineedit()
{
    linenoiseSetCompletionCallback(completion);
    linenoiseHistorySetMaxLen(300);
}

#define MIN_ARGC_REQUIRED(AC) \
        if (ac < (AC)) {      \
            printf("Error: missing parameters\n\n"); \
            cmd_help();       \
            goto NEXT;        \
        }

int main(int argc, char **argv) {
#ifdef IOS_JAILBREAK_ELECTRA
    patch_setuidandplatformize();
#endif

    if (getuid() != 0) {
        printf("Please run as 'root'\n");
        exit(-1);
    }

#ifdef IOS_JAILBREAK_ELECTRA
    setuid(0);
    setgid(0);
#endif

    char *input, shell_prompt[64];

    char historyFile[100];
    snprintf(historyFile, sizeof(historyFile), "%s/.mhapp_history", getenv("HOME"));
    linenoiseHistoryLoad(historyFile);

    prepareLineedit();

    // initialize context
    MHContext *context = MH_new();
    MHSaveGlobalContext(context);

    if (argc > 1) {
        cmd_open(context, atoi(argv[1]));
    }

    for (;;) {
        if (context != NULL && context->process_id > 0) {
            snprintf(shell_prompt, sizeof(shell_prompt), "%s[%d] > ", "MH", context->process_id);
        } else {
            snprintf(shell_prompt, sizeof(shell_prompt), "%s[nil] > ", "MH");
        }

        input = linenoise(shell_prompt);
        if (!input)
            break;

        linenoiseHistoryAdd(input); /* Add to the history. */
        linenoiseHistorySave(historyFile); /* Save the history on disk. */

        char **av;
        int  ac;
        av = args_parse(input, &ac);
        if (ac < 1) {
            continue;
        }

        if (!strncmp(av[0], "exit", 4) || !strncmp(av[0], "quit", 4)) {
            break;
        } else if (!strncmp(av[0], "clear", 5)) {
            linenoiseClearScreen();
        } else if (!strncmp(av[0], "help", 4)) {
            cmd_help();
        } else if (!strncmp(av[0], "process-list", 2)) {
            cmd_process_list(context);
        } else if (!strncmp(av[0], "open", 4)) {
            // open PID
            cmd_open(context, atoi(av[1]));
        } else if (!strncmp(av[0], "close", 5)) {
            cmd_close(context);
        } else if (!strncmp(av[0], "dyld", 4)) {
            cmd_process_dyld(context);
        } else if (!strncmp(av[0], "vm-region", 9)) {
            cmd_process_vm_region(context);
        } else if (!strncmp(av[0], "search-string", 13)) {
            MIN_ARGC_REQUIRED(2)

            cmd_search_bytes(context, av[1], strlen(av[1]));
        } else if (!strncmp(av[0], "search-hex", 10)) {
            MIN_ARGC_REQUIRED(2)

            int len = (int) strlen(av[1]);
            if (len % 2 != 0) {
                printf("Bad hex string length\n");
                goto NEXT;
            }
            cmd_search_bytes(context, (char *) mh_hex2bytes(av[1], (size_t) len), (size_t) (len / 2));
        } else if (!strncmp(av[0], "update-search-string", 20)) {
            MIN_ARGC_REQUIRED(2)

            cmd_update_search_bytes(context, av[1], strlen(av[1]));
        } else if (!strncmp(av[0], "update-search-hex", 17)) {
            MIN_ARGC_REQUIRED(2)

            int len = (int) strlen(av[1]);
            if (len % 2 != 0) {
                printf("Bad hex string length\n");
                goto NEXT;
            }
            cmd_update_search_bytes(context, (char *) mh_hex2bytes(av[1], (size_t) len), (size_t) (len / 2));
        } else if (!strncmp(av[0], "result", 6)) {
            cmd_search_result_list(context);
        } else if (!strncmp(av[0], "memory-read", 11)) {
            MIN_ARGC_REQUIRED(3)

            // memory-read ADDRESS BYTES
            mach_vm_address_t address = (mach_vm_address_t) strtoull(av[1], NULL, 16);
            mach_vm_size_t    size    = (mach_vm_size_t) strtoull(av[2], NULL, 16);

            cmd_memory_read(context, address, size);
        } else if (!strncmp(av[0], "memory-write", 12)) {
            MIN_ARGC_REQUIRED(3)

            // memory-write ADDRESS HEX-STRING
            mach_vm_address_t address = (mach_vm_address_t) strtoull(av[1], NULL, 16);
            const char        *bytes  = (const char *) mh_hex2bytes(av[2], strlen(av[2]));
            mh_dump_hex((void *) bytes, strlen(av[2]) / 2, 0);

            cmd_memory_write(context, address, (void *) bytes, (mach_msg_type_number_t) sizeof(bytes));

#if JAVASCRIPT_SUPPORT
            } else if (!strncmp(av[0], "script-run", 10)) {
            MIN_ARGC_REQUIRED(2)
            cmd_script_run(context, av[1]);
#endif //JAVASCRIPT_SUPPORT

        } else if (!strncmp(av[0], "bytes2hex", 8)) {
            MIN_ARGC_REQUIRED(2)
            cmd_utils_bytes2hex(av[1], strlen(av[1]));
        } else if (!strncmp(av[0], "hex2bytes", 8)) {
            MIN_ARGC_REQUIRED(2)
            cmd_utils_hex2bytes(av[1], strlen(av[1]));
        } else if (!strncmp(av[0], "float2hex", 8)) {
            MIN_ARGC_REQUIRED(2)
            cmd_utils_float2hex(strtof(av[1], NULL));;
        } else if (!strncmp(av[0], "double2hex", 9)) {
            MIN_ARGC_REQUIRED(2)
            cmd_utils_double2hex(strtod(av[1], NULL));
        } else if (!strncmp(av[0], "int2hex", 7)) {
            MIN_ARGC_REQUIRED(2)
            cmd_utils_int2hex(strtoll(av[1], NULL, 10));
        } else {
            printf("invalid input: %s\n", input);
        }

        NEXT:
        free(av);
        free(input);
    }

    MH_free(context);

    return 0;
}

// EOF