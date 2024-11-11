'''
def print_hi(name):
    # Use a breakpoint in the code line below to debug your script.
    print(f'Hi, {name}')  # Press Ctrl+F8 to toggle the breakpoint.


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    print_hi('PyCharm')
'''

import math
import numpy as np
from scipy.linalg import dft
from scipy.fft import ifft, fft
from matplotlib import pyplot as plt


def get_lin_convolution(y, z):
    Ns = len(y) + len(z) - 1
    S = [0] * Ns

    y = np.array(y)
    z = np.array(z)
    #Y = np.pad(y, (Ns - len(y), 0), 'constant')
    #Z = np.pad(z[::-1], (0, Ns - len(z)), 'constant')

    #print(Y)
    #print(Z)

    for i in range(Ns):
        for j in range(max(0, i - len(z) + 1), min(i + 1, len(y))):
            S[i] += y[j] * z[i - j]
        # S[i] /= len(y)

    return S


def get_cycle_convolution(y, z):
    Ns = len(y)
    S = [0] * Ns

    y = np.array(y)
    z = np.array(z)

    for i in range(Ns):
        for j in range(Ns):
            k = (i - j) % Ns
            S[i] += y[j] * z[k]
        # S[i] /= Ns

    return S


def get_correlation(y, z):
    Ns = len(y)
    S = [0] * Ns

    y = np.array(y)
    z = np.array(z)

    for i in range(Ns):
        for j in range(Ns):
            if i + j >= Ns:
                continue
            S[i] += y[j] * z[i + j]
        #S[i] /= Ns

    return S


def rFFT(x, dir):
    x = np.asarray(x, dtype='complex_')
    N = x.shape[0]

    if N == 1:
        return x
    else:
        wN = np.exp(dir * -2j * np.pi / N)
        right = np.asarray([])
        left = np.asarray([])
        w = 1

        for j in range(N // 2):
            left = np.append(left, x[j] + x[j + (N // 2)])
            right = np.append(right, (x[j] - x[j + (N // 2)]) * w)
            w = w * wN

        left = rFFT(left, dir)
        right = rFFT(right, dir)

        return np.concatenate((left, right))


def reverse_indexes(x):
    N = len(x)
    result = x
    for i in range(N):
        s = format(i, '0>' + str(int(math.log(N, 2))) + 'b')
        reversed_i = int(s[::-1], 2)
        if reversed_i > i:
            temp = result[reversed_i]
            result[reversed_i] = result[i]
            result[i] = temp
    return result


def FFT(x, dir):
    X = rFFT(x, dir)
    X = reverse_indexes(X)

    if dir == 1:
        N = len(X)
        for i in range(N):
            X[i] /= N

    return X


def check_convolution_with_furie(y, z):
    Ns = len(y)
    #F = [0] * Ns
    F = np.zeros(Ns, dtype='complex_')

    #y = np.array(y)
    y = np.asarray(y, dtype='complex_')
    #z = np.array(z)
    z = np.asarray(z, dtype='complex_')

    Y = FFT(y, 1)
    Z = FFT(z, 1)

    for i in range(Ns):
        F[i] = Y[i] * Z[i]

    rF = FFT(F, -1)
    return rF


if __name__ == '__main__':
    print('It is Lab2')

    N = 8 #8

    x = []
    y = []
    z = []
    x1 = []

    i = 0
    while i < N:
        x.append(i)
        y.append(math.cos(3 * i))
        z.append(math.sin(2 * i))
        i = i + 1

    #print(y)
    #print(z)
    print('\n')

    linear_convolution = get_lin_convolution(y, z)
    i = 0
    while i < N + N - 1:
        x1.append(i)
        i = i + 1

    cycle_convolution = get_cycle_convolution(y, z)
    correlation = get_correlation(y, z)

    convolution_after_furie = check_convolution_with_furie(y, z)

    convol_lib = np.convolve(y, z)
    correlat_lib = corr = np.correlate(y, z, mode='full')[:N][::-1]

    figure, graphs = plt.subplots(2, 4)

    graphs[0, 0].plot(x, y)
    graphs[0, 0].set_title("Original y")

    graphs[1, 0].plot(x, z)
    graphs[1, 0].set_title("Original z")

    graphs[0, 1].plot(x1, linear_convolution)
    graphs[0, 1].set_title("Linear convolution")

    graphs[1, 1].plot(x, cycle_convolution)
    graphs[1, 1].set_title("Cycle convolution")

    graphs[0, 2].plot(x, correlation)
    graphs[0, 2].set_title("Correlation")

    graphs[1, 2].plot(x, convolution_after_furie.real)
    graphs[1, 2].set_title("After Furie")

    graphs[0, 3].plot(x1, convol_lib)
    graphs[0, 3].set_title("Lib Convolution")
    graphs[1, 3].plot(x, correlat_lib)
    graphs[1, 3].set_title("Lib Correaltion")

    plt.show()
