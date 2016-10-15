# NetCDF_MPI
The Read_netcdf_serial.c is the "C" serial code to read the netcdf file and write the vertical profile of different meteorological variable to the ascii file. 
The Read_netcdf_mpi.c is the "C" parallel and optimizes code to read the netcdf file and write the vertical profile of different meteorological variable to the ascii file.
The outer for loop is along the time dimension in both the codes. The "Read_netcdf_mpi.c" makes the time dimension parallel; it will process the data for all the times simultaneously.
