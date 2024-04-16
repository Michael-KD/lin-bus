import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# graph average.csv, scatter plot
df = pd.read_csv('average.csv')
df = df.set_index('Bytes')
plt.scatter(df.index, df['Time (us)'])
plt.xlabel('Bytes')
plt.ylabel('Time (us)')
plt.title('Average Time vs. Bytes')
plt.show()

# print slope of the line
x = df.index
y = df['Time (us)']
slope, intercept = np.polyfit(x, y, 1)
print('Slope:', slope)
print('Intercept:', intercept)


print()