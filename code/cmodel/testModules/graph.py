import pandas as pd
from matplotlib import pyplot as plt
plt.rcParams["font.family"] = "Times New Roman"

ctuBlue="#0065BD"
ctuLightBlue = "#6AADE4"
ctuRed = "#C60C30"
ctuGreen = "#A2AD00"
ctuGreenyBlue = "#00B2A9"
ctuOrange = "#E05206"
ctuYellow = "#F0AB00"

plt.rcParams["figure.figsize"] = [15, 6]
plt.rcParams["figure.autolayout"] = True
columns = ["calculationTime", "commonModeVoltage", "compareLevel", "sw1"]
df = pd.read_csv("./outputData/switchCompareData.csv", names=columns, header=None, skiprows=60, nrows=10000)
print("Contents in csv file:", df)
figure_first = plt.figure("Switch and compare level")
plt.title("Switch and compare level", fontsize=30, fontweight=700)
ax = plt.subplot(111)
plt.yticks(fontsize=15, fontweight=700)
plt.xticks(fontsize=15, weight=700)
plt.tick_params(top='off', bottom='off', left='off', right='off', labelleft='off', labelbottom='on')
plt.grid(color = 'gray', linestyle = '--', linewidth = 0.5)
ax.spines[['right', 'top']].set_visible(False)
plt.plot(df.calculationTime,df.sw1, color=ctuRed)
plt.plot(df.calculationTime,df.compareLevel, color=ctuBlue)



plt.figure("New figure")
plt.plot(df.calculationTime,df.commonModeVoltage, color=ctuBlue)
plt.show()