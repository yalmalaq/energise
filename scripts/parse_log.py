import re
from functools import reduce 
import operator
import statistics
import sys

file = sys.argv[1]

with open(file) as f:
    lines = f.readlines() 
    columns = [] 

    for line in lines:
        c0 = re.search('0 = \d+', line)
        c1 = re.search('1 = \d+', line)
        c2 = re.search('2 = \d+', line)
        c3 = re.search('3 = \d+', line)
        c4 = re.search('4 = \d+', line)
        c5 = re.search('5 = \d+', line)
        c6 = re.search('6 = \d+', line)
        c7 = re.search('7 = \d+', line)
        if c0 != None: 
            print(c0.group().strip().split('0 = ')[1])
        if c1 != None: 
            print(c1.group())
        if c2 != None: 
            print(c2.group())
        if c3 != None: 
            print(c3.group())
        if c4 != None: 
            print(c4.group())
        if c5 != None: 
            print(c5.group())
        if c6 != None: 
            print(c6.group())
        if c7 != None: 
            print(c7.group())


        