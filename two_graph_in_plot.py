import matplotlib.pyplot as plt

n_tr = [3, 4, 5, 6, 7, 8]
frac_tr_80 = [93.8, 88.9, 82.5, 75.1, 67.0, 58.5]
frac_tr_180 = [98.5, 97.4, 95.9, 93.9, 91.5, 88.6]

t_w = [50, 100, 150, 200, 300, 400, 500]
frac_w_80 = [55.6, 77.5, 87.7, 92.9 , 97.5, 99.1, 99.6]
frac_w_180 = [61.6, 83.1, 91.8, 95.8, 98.8, 99.7, 99.9]

plt.figure(figsize=(9, 7))
plt.plot(t_w, frac_w_80, 'ro', label='80 PMTs')
plt.plot(t_w, frac_w_180, 'bs', label='180 PMTs')
plt.xlabel('$t_w, ns$', fontsize=26, fontweight='bold')
plt.xlim((30, 520))
plt.xticks(fontsize=20)
plt.ylabel('$\epsilon$, %', fontsize=26, fontweight='bold')
plt.yticks(fontsize=20)
plt.ylim((52, 102))
plt.legend()
plt.legend(loc=4, prop={'size': 22})
plt.show()
