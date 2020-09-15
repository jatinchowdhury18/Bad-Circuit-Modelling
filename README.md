# Bad Circuit Modelling

[![Build Status](https://travis-ci.com/jatinchowdhury18/Bad-Circuit-Modelling.svg?branch=master)](https://travis-ci.com/jatinchowdhury18/Bad-Circuit-Modelling)

This repository contains models of non-ideal circuits. The idea is to model
realistic variations in circuits, caused by things like imperfect components,
age, temperature, etc.

For each non-ideal circuit concept, we present simulation code, an example
plugin (VST, AU) built with JUCE, and an explanatory article, available
as either a Jupyter Notebook, or a Medium post.

Builds of each plugin are available for Mac and Windows in the [Bin/](./Bin/) directory.
Linux users can find builds available on the
[Open Build Service](https://build.opensuse.org/package/show/home:kill_it:JUCE/Bad-Circuit-Modelling),
courtesy of Konstantin Voinov.

---

Current non-ideal circuit concepts include:

**1. Component Tolerances**
[![Medium](https://img.shields.io/badge/Read%20on-Medium-blue)](https://medium.com/@jatinchowdhury18/bad-circuit-modelling-episode-1-component-tolerances-3ffdbe4e980c)
[![Jupyter](https://img.shields.io/badge/Read%20as-Jupyter-orange)](https://ccrma.stanford.edu/~jatin/Bad-Circuit-Modelling/Tolerances.html)

<img src="https://github.com/jatinchowdhury18/Bad-Circuit-Modelling/blob/master/CMTolerance/Pics/lpf_tgauss_plot.png" alt="Pic" width="500" height="350">

**2. Component Aging**
[![Medium](https://img.shields.io/badge/Read%20on-Medium-blue)](https://medium.com/@jatinchowdhury18/bad-circuit-modelling-episode-2-component-aging-faef126b7029)
[![Jupyter](https://img.shields.io/badge/Read%20as-Jupyter-orange)](https://ccrma.stanford.edu/~jatin/Bad-Circuit-Modelling/Aging.html)

<img src="https://github.com/jatinchowdhury18/Bad-Circuit-Modelling/blob/master/CMAging/Pics/FullFail.png" alt="Pic" width="500" height="350">

**3. Capacitor Temperature**
[![Medium](https://img.shields.io/badge/Read%20on-Medium-blue)](https://medium.com/@jatinchowdhury18/bad-circuit-modelling-episode-3-temperature-e31d124767f1)
[![Jupyter](https://img.shields.io/badge/Read%20as-Jupyter-orange)](https://ccrma.stanford.edu/~jatin/Bad-Circuit-Modelling/Temperature.html)

<img src="https://github.com/jatinchowdhury18/Bad-Circuit-Modelling/blob/master/CMTemperature/Pics/Temperature.png" alt="Pic" width="350" height="377">

**4. Non-ideal Op Amps**
[![Medium](https://img.shields.io/badge/Read%20on-Medium-blue)](https://medium.com/@jatinchowdhury18/bad-circuit-modelling-episode-4-op-amps-a781bfe455af)
[![Jupyter](https://img.shields.io/badge/Read%20as-Jupyter-orange)](https://ccrma.stanford.edu/~jatin/Bad-Circuit-Modelling/OpAmp.html)

<img src="https://github.com/jatinchowdhury18/Bad-Circuit-Modelling/blob/master/OpAmp/Pics/temp_freq_response.png" alt="Pic" width="500" height="350">
