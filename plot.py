import matplotlib.pyplot as plt

fname = "data.txt"
with open(fname) as f:
    content = f.readlines()
# you may also want to remove whitespace characters like `\n` at the end of each line
content = [x.split() for x in content]

idx = []
val = []

i=0
for x in range(len(content[0])):
    if(i==0):
        idx.append(content[0][x])
    else:
        val.append(content[0][x])
    
    i = (i+1)%2

idx = [int(x) for x in idx]
val = [int(x) for x in val]

plt.plot(idx, val, label = "line 2")
 
# naming the x axis
plt.xlabel('x - axis')
# naming the y axis
plt.ylabel('y - axis')
# giving a title to my graph
plt.title('Grafik jeda baris')
 
# show a legend on the plot
plt.legend()
 
# function to show the plot
plt.show()