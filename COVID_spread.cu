//
//  COVID_spread.c 
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
__global__ void gol_init(const int population, unsigned int** Graph, unsigned int* G_data, unsigned int* G_resultdata, unsigned int * Day_of_cure, unsigned int* Num_of_connections_per_person, int max_connections ){
		int i;
		// Initialize G_data, G_resultData and Day_of_cure on the CPU/GPU
		for (i = 0; i < population; i ++){
			G_data[i] = 0;
			G_resultdata[i] = 0;
			Day_of_cure[i] = -1; 
		}
	  
		return;
}


extern "C" void gol_init_master(const int population, unsigned int** Graph, unsigned int* G_data, unsigned int* G_resultdata, unsigned int * Day_of_cure, int myrank, unsigned int* Num_of_connections_per_person, int threadsCount, int max_connections ){

    int N=population;
    int numBlocks = (N+threadsCount-1) / threadsCount;
		
		// Initialize memory 
		cudaMallocManaged(&Graph, (population * sizeof(unsigned int*)));
		cudaMallocManaged(&G_data, (population * sizeof(unsigned int)));
		cudaMallocManaged(&G_resultdata, (population * sizeof(unsigned int)));
		cudaMallocManaged(&Day_of_cure, (population * sizeof(unsigned int)));
		cudaMallocManaged(&Num_of_connections_per_person, (population * sizeof(unsigned int)));
		
		int i;
		for (i = 0; i < population; i++){
			int num_connections = rand() % max_connections + 1; 
			Num_of_connections_per_person[i] = num_connections;
			cudaMallocManaged(&Graph[i], (num_connections * sizeof(unsigned int)));
		}
		
		// Initialize Graph on the CPU/GPU
		for (i = 0; i < population; i++){
				// Generate connections for this indiviudal
				int j;
				for (j = 0; j < Num_of_connections_per_person[j]; j++){
						int curr_connection = rand() % population + 1;
						Graph[i][j] = curr_connection; 
				}			
		}	
		
    gol_init<<<numBlocks, threadsCount>>>(population,Graph,G_data,G_resultdata,Day_of_cure,Num_of_connections_per_person, max_connections);
    
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

// Global Function #2: Iteration 
__global__ void gol_kernel(const unsigned int* G_data, unsigned int* G_resultData, unsigned int** Graph, unsigned int* Day_of_cure, int threshold, int threadsCount, int currDay, const int population, unsigned int* Num_of_connections_per_person, const int recovery_period, unsigned int* invunerable_individuals){
	
	int index = blockIdx.x * blockDim.x + threadIdx.x;

	
	// 1 -> invunerable
	// 2 -> vunerable 
	
	// Cure individuals that recover on this day 
	int j;
	for (j = index; j < population; j += blockDim.x * gridDim.x){
		
		// If the individual is infected 
		if (G_data[j] == 1){
			// In the case today is the day this individual is to be cured 
			if (Day_of_cure[j] == currDay){
		
				G_resultData[j] = 0;
				invunerable_individuals[j] = 1;
			}		
		}
	}
	
	// Process individuals that are infected 
	for (j = index; j < population; j+= blockDim.x * gridDim.x){
		
		// In the case that the current indiviudal is infected 
		if (G_data[j] == 1){
			
			// Process his/her connections and note its spread 
			int i;
			for (i = 0; i < Num_of_connections_per_person[j]; i++){
				
				// If the connection is not currently infected 
				if (G_data[i] == 0 && invunerable_individuals[i] != 1){
					
					G_resultData[i] = 1;
					Day_of_cure[i] = currDay + recovery_period;
				}				
			}
		}
	}
	
	return;
}

//count the number of infected people in one iteration
__global__ void countInfectedPeople (unsigned int* G_data, const int population, int* count){

	int i;
	int index = blockIdx.x *blockDim.x + threadIdx.x;
	for (i=index;i<population;i+=blockDim.x * gridDim.x){
		if (G_data[i]==1) *count++;
	}
	return; 
}

extern "C"  int getInfectedPeople(unsigned int* G_data, const int population, int threadsCount){
	int N=population;
	int numBlocks= (N+threadsCount-1)/threadsCount;
	int count = 0;
	countInfectedPeople<<<numBlocks, threadsCount>>>(G_data, population, &count);
	return count;
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
																unsigned int*** Graph,
                              	unsigned int** Day_of_cure,
			      										const int population,
                              	const int threshold,
                              	int threadsCount,
																unsigned int** Num_of_connections_per_person,
																const int recovery_period
														){

unsigned int* D_data = *G_data;
unsigned int* D_resultData = *G_resultData;
int N=population;
int numBlocks= (N+threadsCount-1)/threadsCount;

int infected = getInfectedPeople(D_data,population,threadsCount);

int i=0;
    
    while(infected<=threshold)
    {
				unsigned int* invunerable_individuals;
				cudaMallocManaged(&invunerable_individuals, population * sizeof(unsigned int));
				int j;
				for (j = 0; j < population; j++){
					invunerable_individuals[j] = 0;
				}
				
			
        gol_kernel<<<numBlocks, threadsCount>>>(D_data, D_resultData, *Graph, *Day_of_cure, threshold, threadsCount, i, population, *Num_of_connections_per_person, recovery_period, invunerable_individuals);
        infected=getInfectedPeople(D_resultData,population,threadsCount);
        cudaDeviceSynchronize();
				gol_swap(&D_data, &D_resultData);
				i++;
  
				cudaFree(&invunerable_individuals);
				
		}
		
		G_data = &D_data;
		G_resultData = &D_resultData;
		
    cudaDeviceSynchronize();
    return i;
}



// print the indexes of infected people when the number of infected people is larger than threshold

extern "C"  void gol_print_infected(unsigned int* G_data, const int population)
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

extern "C"  void gol_print_cured(unsigned int** Day_of_cure, const int population){


}


