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

#ifdef __cplusplus
extern "C" {
#endif

static char *cvs_id = "$Id: linzgrid.c,v 1.2 2001/05/23 00:09:16 ccrook Exp $";

/***********************************************************************
*  lnzGridCreateFromNTv2Asc
* 
*  Function to load a grid model from an ASCII NTv2 file definition.  The
*  grid is created even if it cannot be loaded correctly.  (It is only not
*  created if there is insufficient memory to create the grid structure 
*  itself).  The lnzGridIsOk function may be used to check that the grid
*  was loaded correctly, and the lnzGridLastError to discover what went
*  wrong if it did not 
* 
***********************************************************************
*/

lnzGrid *lnzGridCreateFromNTv2Asc( char *ascfile ){
    
   lnzGrid *grid;
   FILE *f;
   char buf[80];
   int nfile;
   int norec;
   int nsrec;
   int issecs;
   int ok;
   int got_slat;
   int got_nlat;
   int got_elon;
   int got_wlon;
   int got_latinc;
   int got_loninc;
   int got_gscount;
   long gscount;
   double convdeg;
   int nlt;
   int nln;
   float *gv;

   /* First try to allocate space for the grid */

   grid = (lnzGrid *) malloc( sizeof(lnzGrid) );
   if( ! grid ) return grid;

   /* Initiallize the grid structure */

   grid->gridok = 0;
   grid->owngrid=1;
   grid->datum0[0] = 0;
   grid->datum1[0] = 0;
   grid->errmsg[0] = 0;
   grid->grid = NULL;

   /* Now try to open the file */

   f = fopen(ascfile,"r");
   if( !f ) {
      strcpy( grid->errmsg, "Cannot open grid data file" );
      return grid;
      }

   /* Read the file header */

   ok = 1;
   nfile = 0;
   issecs = 0;
   nsrec = 0;

   if( ! fgets(buf,80,f) 
        || strncmp(buf,"NUM_OREC",8) != 0 
        || sscanf(buf+8,"%d",&norec) != 1 
        || norec < 1 ) {
       strcpy( grid->errmsg, "Invalid file header - no NUM_OREC record" );
       ok = 0;
       }
 
    /* Read the remaining header records */ 

    norec--;
    while( ok && norec-- ) {
       if( ! fgets(buf,80,f) ) {
           strcpy( grid->errmsg, "Grid file truncated - header incomplete" );
           ok = 0;
           continue;
           }
       
       if( strncmp(buf,"NUM_SREC",8) == 0 ) {
           if( sscanf(buf+8,"%d",&nsrec) != 1 || nsrec < 7 ) {
              strcpy( grid->errmsg, "Invalid NUM_SREC record" );
              ok = 0;
              }
           }
       else if( strncmp(buf,"NUM_FILE",8) == 0 ) {
           if( sscanf(buf+8,"%d",&nfile) != 1 || nfile < 1 ) {
              strcpy( grid->errmsg, "Invalid NUM_FILE record" );
              ok = 0;
              }
           if( nfile != 1 ) {
              strcpy( grid->errmsg, "Grid file contains multiple grids - not supported");
              ok = 0;
              }
           }
       else if ( strncmp(buf,"GS_TYPE ",8) == 0 ) {
           if( strncmp(buf+8,"SECONDS",7) == 0 ) {
               issecs = 1;
               }
           else {
               strcpy( grid->errmsg, "Unsupported GS_TYPE in grid file - must be SECONDS");
               ok = 0;
               }
           }
       else if ( strncmp(buf,"SYSTEM_F",8) == 0 ) {
           int c;
           strncpy(grid->datum0, buf+8, MAX_DATUM_LEN );
           grid->datum0[MAX_DATUM_LEN-1] = 0;
           for( c = strlen(grid->datum0); c-- && isspace(grid->datum0[c]); ) {
              grid->datum0[c] = 0;
              }
           }
       else if ( strncmp(buf,"SYSTEM_T",8) == 0 ) {
           int c;
           strncpy(grid->datum1, buf+8, MAX_DATUM_LEN );
           grid->datum1[MAX_DATUM_LEN-1] = 0;
           for( c = strlen(grid->datum1); c-- && isspace(grid->datum1[c]); ) {
              grid->datum1[c] = 0;
              }
           }
       }

   /* Check that no records are missing */

   if( ok && nsrec == 0 ) {
       strcpy( grid->errmsg, "Missing NUM_SREC record in grid file header" );
       ok = 0;
       }

   if( ok && nfile != 1 ) {
       strcpy( grid->errmsg, "Missing NUM_FILE record in grid file header" );
       ok = 0;
       }

   if( ok && ! issecs ) {
       strcpy( grid->errmsg, "Missing GS_TYPE record in grid file header" );
       ok = 0;
       }

   /* Now read the subgrid header */

   got_slat = 0;
   got_nlat = 0;
   got_elon = 0;
   got_wlon = 0;
   got_latinc = 0;
   got_loninc = 0;
   got_gscount = 0;

   convdeg = 1.0/3600.0;

   while( ok && nsrec-- ) {
       if( ! fgets(buf,80,f) ) {
           strcpy( grid->errmsg, "Grid file truncated - sub grid header incomplete" );
           ok = 0;
           continue;
           }

       if( strncmp(buf,"S_LAT   ",8) == 0 ) {
           if( sscanf(buf+8,"%lf",&(grid->lat0)) != 1 ) {
              strcpy( grid->errmsg, "Invalid S_LAT record in sub grid header" );
              ok = 0;
              }
           else {
              got_slat = 1;
              grid->lat0 *= convdeg;
              }
           }
       else if( strncmp(buf,"N_LAT   ",8) == 0 ) {
           if( sscanf(buf+8,"%lf",&(grid->lat1)) != 1 ) {
              strcpy( grid->errmsg, "Invalid N_LAT record in sub grid header" );
              ok = 0;
              }
           else {
              got_nlat = 1;
              grid->lat1 *= convdeg;
              }
           }
       else if( strncmp(buf,"W_LONG  ",8) == 0 ) {
           if( sscanf(buf+8,"%lf",&(grid->lon0)) != 1 ) {
              strcpy( grid->errmsg, "Invalid W_LONG record in sub grid header" );
              ok = 0;
              }
           else {
              got_wlon = 1;
              grid->lon0 *= -convdeg;
              }
           }
       else if( strncmp(buf,"E_LONG  ",8) == 0 ) {
           if( sscanf(buf+8,"%lf",&(grid->lon1)) != 1 ) {
              strcpy( grid->errmsg, "Invalid E_LONG record in sub grid header" );
              ok = 0;
              }
           else {
              got_elon = 1;
              grid->lon1 *= -convdeg;
              }
           }
       else if( strncmp(buf,"LAT_INC ",8) == 0 ) {
           if( sscanf(buf+8,"%lf",&(grid->dlat)) != 1 || grid->dlat <= 0.0 ) {
              strcpy( grid->errmsg, "Invalid LAT_INC record in sub grid header" );
              ok = 0;
              }
           else {
              got_latinc = 1;
              grid->dlat *= convdeg;
              }
           }
       else if( strncmp(buf,"LONG_INC",8) == 0 ) {
           if( sscanf(buf+8,"%lf",&(grid->dlon)) != 1 || grid->dlon <= 0.0 ) {
              strcpy( grid->errmsg, "Invalid LONG_INC record in sub grid header" );
              ok = 0;
              }
           else {
              got_loninc = 1;
              grid->dlon *= convdeg;
              }
           }
       else if( strncmp(buf,"GS_COUNT",8) == 0 ) {
           if( sscanf(buf+8,"%ld",&gscount) != 1 || gscount <= 0 ) {
              strcpy( grid->errmsg, "Invalid GS_COUNT record in sub grid header" );
              ok = 0;
              }
           else {
              got_gscount = 1;
              }
           }
       }

   /* Check for missing sub grid records */

   if( ok && ! got_slat ) {
       strcpy( grid->errmsg, "S_LAT record is missing in sub grid header");
       ok = 0;
       } 

   if( ok && ! got_nlat ) {
       strcpy( grid->errmsg, "N_LAT record is missing in sub grid header");
       ok = 0;
       } 

   if( ok && ! got_elon ) {
       strcpy( grid->errmsg, "E_LONG record is missing in sub grid header");
       ok = 0;
       } 

   if( ok && ! got_wlon ) {
       strcpy( grid->errmsg, "W_LONG record is missing in sub grid header");
       ok = 0;
       } 

   if( ok && ! got_latinc ) {
       strcpy( grid->errmsg, "LAT_INC record is missing in sub grid header");
       ok = 0;
       } 

   if( ok && ! got_loninc ) {
       strcpy( grid->errmsg, "LONG_INC record is missing in sub grid header");
       ok = 0;
       } 

   if( ok && ! got_gscount ) {
       strcpy( grid->errmsg, "GS_COUNT record is missing in sub grid header");
       ok = 0;
       } 

   /* Calculate the grid dimensions and compare with the gscount record */

   if( ok ) {
       grid->nlat = (int) ((grid->lat1 - grid->lat0)/grid->dlat + 1.1);
       grid->nlon = (int) ((grid->lon1 - grid->lon0)/grid->dlon + 1.1);
       if( (long)(grid->nlat)*(long)(grid->nlon) != gscount ) {
          strcpy( grid->errmsg, "GS_COUNT does not match calculated grid dimensions");
          ok = 0;
          }
       }

   /* Allocate the grid array */

   if( ok ) {
       grid->grid = (float **) malloc( grid->nlat * sizeof(float *) );
       if( ! grid->grid ) {
          ok = 0;
          }
       else {
          int nlt;
          for( nlt = 0; nlt < grid->nlat; nlt++ ) { grid->grid[nlt] = NULL; }
          for( nlt = 0; ok && nlt < grid->nlat; nlt++ ) {
              grid->grid[nlt] = (float *) malloc( 2 * grid->nlon * sizeof(float) );
              if( ! grid->grid[nlt] ) ok = 0;
              }
          }
       if( ! ok ) strcpy( grid->errmsg,"Cannot allocate sufficient memory for grid");
       }

   /* Read the grid data */

   if( ok ) {
       for( nlt = 0; ok && nlt < grid->nlat; nlt++ ) {
           gv = grid->grid[nlt];
           for( nln = grid->nlon; ok && nln--; ) {
              float dlt, dln;
              if( ! fgets( buf, 80, f )  ) {
                  strcpy( grid->errmsg, "Grid file truncated in grid data");
                  ok = 0;
                  continue;
                  }
              if( sscanf(buf,"%10f%10f",&dlt,&dln) != 2 ) {
                  strcpy( grid->errmsg, "Invalid grid data in grid file");
                  ok = 0;
                  continue;
                  }
              gv[2*nln] = dlt*convdeg;
              gv[2*nln+1] = -dln*convdeg;
              }
           }
       } 
   
   /* Close the file, set the final grid status, and return the 
      generated grid */

   fclose(f);
  
   grid->gridok = ok;
                  
   return grid;
   }

#ifdef __cplusplus
};
#endif

