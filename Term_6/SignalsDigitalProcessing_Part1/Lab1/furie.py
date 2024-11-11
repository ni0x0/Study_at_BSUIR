import math
import numpy as np
from scipy.linalg import dft
from scipy.fft import ifft, fft
from matplotlib import pyplot as plt


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


if __name__ == '__main__':

    N = 64
    T = N / 1

    x = []
    axises = []

    i = 0
    while i < T:
        x.append(math.sin(5 * i) + math.cos(i))
        axises.append(i)
        i = i + T / N

    X = FFT(x, 1)

    mag = []
    phase = []
    freqs = []

    for i in range(N):
        mag.append(math.sqrt((X.real[i] ** 2) + (X.imag[i] ** 2)))
        phase.append(math.atan(X.imag[i] / X.real[i]))

    mag = mag[-N // 2:] + mag[:-N // 2]
    phase = phase[-N // 2:] + phase[:-N // 2]

    for i in range(-N // 2, N // 2):
        freqs.append(i / T)

    reconstructedX = FFT(X, -1)

    libMag = []
    libPhase = []
    libX = fft(x)

    for i in range(N):
        libX[i] /= N

    for i in range(N):
        libMag.append(math.sqrt((libX.real[i] ** 2) + (libX.imag[i] ** 2)))
        libPhase.append(math.atan(libX.imag[i] / libX.real[i]))

    libMag = libMag[-N // 2:] + libMag[:-N // 2]
    libPhase = libPhase[-N // 2:] + libPhase[:-N // 2]

    recontructedLibX = ifft(libX)

    figure, axis = plt.subplots(3, 4)

    axis[0, 0].plot(axises, x)
    axis[0, 0].set_title("Original x")

    axis[0, 1].plot(axises, reconstructedX.real)
    axis[0, 1].set_title("Reconstruted x")

    axis[1, 0].plot(freqs, mag)
    axis[1, 0].set_title("FFT Mag")

    axis[1, 1].plot(freqs, phase)
    axis[1, 1].set_title("FFT Phase")

    axis[2, 0].plot(range(N), X.real)
    axis[2, 0].set_title("FFT Real")

    axis[2, 1].plot(range(N), X.imag)
    axis[2, 1].set_title("FFT Imag")

    axis[0, 2].plot(axises, x)
    axis[0, 2].set_title("Original x")

    axis[0, 3].plot(axises, recontructedLibX.real)
    axis[0, 3].set_title("Reconstruted x Lib")

    axis[1, 2].plot(freqs, libMag)
    axis[1, 2].set_title("FFT Mag Lib")

    axis[1, 3].plot(freqs, libPhase)
    axis[1, 3].set_title("FFT Phase Lib")

    axis[2, 2].plot(range(N), libX.real)
    axis[2, 2].set_title("FFT Real Lib")

    axis[2, 3].plot(range(N), libX.imag)
    axis[2, 3].set_title("FFT Imag Lib")

    plt.show()