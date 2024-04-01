/**
 * @author Ben Song <benedict04song@gmail.com>
 */
#include <csignal>
#include <iostream>
#include "./Hook.hpp"

#ifdef DEBUG
#define dbg_assert(expr) assert(expr)
#else
#define dbg_discard_expr_(...) ((void)((0) && printf(__VA_ARGS__)))
#define dbg_assert(expr) dbg_discard_expr_("%d", !(expr))
#endif

PyHook::PyHook() {
    // Improper initialization
    this->is_initialized = false;
}

PyHook::PyHook(std::string src) {
    // Initialize interpreter as being stopped
    this->stopped = true;
    // Initialize last jouleage readings as 0
    this->last_usage_CPU = 0;
    this->last_usage_GPU = 0;
    // Initialize file path member
    this->src_file = src; 

    // Initialize the interpreter
    this->state = PyGILState_Ensure();
    Py_SetProgramName(PY_VER);
    Py_Initialize();
    this->is_initialized = true;
}

void PyHook::execute() {
    FILE *src_fp = fopen(this->src_file.c_str(), "r");
    PyRun_SimpleFileEx(src_fp, this->src_file.c_str(), false);
}

void PyHook::freeze() {
    dbg_assert(!this->stopped);
    // Get the jouleages
    this->last_usage_CPU = _get_CPU_joules();
    this->last_usage_GPU = _get_GPU_joules();
    // Stop (but not kill!) interpreter
    // Should be fine probably, if I understand the GIL (I don't).
    // In particular, how can we ensure the correct interpreter is getting SIGTSTP'ed?
    PyErr_SetInterruptEx(SIGTSTP);
    this->stopped = true;
}

PyFrameObject *PyHook::get_frame() {
    return PyThreadState_GetFrame(PyThreadState_Get());
}

void PyHook::thaw() {
    dbg_assert(this->stopped);
    PyErr_SetInterruptEx(SIGCONT);
    this->stopped = false;
}

snapshot_t PyHook::cycle() {
    snapshot_t s = { 0 };
    this->freeze();
    s.frame = this->get_frame();
    // TODO: Add in other fields later
    return s;
}

PyHook::~PyHook() {
    // Do not destroy an unstopped interpreter
    dbg_assert(this->stopped);
    if (this->is_initialized) {
        Py_FinalizeEx();
        PyGILState_Release(this->state);
    }
}

