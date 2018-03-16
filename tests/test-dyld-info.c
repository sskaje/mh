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

    pointer_t *imagePtr = NULL;
    int imageCount = 0;
    mh_read_dyld_images(&process, (pointer_t *) &imagePtr, &imageCount);

    MHImage* images = (MHImage *) imagePtr;

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
            struct mach_header    *mach = (struct mach_header    *) tmp;
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

    return 0;
}
