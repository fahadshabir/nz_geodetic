New Zealand Map Grid routines
=============================

The files nzmg.c and nzmg.h contain ANSI C code for converting coordinates 
between the New Zealand Map Grid and latitude and longitude on the
New Zealand Geodetic Datum 1949.  


The code uses one precompiler macro.

TEST_NZMG:  if defined causes nzmg.c to compile into a test program
            which converts NZMG coordinates to NZGD49 and back
            again.  The file TEST.DAT contains sample data which
            can be entered into the test program - the output should
            match the file TEST.OUT.
                  
The NZMG module provides two functions 

void nzmg_geod( double n, double e, double *lt, double *ln );
void geod_nzmg( double lt, double ln, double *n, double *e );

The first converts a northing n and easting e to a latitude lt and 
longitude ln, and the second performs the inverse function.

Northings and eastings are in metres. Latitudes and longitudes are in
radians.
