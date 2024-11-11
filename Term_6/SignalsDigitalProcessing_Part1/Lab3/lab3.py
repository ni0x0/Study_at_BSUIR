import math
import numpy as np
import scipy
from scipy.linalg import dft
from scipy.fft import ifft, fft
from matplotlib import pyplot as plt
from scipy import signal

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

        for j in range(N//2):
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
        s = format(i, '0>'+ str(int(math.log(N, 2))) +'b')
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

def get_lin_convolution(y, h, M):
    result = np.array(y).copy()
    for i in range(M, np.size(y)):
        result[i] = 0
        for j in range(M + 1):
            result[i] = result[i] + y[i - j] * h[j]

    return result

def hamming_window(N, n):
    return 0.54 + 0.46 * np.cos(2 * np.pi * n / N)

def get_filter_impulse(M, Fc):
    filter_arguments = []
    for i in range(M + 1):
        window = hamming_window(M, i)
        temp = i - M / 2
        if temp == 0:
            filter_arguments.append(2 * np.pi * Fc)
        else:
            filter_arguments.append(np.sin(2 * np.pi * Fc * temp) / temp)
        filter_arguments[i] = filter_arguments[i] * window

    return [element / np.sum(filter_arguments) for element in filter_arguments]

def bandpass_hamming_window_filter(seq, M, fd, lowWn, highWn):
    Fc = lowWn/fd
    low_filter_impulse = get_filter_impulse(M, Fc)

    Fc = highWn/fd
    high_filter_impulse = get_filter_impulse(M, Fc)

    i = 0
    while i < M + 1:
        high_filter_impulse[i] = -high_filter_impulse[i]
        i += 1
    high_filter_impulse[M//2] += 1

    result_filter_impulse = [a+b for (a, b) in zip(low_filter_impulse, high_filter_impulse)]

    i = 0
    while i < M + 1:
        result_filter_impulse[i] = -result_filter_impulse[i]
        i += 1
    result_filter_impulse[M//2] += 1


    coagulation = get_lin_convolution(seq, result_filter_impulse, M)
    return coagulation, result_filter_impulse


def lowpass(data: np.ndarray, cutoff: float, sample_rate: float, poles: int = 5):
    sos = scipy.signal.butter(poles, cutoff, 'lowpass', fs=sample_rate, output='sos')
    filtered_data = scipy.signal.sosfiltfilt(sos, data)
    return filtered_data

def highpass(data: np.ndarray, cutoff: float, sample_rate: float, poles: int = 5):
    sos = scipy.signal.butter(poles, cutoff, 'highpass', fs=sample_rate, output='sos')
    filtered_data = scipy.signal.sosfiltfilt(sos, data)
    return filtered_data

if __name__ == '__main__':

    N = 4096
    M = 1024
    PERIOD = 2 * np.pi
    arguments = np.linspace(0, PERIOD, N)
    initial_sequence = list(map(lambda x: np.cos(7 * x), arguments))
    noise = list(map(lambda x: np.sin(PERIOD * 15 * x) + np.cos(x), arguments))
    noise_sequence = [i + j for (i, j) in zip(initial_sequence, noise)]

    filtredLowX, test = bandpass_hamming_window_filter(noise_sequence, M, N/PERIOD, 1, 5)
    libHigh = highpass(noise_sequence, 1, N/PERIOD)
    libLow = lowpass(libHigh, 5, N/PERIOD)
    noise_only, test = bandpass_hamming_window_filter(noise_sequence, M, N/PERIOD, 14, 17)


    figure, axis = plt.subplots(3, 2) 

    axis[0, 0].plot(arguments, initial_sequence)
    axis[0, 0].set_title("initial_sequence") 

    axis[0, 1].plot(arguments, noise_sequence)
    axis[0, 1].set_title("noised_sequence") 

    axis[1, 0].plot(arguments[M+1:], filtredLowX[M+1:])
    axis[1, 0].set_title("Own filtered signal") 

    axis[1, 1].plot(arguments, libLow)
    axis[1, 1].set_title("Lib filtred signal") 

    axis[2, 0].plot(range(M + 1), test)
    axis[2, 0].set_title("Filter impulse") 
    
    axis[2, 1].plot(arguments[M+1:], noise_only[M+1:])
    axis[2, 1].set_title("Noise only")



    plt.show()