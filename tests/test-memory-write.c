
#include <mh/mh.h>

int main(int argc, char **argv) {

    if (argc < 4) {
        printf("%s PID ADDRESS BYTES\n", argv[0]);
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

    address = (mach_vm_address_t) strtoull(argv[2], NULL, 16);
    const char *bytes = (const char *) mh_hex2bytes(argv[3], strlen(argv[3]));

    printf("Write memory: addr=%016llx\n", address);

//    char tmp[0x13] = {
//            0x12, 'I', 'T','O','G','3','_','I','A','P','_','P','R','O','_','F','R','E','E'
//    };

    mach_vm_write(process.process_task, address, (vm_offset_t) bytes, (mach_msg_type_number_t) sizeof(bytes));

    return 0;
}
