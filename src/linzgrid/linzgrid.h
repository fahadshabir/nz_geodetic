#ifndef LINZGRID_H
#define LINZGRID_H
/***********************************************************************
*
*  $Id: linzgrid.h,v 1.2 2001/05/23 00:09:16 ccrook Exp $
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
*  Copyright: Land Information New Zealand 2001
*  Author: Chris Crook
*
*  $Log: linzgrid.h,v $
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

#ifdef __cplusplus
extern "C" {
#endif

/* Structure defining the grid transformation model */

#define MAX_ERRMSG_LEN 80
#define MAX_DATUM_LEN  16

typedef struct {
   double lat0;    /* The minimum latitude */
   double lat1;    /* The maximum latitude */
   double dlat;    /* The latitude increment */
   double lon0;    /* The minimum longitude */
   double lon1;    /* The maximum longitude */
   double dlon;    /* The longitude increment */
   int nlat;       /* The number of latitude grid values */
   int nlon;       /* The number of longitude grid values */
   int gridok;     /* Flags whether the grid has been correctly loaded */
   int owngrid;    /* Flags whether this object owns the grid data */
   char datum0[MAX_DATUM_LEN];  /* The source datum for the transformation */
   char datum1[MAX_DATUM_LEN];  /* The target datum for the transformation */
   char errmsg[MAX_ERRMSG_LEN]; /* The last error message recorded */
   float **grid;   /* The grid data */
} lnzGrid;


/* Functions to manage the grid and transform coordinates.  See the comments
   in the source code file linzgrid.c for details of these functions
*/

void     lnzGridDestroy( lnzGrid *grid );

int      lnzGridIsOk( lnzGrid *grid );

char *   lnzGridLastError( lnzGrid *grid );

char *    lnzGridDatum( lnzGrid *grid, int reverse );
   
int      lnzGridTransform( lnzGrid *grid, double lts, double lns, 
                           double *ltd, double *lnd, int reverse );

#ifdef __cplusplus
};
#endif

#endif

