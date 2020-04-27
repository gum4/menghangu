//
//  COVID_spread.cu
//  Parallel Programming, Spring 2020 
//

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include<cuda.h>
#include<cuda_runtime.h>



//use random number generator to initialize the Graph
//this init function is after the initialization of MPI
//initialize Graph, G_data, G_resultdata and Day_of_cure

// Global function #1: Initialization 
__global__ void gol_init(const int population, const int max_connections, unsigned int** Graph, unsigned int* G_data, unsigned int* G_resultdata, unsigned int * Day_of_cure ){

		// Initialize Graph on the CPU/GPU
		cudaMallocManaged(Graph, (population * sizeof(unsigned int*)));
		int i;
		for (i = 0; i < population; i++){
			
				// Generate # of connections for this individual 
				int num_connections = rand() % max_connections + 1; 
				 
				// Generate connections for this indiviudal
				cudaMallocManaged(Graph[i], (num_connections * sizeof(unsigned int)));
				int j;
				for (j = 0; j < num_connections; j++){
						int curr_connection = rand() % population + 1;
						Graph[i][j] = curr_connection; 
				}			
		}
		
		// Initialize G_data, G_resultData and Day_of_cure on the CPU/GPU
		cudaMallocManaged(G_data, (population * sizeof(unsigned int)));
		cudaMallocManaged(G_resultData, (population * sizeof(unsigned int)));
		cudaMallocManaged(Day_of_cure, (population * sizeof(unsigned int)));
		for (i = 0; i < population; i ++){
			G_data[i] = 0;
			G_resultData = 0;
			Day_of_cure = -1; 
		}
	  
		return;
}


extern "C" void gol_init_master(const int population, unsigned int** Graph, unsigned int* G_data, unsigned int* G_resultdata, unsigned int ** Day_of_cure, int myrank ){

	
    int N=population;
    int numBlocks = (N+threadsCount-1) / threadsCount;
    gol_init<<<numBlocks, threadsCount>>>(population,Graph,G_data,G_resultdata,Day_of_cure);
    
    
    int cE, cudaDeviceCount;
    if( (cE = cudaGetDeviceCount( &cudaDeviceCount)) != cudaSuccess )
    {
      printf(" Unable to determine cuda device count, error is %d, count is %d\n",
             cE, cudaDeviceCount );
      exit(-1); 
    }
	
    if( (cE = cudaSetDevice( myrank % cudaDeviceCount )) != cudaSuccess )
    {
      printf(" Unable to have rank %d set to cuda device %d, error is %d \n",
             myrank, (myrank % cudaDeviceCount), cE);
      exit(-1); 
    }
} 

//Generate G_resultdata from current G_data and current Day_of_cure, update Day_of_cure

__global__ void gol_kernel(const unsigned int* G_data,
                                   unsigned int** Day_of_cure,
                                   unsigned int* G_resultData){

	

}

//count the number of infected people in one iteration
__global__ int  countInfectedPeople (unsigned int* G_data, int population){

	int i;
	int count=0;
	int index = blockIdx.x *blockDim.x + threadIdx.x;
	for (i=index;i<population;i+=blockDim.x * gridDim.x){
		if (G_data[i]==1) count++;
	}
	return count;
}


extern "C"  int getInfectedPeople(unsigned int* G_data, int population, ushort threads count){
	int N=population;
	int numBlocks= (N+threadsCount-1)/threadsCount;
	int infected = countInfectedPeople<<<numBlocks, threadsCount>>>(G_data, population);
	return infected;
}

extern "C"  void gol_swap( unsigned int **G_data, unsigned int **G_resultdata)
{
    // You write this function - it should swap the pointers of pA and pB.
    
    unsigned int * tmp;
    //reset all elements in pB to 0
    tmp=*G_data;
    *G_data=*G_resultdata;
    *G_resultdata=tmp;
}

//gol_kernelLaunch returns the number of iterations to let the number infected people be larger than threshold
extern "C" int gol_kernelLaunch(unsigned int** G_data,
                              unsigned int** G_resultData,
                              unsigned int** Day_of_cure,
			      const int population,
                              const int threshold,
                              ushort threadsCount){

unsigned int* D_data = & G_data;
unsigned int* D_resultData = & G_resultData;
int N=population;
int numBlocks= (N+threadsCount-1)/threadsCount;

int infected = getInfectedPeople(D_data,population,threadscount);

size_t i=0;
    
    while(infected<=threshold)
    {
                
	// swap
        
        gol_kernel<<<numBlocks, threadsCount>>>(D_data, D_resultData, Day_of_cure, threshold, threadsCount);
        infected=getInfectedPeople(D_resultdata,population,threadscount);
        cudaDeviceSynchronize();
	gol_swap(G_data, G_resultData);
	i++;
    }
    cudaDeviceSynchronize();
    return i;
}



// print the indexes of infected people when the number of infected people is larger than threshold

extern "C"  void gol_print_infected(unsigned int* G_data, int population)
{
    int i;
    
    for( i = 0; i < population; i++)
    {
        
        printf("%d ",G_data[i]);
	//print 100 indexes in each line
	if (i+1% 100==0){
	    printf("\n");
	} 
    }
    
    printf("\n\n");
}

extern "C"  void gol_print_cured(unsigned int** Day_of_cure, int population){


}


