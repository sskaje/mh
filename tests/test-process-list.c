#include <stdio.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <mach/machine.h>
#include <stdbool.h>
#include <assert.h>


int main() {

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
        || (processListActualSize == 0))
    {
        free(processList);
        return -3;
    }


    bool isRoot = (geteuid() == 0);

    const size_t processCount = processListActualSize / sizeof(*processList);

    printf("Process count=%d\n", (int) processCount);

    for (size_t processIndex = 0; processIndex < processCount; processIndex++)
    {
        struct kinfo_proc processInfo = processList[processIndex];

        uid_t uid = processInfo.kp_eproc.e_ucred.cr_uid;
        pid_t processIdentifier = processInfo.kp_proc.p_pid;

        // We want user processes, not zombies!
        bool isBeingForked = (processInfo.kp_proc.p_stat & SIDL) != 0;
        if (processIdentifier != -1 && (uid == getuid() || isRoot) && !isBeingForked)
        {
            cpu_type_t cpuType = 0;
            size_t cpuTypeSize = sizeof(cpuType);

            // Grab CPU architecture type
            _processTypeName[_processTypeNameLength - 1] = processIdentifier;
            if (sysctl(_processTypeName, (u_int)_processTypeNameLength, &cpuType, &cpuTypeSize, NULL, 0) == 0)
            {
                bool is64Bit = ((cpuType & CPU_ARCH_ABI64) != 0);

                const char *internalName = processInfo.kp_proc.p_comm;

                printf("PID=% 5d\t64bit=%d\tName=%s\n", processIdentifier, is64Bit, internalName);
            }
        }
    }

    free(processList);


    return 0;
}