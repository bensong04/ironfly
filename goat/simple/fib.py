"""
Simple test Python script using loops, recursion, syscalls.
"""
import os

def fib(n: int) -> int:
    if n == 0:
        return 1
    if n == 1:
        return 1
    return fib(n - 1) + fib(n - 2)

def enumerate_files(idir: str):
    for idx, root, _, files in enumerate(os.walk(idir, topdown=False)):
        if idx >= 15:
            break
        print(f"The {idx}th Fibonacci number is {fib(idx)}.")
        for name in files:
            print(os.path.join(root, name))

if __name__ == "__main__":
    enumerate_files(".")