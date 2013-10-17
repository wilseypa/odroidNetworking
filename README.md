odroidNetworking
================

Data/code/notes relating to our studies with low latency networking and big.LITTLE task assignment on the odroid platform.

We are studying the construction of low-cost Beowulf Clusters constructed with the odroid platform.  Our initial studies 
are focusing on low-latency networking and with the assignment of tasks to the big and LITTLE cores on the XU platform.  In particular, we are examining the deployment of a IBoE/RoCE-based driver for networking.  Furthering that work, we are also hoping to evaluate a polling based (instead of interrupt based) model for processing ethernet traffic.  We have performed similar studies on virtual machines in the x86 platform which have resulted in a reduction in network latency of 60% of native.  

Ultimately our work is targeting the construction of a platform for studies with Parallel Discrete-Driven Simulation (PDES) using the Time Warp mechanism.  Thus, some of our studies may also break out some of the house keeping functions for execution on the LITTLE cores. 
