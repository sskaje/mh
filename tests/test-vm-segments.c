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

    mach_vm_address_t tmpSize;

    void *tmp = NULL;

    if (process.is64bit) {
        tmpSize = sizeof(struct mach_header_64);
    } else {
        tmpSize = sizeof(struct mach_header);
    }

    mach_vm_read(
            process.process_task,
            images[0].load_address,
            tmpSize,
            (vm_offset_t *) &tmp,
            (mach_msg_type_number_t *) &tmpSize
    );

    mach_vm_address_t regionAddress = 0x0;
    mach_vm_size_t regionSize = 0;

    vm_region_basic_info_data_64_t info;
    mach_port_t objectName = MACH_PORT_NULL;
    mach_msg_type_number_t infoCount;
    vm_region_flavor_t flavor;

    if (process.is64bit) {
        struct mach_header_64 *mach = (struct mach_header_64 *) tmp;
        printf("Bits=64, Magic=%08x, ", mach->magic);
        if (mach->magic == MH_CIGAM_64) {
            printf("Byte ordering mismatch\n");
            return -5;
        }
    } else {
        struct mach_header    *mach = (struct mach_header    *) tmp;
        printf("Bits=32, Magic=%08x, ", mach->magic);
        if (mach->magic == MH_CIGAM) {
            printf("Byte ordering mismatch\n");
            return -5;
        }
    }

    infoCount = VM_REGION_BASIC_INFO_COUNT_64;
    flavor    = VM_REGION_BASIC_INFO_64;

    // find first region
    mach_vm_region(process.process_task, &regionAddress, &regionSize, flavor, (vm_region_info_t) &info,
                   &infoCount,
                   &objectName);

    printf("address=%016llx, size=%016llx, shared=%d, offset=%016llx\n", regionAddress, regionSize, info.shared, info.offset);


    mach_vm_read(
            process.process_task,
            regionAddress,
            regionSize,
            (vm_offset_t *) &tmp,
            (mach_msg_type_number_t *) &regionSize
    );




//
//    for (int i = 0; i < imageCount; i++) {
//        printf("[%d] ", i);
//        mach_vm_address_t tmpSize;
//
//        void *tmp = NULL;
//
//        if (process.is64bit) {
//            tmpSize = sizeof(struct mach_header_64);
//        } else {
//            tmpSize = sizeof(struct mach_header);
//        }
//
//        mach_vm_read(
//                process.process_task,
//                images[i].load_address,
//                tmpSize,
//                (vm_offset_t *) &tmp,
//                (mach_msg_type_number_t *) &tmpSize
//        );
//
//        if (process.is64bit) {
//            struct mach_header_64 *mach = (struct mach_header_64 *) tmp;
//            printf("Bits=64, Magic=%08x, ", mach->magic);
//        } else {
//            struct mach_header    *mach = (struct mach_header    *) tmp;
//            printf("Bits=32, Magic=%08x, ", mach->magic);
//        }
//
//
//        char *image_file_path = NULL;
//        tmpSize = 1024;
//        mach_vm_read(
//                process.process_task,
//                images[i].file_path,
//                tmpSize,
//                (vm_offset_t *) &image_file_path,
//                (mach_msg_type_number_t *) &tmpSize
//        );
//
//        printf("%016llx %s\n", images[i].load_address, (const char *) image_file_path);
//    }

    free(images);

    return 0;
}
