
#include <mh/mh.h>


int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Missing PID\n");
        return -1;
    }

    int err;

    MHProcess process;
    err = mh_open_task(atoi(argv[1]), &process);
    if (err) {
        exit(err);
    }

    printf("Current PID=%d\n", process.process_id);

    MHDyldInfo dyldInfo;
    mh_read_dyld(&process, &dyldInfo);

    mach_vm_address_t address = 0x0;
    mach_vm_size_t size;

    vm_region_basic_info_data_64_t info;
    mach_msg_type_number_t infoCount;
    mach_port_t objectName = MACH_PORT_NULL;

    while (1) {
        infoCount = VM_REGION_BASIC_INFO_COUNT_64;
        err = mach_vm_region(process.process_task, &address, &size, VM_REGION_BASIC_INFO_64, (vm_region_info_t) &info,
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
                (info.protection & VM_PROT_READ)    ? 'r' : '-',
                (info.protection & VM_PROT_WRITE)   ? 'w' : '-',
                (info.protection & VM_PROT_EXECUTE) ? 'x' : '-',
                (info.max_protection & VM_PROT_READ)    ? 'r' : '-',
                (info.max_protection & VM_PROT_WRITE)   ? 'w' : '-',
                (info.max_protection & VM_PROT_EXECUTE) ? 'x' : '-',
                mh_usertag_to_text(&process, address, size)
        );
//
//        mach_vm_address_t regionAddress = address;
//        mach_vm_size_t regionSize = size;
//        vm_region_submap_info_data_64_t submapInfo;
//
//        mach_msg_type_number_t subInfoCount;
//        natural_t depth = 0;
//        while (1) {
//            err = mach_vm_region_recurse(
//                    process.process_task,
//                    &regionAddress,
//                    &regionSize,
//                    &depth,
//                    (vm_region_recurse_info_t) &submapInfo,
//                    &subInfoCount
//            );
//
//            if (err != KERN_SUCCESS) {
//                printf("task_for_pid() failure: %d - %s\n", err, mach_error_string(err));
//                break;
//            }
//
//            printf("\t[%d]\n", submapInfo.is_submap);
//            ++depth;
//        }


        address += size;
    }

    return 0;
}
