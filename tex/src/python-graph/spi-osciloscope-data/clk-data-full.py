# importing dependencies
# GRAPH USED IN A CONJUCTION WITH testCalculationLoop file program
import numpy as np
import pandas as pd
from matplotlib import pyplot as plt

# setting global font for plots
plt.rcParams["font.family"] = "Times New Roman"

# defining custom CTU FEE colors
ctuBlue = "#0065BD"
ctuLightBlue = "#6AADE4"
ctuRed = "#C60C30"
ctuGreen = "#A2AD00"
ctuGreenyBlue = "#00B2A9"
ctuOrange = "#E05206"
ctuYellow = "#F0AB00"

# setting figure size, the value is in inches, but help from one tutorial gives info, that because of dpi it translates to pixels like inches * 100 = pixels
plt.rcParams["figure.figsize"] = [15, 9]

# some kind of autolayout
plt.rcParams["figure.autolayout"] = True

# defining which columns are in inported CSV
columns = ["time", "data", "clk"]
df = pd.read_csv("k26-spi-clk-10-mhz-data-0x10000001-copy.csv", names=columns, header=None,)


# print out part of the csv files as a text to terminal
print("Contents in csv file:", df)

# first figure
figure_first = plt.figure("clk and data", constrained_layout=True)

plt.title("clk and data", fontsize=30, fontweight=700)

# ax = axis, but subplot is here mainly for deleting some axis on top and right

# first plot
ax = plt.subplot(211)
ax.set_title("SPI CLK signál 10 MHz", fontsize=30, fontweight=700)
# formating ticks, there can be used fontweight="bold"
plt.yticks(fontsize=15, fontweight=700)
plt.xticks(fontsize=15, weight=700)
# adding grid
plt.grid(color='gray', linestyle='--', linewidth=0.5)
ax.spines[['right', 'top']].set_visible(False)

plt.plot(df.time, df.clk, color=ctuBlue, label="MOSI")

# setting ticks based on min and max time (on x axis)
plt.xticks(np.arange(df.time.min(), df.time.max(), 0.0000002))
# y axis
plt.yticks(np.arange(-1, 4.5, 0.5))

# setting plot labels
plt.xlabel("time (s)", fontsize=20, fontweight=400, loc="right")
plt.ylabel("voltage (V)", fontsize=20, fontweight=400,loc = "top", rotation=0,  labelpad=20)

# setting legend
plt.legend(bbox_to_anchor=(0.5, -0.1), ncol=2, fontsize=15, frameon=False)

# offset text on x axis

# ax.xaxis.get_offset_text().set_visible(False) # disable offset text - for eg. 10^(6)

ax.xaxis.get_offset_text().set_fontsize(18)
offset_text = ax.xaxis.get_offset_text()
offset_text.set_position((0, 0))  # move horizontally to the center and down by 

# setting y axis label coordinates
ax.yaxis.set_label_coords(-0.05, 0.5)




# second plot
ax2 = plt.subplot(212)

# title
ax2.set_title("SPI MOSI signál 0x10000001", fontsize=30, fontweight=700)

# hiding axis
ax2.spines[['right', 'top']].set_visible(False)

# formating ticks, there can be used fontweight="bold"
plt.yticks(fontsize=15, fontweight=700)
plt.xticks(fontsize=15, weight=700)

# adding grid
plt.grid(color='gray', linestyle='--', linewidth=0.5)

# main data plot
plt.plot(df.time, df.data, color=ctuRed, label="CLK")

# ticks x axis
plt.xticks(np.arange(df.time.min(), df.time.max(), 0.0000002))
# y axis
plt.yticks(np.arange(-1, 4.5, 0.5))

# axis labels
plt.xlabel("time (s)", fontsize=20, fontweight=400, loc="right")
plt.ylabel("voltage (V)", fontsize=20, fontweight=400,loc = "top", rotation=0,  labelpad=20)

# legend
plt.legend(bbox_to_anchor=(0.5, -0.1), ncol=2, fontsize=15, frameon=False)

# ax2.xaxis.get_offset_text().set_visible(False)
ax2.yaxis.set_label_coords(-0.05, 0.5)

#ticks
plt.tick_params(axis="both", labelsize=15)

# offset x axis label
ax2.xaxis.get_offset_text().set_fontsize(18)
offset_text = ax2.xaxis.get_offset_text()
offset_text.set_position((0, 0))

# save as a file
plt.savefig("pdf/clk-data-full-graph.pdf")

# show all preceeding graphs
plt.show()
