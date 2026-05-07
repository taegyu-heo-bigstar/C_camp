CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -g

TARGETS := same_va_fork same_va_mmap

.PHONY: all clean run-fork run-mmap

all: $(TARGETS)

same_va_fork: same_va_fork.c
	$(CC) $(CFLAGS) $< -o $@

same_va_mmap: same_va_mmap.c
	$(CC) $(CFLAGS) $< -o $@

run-fork: same_va_fork
	./same_va_fork

run-mmap: same_va_mmap
	./same_va_mmap 111 & ./same_va_mmap 222 & wait

clean:
	rm -f $(TARGETS)
