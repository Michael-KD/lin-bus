import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import linregress

data = pd.read_csv('data.csv').to_numpy()

#convert data to graphable form
bytes = data[:, 0]
reformattedData = []
i = 0
for byteData in data:
    print(byteData)
    for time in byteData[:]:
        reformattedData.append([bytes[i], time / 1000])
    i += 1
reformattedData = np.array(reformattedData)

print(reformattedData)