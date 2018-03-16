//
// Created by sskaje on 2017/8/8.
//

#include <mh/mh.h>

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    // if it's iOS, read 512kb every time
//    #define MAX_READ_MEMORY_SIZE 0x80000
    // iPhone 5S下内存已经足够了，可以不需要分段
    #define MAX_READ_MEMORY_SIZE 0
#else
    #define MAX_READ_MEMORY_SIZE 0
#endif


int main(int argc, char* argv[])
{

    if (argc < 4) {
        printf("Usage: %s PID TYPE PATTERN\n", argv[0]);
        printf("  TYPE: string, hex\n");
        return -1;
    }

    char* type = argv[2];
    int needle_length;

    char* needle;
    if (strncmp(type, "string", 6) == 0) {
        needle = argv[3];
        needle_length = (int) strlen(needle);
    } else if (strncmp(type, "hex", 3) == 0) {
        needle = (char *) mh_hex2bytes(argv[3], strlen(argv[3]));
        needle_length = (int) strlen(argv[3]) / 2;
    } else {
        printf("Invalid type, only 'string' and 'hex' accepted\n");
        return -3;
    };


    if (needle_length < 1) {
        printf("Invalid search pattern\n");
        return -2;
    }

    printf("search:string{%d} %s[%s]\n", needle_length, argv[3], needle);

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

        const char* region_usertag = mh_usertag_to_text(&process, address, size);
        fprintf(
                stderr,
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
                region_usertag
        );

        // iOS 上全部搜出现bus error 10，所以只搜MALLOC开头的内存段
        if (info.protection & VM_PROT_READ && region_usertag[0] == 'M') {

#if MAX_READ_MEMORY_SIZE > 0
            mach_vm_address_t original_address = address;
            mach_vm_size_t original_size = size;

            if (original_size > MAX_READ_MEMORY_SIZE) {
                size = MAX_READ_MEMORY_SIZE;
            }

            while (address + size <= original_address + original_size) {
                fprintf(stderr, ">%016llx-%016llx size=%016llx\n", address, address+size, size);

#endif

                void *mem = NULL;

                err = mach_vm_read(
                        process.process_task,
                        address,
                        size,
                        (vm_offset_t *) &mem,
                        (mach_msg_type_number_t *) &size
                );

                if (err != KERN_SUCCESS) {
                    printf("mach_vm_read() failure: %d - %s\n", err, mach_error_string(err));

#if MAX_READ_MEMORY_SIZE > 0
                    break;
#else
                    address += size;
                    continue;
#endif
                }

                vm_offset_t *offsets;

                if (needle_length > 1 && needle[0] != needle[1]) {
                    offsets = malloc((size / needle_length + 1) * sizeof(int));
                } else {
                    offsets = malloc(size * sizeof(int));
                }

                int result_count = mh_search_bytes(mem, size, (unsigned char *) needle, needle_length, offsets);
//            printf("%d result(s) found\n", result_count);

                for (int i = 0; i < result_count; i++) {
                    printf("[%d]\n", i);
                    mach_vm_address_t start =
                            ((mach_vm_address_t) mem + offsets[i]) - ((mach_vm_address_t) mem + offsets[i]) % 16 - 16;
                    mh_dump_hex((void *) start, (vm_size_t) (needle_length / 16 + 3) * 16,
                                address + offsets[i] - (address + offsets[i]) % 16);
                }

                free(offsets);

#if MAX_READ_MEMORY_SIZE > 0
                address += size;

            };

            address = original_address;
            size = original_size;
#endif

        }

        address += size;
    }

    return 0;
}


