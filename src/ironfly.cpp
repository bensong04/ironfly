/**
 * @author Ben Song <benedict04song@gmail.com>
 */

#include <iostream>
#include <csignal>
#include <functional>
#include <unistd.h>
#include <errno.h>
#include "sys/Systems.hpp"
#include "profiler/Hook.hpp"

// Code taken from CSAPP
typedef void handler_t(int);

PyHook hook; // hacky fix

// Code taken from CSAPP
handler_t *Signal(int signum, handler_t *handler) {
    struct sigaction action, old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* Block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* Restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0) {
        perror("Signal error");
        exit(1);
    }

    return old_action.sa_handler;
}

void cycle(int sig) {
    snapshot_t sn = hook.cycle();
    // Just print the function name
    std::cout << PyFrame_GetCode(sn.frame)->co_name << std::endl;
    hook.thaw();
}

int main(int argc, char **argv) {
    std::string python_src = argv[1];
    auto child_pid = fork();
    if (!child_pid) {
        // Child process
        hook = PyHook(python_src);
        // Install signal handlers
        Signal(SIGSTOP, &cycle);
        // Start up parent loop
        kill(getppid(), SIGCONT);
        hook.execute();
    } else {
        sigset_t cont;
        sigfillset(&cont);
        sigprocmask(SIGCONT, &cont, NULL);
        sigsuspend(&cont);
        while (!waitpid(child_pid, NULL, WNOHANG)) {
            timespec ns = {0, 1000000};
            nanosleep(&ns, &ns);
            kill(child_pid, SIGSTOP);
        }
    }
    return 0;
}