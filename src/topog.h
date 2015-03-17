/*$*****************************************************************************
* FILE: 
*  topog.c
*
* AUTHOR:
*  Andrew Michaelis, amac@hyperplane.org
* 
* LICENSE:
*  This file is part of topomodel.
*
*  topomodel is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
* 
*  topomodel is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
* 
*  You should have received a copy of the GNU General Public License
*  along with topomodel. If not, see http://www.gnu.org/licenses/.
*
*  Copyright (C) 2015 Andrew Michaelis
* 
***************************************************************************$***/
#ifndef TOPOG_H
#define TOPOG_H 1

#ifdef HAVE_CONFIG_H
 #include <config.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define DATATYPE float

/*------------------------------------------------------------------------------
*  This routines computes the slope and aspect using x, y, z, and mask grids.
*  See Horn, 1981.
*/
int slope_aspect( DATATYPE* xgridb, DATATYPE* ygridb, DATATYPE* zgridb, 
                  char* mask, DATATYPE* sgridb, DATATYPE* agridb,
                  long lines, long samples, double earthsurad
                );

/*------------------------------------------------------------------------------
*  This routines computes sphereical distances for geographic based
*  y,x pseudo grids. The longitude and latitude units are decimal
*  degrees. The earths mean radius units are meters.
*/
double sphere_dist( double lat1, double lon1, double lat2, 
                    double lon2, double earthMeanR
                  );


#ifdef __cplusplus
}
#endif

#endif

