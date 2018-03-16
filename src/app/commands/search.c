/**
 * src/app/commands/search.c -- mh_cli: search command
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

#include <app/app.h>
#include "commands.h"
#include "../app.h"


int cmd_search_bytes(MHContext *context, char *needle, size_t needle_length)
{
    if (context->result_count) {
        mh_result_free(&context->results);
        context->result_count = 0;
    }

    COMMAND_REQUIRE_PROCESS()

    mach_vm_address_t address                 = 0x0;
    mach_vm_size_t    size;
    mach_vm_address_t region_address          = 0x0;
    mach_vm_size_t    region_size;

    vm_region_basic_info_data_64_t info;
    mach_msg_type_number_t         infoCount;
    mach_port_t                    objectName = MACH_PORT_NULL;

    int err;

    size_t total_result_count = 0;

    while (1) {
        infoCount = VM_REGION_BASIC_INFO_COUNT_64;

        err = mach_vm_region(
                context->process.process_task, &region_address, &region_size, VM_REGION_BASIC_INFO_64,
                (vm_region_info_t) &info,
                &infoCount,
                &objectName);

        if (err != KERN_SUCCESS) {
            break;
        }

        const char *region_usertag = mh_usertag_to_text(&context->process, region_address, region_size);

        // currently only search for MALLOC_** and STACK
        if (!(info.protection & VM_PROT_READ) ||
            (region_usertag[0] != 'M' && region_usertag[0] != 'S' && region_usertag[0] != 'T')) {
            goto NEXT;
        }

        address = region_address;
        size    = region_size;

#if MAX_READ_MEMORY_SIZE > 0
        if (region_size > MAX_READ_MEMORY_SIZE) {
            size = MAX_READ_MEMORY_SIZE;
        }

        while (address + size <= region_address + region_size) {
//                fprintf(stderr, ">%016llx-%016llx size=%016llx\n", address, address+size, size);

#endif

        void *mem = NULL;

        err = mach_vm_read(
                context->process.process_task,
                address,
                size,
                (vm_offset_t *) &mem,
                (mach_msg_type_number_t *) &size
                          );

        if (err != KERN_SUCCESS) {
            printf("mach_vm_read(%016llx:%llx) failure: %d - %s\n", address, size, err, mach_error_string(err));

            goto NEXT;
        }

        vm_offset_t *offsets;

        if (needle_length > 1 && needle[0] != needle[1]) {
            offsets = malloc((size / needle_length + 1) * sizeof(int));
        } else {
            offsets = malloc(size * sizeof(int));
        }

        int result_count = mh_search_bytes(mem, size, (unsigned char *) needle, (int) needle_length, offsets);

        for (int i = 0; i < result_count; i++) {
            mh_result_add(&context->results, address + offsets[i], region_address, region_size);
            /*
            printf("[%d]\n", i);
            mach_vm_address_t start =
                                      ((mach_vm_address_t) mem + offsets[i]) -
                                      ((mach_vm_address_t) mem + offsets[i]) % 16 - 16;
            mh_dump_hex((void *) start, (vm_size_t) (needle_length / 16 + 3) * 16,
                        address + offsets[i] - (address + offsets[i]) % 16);
            */
        }

        total_result_count += result_count;

        free(offsets);

#if MAX_READ_MEMORY_SIZE > 0
        address += size;

    };

#endif

        NEXT:
        region_address += region_size;
    }

    context->result_count = total_result_count;
    context->query_size   = MAX(((needle_length / 16) + 1) * 16, context->query_size);

    printf("Found %d result(s).\n", (int) context->result_count);

    return 0;
}

int cmd_update_search_bytes(MHContext *context, char *needle, size_t needle_length)
{
    COMMAND_REQUIRE_PROCESS()

    if (mh_result_empty(&context->results)) {
        printf("Please use search command before using update-search\n");
        return -1;
    }
    if (context->result_count < 1) {
        printf("No result left\n");
        return -2;
    }

    int            err;
    void           *mem = NULL;
    mach_vm_size_t size;

    struct result_entry *np = NULL;
    STAILQ_FOREACH(np, &context->results, next) {
        printf("update search @address:%016llx\n", np->address);

        err = mach_vm_read(
                context->process.process_task,
                np->address,
                (mach_vm_size_t) needle_length,
                (vm_offset_t *) &mem,
                (mach_msg_type_number_t *) &size
                          );

        if (err != KERN_SUCCESS) {
            printf(
                    "mach_vm_read(%016llx:%llx) failure: %d - %s\n",
                    np->address,
                    (unsigned long long int) needle_length,
                    err,
                    mach_error_string(err)
                  );

            continue;
        }

        if (strncmp(needle, mem, needle_length) != 0) {
            // mismatch, remove
            STAILQ_REMOVE(&context->results, np, result_entry, next);
            free(np);
            --context->result_count;
        }
    }

    context->query_size = MAX(((needle_length / 16) + 1) * 16, context->query_size);

    printf("Found %d result(s).\n", (int) context->result_count);

    return 0;
}

int cmd_search_result_list(MHContext *context)
{
    COMMAND_REQUIRE_PROCESS()

    int            err;
    void           *mem = NULL;
    mach_vm_size_t size;

    int i = 0;

    struct result_entry *np = NULL;
    STAILQ_FOREACH(np, &context->results, next) {
        printf("update search @address:%016llx\n", np->address);

        err = mach_vm_read(
                context->process.process_task,
                np->address,
                (mach_vm_size_t) context->query_size,
                (vm_offset_t *) &mem,
                (mach_msg_type_number_t *) &size
              );

        if (err != KERN_SUCCESS) {
            printf(
                    "mach_vm_read(%016llx:%llx) failure: %d - %s\n",
                    np->address,
                    (unsigned long long int) context->query_size,
                    err,
                    mach_error_string(err)
                  );

            continue;
        }

        vm_region_basic_info_data_64_t info;
        mach_msg_type_number_t         infoCount;
        mach_port_t                    objectName = MACH_PORT_NULL;

        infoCount = VM_REGION_BASIC_INFO_COUNT_64;

        err = mach_vm_region(
                context->process.process_task, &np->region_address, &np->region_size,
                VM_REGION_BASIC_INFO_64, (vm_region_info_t) &info,
                &infoCount,
                &objectName
                            );

        if (err != KERN_SUCCESS) {
            break;
        }

        const char *region_usertag = mh_usertag_to_text(&context->process, np->region_address, np->region_size);
        printf(
                "[%d] 0x%016llx-0x%016llx size=0x%08llx offset=%016llx, %c%c%c/%c%c%c, %s\n",
                i++,
                np->region_address,
                np->region_address + np->region_size,
                size,
                info.offset,
                (info.protection & VM_PROT_READ) ? 'r' : '-',
                (info.protection & VM_PROT_WRITE) ? 'w' : '-',
                (info.protection & VM_PROT_EXECUTE) ? 'x' : '-',
                (info.max_protection & VM_PROT_READ) ? 'r' : '-',
                (info.max_protection & VM_PROT_WRITE) ? 'w' : '-',
                (info.max_protection & VM_PROT_EXECUTE) ? 'x' : '-',
                region_usertag
              );

        mh_dump_hex((void *) mem, (vm_size_t) context->query_size, np->address);
    }

    return 0;
}

// EOF
