#!/bin/bash
#SBATCH -J "main.out - ping pong assignment - Prakhar Rathi"
#SBATCH -A <write your account name here>
#SBATCH -t 00:10:00
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=1

module purge
module load DefaultModules
module load toolchain/gompi
module load mpi/OpenMPI/4.0.3-GCC-9.3.0

mpiexec ./main
