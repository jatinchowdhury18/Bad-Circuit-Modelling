# %%
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import ticker
import audio_dspy as adsp
import random as r
import scipy.optimize as opt

plt.style.use('dark_background')

# %% [markdown]
# # Bad Circuit Modelling: Op Amps
# 
# So far in this bad circuit modelling series, we've been
# assuming that any op amps in our circuits are perfect, ideal
# components. Today, we're going to flip that paradigm, and assume
# that all of the components in our circuit are ideal *except*
# for the op amps. In this writing, we'll examine the popular
# Texas Instruments LM741 op amp, but the same principles could
# be applied to any similar op amp.
# 
# ## Op Amp Limitations
# 
# There are a lot of ways to look at op amps in a non-idealized
# way. For instance, op amps are themselves made up of components
# including resistors, capacitors, transistors, and diodes. It would
# be possible to develop a model of non-ideal op amps based exclusively
# on the non-ideal behaviors of these internal components, however
# this would prove to be quite complex, and given the vast experience
# of engineers and circuit designers working with op amps as
# isolated components in and of themselves, in this article,
# we'll focus on op amps as holistic, indivisible components.
# 
# In circuit design we usually assume that op amps have zero input
# current, infinite input impedance, and zero output impedance.
# While the LM741 does allow a tiny bit of input current (~80 nA),
# and does not have perfect impedance characteristics, these
# imperfections are usually negligible for audio circuits. Further,
# op amps have a "slew rate" that limits their behavior at extremely
# high frequencies, much lower than the frequencies we typically
# work with in audio circuits.
# 
# In this article, we'll focus on two main aspects of non-ideal
# op amps: the effects of temperature and aging on the op amp's
# bandwidth and noise characteristics.
# 
# ## Circuit Analysis
# 
# As usual, we'll start with our circuit: the Sallen-Key lowpass filter.

# %%
from IPython.display import Image
Image(filename='../CMTolerance/Pics/Sallen-Key.png')

# %% [markdown]
# Once again, the filter cutoff frequency and Q value are as follows:
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
def design_SKLPF(R, C, R1, R2, fs, age=1, temp=300):
    fc = 1.0 / (2 * np.pi * R * C)
    Q = 1.0 / (2 - (R2 / R1))

    return adsp.design_LPF2(fc, Q, fs)

fs = 44100
b, a = design_SKLPF(4.7e-9, 33800, 1000, 1500, fs)
adsp.plot_magnitude_response(b, a, fs=fs)
plt.ylim(-60)
plt.title('Sallen-Key LPF')

# %% [markdown]
# # Op Amp Temperature Dependence
# 
# In 2002, NASA released an interesting [paper](https://nepp.nasa.gov/docuploads/AE0ACAE4-7009-423A-AB47AC03FD7C6B30/LT%20Test%20Report%20Op%20181.pdf)
# documenting the effects of extreme temperatures on Op-Amp performance.
# While the paper examines the now defunct OP181 op amp, we
# can scale their results to apply to the LM741. Most notably,
# the paper finds that the op-amp displays a temperature-bandwidth
# dependence described by the following chart.

# %%
Image(filename='./Pics/NASA.jpg')

# %% [markdown]
# We can approximate this bandwidth limitation as a first-order
# lowpass filter, with cutoff frequency dependent on the operating
# temperature. Let's take a look at the distribution of the cutoff
# frequencies as a function of temperature.

# %%
x = np.array([90, 25, -125, -170, -185]) + 273
y = np.array([6, 6, 5, 3, 1.5])

plt.plot(x,y,'o')
plt.title('Cutoff Frequency vs. Temperature')
plt.xlabel('Temerature [K]')
plt.ylabel('Cutoff Frequency [kHz]')

# %% [markdown]
# Now we can fit this data to a curve, in this case we'll use a
# [binding](https://www.graphpad.com/guides/prism/7/curve-fitting/reg_classic_hyperbola.htm?toc=0)
# function.

# %%
def binding(x,kd,bmax):
    return (bmax*x)/(x+kd)
param=opt.curve_fit(binding, x,y)

plt.plot(x,y,'o',np.arange(500),binding(np.arange(500),*param[0]))
plt.title('Cutoff Frequency vs. Temperature')
plt.xlabel('Temerature [K]')
plt.ylabel('Cutoff Frequency [kHz]')

print(param[0])

# %% [markdown]
# After scaling the data to be appropriate for the LM741, we get something like the following:
# 
# $$
# f_c = \frac{30.96 T}{T + 290.48}
# $$
# 
# where T is the temperature in Kelvin, and $f_c$ is the cutoff frequency in kHz.

# %%
x2 = np.array([90, 25, -125, -170, -185]) + 273
y2 = np.array([6, 6, 5, 3, 1.5]) * 2.7
param2=opt.curve_fit(binding, x2,y2)

plt.plot(x,y,'o',np.arange(500),binding(np.arange(500),*param[0]))
plt.plot(x2,y2,'o',np.arange(500),binding(np.arange(500),*param2[0]))
plt.title('Cutoff Frequency vs. Temperature (LM741)')
plt.xlabel('Temerature [K]')
plt.ylabel('Cutoff Frequency [kHz]')
plt.legend(['OP181 Data', 'OP181 Curve', 'LM741 Scaled Data', 'LM741 Curve'])

print(param2[0])

# %% [markdown]
# Then we can add the following frequency response to the ideal frequency response
# of the lowpass filter.

# %%
fs = 96000

x = np.array([25, -125, -185, -225]) + 273
legend = []

for T in x:
    fc = binding(T, *param[0])*1000
    b, a = adsp.design_LPF1(fc, fs)
    
    adsp.plot_magnitude_response(b, a, fs=fs)
    legend.append('Temperature: {}K'.format(T))
    
plt.ylim(-15)
plt.xlim((10, 50000))
plt.legend(legend)
plt.title('Temperature-Frequency Response Dependence')

# %% [markdown]
# ## Op Amp Aging
# 
# R.G. Keen has a wonderful [article](http://www.geofex.com/circuits/when_good_opamps_go_bad.htm)
# about op amp aging. Essentially as op amps grow old, and are
# subjected various sorts of "zaps", they begin to develop
# a noise characteristic, and lose a bit more of their
# bandwidth. The noisep power should rise with age, with
# a characteristic similar to the chart shown below.

# %%
x = np.array([0, 10, 30, 50, 100])
y = np.array([-120, -60, -45, -30, -20]) + 120

param=opt.curve_fit(binding, x, y)

plt.plot(x,y-120,'o',np.arange(100), binding(np.arange(100),*param[0])-120)
plt.title('Noise Level vs. Age')
plt.xlabel('Age [yrs]')
plt.ylabel('Level [dB]')

print(param[0])

# %% [markdown]
# From my own personal experience, I've found that as op amps get old and start to fail, they tend
# to display a distortion characteristic similar to the well-known "dropout" nonlinearity (shown
# below).

# %%
Image(filename='../../Complex_Nonlinearities/Paper/Pics/dropout.png')

# %% [markdown]
# ## Implementation
# 
# As usual, I have implemented a model of the Sallen-Key lowpass filter
# with variable temperature and aging parameters, to show how the imperfections
# explained above affect the overall sound of the filter. Source code for
# this implementation is available on [GitHub](https://github.com/jatinchowdhury18/Bad-Circuit-Modelling),
# and a video demo can be found on [YouTube](https://youtu.be/xMLNG588b5A).

# %%


