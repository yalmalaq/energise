#!/bin/env python3
#             perf  4951 [000]  4955.626825:       sched:sched_switch: prev_comm=perf prev_pid=4951 prev_prio=120 prev_state=R+ ==> next_comm=migrati>

from collections import defaultdict

ANDROID = True
pidcomm = defaultdict(lambda: '??')

def _parse_events():
    line = ''
    while True:
        if ANDROID:
            while not line.startswith('record sample: type 9'):
                line = input()
            line  = input()
            
            pid, cpu, time, evt, attrs = None, None, None, None, {}
            evt = 'sched:sched_switch' # sample_type:', '0x5c7 ??
            attrs = dict()
            
            while line.startswith(' '):
                line = line.strip()
                
                if line.startswith('pid'):
                    pid = int(line.split()[-1])
                elif line.startswith('cpu'):
                    cpu = int(line.split()[1].removesuffix(','))
                elif line.startswith('time'):
                    time = float(line.split()[1]) / 1000000000
                elif line.startswith('next_pid'):
                    attrs['next_pid'] = int(line.split()[1])
                elif line.startswith('next_comm'):
                    attrs['next_comm'] = line.split(maxsplit=1)[1]

                line = input()

            pidcomm[attrs['next_pid']] = attrs['next_comm']

        else:
            line = input()

            name = line[:l]
            line = line[l + 1:]
            pid, cpu, time, evt, fields = line.split(maxsplit=4)

            pid = int(pid)
            evt = evt[:-1]
            cpu = int(cpu[1:-1])
            time = float(time[:-1])

            pidcomm[pid] = name

            attrs = dict()
            for f in fields.split(' '):
                if '=' in f and '==' not in f:
                    name, val = f.split('=')
                    attrs[name] = val

        yield pid, cpu, time, evt, attrs

def parse_events():
    try:
        for el in _parse_events():
            yield el
    except EOFError:
        return

# processing the events

start_time = None
end_time   = None

cpu_on            = defaultdict(lambda: None)
cpu_off           = defaultdict(lambda: None)
cpu_on_durations  = defaultdict(lambda: [])
cpu_off_durations = defaultdict(lambda: [])
cpu_bars          = defaultdict(lambda: [])

pid_on_time       = defaultdict(lambda: None)
pid_off_time      = defaultdict(lambda: None)
pid_on_durations  = defaultdict(lambda: [])
pid_off_durations = defaultdict(lambda: [])
    
for lpid, cpu, time, evt, attrs in parse_events():
    if evt == 'sched:sched_switch':
        if start_time is None:
            start_time = time
        end_time = time

        npid   = int(attrs['next_pid'])
        to_off = npid == 0

        if npid == 0 and lpid != 0:
            # Idle -> task
            cpu_off[cpu] = time
            if cpu_on[cpu]:
                dur = time - cpu_on[cpu]
                cpu_on_durations[cpu].append(dur)

        elif npid != 0 and lpid == 0:
            # Task -> idle
            cpu_on[cpu] = time
            if cpu_off[cpu]:
                dur = time - cpu_off[cpu]
                cpu_off_durations[cpu].append(dur)

        if pid_on_time[lpid]:
            # Process the end of "l"'s timeslice
            dur = time - pid_on_time[lpid]
            pid_on_durations[lpid].append(dur)

            cpu_bars[cpu].append((pid_on_time[lpid],
                                  time,
                                  lpid))

        if pid_off_time[npid]:
            # Process the start of "n"s timeslice
            dur = time - pid_off_time[npid]
            pid_off_durations[npid].append(dur)

        pid_on_time[npid]  = time
        pid_off_time[lpid] = time

# stats

print('Trace for {:5.3f} seconds'.format(end_time - start_time))
print('=== pid')
for pid in pid_on_durations:
    if pid == 0:
        continue
    l = pid_on_durations[pid]
    o = pid_off_durations[pid]
    
    print('{:30s} {:7d}: {:6d} wkups {:10.2f} ms avg {:10.2f} ms tot {:10.2f} ms avg sleep'.format(
        pidcomm[pid],
          pid,
          len(l),
          sum(l) / len(l) * 1000,
          sum(l) * 1000,
          sum(o) / len(o) * 1000 if o else 0))

    
print('=== cpu')
for cpu in cpu_on_durations:
    l = cpu_on_durations[cpu]
    o = cpu_off_durations[cpu]

    print('{:7d}: {:6d} wkups {:10.2f} ms avg {:10.2f} ms tot {:10.2f} ms avg sleep'.format(
          cpu,
          len(l),
          sum(l) / len(l) * 1000,
          sum(l) * 1000,
          sum(o) / len(o) * 1000))

# plots

print('NOTE: only plotting the last two seconds of the trace')
ONLY_PLOT_LAST=True
    
import matplotlib.pyplot as plt
fig = plt.figure()
ax = fig.add_subplot()
for cpu in cpu_bars:
    width = []
    left  = []
    for start, end, pid in cpu_bars[cpu]:
        if ONLY_PLOT_LAST and end < end_time - 2:
            continue
        
        left.append (start - start_time)
        width.append(end - start)

    ax.barh(cpu, width, left=left)
fig.tight_layout()
plt.show()

