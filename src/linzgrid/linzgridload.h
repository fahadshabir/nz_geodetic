#ifndef LINZGRIDLOAD_H
#define LINZGRIDLOAD_H
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

#include "linzgrid.h"

lnzGrid *lnzGridCreateFromNTv2Asc( char *ascfile );

#ifdef __cplusplus
};
#endif

#endif

