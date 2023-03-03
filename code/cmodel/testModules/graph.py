# using python for graphs, because it is faster than mathematica for this ammount of data and not locked as matlab to one folder


# importing dependencies
import pandas as pd
from matplotlib import pyplot as plt

# setting global font for plots
plt.rcParams["font.family"] = "Times New Roman"


# defining custom CTU FEE colors
ctuBlue="#0065BD"
ctuLightBlue = "#6AADE4"
ctuRed = "#C60C30"
ctuGreen = "#A2AD00"
ctuGreenyBlue = "#00B2A9"
ctuOrange = "#E05206"
ctuYellow = "#F0AB00"


# setting figure size, the value is in inches, but help from one tutorial gives info, that because of dpi it translates to pixels like inches * 100 = pixels
plt.rcParams["figure.figsize"] = [15, 6]

# some kind of autolayout
plt.rcParams["figure.autolayout"] = True

# defining which columns are in inported CSV
columns = ["calculationTime", "commonModeVoltage", "compareLevel", "sw1"]

# reading csv, using defined column names for columns in imported data to be referenced later
# header=None is information, that there is no header names in csv = just plain data
# skiprows - skip certain amount rows from the start of file
# nrows - read only number of rows after skipped rows
df = pd.read_csv("./outputData/switchCompareData.csv", names=columns, header=None, skiprows=60, nrows=10000)

# print out part of the csv files as a text to terminal
print("Contents in csv file:", df)

# naming figure window
figure_first = plt.figure("Switch and compare level")

# setting plot title and font
plt.title("Switch and compare level", fontsize=30, fontweight=700)

# ax = axis, but subplot is here mainly for deleting some axis on top and right
ax = plt.subplot(111)

# formating ticks, there can be used fontweight="bold"
plt.yticks(fontsize=15, fontweight=700)
plt.xticks(fontsize=15, weight=700)

#  disabling ticks

# adding grid
plt.grid(color = 'gray', linestyle = '--', linewidth = 0.5)

# thanks to subplot disabling right and top axis = basically deleting frame (not frame exacly, because it can be added, deleting axis is better name for that operation)
ax.spines[['right', 'top']].set_visible(False)

# ploting to graphs to one figure with different colors
plt.plot(df.calculationTime,df.sw1, color=ctuRed)
plt.plot(df.calculationTime,df.compareLevel, color=ctuBlue)


# naming new figure to not paste figure to same subplot, it would be basted if just number value would be used instead of name inf plt.figure("HER WRITE YOUR FIGURE NAME")
plt.figure("New figure")
plt.plot(df.calculationTime,df.commonModeVoltage, color=ctuBlue)

# show all plots and figures which were not shown
# if show is pasted after every plot/figure block, it displays just one figure, after that figure is closed, next figure is displayed
plt.show()