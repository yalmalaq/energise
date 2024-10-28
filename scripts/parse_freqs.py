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

    print ("Time (MS),Core Type,Frequency")

    i = 0

    for line in lines:
        
        small1 = 0
        small2 = 0
        small3 = 0
        small4 = 0
        medium1 = 0
        medium2 = 0
        large1 = 0
        large2 = 0



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
            small1 = c0.group().strip().split('0 = ')[1]
        if c1 != None:
            small2 = c1.group().strip().split('1 = ')[1]
            # print(c1.group())
        if c2 != None: 
            small3 = c2.group().strip().split('2 = ')[1]
            # print(c2.group())
        if c3 != None: 
            smal4 = c3.group().strip().split('3 = ')[1]
            # print(c3.group())
        if c4 != None: 
            medium1 = c4.group().strip().split('4 = ')[1] 
            # print(c4.group())
        if c5 != None: 
            medium2 = c5.group().strip().split('5 = ')[1]
            # print(c5.group())
        if c6 != None: 
            large1 = c6.group().strip().split('6 = ')[1]
            # print(c6.group())
        if c7 != None: 
            large2 = c7.group().strip().split('7 = ')[1]
            # print(c7.group())

        if ((c0 != None or c1 != None or c2 != None or c3 != None or c4 != None or c5 != None or c6 != None or c7 != None ) and i % 23 == 0):
            #print(str(time) + "," + "small 1" + "," + str(small1))
            #print(str(time) + "," + "small 2" + "," + str(small2))
            #print(str(time) + "," + "small 3" + "," + str(small3))
            #print(str(time) + "," + "small 4" + "," + str(small4))

            #print(str(time) + "," + "medium1" + "," + str(medium1))
            #print(str(time) + "," + "medium2" + "," + str(medium2))

            #print(str(time) + "," + "large1"+ "," + str(large1))
            print(str(time) + "," + "large2"+ "," + str(large2))
            time = time+5


        i = i + 1
        