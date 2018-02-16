#include <check.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <server/server.h>

/**
 * Test execve with pipes
 */
START_TEST (test_execve_cat)

    int in[2];
    int out[2];

    int r1 = pipe(in);
    int r2 = pipe(out);
    if (r1 < 0 || r2 < 0) {
        perror("pipe() failed");
        return;
    }

    pid_t pid;
    if ((pid = fork()) == -1) {
        perror("fork() failed");
    } else if (pid == 0) {
        dup2(in[0], STDIN_FILENO);
        dup2(out[1], STDOUT_FILENO);

        close(in[1]);
        close(out[0]);

        char * args[4] = {"bash", "-c", "cat", NULL};
        int ret = execve("/bin/bash", args, NULL);
        if (ret == -1) {
            perror("execve() failed");
        }
    } else {
        int write_fd = in[1];
        int read_fd = out[0];

        close(in[0]);
        close(out[1]);

        char *write_buffer = "Hello World!\n";
        char read_buffer[BUFFER_SIZE] = {0};

        ssize_t bytes_send = write(write_fd, write_buffer, strlen(write_buffer));
        ssize_t bytes_read = read(read_fd, read_buffer, BUFFER_SIZE);

        ck_assert_uint_eq(bytes_send, bytes_read);
        ck_assert_str_eq(write_buffer, read_buffer);
    }

END_TEST


Suite * suite(void) {
    Suite *s   = suite_create("execve");
    TCase *tc  = tcase_create("execve_cat");

    tcase_add_test(tc, test_execve_cat);
    suite_add_tcase(s, tc);

    return s;
}

int main(void) {
    int number_failed;
    SRunner *sr  = srunner_create(suite());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
