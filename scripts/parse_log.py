import re
from functools import reduce 
import operator
import statistics
import sys

file = sys.argv[1]

with open(file) as f:
    lines = f.readlines() 
    columns = [] 

    time = 0

    print ("Time (MS),Core Type,Cores Active")

    for line in lines:
        
        small = 0
        medium = 0
        large = 0



        c0 = re.search('0 = \d+', line)
        c1 = re.search('1 = \d+', line)
        c2 = re.search('2 = \d+', line)
        c3 = re.search('3 = \d+', line)
        c4 = re.search('4 = \d+', line)
        c5 = re.search('5 = \d+', line)
        c6 = re.search('6 = \d+', line)
        c7 = re.search('7 = \d+', line)
        if c0 != None: 
            # print(c0.group().strip().split('0 = ')[1])
            small = small + 1
        if c1 != None:
            small = small + 1 
            # print(c1.group())
        if c2 != None: 
            small = small + 1
            # print(c2.group())
        if c3 != None: 
            small = small + 1
            # print(c3.group())
        if c4 != None: 
            medium = medium + 1 
            # print(c4.group())
        if c5 != None: 
            medium = medium + 1
            # print(c5.group())
        if c6 != None: 
            large = large + 1 
            # print(c6.group())
        if c7 != None: 
            large = large + 1 
            # print(c7.group())

        if (c0 != None or c1 != None or c2 != None or c3 != None or c4 != None or c5 != None or c6 != None or c7 != None ):
            time = time+100
            print(str(time) + "," + "small" + "," + str(small))
            print(str(time) + "," + "medium" + "," + str(medium))
            print(str(time) + "," + "large"+ "," + str(large))



        