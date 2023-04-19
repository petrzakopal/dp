# importing dependencies
# GRAPH USED IN A CONJUCTION WITH testCalculationLoop file program
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
columns = ["globalSimulationTime", "psi2amplitude","motorMechanicalAngularVelocity", "idRegulatorMeasuredValue", "idRegulatorWantedValue", "velocityRegulatorWantedValue", "velocityRegulatorSaturationOutput", "velocityRegulatorClampingStatus", "fluxRegulatorISum"]
df = pd.read_csv("./globalSimulationData.csv", names=columns, header=None, skiprows=0, nrows=10000000)

# print out part of the csv files as a text to terminal
print("Contents in csv file:", df)

figure_first = plt.figure("psi2amplitude")

plt.title("psi2amplitude", fontsize=30, fontweight=700)

# ax = axis, but subplot is here mainly for deleting some axis on top and right
ax = plt.subplot(111)
ax.spines[['right', 'top']].set_visible(False)
# formating ticks, there can be used fontweight="bold"
plt.yticks(fontsize=15, fontweight=700)
plt.xticks(fontsize=15, weight=700)

# adding grid
plt.grid(color = 'gray', linestyle = '--', linewidth = 0.5)

plt.plot(df.globalSimulationTime, df.psi2amplitude, color=ctuBlue, label="psi2amplitude")
plt.plot(df.globalSimulationTime, df.motorMechanicalAngularVelocity, color=ctuRed, label="angularVelocity")
plt.xlabel("time (s)",fontsize=20, fontweight=400, loc = "right")
plt.ylabel("psi2amplitude (Wb)\nangularVelocity (s^(-1))",fontsize=10 ,fontweight=400, loc = "top", rotation=0)
plt.legend(  bbox_to_anchor=(0.5, -0.05), ncol=2 )


plt.show()