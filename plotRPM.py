from matplotlib import pyplot as plt
import csv
import numpy as np
import pandas as pd
from pandas import Series, DataFrame

df = pd.read_csv('RPM10.csv')

sample = df['Sample#']
time = df['Time (msec.)']
rpm = df[' RPM']
maxrpm = df[' Max. RPM']

plt.grid(True, lw=2, ls='--', c='.75')
plt.plot(time, rpm, label="RPM", c='blue')
plt.plot(time, maxrpm, label="Max. RPM", c='red', lw=2, ls='-.')

plt.xlabel("Time (sec.)")
plt.ylabel("RPM")
plt.title("RPM and Max. RPM")

plt.legend()
plt.show()
