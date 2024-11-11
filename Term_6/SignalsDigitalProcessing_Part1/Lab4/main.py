import random
import math
import numpy as np
from scipy.linalg import dft
from scipy.fft import ifft, fft
from matplotlib import pyplot as plt


def haar_transform(vector, ind):
    n = len(vector)
    if n == 1:
        print(f"L{ind - 1}:", vector[0])
        return vector

    high = []
    low = []
    for i in range(0, n, 2):
        high.append((vector[i] + vector[i + 1]) / 2)
        low.append((vector[i] - vector[i + 1]) / 2)

    print(f"H{ind}:", low)
    return np.concatenate((haar_transform(high, ind + 1), low))


def inverse_haar_transform(vector):
    n = math.log(len(vector), 2)

    hs1 = [0] * len(vector)
    hs2 = [0] * len(vector)
    hs1[0] = vector[0]

    for i in range(int(n)):
        for j in range(2 ** i):
            a1 = (hs1[j] + vector[2 ** i + j])
            hs2[2 * j] = a1
            a2 = (hs1[j] - vector[2 ** i + j])
            hs2[2 * j + 1] = a2
        hs1[:] = hs2[:]
        print(f"Restored L{int(n) - i - 1}: {hs2}")

    return hs2


if __name__ == '__main__':
    print('Hi! It is Lab4 :)')

    N = 32 #8

    vect = [random.randint(1, 100) for _ in range(N)]
    #vect = [60, 21, 16, 54, 72, 81, 66, 66]
    print(f"Original vector: {vect} \n")

    ht = haar_transform(vect, 1)
    print(f"\nHaar result: {ht}\n")

    restored_vect = inverse_haar_transform(ht)
    restored_vect = [round(float(num)) for num in restored_vect]
    print(f"\nRestored vector: {restored_vect}")

