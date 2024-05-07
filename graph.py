import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import linregress

data = pd.read_csv('data.csv').to_numpy()

#convert data to graphable form
bytes = data[:, 0]
reformattedData = []
i = 0

for byteData in data[1:]:
    for time in byteData[1:]:
        reformattedData.append([bytes[i], time / 1000])
    i += 1
reformattedData = np.array(reformattedData)

#linreg
regression = linregress(reformattedData[:, 0], reformattedData[:, 1])

#residuals
residuals = []
for i in reformattedData:
    residuals.append([i[0], i[1] - (regression.intercept + regression.slope * i[0])])
residuals = np.array(residuals)
residuals[:, 1] *= 1000

#graph
fig, ax = plt.subplots(2, constrained_layout=True)
ax[0].axline((0, regression.intercept), (1, regression.intercept + regression.slope), color='gray', lw=0.5)
ax[0].scatter(reformattedData[:, 0], reformattedData[:, 1], zorder=10, marker='o')
ax[0].set_xlim([0, 512])
ax[0].set_ylim([0, 300])
ax[0].set_title("Time to send and receive, dependent on packet size")
ax[0].set_xlabel("Bytes of Data")
ax[0].set_ylabel("Time (ms)")
ax[0].text(250, 250, "y = " + "%.2f" % regression.slope + "x + " + "%.2f" % regression.intercept, color='gray')
ax[0].grid(color='lightgray')

ax[1].scatter(residuals[:, 0], residuals[:, 1], zorder=10, marker='o', alpha=0.1)
ax[1].axhline(color='black', lw=1)
ax[1].set_xlim([0, 512])
ax[1].set_title("Residual plot of time to send and receive")
ax[1].set_xlabel("Bytes of Data")
ax[1].set_ylabel("Deviation from regression (us)")
ax[1].grid(color='lightgray')

plt.savefig("graphs.png")
#plt.show()
