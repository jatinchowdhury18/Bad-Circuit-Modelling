# %%
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import ticker
import audio_dspy as adsp
import random as r
from IPython.display import Image

plt.style.use('dark_background')

# %% [markdown]
# # Bad Circuit Modelling: Temperature
#
# Today, we'll be discussing one more way by which real-world analog
# circuits can exhbit non-ideal behavior. Circuit components tend to
# behave differently at different operating temperatures. So a guitar
# pedal, for instance, might sound a little bit different at a chilly
# winter concert in Montreal, compared to a sweltering summer gig in
# Austin, Texas. As with the previous two articles in this series, we'll
# be focusing on resistors and capacitors, but a similar thoertical
# approach can be applied to any circuit component.
#
# ## Circuit Analysis
#
# Once again, we'll be using a Sallen-Key lowpass filter as an example
# circuit. And again, we'll assume that the circuit op-amp is ideal, and
# only consider the non-ideal behavior of the circuit resistors and
# capacitors.

# %%
Image(filename='../CMTolerance/Pics/Sallen-Key.png')

# %% [markdown]
# The filter cutoff frequency and Q values can be calculated as follows:
#
# $$
# f_c = \frac{1}{2 \pi R C}
# $$
# $$
# Q = \frac{1}{2 - \frac{R_2}{R_1}}
# $$
#
# Let's design our filter for $f_c = 1 \text{ kHz}$, and $Q = 2$.
# Thus, our component values should be $C = 4.7 \text{ nF}$,
# $R = 33.8 \text{ kOhms}$, $R_2 = 1.5 \text{ kOhms}$, and
# $R_1 = 1 \text{ kOhms}$.

# %%
def getResVal(val, temp, alpha):
    return val * (1 + alpha * 10e-6 * (temp - 20))

def getCapVal(val, temp):
    return val

def design_SKLPF(R, C, R1, R2, fs, temp=20, alpha=0):
    fc = 1.0 / (2 * np.pi * getResVal(R, temp, alpha) * getCapVal(C, temp))
    Q = 1.0 / (2 - (getResVal(R2, temp, alpha) / getResVal(R1, temp, alpha)))

    return adsp.design_LPF2(fc, Q, fs)

fs = 44100
b, a = design_SKLPF(4.7e-9, 33800, 1000, 1500, fs)
adsp.plot_magnitude_response(b, a, fs=fs)
plt.ylim(-60)
plt.title('Sallen-Key LPF')

# %% [markdown]
# ## Resistor Temperature Dependence
#
# The temperature dependence of a resistor is described by the
# following equation:
#
# $$
# R = R_{ref} (1 + \alpha (T - T_{ref}))
# $$
#
# Where $R_{ref}$ is the ideal resistor value, $T$ is the operating
# temperature (measured in Celsius), $T_{ref}$ is the reference
# temperature where the ideal resistance value was measured (usually
# 20 Celsuis), and $\alpha$ is the temperature coefficient of the
# resistor material.
#
# Below, we show how a $1 k\Omega$ resistor is affected by temperature
# for resistors made of Carbon, Silicon, and Germanium.

# %%
R_ref = 1000
T_ref = 20
T = np.linspace(-273, 1000, num=100)

alphas = [-0.075, -0.048, -0.0005]
names = ['Silicon', 'Germanium', 'Carbon']

plt.figure()
for i, a in enumerate (alphas):
    plt.plot(T, R_ref * (1 + (a * 10e-6 * (T - T_ref))) / 1000, label=names[i])

plt.legend()
plt.xlabel(r'Temperature $^{\circ}C$')
plt.ylabel(r'Resistance $k\Omega$')
plt.ylim((0.999, 1.0006))

# %% [markdown]
# So for resistors, it seems that we don't see much change in the
# resistor's behavior, at least until we go out to extraordinarily
# extreme temperatures (for an average audio circuit). That said, it
# can still be interesting to see what happens to our Sallen-Key filter
# for different resistor temperatures. For this example, we'll use Silicon
# resistors.

# %%
alpha = -0.075
legend = []
for temp in [-100, 20, 1000, 10000, 100000]:
    b, a = design_SKLPF(4.7e-9, 33800, 1000, 1500, fs, temp=temp, alpha=alpha)
    adsp.plot_magnitude_response(b, a, fs=fs)
    legend.append('T = {}C'.format(temp))

plt.ylim(-60)
plt.title('Sallen-Key LPF with Resistor Temp. Dependence')
plt.legend(legend)


# %%
