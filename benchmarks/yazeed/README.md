# How to run it benchmark with all cores or specific core numbers:
 
- First see the benchmark ex: social-media.cpp
 
In the main function:
enable_cores(S(0) | S(1) | S(2) | S(3) | M(0) | M(1) | L(0) | L(1));
This means all the cores are on. You can modify this if you need specific cores.
 
 
- Second to run it:
 
Look at newrun.sh:
You can run all the benchmarks at once or one by one. If you want to run one benchmark alone for example social-media just uncomment the social media part and comment the rest (which is already there) . To decide how many runs you want for a benchmark, modify the for loop: for i in {1..3}; do  > this will run the benchmark three time…
 
- Then just run it: ./newrun.sh
