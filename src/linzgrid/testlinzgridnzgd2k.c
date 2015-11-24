#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "linzgridnzgd2k.h"

int main( int argc, char *argv[] ) {
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

   if( argc != 2 && argc != 3 ) {
      fprintf(stderr,"Syntax: linzgrid [-r] [-q] data_file [output_file]\n");
      return 1;
      }

   datafile = argv[1];
   outfile = NULL;
   if( argc == 3 ) outfile = argv[2];
   
   grid = lnzGridCreateNzgd2k();
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

