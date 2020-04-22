# signals.py
#
#  Functions for common system identification signals
#
#  author: Tyler McCown (tylermccown@engineering.ucla.edu)
#  created: 15 March 2020

from math import pi, sin, log, exp

# sine signal
sine_amp = 0
sine_off = 0
sine_freq = 0

# logarithmic chirp signal
chirp_amp = 0
chirp_w1 = 1
chirp_w2 = 2
chirp_per = 2**15-1
chirp_alpha = 1./chirp_per*log(chirp_w2/chirp_w1)
chirp_off = 0


def configure_sine(amp, freq, offset):
    global sine_amp
    global sine_off
    global sine_freq

    sine_amp = amp
    sine_off = offset
    sine_freq = freq


def configure_chirp(amp, start_freq, stop_freq, per, offset):
    global chirp_amp
    global chirp_w1
    global chirp_w2
    global chirp_per
    global chirp_off
    global chirp_alpha

    chirp_amp = amp
    chirp_w1 = start_freq
    chirp_w2 = stop_freq
    chirp_per = per
    chirp_off = offset
    chirp_alpha = 1. / chirp_per * log(chirp_w2 / chirp_w1)


def sine(t):
    return sine_off + sine_amp*sin(2*pi*sine_freq*t)


def chirp(t):
    t = t % chirp_per
    f = chirp_w1/chirp_alpha*(exp(chirp_alpha*t) - 1)
    return chirp_off + chirp_amp*sin(f)
