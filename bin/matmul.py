import numpy as np
from time import time
import sys

def main(argv: list):
    N = 0
    for arg in argv:
        if arg.isdigit():
            N = int(arg)
            break
    if N <= 0 or N > 10000:
        print("Input value for N")
        return
    print("Filling matrices with random numbers...")
    start = time()
    mat1 = np.random.randint(0, 20, size=(N, N))
    mat2 = np.random.randint(0, 20, size=(N, N))
    print(f"Finished in {time() - start:.6f} seconds")
    print("Performing matrix multiplication with numpy...")
    start = time()
    np.matmul(mat1, mat2)
    print(f"Finished in {time() - start:.6f} seconds")

if __name__ == "__main__":
    main(sys.argv)