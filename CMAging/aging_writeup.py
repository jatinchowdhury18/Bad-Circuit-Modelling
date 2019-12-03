#%%
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import ticker
import audio_dspy as adsp
import random as r

plt.style.use('dark_background')

#%% [markdown]
# # Bad Circuit Modelling: Aging
#
# In this article, we'll be discussing another
# way that circuits in the real world can behave
# in a non-ideal way: component aging. Real circuit
# components always change with age, and usually
# for the worse. In this article, we'll be focusing
# specifically on resistors and capacitors, but in
# theory the same approach can be used for any
# circuit component.
#
# ## Circuit Analysis
#
# As with our previous article, we'll be examining
# a Sallen-Key lowpass filter.

#%%
from IPython.display import Image
Image(filename='../CMTolerance/Pics/Sallen-Key.png')

#%% [markdown]
# As before, we'll be assuming that the op-amp is
# ideal, and examining what happens to the circuit
# for non-ideal resistors and capacitors. Once again,
# the filter cutoff frequency and Q value are as
# follows:
#
# $$
# f_c = \frac{1}{2 \pi R C}
# $$
# $$
# Q = \frac{1}{2 - \frac{R_2}{R_1}}
# $$
#
# And again, we'll design our filter for $f_c
# = 1 \text{ kHz}$, and $Q = 2$; $C = 4.7
# \text{ nF}$, $R = 33.8 \text{ kOhms}$,
# $R_2 = 1.5 \text{ kOhms}$, and$R_1 = 1
# \text{ kOhms}$.

# %%
def getResVal(val, age, temp):
    frac = 1.51e12 * age**0.61 * np.exp(-15087 / temp)
    return val * (1 + frac)

def getCapVal(val, age, temp, fail=True):
    if (fail):
        pred_lifetime = 10000 * 2**((450 - temp) / 10)
        lifetime = r.gauss(1.2*pred_lifetime, 0.1*pred_lifetime)

        if (age > lifetime): # failure
            return r.uniform(0.0, 1.0) * 15.0*val + 0.5*val

    return val * (1 - 0.025 * np.log10(age))

def design_SKLPF(R, C, R1, R2, fs, age=1, temp=300, ageRs=True, ageCs=True, failCs=True):
    fc = 1.0 / (2 * np.pi * getResVal(R, age if ageRs else 1, temp) * getCapVal(C, age if ageCs else 1, temp, fail=failCs))
    Q = 1.0 / (2 - (getResVal(R2, age if ageRs else 1, temp) / getResVal(R1, age if ageRs else 1, temp)))

    return adsp.design_LPF2(fc, Q, fs)

fs = 44100
b, a = design_SKLPF(4.7e-9, 33800, 1000, 1500, fs)
adsp.plot_magnitude_response(b, a, fs=fs)
plt.ylim(-60)
plt.title('Sallen-Key LPF')

# %% [markdown]
# ## Resistor Aging
#
# As a resistor grows old, the resistance tends to
# increase. For a typical thin-film resistor, the
# age dependence is described by [[1]]:
#
# $$
# \frac{\Delta R}{R} = (1.51 \times 10^{12})\ 
# t^{0.61}\  e^{- 15,087 / T}
# $$
#
# where $t$ is the length of time the resistor
# has been used (measured in hours), and $T$ is
# the operating temperature of the resistor,
# measured in Kelvins

# %%
temp = 400
R = 1000
time = np.arange(100 * 365 * 24)

plt.plot(time / (365*24), getResVal(R, time, temp) / 1000)
plt.ylim((R / 1000, 1.5*R / 1000))

plt.title('Resistance vs. Time')
plt.xlabel('Time [years]')
plt.ylabel('Resistance [kOhms]')

# %% [markdown]
# The plot above shows what happens to a 1 kOhms
# resistor at an operating temperature of 400 K,
# over a period of 100 years. While these changes
# may seem relatively small, they compound over
# all the resistors in the circuit. Below we show
# the frequency response of the Sallen-Key lowpass
# filter at different ages, again with an operating
# temperature of 400 K.

# %%
legend = []
for age in [1, 100, 200, 300]:
    b, a = design_SKLPF(4.7e-9, 33800, 1000, 1500, fs, age=age*365*24, temp=400, ageCs=False)
    adsp.plot_magnitude_response(b, a, fs=fs)
    legend.append('{} years'.format(age))

plt.legend(legend)
plt.ylim(-30)
plt.xlim((100, 10000))
plt.title('Sallen-Key LPF with Resistor Aging')

#%% [markdown]
# ## Capacitor Aging
#
# For audio, it is typical to use class II X7R
# capacitor, since this type of capacitor has
# a minimal amount of voltage dependence, meaning
# the resulting circuit will have a minimal Total
# Harmonic Distortion (THD). For this class of
# resistor, the capacitance decreases by ~2.5%
# per decade hour (one decade hour means the number
# of hours has been multiplied by 10) [[2]]. Below
# we show how a capacitor ages over a period of 100
# years.

#%%
C = 1e-6
time = np.arange(1, 100 * 365 * 24)

plt.semilogx(time / (365*24), getCapVal(C, time, 400, fail=False) * 1e6)
plt.gca().xaxis.set_major_formatter(ticker.LogFormatter())

plt.title('Capacitance vs. Time')
plt.xlabel('Time [years]')
plt.ylabel(r'Capacitance [$\mu$F]')

#%% [markdown]
# And again, we show how the frequency response of
# the Sallen-Key lowpass filter changes as the
# capacitors age.

#%%
legend = []
for age in [1, 100, 200, 300, 400, 500]:
    b, a = design_SKLPF(4.7e-9, 33800, 1000, 1500, fs, age=age*365*24, temp=400, ageRs=False, failCs=False)
    adsp.plot_magnitude_response(b, a, fs=fs)
    legend.append('{} years'.format(age))

plt.legend(legend)
plt.ylim(-30)
plt.xlim((100, 10000))
plt.title('Sallen-Key LPF with Capacitor Aging')

#%%[markdown]
# ### Capacitor Failure
#
# While Capacitor aging makes a pretty small difference,
# a bigger issue in old electronic circuits tends to be
# capacitor failure. For the class of capacitors
# considered here, the expected lifetime is approximately:
#
# $$
# L = (5000 \text{ hours})\ 2^{(373 - T) / 10}
# $$
#
# where $T$ is again the operating temperature in
# Kelvin. While there is some fluctuation in actual
# failure times, capacitor lifetime tends to follow
# a normal distribution [[3]]. When we include failure in
# our capacitor model, we see:

#%%
legend = []
for age in [1, 10, 100]:
    b, a = design_SKLPF(4.7e-9, 33800, 1000, 1500, fs, age=age*365*24, temp=400, ageRs=False)
    adsp.plot_magnitude_response(b, a, fs=fs)
    legend.append('{} years'.format(age))

plt.legend(legend)
plt.ylim(-30)
plt.xlim((100, 10000))
plt.title('Sallen-Key LPF with Capacitor Aging and Failure')

#%%[markdown]
# In this case the capacitor failed after 100 years, causing the large jump
# in the cutoff frequency of the filter.
#
# ## Putting It All Together
#
# Ultimately, when  we combine the results of resistor and capacitor aging
# and failure, we see a dependence on age that looks like this:

#%%
legend = []
worN = np.logspace(1, 3.3, num=1000, base=20)
for age in [1, 10, 20, 30, 80]:
    b, a = design_SKLPF(4.7e-9, 33800, 1000, 1500, fs, age=age*365*24, temp=400)
    adsp.plot_magnitude_response(b, a, fs=fs)
    legend.append('{} years'.format(age))

plt.legend(legend)
plt.ylim(-30)
plt.xlim((100, 10000))
plt.title('Sallen-Key LPF with Resistor and Capacitor Aging and Failure')

# %% [markdown]
# ## Sources
# 
# [[1]] Brady, et al., *Thermal Oxidation and
# Resistivity of Tantalum Nitride Films*, Thin
# Solid Films 66 (1980), pp. 287-302
#
# [[2]] England, C., *Ceramic Capacitor Aging Made
# Simple*, https://johansondielectrics.com/downloads/ceramic-capacitor-aging-made-simple.pdf
#
# [[3]] Parler, S., *Reliability of CDE Aluminum
# Electrolytic Capacitors*, https://www.cde.com/resources/technical-papers/reliability.pdf