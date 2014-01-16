#!/bin/bash

#This script will enable performance counters for all CPUs

num_cpu=4

for ((i=0; i<$num_cpu; ++i)) ; do
    taskset -c $i modprobe enable_perf
    rmmod enable_perf
done
