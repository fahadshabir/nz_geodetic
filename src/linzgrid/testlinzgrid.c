/***********************************************************************
*
*  $Id: linzgrid.c,v 1.2 2001/05/23 00:09:16 ccrook Exp $
*
*  Description:
*
*  This module implements grid based transformation of 
*  latitude/longitude coordinates using an NTv2 ascii formatted file.
*  It is intended for the conversion of latitudes and longitudes between
*  the New Zealand geodetic datums NZGD1949 and NZGD2000.  
*
*  This is a minimal implementation of a grid transformation with some
*  significant limitations including:
*    1) It cannot work with the binary NTv2 format 
*    2) It does not support subgrids - it only works with a single grid
*    3) It always loads the entire grid into memory - there is no
*       load on demand or caching
*    4) It only calculates the adjustment to coordinates, not the
*       error of the adjustment.
*
*  This software may be freely used and modified.  It is provided as is, 
*  where is, with no guarantees as to its correct functioning.  Land
*  Information New Zealand accepts no liability for any consequences 
*  of the use of this software.
*
***********************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "linzgrid.h"
#include "linzgridload.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************
* Test code for the grid file.  If compiled this creates a main routine
* so that the code generates an executable program linzgrid.  This 
* program takes two parameters, a grid file name and a data file name.
* The data file should contain a latitude and longitude in decimal 
* degrees on each line.  The program will write the converted latitude
* and longitude to the standard output channel.  The program takes one 
* option -r, which if present causes the reverse transformation to be
* applied.
**********************************************************************
*/

int main( int argc, char *argv[] ) {
   char *gridfile;
   char *datafile;
   char *outfile;
   lnzGrid *grid;
   FILE *d;
   FILE *o;
   char buf[80];
   int reverse;
   int quiet;
   double lt0, ln0;
   double lt1, ln1;

   reverse = 0;
   quiet = 0;

   while( argc > 1 && argv[1][0] == '-' ) {
      if( strcmp(argv[1],"-r") == 0 ) {
         reverse = 1;
         }
      else if (strcmp(argv[1],"-q") == 0 ) {
         quiet = 1;
         }
      else {
         fprintf(stderr,"Invalid switch %s to linzgrid\n",argv[1]);
         return 1;
         }
      argc--;
      argv++;
      }

   if( argc != 3 && argc != 4 ) {
      fprintf(stderr,"Syntax: linzgrid [-r] [-q] ntv2_grid_file data_file [output_file]\n");
      return 1;
      }

   gridfile = argv[1];
   datafile = argv[2];
   outfile = NULL;
   if( argc == 4 ) outfile = argv[3];
   
   grid = lnzGridCreateFromNTv2Asc( gridfile );
   if( ! lnzGridIsOk( grid ) ) {
       fprintf(stderr,"Error: %s\n", lnzGridLastError(grid) );
       lnzGridDestroy( grid );
       return 2;    
       }
 
   if( ! quiet ) {
      printf("Grid definition\n");
      printf("  lat0 = %.5f\n", grid->lat0);
      printf("  lat1 = %.5f\n", grid->lat1);
      printf("  dlat = %.5f\n", grid->dlat);
      printf("  lon0 = %.5f\n", grid->lon0);
      printf("  lon1 = %.5f\n", grid->lon1);
      printf("  dlon = %.5f\n", grid->dlon);
      printf("  nlat = %d\n", grid->nlat);
      printf("  nlon = %d\n", grid->nlon);
      printf("  datum0 = %s\n", grid->datum0);
      printf("  datum1 = %s\n", grid->datum1);
      }
   
   if( outfile && strcmp(outfile,"-") != 0 ) {
      o = fopen(outfile,"w");
      if( ! o ) {
         fprintf(stderr,"Error: Cannot open output file %s\n", outfile );
         }
      }    
   else {
      o = stdout;
      }

   if( strcmp(datafile,"-") != 0 ) {
      d = fopen(datafile,"r");
      if( ! d ) {
          fprintf(stderr,"Error: Cannot open data file %s\n", datafile );
          }
      }
   else {
      d = stdin;
      }

   if( d && o ) {
       while( fgets(buf,80,d) ) {
           if( sscanf(buf,"%lf%lf",&lt0,&ln0) != 2 ) {
               fprintf(o,"Error: Invalid input data\n");
               }
           else if( lnzGridTransform( grid, lt0, ln0, &lt1, &ln1, reverse ) ) {
               fprintf(o,"%12.8f %12.8f\n",lt1,ln1);
               }
           else {
               fprintf(o,"Error: %s\n",lnzGridLastError(grid) );
               }
           }
       }

   if( d && d != stdin ) fclose(d);
   if( o && o != stdout ) fclose(o);

   lnzGridDestroy( grid );

   return 0;
   }

#ifdef __cplusplus
};
#endif

