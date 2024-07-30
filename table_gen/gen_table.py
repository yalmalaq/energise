from collections import defaultdict
from pprint import pprint
import numpy.linalg
import numpy as np
import os

LOAD_FROM = '../data/raw_data2'

MODE = 'rf'
TOTAL_CAP = 250 * 4 + 620 * 2 + 1024 * 2
MARGIN = 1 / (1 + 0.2)
BASE = 0.2

small_freqs = [300000, 574000, 738000, 930000, 1098000, 1197000, 1328000, 1401000, 1598000, 1704000, 1803000]
med_freqs   = [400000, 553000, 696000, 799000, 910000, 1024000, 1197000, 1328000, 1491000, 1663000, 1836000, 1999000, 2130000, 2253000]
large_freqs = [500000, 851000, 984000, 1106000, 1277000, 1426000, 1582000, 1745000, 1826000, 2048000, 2188000, 2252000, 2401000, 2507000, 2630000, 2704000, 2802000]

freqs      = None
counts     = None
work_rate  = None
total_work = None
power      = None

#rows = [(freqs, counts, work_rate, total_work, power)]
rows = []

'''

BUCK1M(S1M_VDD_MIF):MIF
BUCK2M(S2M_VDD_CPUCL2):CPU(BIG)
BUCK3M(S3M_VDD_CPUCL1):CPU(MID)
BUCK4M(S4M_VDD_CPUCL0):CPU(LITTLE)
BUCK5M(S5M_VDD_INT):INT
BUCK6M(S6M_VDD_CPUCL2_M):CPU(BIG)
BUCK7M(S7M_VDD_INT_M):INT
BUCK8M(S8M_LLDO2):LDO
BUCK9M(S9M_LLDO3):LDO
BUCK10M(S10M_VDD_TPU):TPU
LDO1M(L1M_ALIVE):Alive
LDO2M(L2M_ALIVE):DDR
LDO3M(L3M):IO
LDO4M(L4M):IO
LDO5M(L5M_TCXO):PLL
LDO6M(L6M_PLL):PLL
LDO7M(L7M_VDD_HSI):IO
LDO8M(L8M_USB):USB
LDO9M(L9M_USB):USB
LDO10M(L10M_USB):USB
LDO11M(L11M_VDD_CPUCL1_M):CPU(MID)
LDO12M(L12M_VDD_CPUCL0_M):CPU(LITTLE)
LDO13M(L13M_VDD_TPU_M):TPU
LDO14M(L14M_AVDD_TCXO):PLL
LDO15M(L15M_VDD_SLC_M):SLC
LDO16M(L16M_PCIE0):PCIE
LDO17M(L17M_PCIE1):PCIE
LDO18M(L18M_PCIE0):PCIE
LDO19M(L19M_PCIE1):PCIE
LDO20M(L20M_DMICS):DMIC
LDO21M(L21M_DTLS):DTLS
LDO22M(L22M_DISP_VCI):Display
LDO23M(L23M_DISP_VDDI):Display
LDO24M(L24M_UNUSED):Spare
LDO25M(L25M_TSP_DVDD):Touch
LDO26M(L26M_TS_AVDD):Touch
LDO27M(L27M_UDFPS_AVDD):UDFPS
LDO28M(L28M_DISP_VDDD):Display
LDO29M(L29M_UNUSED):Spare
LDO30M(L30M_UNUSED):Spare
LDO31M(L31M_DISP_VDDI):NFC
VSEN_C1(VSYS_PWR_DISPLAY):Display
VSEN_C2(VSEN_C2_NC):Spare
VSEN_C3(VSEN_C3_NC):Spare
BUCK1S(S1S_VDD_CAM):Multimedia
BUCK2S(S2S_VDD_G3D):GPU
BUCK3S(S3S_LLDO1):LDO
BUCK4S(S4S_VDD2H_MEM):DDR
BUCK5S(S5S_VDDQ_MEM):DDR
BUCK6S(S6S_LLDO4):LDO
BUCK7S(S7S_MLDO):LDO
BUCK8S(S8S_VDD_G3D_L2):GPU
BUCK9S(S9S_VDD_AOC):AOC
BUCK10S(S10S_VDD2L):DDR
BUCKD(SD):UFS
BUCKA(SA):IO
BUCKBOOST(BB_HLDO):LDO
LDO1S(L1S_VDD_G3D_M):GPU
LDO2S(L2S_VDD_AOC_RET):AOC
LDO3S(L3S_UNUSED):Spare
LDO4S(L4S_UWB):UWB
LDO5S(L5S_UNUSED):Spare
LDO6S(L6S_PROX):Sensors
LDO7S(L7S_SENSORS):Sensors
LDO8S(L8S_UFS_VCCQ):UFS
LDO9S(L9S_GNSS_CORE):GPS
LDO10S(L10S_GNSS_RF):GPS
LDO11S(L11S_GNSS_AUX):GPS
LDO12S(L12S_UNUSED):Spare
LDO13S(L13S_DPAUX):Display
LDO14S(L14S_UWB):UWB
LDO15S(L15S_UNUSED):Spare
VSEN_C4(VSEN_C4_NC):Spare
VSEN_C5(VSEN_C5_NC):Spare
VSEN_C6(VSEN_C6_NC):Spare


 'S3M_VDD_CPUCL1': 0.6534038246566592,
 'S4M_VDD_CPUCL0': 0.32597193388693707,
 'S2M_VDD_CPUCL2': 1.2794570424784415,


'''

#ms uws(=Jx10-6)
def post_process(row):
    freq, count, work_rate, total_work, power = row
    if count[0] == 0:
        freq[0] = 0
    if count[1] == 0:
        freq[1] = 0
    if count[2] == 0:
        freq[2] = 0
    power = power['S4M_VDD_CPUCL0'] + power['S3M_VDD_CPUCL1'] + power['S2M_VDD_CPUCL2']
    return freq, count, work_rate, total_work, [power]

for file in os.listdir(LOAD_FROM):
    with open(LOAD_FROM + '/' + file) as f:
        for i, line in enumerate(f):
            if 'smallf' in line or 'stopped' in line:
                if rows:
                    rows[-1] = post_process(rows[-1])
                freqs      = [0] * 3
                counts     = [0] * 3
                work_rate  = [0] * 8
                total_work = [0]
                power = {}
                rows.append((freqs, counts, work_rate, total_work, power))

            ###
            if line.startswith('smallf'):
                v = int(line.split()[1])
                freqs[0] = v
                
            elif line.startswith('medf'):
                v = int(line.split()[1])
                freqs[1] = v

            elif line.startswith('largef'):
                v = int(line.split()[1])
                freqs[2] = v

            elif line.startswith('enabled'):
                v = int(line.split()[1])
                for i in range(0, 8):
                    if ((v >> i) & 1):
                        if i in range(0, 4):
                            counts[0] += 1
                        elif i in range(4, 6):
                            counts[1] += 1
                        elif i in range(6, 8):
                            counts[2] += 1

            elif line.startswith('core_wr'):
                core =   int(line.split()[1])
                v    = float(line.split()[2])
                work_rate [core]  = v
                total_work[0]    += v
                        
            elif line.startswith('CH'):
                line, val = line.split(',')
                val = int(val.strip())
                
                time, name = line.split('[')
                name = name.split(']')[0]
                time = int(time.split('=')[1].split(')')[0])

                if name in power:
                    power[name] = ((val - power[name][1])/10**6) / ((time - power[name][0])/10**3)
                else:
                    power[name] = (time, val)

rows[-1] = post_process(rows[-1])
##########

max_work  = -1
max_power = -1

raw_values = dict() # conf -> (work, perf)

in_mat  = []
out_mat = []
for fs, cs, wrs, tw, tp in rows:
    in_rows  = [1] # base power
    for f in small_freqs:
        in_rows.append(cs[0] if fs[0] == f else 0)
        
    for f in med_freqs:
        in_rows.append(cs[1] if fs[1] == f else 0)
        
    for f in large_freqs:
        in_rows.append(cs[2] if fs[2] == f else 0)

    in_mat.append(in_rows)
    out_mat.append([tw[0], tp[0]])

    if tw[0] > max_work:
        max_work = tw[0]
    if tp[0] > max_power:
        max_power = tp[0]

    ###
    key = fs[0], cs[0], fs[1], cs[1], fs[2], cs[2]
    raw_values[key] = (tw[0], tp[0])
    print(max_work, fs, cs)

in_mat = np.array(in_mat)
out_mat = np.array(out_mat)


in_row = [0] * (1 + len(small_freqs) + len(med_freqs) + len(large_freqs))
in_row = np.array([in_row])




small_f_best = [raw_values[f, 1, 0, 0, 0, 0][0] for f in small_freqs]
med_f_best   = [raw_values[0, 0, f, 1, 0, 0][0] for f in med_freqs  ]
lar_f_best   = [raw_values[0, 0, 0, 0, f, 1][0] for f in large_freqs]
max_cpu_work = max(max(f) for f in (small_f_best, med_f_best, lar_f_best))


if MODE == 'linear':
    # [sample][in var]   [sample] or [sample][out var]    -> [invar] or [invar][outvar]
    coef, res, rank, s, = np.linalg.lstsq(in_mat, out_mat)

    print(coef)
    #max_cpu_work = coef[-1][0]

    def predict(sf, sc, mf, mc, lf, lc):
        v = np.array(coef[0])
        v += sc * coef[1 + sf]
        v += mc * coef[1 + len(small_freqs) + mf]
        v += lc * coef[1 + len(small_freqs) + len(med_freqs) + lf]

        bestw = 0
        if sc > 0:
            bestw = max(bestw, coef[1 + sf][0])
        if mc > 0:
            bestw = max(bestw, coef[1 + len(small_freqs) + mf][0])
        if lc > 0:
            bestw = max(bestw, coef[1 + len(small_freqs) + len(med_freqs) + lf][0])

        return v, bestw

elif MODE == 'rf':
    from sklearn.ensemble import RandomForestRegressor
    rf = RandomForestRegressor()
    rf.fit(in_mat, out_mat)

    def predict_(sf, sc, mf, mc, lf, lc):
        v = in_row * 0
        v[0][0] = 1
        v[0][1 + sf] = sc
        v[0][1 + len(small_freqs) + mf] = mc
        v[0][1 + len(small_freqs) + len(med_freqs) + lf] = lc
        
        pred = rf.predict(v)[0]
        return pred

    def predict(sf, sc, mf, mc, lf, lc):
        b = 0
        
        if sc:
            b = max(b, small_f_best[sf])
        if mc:
            b = max(b, med_f_best[mf])
        if lc:
            b = max(b, lar_f_best[lf])            
            
        return predict_(sf, sc, mf, mc, lf, lc), b
else:
    assert False 



def pp(x):
    return int(x / max_work * 100)

l = 1e100
best_power   = [[ l    for i in range(100) ] for j in range(100)]
best_configs = [[ None for i in range(100) ] for j in range(100)]

for si, sf in enumerate(small_freqs):
    for mi, mf in enumerate(med_freqs):
        for li, lf in enumerate(large_freqs):
            for sc in range(5):
                for mc in range(3):
                    for lc in range(3):
                        (total_work, power), bw = predict(si, sc, mi, mc, li, lc)
                        string = f'{sf}x{sc} {mf}x{mc} {lf}x{lc}', (sf,sc,mf,mc,lf, lc)

                        pp(bw)
                        total_work *= MARGIN
                        bw *= MARGIN

                        if total_work < small_f_best[-1] * 2:
                            total_work = 0
                            bw = 0
                        
                        for i in range(0, pp(total_work) + 1):
                            if i >= 100:
                                continue

                            for j in range(0, pp(bw) + 1):
                                if j >= 100:
                                    continue

                                if best_power[i][j] > power:
                                    best_power  [i][j] = power
                                    best_configs[i][j] = string


max_conf = small_freqs[-1], 4, med_freqs[-1], 2, large_freqs[-1], 2
def get_best(c, i, j):
    best = None
    for j in range(j + 1):
        if c[i][j]:
            best = c[i][j]
    if best is None:
        return 'max', max_conf
    return best
        

 #* TABLE FORMAT:
 #*
 #*  TBL  := scale num_util SYS_POINT+
 #*  SYS_POINT := window_us ncpus CPU_POINT+
 #*  CPU_POINT := freq_min freq_max idle_us
 #*
 #* each is an unsigned 4 byte int (in native byte order)




fo = open('table.dat', 'wb')
def writeout(x):
    fo.write(x.to_bytes(length=4, byteorder='little'))

scale = int(TOTAL_CAP / 100)
writeout(scale)
writeout(100)

for i in range(100):
    for j in range(100):
        writeout(100000)
        writeout(8)

        s, bc = get_best(best_configs, j, i)
        print(i, j, s)

        def cpu(f, online):
            writeout(f)
            writeout(f)
            writeout(online)

        sf, sc, mf, mc, lf, lc = bc
        for ii in range(4):
            cpu(sf, ii < sc)

        for ii in range(2):
            cpu(mf, ii < mc)

        for ii in range(2):
            cpu(lf, ii < lc)
