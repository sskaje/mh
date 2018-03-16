
#include <mh/mh.h>

int main(int argc, char **argv) {

    if (argc < 4) {
        printf("%s PID ADDRESS COUNT\n", argv[0]);
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

    mach_vm_address_t address;
    mach_vm_size_t size;

    address = (mach_vm_address_t) strtoull(argv[2], NULL, 16);
    size = (mach_vm_size_t) strtoull(argv[3], NULL, 16);

    printf("Read memory: addr=%016llx, size=%016llx\n", address, size);

    mh_dump_memory(&process, address, size);

    return 0;
}
