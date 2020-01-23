import numpy as np
from scipy.stats import norm
import matplotlib.pyplot as plt
plt.style.use('dark_background')

def trunc_gauss(x, mu, sig, start, end):
    y = np.zeros(len(x))
    ind = np.argwhere(np.logical_and(np.abs(x-mu) < end, np.abs(x-mu) > start))
    y[ind] = norm.pdf(x[ind], mu, sig)
    return y

R = 1000
diff = 200
tol_low = 0.05
tol_high = 0.1
x = np.arange(R-diff, R+diff, 0.1)
y = trunc_gauss(x, R, R/20, R*tol_low, R*tol_high)
y2 = trunc_gauss(x, R, R/20, 0, 2*R)

plt.plot(x, y2, '--')
plt.plot(x, y)
plt.legend(['Gaussian Distribution', 'Truncated Distribution'])

plt.xlabel('Resistance [Ohms]')
plt.ylabel('Probability')
plt.title('Probability Distribution of 1 kOhm Resistor with 10% tolerance')
plt.show()
