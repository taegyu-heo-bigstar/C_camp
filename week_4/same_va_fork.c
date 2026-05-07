#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static int global_value = 100;

static void die(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

static void print_state(
    const char *tag,
    int *heap_value,
    int *local_value,
    const char *literal)
{
    printf("[%s] pid=%ld\n", tag, (long)getpid());
    printf("  &global_value = %p, global_value = %d\n",
           (void *)&global_value, global_value);
    printf("  heap_value    = %p, *heap_value = %d\n",
           (void *)heap_value, *heap_value);
    printf("  &local_value  = %p, local_value = %d\n",
           (void *)local_value, *local_value);
    printf("  literal       = %p, text = \"%s\"\n",
           (void *)literal, literal);
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);

    int local_value = 300;

    int *heap_value = malloc(sizeof *heap_value);
    if (heap_value == NULL) {
        die("malloc");
    }

    *heap_value = 200;

    const char *literal = "same virtual address demo";

    int ready_pipe[2];
    if (pipe(ready_pipe) == -1) {
        free(heap_value);
        die("pipe");
    }

    printf("---- before fork ----\n");
    print_state("parent before fork", heap_value, &local_value, literal);

    pid_t child = fork();

    if (child == -1) {
        free(heap_value);
        die("fork");
    }

    if (child == 0) {
        close(ready_pipe[0]);

        printf("\n---- child before write ----\n");
        print_state("child before write", heap_value, &local_value, literal);

        /*
         * 자식 프로세스에서 값을 변경한다.
         * 주소값은 부모와 같게 보이지만, 부모의 값은 바뀌지 않는다.
         */
        global_value = 1000;
        *heap_value = 2000;
        local_value = 3000;

        printf("\n---- child after write ----\n");
        print_state("child after write", heap_value, &local_value, literal);

        if (write(ready_pipe[1], "x", 1) != 1) {
            die("write pipe");
        }

        printf("\n[child] inspect command:\n");
        printf("  cat /proc/%ld/maps\n", (long)getpid());

        sleep(15);
        _exit(EXIT_SUCCESS);
    }

    close(ready_pipe[1]);

    char dummy;
    if (read(ready_pipe[0], &dummy, 1) != 1) {
        free(heap_value);
        die("read pipe");
    }

    printf("\n---- parent after child wrote ----\n");
    print_state("parent after child wrote", heap_value, &local_value, literal);

    printf("\n[parent] inspect command:\n");
    printf("  cat /proc/%ld/maps\n", (long)getpid());

    sleep(15);

    int status;
    if (waitpid(child, &status, 0) == -1) {
        free(heap_value);
        die("waitpid");
    }

    free(heap_value);
    return 0;
}
