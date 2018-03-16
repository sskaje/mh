#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <mh/mh.h>

#include "linenoise.h"


char* cmds[] = {
        "ps",

        "open", "close",
        "exit", "quit",

        "search hex", "search string",

        "dump",

        // "script",

        "dyld",
        "vm-regions",

        "clear",

        "help",
        NULL
};

struct MAHint {
    const char command[16];
};

char* hints[] = {
        "dump ADDRESS SIZE",
        "open PID",
        NULL
};

void completion(const char *buf, linenoiseCompletions *lc) {
    size_t i;

    for (i = 0;  cmds[i] != NULL; ++i) {
        if (strncmp(buf, cmds[i], strlen(buf)) == 0) {
            linenoiseAddCompletion(lc, cmds[i]);
        }
    }
}

char *hintsCallback(const char *buf, int *color, int *bold) {

    size_t i;

    for (i = 0;  hints[i] != NULL; ++i) {
        if (strncmp(buf, hints[i], strlen(buf)) == 0) {
            *color = 35;
            *bold = 0;
            return hints[i] + strlen(buf);
        }
    }

    return NULL;
}

void cmd_help()
{
    printf(
        "=== Memory Hacker ===\n"
        "Author: sskaje\n\n"
        "Command                        Description\n"
        "ps                             Print process list\n"
        "open [PID]                     Open a task\n"
        "close                          Close current task\n"
        "search hex [HEX]               Search HEX bytes, like search-hex 1a2b3c4d\n"
        "search string [STRING]         Search string, like search-string hello\n"
        "rescan                         Redo search to filter results\n"
        "dyld                           Print dyld info\n"
        "vm-regions                     Print mach_vm_region()\n"
        "clear                          Clear screen\n"
        "help                           Print this message\n"
        "\n"
    );
}

int cmd_ps()
{
    int _processTypeName[CTL_MAXNAME];
    size_t _processTypeNameLength = 0;


    const size_t maxLength = sizeof(_processTypeName) / sizeof(*_processTypeName);
    _processTypeNameLength = maxLength;

#define SYSCTL_PROC_CPUTYPE "sysctl.proc_cputype"

    int result = sysctlnametomib(SYSCTL_PROC_CPUTYPE, _processTypeName, &_processTypeNameLength);
    assert(result == 0);
    assert(_processTypeNameLength < maxLength);

    // last element in the name MIB will be the process ID that the client fills in before calling sysctl()
    _processTypeNameLength++;

    int processListName[] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL};
    size_t processListNameLength = sizeof(processListName) / sizeof(*processListName);

    size_t processListRequestSize = 0;
    if (sysctl(processListName, (u_int)processListNameLength, NULL, &processListRequestSize, NULL, 0) != 0) return -1;
    struct kinfo_proc *processList = malloc(processListRequestSize);
    if (processList == NULL) return -2;

    size_t processListActualSize = processListRequestSize;
    if (sysctl(processListName, (u_int)processListNameLength, processList, &processListActualSize, NULL, 0) != 0
        || (processListActualSize == 0)) {
        free(processList);
        return -3;
    }

    const size_t processCount = processListActualSize / sizeof(*processList);

    printf("PID\tUID\tbits\tName\n");

    for (size_t processIndex = processCount; processIndex > 0; processIndex--) {
        struct kinfo_proc processInfo = processList[processIndex-1];

        uid_t uid = processInfo.kp_eproc.e_ucred.cr_uid;
        pid_t processIdentifier = processInfo.kp_proc.p_pid;

//        bool isBeingForked = (processInfo.kp_proc.p_stat & SIDL) != 0;
        if (processIdentifier != -1) {
            cpu_type_t cpuType = 0;
            size_t cpuTypeSize = sizeof(cpuType);

            // Grab CPU architecture type
            _processTypeName[_processTypeNameLength - 1] = processIdentifier;
            if (sysctl(_processTypeName, (u_int)_processTypeNameLength, &cpuType, &cpuTypeSize, NULL, 0) == 0) {
                bool is64Bit = ((cpuType & CPU_ARCH_ABI64) != 0);

                const char *internalName = processInfo.kp_proc.p_comm;

                printf("%-5d\t%-5d\t%dbit\t%s\n", processIdentifier, uid, is64Bit ? 64 : 32, internalName);
            }
        }
    }
    printf("Process count=%d\n", (int) processCount);

    free(processList);

    return 0;
}

int main()
{
    if (getuid() != 0) {
        printf("Root only\n");
        exit(-1);
    }

    char* input, shell_prompt[100];

    pid_t task_pid = 0;
    MHProcess process;
    MHDyldInfo dyldInfo;
    int err;

    linenoiseSetCompletionCallback(completion);
    linenoiseSetHintsCallback(hintsCallback);

    char historyFile[100];
    snprintf(historyFile, sizeof(historyFile), "%s/.mhapp_history", getenv("HOME"));
    linenoiseHistoryLoad(historyFile);

    linenoiseHistorySetMaxLen(300);

    for(;;) {
        if (task_pid > 0) {
            snprintf(shell_prompt, sizeof(shell_prompt), "%s[%d] > ", "MH", task_pid);
        } else {
            snprintf(shell_prompt, sizeof(shell_prompt), "%s[nil] > ", "MH");
        }

        input = linenoise(shell_prompt);
        if (!input)
            break;

        if (!strncmp(input, "exit", 4) || !strncmp(input, "quit", 4)) {
            break;
        } else if (!strncmp(input, "clear", 5)) {
            linenoiseClearScreen();
        } else if (!strncmp(input, "help", 4)) {
            cmd_help();
        } else if (!strncmp(input, "ps", 2)) {
            cmd_ps();
        } else if (!strncmp(input, "open", 4)) {
            task_pid = atoi(input + 4);

            err = mh_open_task(task_pid, &process);
            if (err) {
                printf("Failed to open task, pid=%d. Error: %d\n", task_pid, err);
                task_pid = 0;
                goto NEXT;
            }

            printf("Current PID=%d\n", process.process_id);

            mh_read_dyld(&process, &dyldInfo);

        } else if (!strncmp(input, "close", 5)) {
            task_pid = 0;
            mh_reset_process(&process);

        } else if (!strncmp(input, "dyld", 4)) {

            pointer_t *imagePtr = NULL;
            int imageCount = 0;
            mh_read_dyld_images(&process, (pointer_t *) &imagePtr, &imageCount);

            MHImage *images = (MHImage *) imagePtr;

            for (int i = 0; i < imageCount; i++) {
                printf("[%d] ", i);
                mach_vm_address_t tmpSize;

                void *tmp = NULL;

                if (process.is64bit) {
                    tmpSize = sizeof(struct mach_header_64);
                } else {
                    tmpSize = sizeof(struct mach_header);
                }

                mach_vm_read(
                        process.process_task,
                        images[i].load_address,
                        tmpSize,
                        (vm_offset_t *) &tmp,
                        (mach_msg_type_number_t *) &tmpSize
                );

                if (process.is64bit) {
                    struct mach_header_64 *mach = (struct mach_header_64 *) tmp;
                    printf("Bits=64, Magic=%08x, ", mach->magic);
                } else {
                    struct mach_header *mach = (struct mach_header *) tmp;
                    printf("Bits=32, Magic=%08x, ", mach->magic);
                }

                char *imageFilePath = NULL;
                tmpSize = 1024;
                mach_vm_read(
                        process.process_task,
                        images[i].file_path,
                        tmpSize,
                        (vm_offset_t *) &imageFilePath,
                        (mach_msg_type_number_t *) &tmpSize
                );

                printf("%016llx %s\n", images[i].load_address, (const char *) imageFilePath);
            }

            free(images);

        } else if (!strncmp(input, "vm-regions", 10)) {

            mach_vm_address_t address = 0x0;
            mach_vm_size_t size;

            vm_region_basic_info_data_64_t info;
            mach_msg_type_number_t infoCount;
            mach_port_t objectName = MACH_PORT_NULL;

            while (1) {
                infoCount = VM_REGION_BASIC_INFO_COUNT_64;
                err = mach_vm_region(
                        process.process_task, &address, &size, VM_REGION_BASIC_INFO_64,
                        (vm_region_info_t) &info,
                        &infoCount,
                        &objectName);
                if (err != KERN_SUCCESS) {
                    break;
                }

                printf(
                        "0x%016llx-0x%016llx size=0x%08llx offset=%016llx, %c%c%c/%c%c%c, %s\n",
                        address,
                        address + size,
                        size,
                        info.offset,
                        (info.protection & VM_PROT_READ) ? 'r' : '-',
                        (info.protection & VM_PROT_WRITE) ? 'w' : '-',
                        (info.protection & VM_PROT_EXECUTE) ? 'x' : '-',
                        (info.max_protection & VM_PROT_READ) ? 'r' : '-',
                        (info.max_protection & VM_PROT_WRITE) ? 'w' : '-',
                        (info.max_protection & VM_PROT_EXECUTE) ? 'x' : '-',
                        mh_usertag_to_text(&process, address, size)
                );
/*

                mach_vm_address_t regionAddress = address;
                mach_vm_size_t regionSize = size;
                vm_region_submap_info_data_64_t submapInfo;

                mach_msg_type_number_t subInfoCount;
                natural_t depth = 0;
                while (1) {
                    err = mach_vm_region_recurse(
                            process.process_task,
                            &regionAddress,
                            &regionSize,
                            &depth,
                            (vm_region_recurse_info_t) &submapInfo,
                            &subInfoCount
                    );

                    if (err != KERN_SUCCESS) {
                        printf("task_for_pid() failure: %d - %s\n", err, mach_error_string(err));
                        break;
                    }

                    printf("\t[%d]\n", submapInfo.is_submap);
                    ++depth;
                }
*/

                address += size;
            }
        } else if (!strncmp(input, "dump", 4)) {

            mach_vm_address_t address = 0;
            mach_vm_size_t size = 0;

//            address = (mach_vm_address_t) strtoull(argv[2], NULL, 16);
//            size = (mach_vm_size_t) strtoull(argv[3], NULL, 16);

            printf("Read memory: addr=%016llx, size=%016llx\n", address, size);

            mh_dump_memory(&process, address, size);
        } else {

            linenoiseHistoryAdd(input); /* Add to the history. */
            linenoiseHistorySave(historyFile); /* Save the history on disk. */

            printf("input: %s\n", input);
        }

    NEXT:
        free(input);
    }

    return 0;
}