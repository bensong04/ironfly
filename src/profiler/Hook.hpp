/**
 * @brief A simple "hook" into a Python interpreter.
 * Less of a hook and more of a wrapper around an interpreter.
 * 
 * Due to disgusting GIL idiosyncrasies, only one PyHook may exist per process. 
 * Eventually, we want multithreading to parallelize profiling across multiple 
 * machine/emulators and/or configs or whatever. However, PyTorch doesn't support 
 * PEP 684, so we can't do multiprocessing yet. Sad!
 * 
 * @author Ben Song <benedict04song@gmail.com>
 */
#pragma once
#include <cstdlib>
#include <string>

#include "Python.h"
#include "frameobject.h"
#include "../sys/Systems.hpp"

#define PY_VER (L"python3.12")

typedef struct snapshot {
    PyFrameObject *frame;
    joule_t cpu;
    joule_t gpu;
} snapshot_t;

class PyHook {
    private:
        /** @brief GIL state at moment of initialization */
        PyGILState_STATE state;

        /** @brief Monitors whether interpreter is stopped or not */
        bool stopped;

        /** @brief Last CPU joule usage value measured when last frozen. Starts at 0. */
        joule_t last_usage_CPU;

        /** @brief Last GPU joule usage value measured when last frozen. Starts at 0. */
        joule_t last_usage_GPU;

        /** @brief Path to source code file. */
        std::string src_file;
        
        /** @brief Whether the PyHook has been properly initialized or not (hacky fix). */
        bool is_initialized;
        
    public:
        /**
         * @brief Empty initializer
         */
        PyHook();

        /**
         * @brief Initializes a new PyHook object.
         */
        PyHook(std::string src);

        /**
         * @brief Starts executing the PyHook source file.
         * 
         * This function will not return normally. You need to install signal handlers
         * for profiling to work properly.
         */
        void execute();

        /**
         * @brief Freezes the interpreter and records jouleage at the moment of freezing.
         */
        void freeze();

        /**
         * @brief Returns the stack frame of the interpreter.
         * 
         * @pre Interpreter is stopped.
         */
        PyFrameObject *get_frame();

        /**
         * @brief Returns the line number of execution.
         * 
         * @pre Interpreter is stopped.
         */
        size_t get_line_no();

        /**
         * @brief Returns the CPU usage recorded at last freeze.
         * 
         * @pre Interpreter is stopped.
         */
        joule_t get_last_usage_CPU();

        /**
         * @brief Returns the GPU usage recorded at last freeze.
         * 
         * @pre Interpreter is stopped.
         */
        joule_t get_last_usage_GPU();

        /**
         * @brief Unfreezes the interpreter.
         */
        void thaw();

        /**
         * @brief A single freeze-get_frame-get_last_usage cycle.
         */
        snapshot_t cycle();

        /**
         * @brief Destructor for this PyHook object.
         */
        ~PyHook();
};