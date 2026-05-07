#define _GNU_SOURCE

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#ifndef MAP_FIXED_NOREPLACE
#define MAP_FIXED_NOREPLACE 0x100000
#endif

static void die(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

static void *map_fixed_private_page(size_t page_size)
{
    /*
     * x86-64 Linux 사용자 공간에서 대체로 비어 있을 가능성이 높은 주소 후보들.
     * 환경에 따라 실패할 수 있으므로 여러 후보를 시도한다.
     */
    static const uintptr_t candidates[] = {
        0x700000000000ULL,
        0x600000000000ULL,
        0x500000000000ULL,
        0x400000000000ULL
    };

    for (size_t i = 0; i < sizeof candidates / sizeof candidates[0]; i++) {
        void *wanted = (void *)candidates[i];

        void *mapped = mmap(
            wanted,
            page_size,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE,
            -1,
            0
        );

        if (mapped == wanted) {
            return mapped;
        }

        /*
         * 오래된 커널이 MAP_FIXED_NOREPLACE를 제대로 이해하지 못하면
         * 요청 주소와 다른 주소를 반환할 수 있다.
         * 그런 경우에는 교육 목적에 맞지 않으므로 즉시 해제한다.
         */
        if (mapped != MAP_FAILED) {
            munmap(mapped, page_size);
        }
    }

    return MAP_FAILED;
}

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);

    long label = 0;
    if (argc >= 2) {
        label = strtol(argv[1], NULL, 10);
    }

    long page_size_long = sysconf(_SC_PAGESIZE);
    if (page_size_long <= 0) {
        die("sysconf");
    }

    size_t page_size = (size_t)page_size_long;

    char *page = map_fixed_private_page(page_size);
    if (page == MAP_FAILED) {
        fprintf(stderr, "failed to map a fixed virtual address\n");
        fprintf(stderr, "try changing candidate addresses in the source\n");
        return EXIT_FAILURE;
    }

    snprintf(
        page,
        page_size,
        "pid=%ld, label=%ld, virtual_address=%p",
        (long)getpid(),
        label,
        (void *)page
    );

    printf("pid              = %ld\n", (long)getpid());
    printf("mapped VA        = %p\n", (void *)page);
    printf("content at VA    = \"%s\"\n", page);
    printf("inspect command  = cat /proc/%ld/maps | grep '%lx'\n",
           (long)getpid(), (unsigned long)(uintptr_t)page);

    sleep(20);

    munmap(page, page_size);
    return 0;
}
