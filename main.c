// Author: <Prakhar Rathi>

#include <stdio.h>  // printf, perror
#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <mpi.h>


int main(int argc, char* argv[]){
  double delay=0.0, bandwidth=0.0;
  int my_rank;
  int number=20; // to send
  double startTime, finishTime, elapsedTime;
  double startTime_bandwidth, finishTime_bandwidth, elapsedTime_bandwidth;
  int index;
  int world_size;
  int n_transfers=100; 
  int tag_zero_to_one=0;
  int tag_one_to_zero=1;
  
  // OBSERVATIONS : DELAY - CALCULATION:
  // I tried for n_transfers = 50, 60 , 65, 68, 70, 75, 80, 100, and 500, with repetition of 
  // mpiexec command (50 local executions of each main.out file).
  // with n_transfers = 100 --> Average time ranges from 0.96 to 1.11 micro-seconds as per my code.
  // Thus, on the basis of the graph in introduction --> I choose n-transfers = 100 

  MPI_Init(NULL,NULL);
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

  //There should be exactly two nodes for the problem statement
  if(world_size!=2){
    if(my_rank==0)
      printf("This problem statement requires exactly two nodes, run 'mpiexec -n 2 ./main'");
    //exit in this case
    MPI_Finalize();
    exit(0);
  }   
  //ignoring the first loop of communication
  if(my_rank==0) {
    MPI_Send(&number,1,MPI_INT,1,tag_zero_to_one,MPI_COMM_WORLD);
    MPI_Recv(&number,1,MPI_INT,1,tag_one_to_zero,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
  }
  else
  {
    MPI_Recv(&number,1,MPI_INT,0,tag_zero_to_one,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    MPI_Send(&number,1,MPI_INT,0,tag_one_to_zero,MPI_COMM_WORLD);
  }
  
  // PART-1: DELAY CALCULATION BEGINS HERE:
  MPI_Barrier(MPI_COMM_WORLD);
  startTime=MPI_Wtime();
  for(index=0;index<n_transfers;index++){
    if(my_rank==0){
      MPI_Send(&number,1,MPI_INT,1,tag_zero_to_one,MPI_COMM_WORLD);
      MPI_Recv(&number,1,MPI_INT,1,tag_one_to_zero,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }
    else
    {
      MPI_Recv(&number,1,MPI_INT,0,tag_zero_to_one,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      MPI_Send(&number,1,MPI_INT,0,tag_one_to_zero,MPI_COMM_WORLD);
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  finishTime=MPI_Wtime();
  elapsedTime=finishTime-startTime;
  //multiply by 1,000,000 to convert seconds into micro-seconds. 
  delay = 1000000*(elapsedTime/(n_transfers*2)); 
 

  //PART-2: BANDWIDTH CALCULATION BEGINS HERE.
  //Big Chunk of data to be sent:
  //Approach:
  //1. Initiate a 32 MB array. (chunk size for which Bandwidth is to be calculated).
  //2. Loop over the transfer process (n_transfers x 2) times.
  //3. Calculate Bandwidth after type conversion: Data in GB and Time in seconds.

  //Begin
  // 32 Mega Bytes = 2 raised to the power of 22 Bytes.
  long int size_of_buffer=1<<22; //bitwise shift
  //allocate 32MB memory to a buffer array of type double.
  double *buffer_array=(double*)malloc(size_of_buffer*sizeof(double));
  //Do not pass an empty array --> Initialize all the elements with a value 1.0.
  for(index=0;index<size_of_buffer;index++){
    buffer_array[index]=1.0;
  }
  //Begin the avg time calculation
  MPI_Barrier(MPI_COMM_WORLD);
  startTime_bandwidth=MPI_Wtime();
  for(index=0;index<n_transfers;index ++){
    if(my_rank==0){
      MPI_Send(buffer_array,size_of_buffer,MPI_DOUBLE,1,tag_zero_to_one,MPI_COMM_WORLD);
      MPI_Recv(buffer_array, size_of_buffer,MPI_DOUBLE,1,tag_one_to_zero,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }
    else
    {
      MPI_Recv(buffer_array,size_of_buffer,MPI_DOUBLE,0,tag_zero_to_one,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      MPI_Send(buffer_array,size_of_buffer,MPI_DOUBLE,0,tag_one_to_zero,MPI_COMM_WORLD);
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  finishTime_bandwidth=MPI_Wtime();
  elapsedTime_bandwidth=finishTime_bandwidth-startTime_bandwidth;
  // since MPI_Wtime() returns the number of seconds, our need --> convert MB to GB
  // 1 GB = 2 raised to the power of 30 Bits; 1 Byte = 8 bits.
  double data_32_MB_in_GB=(double)8*size_of_buffer/(double)(1<<30); // bitwise shift.
  
  double delay_bandwidth=elapsedTime_bandwidth/(2*n_transfers);
  bandwidth=data_32_MB_in_GB/delay_bandwidth;
  free(buffer_array);
  // The code runs on both the nodes --> Specify which node prints the final value.
  if(my_rank==0)
  {
    printf("Delay:%lf\n", delay);  //in microseconds
    printf("Bandwidth:%lf\n", bandwidth); //  GB/s using chunk size 32MiB
  }
 /*
  // TO CHECK IF THE METHOD IS CORRECT AS PER THE GRAPH IN THE INTRODUCTION PDF
  // SENDING A DATA CHUNK OF 1 Byte.
  
  long int check_array_size = 1;

  double *check_buffer_array = (double*) malloc(check_array_size * sizeof(double));

  for(index = 0 ; index < check_array_size ; index++){
    check_buffer_array[index] = 1.0;
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  double startTime_bandwidth_check = MPI_Wtime();

  for(index = 0 ; index < n_transfers * 2; index ++){
    if(my_rank == 0){
      MPI_Send( check_buffer_array , check_array_size , MPI_DOUBLE , 1 , tag_zero_to_one , MPI_COMM_WORLD);
      MPI_Recv( check_buffer_array , check_array_size , MPI_DOUBLE , 1 , tag_one_to_zero , MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }

    else
    {
      MPI_Recv( check_buffer_array , check_array_size , MPI_DOUBLE, 0 , tag_zero_to_one , MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      MPI_Send( check_buffer_array , check_array_size , MPI_DOUBLE , 0 , tag_one_to_zero , MPI_COMM_WORLD);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  double finishTime_bandwidth_check = MPI_Wtime();
  
  double elapsedTime_bandwidth_check = finishTime_bandwidth_check - startTime_bandwidth_check;

  // since MPI_Wtime() returns the number of seconds, our need --> convert MB to GB

  double data_1B_in_GB =  (double)8*check_array_size / (double)(1 << 30) ;

  double average_time_per_transfer_check = elapsedTime_bandwidth_check / n_transfers;

  double delay_bandwidth_check = (double) average_time_per_transfer_check / 2;

  double bandwidth_check = sizeof(double) * data_1B_in_GB / delay_bandwidth_check;
 
  if(my_rank==0){
   printf("The check bandwidth is: %lf", bandwidth_check);
  }
  */
  MPI_Finalize();

  //printf("Delay:%lf\n", delay);     // in microseconds
  //printf("Bandwidth:%lf\n", bandwidth); //  GB/s using chunk size 32MiB
  return EXIT_SUCCESS;
}

