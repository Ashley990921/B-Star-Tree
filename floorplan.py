import matplotlib 
import matplotlib.pyplot as plt 
  
  
fig = plt.figure() 
ax = fig.add_subplot(111)

x_cor = []
y_cor = []
width = []
height = []


with open('out') as f:
    next(f)
    next(f)
    next(f)
    lines=f.readline()
    s = lines.split(' ')
    xlength=float(s[0])
    ylength=float(s[1])
    next(f)
    for line in f.readlines():
        s = line.split(' ')
        x_cor.append(float(s[1]))
        y_cor.append(float(s[2]))
        width.append(float(s[3])-float(s[1]))
        height.append(float(s[4])-float(s[2]))


for i,j,k,l in zip(x_cor, y_cor, width, height):
    rect1 = matplotlib.patches.Rectangle((i, j), 
                                     k, l,   
                                     color ='pink',
                                     fill=False)
    ax.add_patch(rect1)


ax.set_aspect('equal', adjustable='box')
border=max(xlength,ylength)  
plt.xlim([0, border+200]) 
plt.ylim([0, border+200]) 
  
plt.show()
