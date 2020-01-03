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
# Today, we'll give a brief discussion of another method by which
# real-world analog circuits can exhbit non-ideal behavior. Circuit
# components tend to behave differently at different operating temperatures.
# So a guitar pedal, for instance, might sound a little bit different at a
# chilly winter concert in Montreal, compared to a sweltering summer gig in
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
def design_SKLPF(R, C, R1, R2):
    fc = 1.0 / (2 * np.pi * R * C)
    Q = 1.0 / (2 - (R2 / R1))

    return adsp.design_LPF2(fc, Q, fs)

fs = 44100
b, a = design_SKLPF(4.7e-9, 33800, 1000, 1500)
adsp.plot_magnitude_response(b, a, fs=fs)
plt.ylim(-60)
plt.title('Sallen-Key LPF')

# %% [markdown]
# ## Capacitor Temperature Dependence
#
# While the resistance of the circuit resistors as well as
# the wires in the circuit are affected somewhat by the
# circuit operating temperature, these effects are mostly
# negligible. The most noticeable effect that temperature
# fluctutations will have on the circuit is that the capcitors
# will be noisier at higher temperatures. The standard deviation
# of the capacitor noise is defined by:
#
# $$
# Q_n = \sqrt{k_B T C}
# $$
#
# where $k_B$ is Boltzmann's constant, $T$ is the temperature
# in Kelvin, and $C$ is the capacitance value (for more information on
# capcitor noise of this type, see [here](https://en.wikipedia.org/wiki/Johnson%E2%80%93Nyquist_noise)).
# We can then determine the voltage that the noise from each capacitor
# will add to the output.
#
# For the capacitor in the upper feedback loop, the noise voltage can
# be added directly to the output. For the capacitor connected to the
# positive terminal of the op-amp, the noise voltage will be added
# through the voltage divider made up of R1 and R2.
#
# $$
# V_{out} = V_{LPF} + \delta V_{C1} + \delta V_{C2} \left(\frac{R_1 + R_2}{R_1}\right)
# $$
#
# ## Implementation
# Visualizing the effects of this capacitor noise is a little bit
# difficult, so to demonstrate these effects I have developed a simple
# audio plugin using the JUCE framework that implements the capacitor
# noise into a model of the Sallen-Key LPF circuit. Source code is
# available on [GitHub](https://github.com/jatinchowdhury18/Bad-Circuit-Modelling),
# and a video demo is available on [YouTube](https://youtu.be/Ww4Ihfai8go).
