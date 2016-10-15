#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>
#include <time.h>

/* This is the name of the data file we will read. */

#define FILE_NAME "test.nc"
/* We are reading 4D data, a 3x36x700x640 grid. */
#define NX 640	
#define NY 700
#define NT 3
#define NZ 36


/* Handle errors by printing an error message and exiting with a
 *  * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

int main(int argc, char *argv[] )
{

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


    for(t=0;t<1;t++)
    {
      //t=mpi_rank;

      for(x=0;x<NX;x++)
      { 
        for(y=0;y<NY;y++)
        {
          float  u1, v1, Z1, p1, t1, rh1 ;
          //we need this file to be opened for each x,y, to hold all the z
          //opening per z is a lot of extra IO overhead 
          sprintf(filename,"WRF_f_%d_%d_%d.txt",t,y,x); // Open file to write data to file
          FILE *f1 = fopen(filename, "w"); // Write data to ascii file
          fprintf(f1,"t p u v Z rh\n");		// print the header

          size_t idx[4];					// stack-size the data
          
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
            nc_get_var1_float(ncid,varid_z,&idx,&Z1);
            nc_get_var1_float(ncid,varid_q,&idx,&rh1);

            fprintf(f1,"%f %f %f %f %f %f \n", t1, p1, u1, v1, Z1, rh1);
          }

          //close our open file handles!
          fclose(f1);
        }
      }
    }


/* Close the file, freeing all resources. */
   if ((retval = nc_close(ncid)))
      ERR(retval);
  
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("++++++ Time required = %f", time_spent); 
    printf("*** SUCCESS reading example file %s!\n", FILE_NAME);

    return 0;

}

