import sys
from pprint import pprint

# Extract sections from the file

def split_experiments(filename):
    lines = None
    with open(filename) as f:
        for line in f:
            if '===' in line:
                if lines is not None:
                    yield lines
                lines = []
            elif lines is not None:
                lines.append(line)
    yield lines

def split_before_after(x):
    for i, l in enumerate(x):
        if '**DONE**' in l:
            return x[:i], x[i+1:]

# Parsing
        
def get_channels(x):
    vals = dict()
    for line in x:
        if line.startswith('CH'):
            _   , rest = line.split('(T=')
            time, rest = rest.split(')[')
            rail, rest = rest.split('], ')
            val        = rest.strip()

            vals[rail] = (int(time), int(val))
    return vals

def get_attrs(x):
    attrs = dict()
    for line in x:
        if line.startswith('name'):
            attrs['name'] = line.split()[1].strip()
        elif line.startswith('Time'):
            attrs['time'] = float(line.split()[1].strip())
    return attrs

# Extract Values

def get_chan_power(before, after, k):
    energy_diff = after[k][1] - before[k][1] # uWs
    time_diff   = after[k][0] - before[k][0] # ms
    return energy_diff / time_diff / 1000

def get_power(before, after):
    chans_before, chans_after = get_channels(before), get_channels(after)
    chan_power = {k: get_chan_power(chans_before, chans_after, k)
                  for k in chans_before}
    return sum(chan_power.values())

def _get_framestats(section):
    jf, tf = None, None
    for line in section:
        if line.startswith('Janky frames:'):
            jf = int(line.split()[2])
        elif line.startswith('Total frames rendered'):
            tf = int(line.split()[3])
    return tf, jf

def get_framestats(before, after, time):
    t1, j1 = _get_framestats(before)
    t2, j2 = _get_framestats(after)
    
    if t1 and j1 and t2 and j2:
        return (t2 - t1) / time, (j2 - j1) / (t2 - t1) * 100
    else:
        return None, None

import pandas as pd
def get_data(**files):
    rows = []
    for dataset, filename in files.items():
        for exp_lines in split_experiments(filename):
            attrs = get_attrs(exp_lines)
            attrs['file'] = dataset

            if split := split_before_after(exp_lines):
                attrs['power'] = get_power(split[0], split[1])
                attrs['fps'], attrs['jank'] = \
                    get_framestats(split[0], split[1], attrs['time'])
                rows.append(attrs)

    return pd.DataFrame(rows)

df = get_data(baseline=sys.argv[1], ours=sys.argv[2])
df = df.groupby(['name', 'file']).mean()
print(df)
