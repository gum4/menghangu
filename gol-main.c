#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "mpi.h"
#include <string.h>


#define threshold 500000
#define population 1000000

//the indexes of infected people
unsigned int* G_data=NULL;
//the indexes of infected people in the next round
unsigned int* G_resultdata=NULL;
unsigned int** Graph;
unsigned int** Day_of_cure;


int main(int argc, char *argv[]) {
    int  namelen, numprocs, rank;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Get_processor_name(processor_name,&namelen);
    MPI_Status status;
    
    MPI_File fh;
    
    gol_init_master(population,  Graph,  G_data,  G_resultdata,  Day_of_cure, rank, Num_of_connections_per_person );
    int Result=gol_kernelLaunch( G_data,
                                 G_resultData,
                                 Graph,
                                 Day_of_cure,
                                 population,
                                 threshold,
                                threadsCount,
                                Num_of_connections_per_person);
    int i;
    if (rank == 0) {
        MPI_File_open(MPI_COMM_SELF, "test.txt",MPI_MODE_CREATE | MPI_MODE_WRONLY,MPI_INFO_NULL,&fh);
        //FILE* f = fopen("test.txt","wb+");
        //if(f==NULL){
        //printf("failed to open file\n");exit(1);
        //}
        
        for (i=0; i < population; i++){
            char buf[1];
            //fprintf(f,"%d \n",i);
            snprintf(buf,1,"%d \n",G_data[i]);
            MPI_File_write(fh,buf,strlen(buf), MPI_CHAR,&status);
        }
        //        fclose(f);
        MPI_File_close(&fh);
    }
    else {
        // do nothing
    }
    
    
    MPI_Finalize();
    return 0;
}
