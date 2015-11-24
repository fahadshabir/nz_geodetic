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
*  To create a standalone test program compile this with the 
*  linzgridload.c and testlinzgrid.c.
*
*  This program can be used to convert a data
*  file of latitude/longitude values.
*
*  The program generated has the following command line syntax:
*
*  linzgrid [-q] [-r] grid_file input_file [output_file]
*
*  The parameters are
*     grid_file   The name of the NTv2 Ascii grid file defining
*                 the transformation
*     input_file  The name of the input file from which coordinates
*                 are to be read.  Each line should contain a single
*                 coordinate specified as latitude and longitude in
*                 decimal degrees separated by one or more spaces.
*                 The file name '-' can be used to specify reading from
*                 standard input.
*     output_file The name of the file to which converted coordinates 
*                 are written.  If this is '-' or omitted then the 
*                 coordinates are written to standard output.
*
*  The options are
*     -q          Suppresses printing information about the grid
*     -r          Applies the reverse transformation to the coordinates
*  
***********************************************************************
*  This module provides the following functions for applying grid based
*  transformations. (See the test code in the testlinzgrid.c 
*  for an example of implementation of this API).
*  
*----------------------------------------------------------------------
*
*  lnzGrid *lnzGridCreateFromNTv2Asc( char *ascfile );
*  
*     Creates a lnzGrid object representing a grid transformation.
*     (provided by linzgridload.c)
*     
*     Parameters
*        ascfile   The name of the Ascii NTv2 grid file from which to read
*                  the grid
*  
*     Returns
*        lnzGrid * A lnzGrid object handle used by the other functions.
*                  The object will be generated even if the file is not
*                  read successfully.  The only circumstance in which it
*                  is not created is if the memory to store the grid could
*                  not be allocated.  Use the lnzGridIsOk function to 
*                  check that the grid is OK, and lnzGridLastError to
*                  obtain a description of the error if it is not.
*  
*----------------------------------------------------------------------
*
*  void lnzGridDestroy( lnzGrid *grid );
*  
*     Destroys the lnzGrid object created using the lnzGridCreateFromNTv2Asc
*     function.
*  
*     Parameters
*        grid      The handle of the grid object to be destroyed
*  
*     Returns
*        none
*  
*----------------------------------------------------------------------
*
*  int lnzGridIsOk( lnzGrid *grid );
*  
*     Returns the status of the grid.  The status is set when the grid is
*     loaded.
*  
*     Parameters
*        grid      The handle of the grid object to query
*  
*  
*     Returns
*        int       Returns 0 (false) if the grid is not usable, and
*                  non-zero (true) if it is
*  
*----------------------------------------------------------------------
*
*  char *lnzGridLastError( lnzGrid * grid )
*  
*     Returns a pointer to a character string describing the last error 
*     incurred by the grid routines.  
*  
*  
*     Parameters
*        grid      The handle of the grid object to query
*  
*     Returns
*        char *    If the grid file was not loaded successfully then 
*                  this will be the error message resulting from
*                  attempting to load the grid.  
*                  If the grid was loaded successfully then this will 
*                  the be error message from the last coordinate 
*                  transformation (ie the last call to lnzGridTransform)
*  
*----------------------------------------------------------------------
*
*  char *lnzGridDatum( lnzGrid *grid, int reverse )
*  
*     Returns a pointer to the name of the source or target datum of the
*     transformation (as read from the grid file header).
*  
*     Parameters
*        grid      The handle of the grid object to query
*        reverse   If this is 0 (false) then the source datum is returned.
*                  If this is non-zero (true) then the target datum
*                  is returned.
*  
*     Returns
*        char *    A pointer to a character string identifying the source
*                  or target datum
*  
*----------------------------------------------------------------------
*
*  int lnzGridTransform( lnzGrid *grid, double lts, double lns, 
*                        double *ltd, double *lnd, int reverse );
*  
*     Applies a forward or reverse transformation to a latitude/longitude
*     coordinate using bi-linear interpolation on the grid.
*  
*     Parameters
*        grid      The handle of the grid object applying the transformation
*        lts       The input latitude
*        lns       The input longitude
*        ltd       Pointer to the output latitude
*        lnd       Pointer to the output longitude
*        reverse   Specifies whether the forward or reverse transformation
*                  is to be applied.  If this is 0 (false) then the 
*                  input coordinates are assumed to be in the source datum,
*                  and the output coordinates are in the target datum. If
*                  it is non-zero (true) then the reverse transformation is
*                  applied
*  
*     Returns
*        int       If this is non-zero (true) then the transformation was
*                  successful.  If this is zero (false) then the transformation
*                  was unsuccessful.  If the transformtion was unsuccessful then
*                  the lnzGridLastError function can be used to find out why.
*  
*
***********************************************************************
*
*  Copyright: Land Information New Zealand 2001
*  Author: Chris Crook
*
*  $Log: linzgrid.c,v $
*  Revision 1.2  2001/05/23 00:09:16  ccrook
*  Updated comments
*
*  Revision 1.1  2001/05/22 19:51:28  ccrook
*  Initial version of LINZ simple NTv2 grid based transformation module.
*
*  
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

static char *notalloc = "Not enough memory to create grid";
static char *nodatum = "";


/***********************************************************************
*   lnzGridDestroy 
*
*   Function to deallocate the memory used by the grid 
***********************************************************************
*/

void lnzGridDestroy( lnzGrid *grid ){
   float **f;
   int nf;
   if( ! grid ) return;

   /* If the grid data has been allocated, the release this memory */

   if( grid->grid && grid->owngrid ) {
       for( f = grid->grid, nf = grid->nlat; nf; nf--, f++ ) {
          if( *f ) { free(*f); *f = NULL; }
          }
       free(grid->grid);
       grid->grid = NULL;
       }
   grid->gridok = 0;
   strcpy( grid->errmsg, "Grid deallocated");
   }

/***********************************************************************
*  lnzGridIsOk
*
*  Function returns the status of the grid.  This is set when the grid 
*  is loaded.  It returns 0 (false) if the grid is loaded correctly,
*  1 otherwise.
*/

int lnzGridIsOk( lnzGrid *grid ){
   if( ! grid || ! grid->gridok ) return 0;
   return 1;
   }

/***********************************************************************
*  lnzGridLastError
*
*  Returns a pointer to a character string defining the last error that
*  occurred.  If the grid did not load correctly then this retains the 
*  error message from loading the grid.  Otherwise it may contain an 
*  error message from the last attempt to transform a point. 
***********************************************************************
*/

char *lnzGridLastError( lnzGrid *grid ){
   if( ! grid ) return notalloc;
   return grid->errmsg;
   }

/***********************************************************************
* lnzGridDatum
*
*  Returns a pointer to the code defining either the source or target 
*  datum for the grid.  If the reverse flag is 0 (false) then the 
*  source datum is returned, if it is non-zero (true) then the target
*  datum is returned
***********************************************************************
*/

char *lnzGridDatum( lnzGrid *grid, int reverse ){
   if( ! grid || ! grid->gridok ) return nodatum;
   return reverse ? grid->datum1 : grid->datum0;
   }

/***********************************************************************
*  lnzGridCalcOffset
*
*  Calculates the latitude and longitude offset at a specified point on the
*  grid.  Assumes that the point is within the range defined for the grid.
***********************************************************************
*/
   
static void lnzGridCalcOffset( lnzGrid *grid, double lts, double lns, 
                                 double *ltshift, double *lnshift ) {
   int nlt, nln;
   double lat0;
   double lon0;
   double dlat;
   double dlon;
   double shlt0;
   double shln0;
   double shlt1;
   double shln1;
   float *f;

   dlat = grid->dlat;
   nlt = floor( (lts - grid->lat0)/dlat );
   if( nlt < 0 ) nlt = 0;
   if( nlt >= grid->nlat-1 ) nlt = grid->nlat-2;
   lat0 = (lts - (grid->lat0 + dlat * nlt))/dlat;

   dlon = grid->dlon;
   nln = floor( (lns - grid->lon0)/dlon );
   if( nln < 0 ) nln = 0;
   if( nln >= grid->nlon-1 ) nln = grid->nlon-2;
   lon0 = (lns - (grid->lon0 + dlon * nln))/dlon;

   f = grid->grid[nlt] + nln*2;
   shlt0 = f[0]*(1-lon0) + f[2]*lon0;
   shln0 = f[1]*(1-lon0) + f[3]*lon0;

   f = grid->grid[nlt+1] + nln*2;
   shlt1 = f[0]*(1-lon0) + f[2]*lon0;
   shln1 = f[1]*(1-lon0) + f[3]*lon0;

   *ltshift = shlt0*(1-lat0) + shlt1*lat0;
   *lnshift = shln0*(1-lat0) + shln1*lat0; 
   }


/***********************************************************************
*  lnzGridTransform
*
*  Uses the grid to transform a latitude and longitude.  This can either be
*  in the forward direction defined by the From and To datums of the grid,
*  or in the opposite direction.  The direction is chosen using the 
*  reverse parameter - 0 (false) transforms in the forward direction, 
*  non-zero (true) transforms in the reverse direction.  
*
*  The function returns non-zero (true) if the transformation is successful,
*  and 0 (false) if it is not.  When the transformation is not successful
*  the lnzGridLastError function can be used to ascertain why
***********************************************************************
*/

int lnzGridTransform( lnzGrid *grid, double lts, double lns, double *ltd, double *lnd, int reverse ){
   double dlat, dlon, ltt, lnt;

   /* Check that the grid is valid */
   if( ! grid || ! grid->gridok ) return 0;

   /* Empty the error message string */
   grid->errmsg[0] = 0;

   /* Check that the latitude and longitude are within range */

   while( lns > grid->lon1 ) lns -= 360.0;
   while( lns < grid->lon0 ) lns += 360.0;

   if( lns > grid->lon1 || lts < grid->lat0 || lts > grid->lat1 ) {
      strcpy( grid->errmsg, "Point is outside the range of the grid");
      return 0;
      }
   
   /* Now do the calculation */

   lnzGridCalcOffset( grid, lts, lns, &dlat, &dlon );
   
   if( ! reverse ) {
       lts += dlat; 
       lns += dlon;
       }
   else {
       /* For the reverse transformation just iterate once - should be
          enough for any sensible grid */
       ltt = lts - dlat;
       lnt = lns - dlon;
       lnzGridCalcOffset( grid, ltt, lnt, &dlat, &dlon );
       lts = lts - dlat;
       lns = lns - dlon;
       }

   /* Return the results */

   if( ltd ) *ltd = lts;
   if( lnd ) *lnd = lns;
       
   return 1;
   }

#ifdef __cplusplus
};
#endif

