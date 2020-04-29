all: gol-main.c COVID_spread.cu
	mpicc -g gol-main.c -c -o gol-main.o
	nvcc -g -G -arch=sm_70 COVID_spread.cu -c -o gol-cuda.o
	mpicc -g gol-main.o gol-cuda.o -o gol-cuda-mpi-exe \
	-L/usr/local/cuda-10.1/lib64/ -lcudadevrt -lcudart -lstdc++