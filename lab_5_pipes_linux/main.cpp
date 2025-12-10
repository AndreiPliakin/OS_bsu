#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

int main() {
    int pipe_MA[2], pipe_AP[2], pipe_PS[2];

    if (pipe(pipe_MA) == -1 || pipe(pipe_AP) == -1 || pipe(pipe_PS) == -1) {
        perror("Ошибка создания pipe");
        return 1;
    }

    pid_t pid_m = fork();
    if (pid_m == 0) {
        close(pipe_MA[0]);
        close(pipe_AP[0]); close(pipe_AP[1]); //закрыть ненужные
        close(pipe_PS[0]); close(pipe_PS[1]);

        dup2(pipe_MA[1], STDOUT_FILENO); //заменяем стандартный вывод на экран на вывод из pid_m
        close(pipe_MA[1]);

        execl("./M", "M", NULL);
        perror("Ошибка запуска M");
        return 1;
    }

    pid_t pid_a = fork();
    if (pid_a == 0) {
        close(pipe_MA[1]);
        close(pipe_AP[0]);
        close(pipe_PS[0]); close(pipe_PS[1]);

        dup2(pipe_MA[0], STDIN_FILENO);
        close(pipe_MA[0]);

        dup2(pipe_AP[1], STDOUT_FILENO);
        close(pipe_AP[1]);

        execl("./A", "A", NULL);
        perror("Ошибка запуска A");
        return 1;
    }

    pid_t pid_p = fork();
    if (pid_p == 0) {
        close(pipe_MA[0]); close(pipe_MA[1]);
        close(pipe_AP[1]);
        close(pipe_PS[0]);

        dup2(pipe_AP[0], STDIN_FILENO);
        close(pipe_AP[0]);

        dup2(pipe_PS[1], STDOUT_FILENO);
        close(pipe_PS[1]);

        execl("./P", "P", NULL);
        perror("Ошибка запуска P");
        return 1;
    }

    pid_t pid_s = fork();
    if (pid_s == 0) {
        close(pipe_MA[0]); close(pipe_MA[1]);
        close(pipe_AP[0]); close(pipe_AP[1]);
        close(pipe_PS[1]);

        dup2(pipe_PS[0], STDIN_FILENO);
        close(pipe_PS[0]);

        execl("./S", "S", NULL);
        perror("Ошибка запуска S");
        return 1;
    }

    close(pipe_MA[0]); close(pipe_MA[1]);
    close(pipe_AP[0]); close(pipe_AP[1]); //все закрыть
    close(pipe_PS[0]); close(pipe_PS[1]);

    waitpid(pid_m, NULL, 0);
    waitpid(pid_a, NULL, 0); //ждем завершения
    waitpid(pid_p, NULL, 0);
    waitpid(pid_s, NULL, 0);

    std::cout << "Цепочка процессов завершена." << std::endl;
    return 0;
}