#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>
#include <time.h>

#include <mpi.h>

/* This is the name of the data file we will read. */

#define FILE_NAME "test.nc"

/* We are reading 4D data, a 3x36x700x640 grid. */

#define NX 6
#define NY 7
#define NT 3
#define NZ 36


/* Handle errors by printing an error message and exiting with a
 *  * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

int main(int argc, char *argv[] )
{

    int  mpi_size, mpi_rank;

    /* Initialize MPI. */

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);


   /* This will be the netCDF ID for the file and data variable. */

   clock_t begin = clock();	
   
   int ncid, varid_t, varid_p, varid_u, varid_v, varid_q, varid_z ;
   FILE *WRF_Profile; 
   char filename[64];

   int x, y, t, z, retval, counter;
 

   if ((retval = nc_open(FILE_NAME, NC_NOWRITE, &ncid)))
      ERR(retval);

/* Get the varid of the data variable, based on its name. Refer the unidata reference card */

   if ((retval = nc_inq_varid(ncid, "TK", &varid_t)))
      ERR(retval);	

 if ((retval = nc_inq_varid(ncid, "P", &varid_p)))
      ERR(retval);
    
   if ((retval = nc_inq_varid(ncid, "QVAPOR", &varid_q)))
      ERR(retval);

   if ((retval = nc_inq_varid(ncid, "U", &varid_u)))
      ERR(retval);

   if ((retval = nc_inq_varid(ncid, "V", &varid_v)))
      ERR(retval);
  
   if ((retval = nc_inq_varid(ncid, "Z", &varid_z)))
      ERR(retval);


      t=mpi_rank;     // use mpi_rank (i.e. number of nodes) for each time dimension to fork the processes on eacb node.

      for(y=0;y<NY;y++)
      { 
        for(x=0;x<NX;x++)
        {
          float  u1, v1, z1, p1, t1, rh1 ;    // define local variables
      
          sprintf(filename,"WRF_Profile_%d_%d_%d",t,y,x);   // Open file to write data to file
          FILE *f1 = fopen(filename, "w"); // Write data to ascii file
	        
          char cmdbuf[BUFSIZ];    // use buffer to issue system commands in C program enviornment.
          char cmdbufr[128];      // use buffer to issue system commands in C program enviornment.
	        size_t idx[4];
          
          for(z=0;z<NZ;z++)
          {
            idx[0]=t; 
            idx[1]=z; 
            idx[2]=y;
            idx[3]=x;
            nc_get_var1_float(ncid,varid_t,&idx,&t1);
            nc_get_var1_float(ncid,varid_p,&idx,&p1);
            nc_get_var1_float(ncid,varid_u,&idx,&u1);
            nc_get_var1_float(ncid,varid_v,&idx,&v1);
            nc_get_var1_float(ncid,varid_z,&idx,&z1);
            nc_get_var1_float(ncid,varid_q,&idx,&rh1);

            // printf("tk = %f\n", t1);

            fprintf(f1,"%f %f %f %f %f %f \n", p1, z1, t1, rh1, u1, v1);  // write data to the file name "WRF_Profile_t_y_x"
        
	  }

          //close our open file handles!
          fclose(f1);
	        snprintf(cmdbuf, sizeof(cmdbuf), "./severe.exe 36 50. WRF_Profile_%d_%d_%d  ./OUTPUT/HCout_%d_%d_%d 0",t,y,x,t,y,x);   // assign system command within C enviornment
          system(cmdbuf);                                                                                                        // issue system command within C enviornment
//	  snprintf(cmdbufr, sizeof(cmdbufr), "rm WRF_Profile_%d_%d_%d",t,y,x); 
//	  system(cmdbufr);
         }
      }


/* Close the file, freeing all resources. */
   if ((retval = nc_close(ncid)))
      ERR(retval);
  
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("++++++ Time required = %f", time_spent); 
    printf("*** SUCCESS reading example file %s!\n", FILE_NAME);

    /* Shut down MPI. */
   
    MPI_Finalize();

    return 0;

}

