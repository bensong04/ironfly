/**
 * @author Ben Song <benedict04song@gmail.com>
 */

#include <iostream>
#include <csignal>
#include <functional>
#include <unistd.h>
#include "sys/Systems.hpp"
#include "profiler/Hook.hpp"

PyHook hook(""); // hacky fix

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
        std::signal(SIGTSTP, &cycle);
        hook.execute();
    } else {
        while (!waitpid(child_pid, NULL, WNOHANG)) {
            timespec ns = {0, 1000000};
            nanosleep(&ns, &ns);
            kill(child_pid, SIGTSTP);
        }
    }
    return 0;
}