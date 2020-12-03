# Problem Statement

Determine the delay and bandwidth of the connection between two nodes with the help of an MPI program, using *MPI_Send* and *MPI_Recv*.

**Delay**

1. Send a number back and forth between two processes, allocating one process per node. Calculate the time it takes for these data transfers. 
2. Calculate the average time for say, 50, of these transfers and the respective average time for a one-way transmission (i.e., the network delay) in microseconds and print it to the console.

*Note*: The very first MPI communication could have a large overhead and should not be included in the average calculation.

**Bandwidth**

1. Instead of sending a single number, send a bigger chunk of data.
2. Calculate the average delay for sending this chunk and from that calculate the bandwidth. 
3. Output the bandwidth achieved in Gigabytes per second (GB/s) for a chunk size of 32MiB (Megabytes) to the console.
