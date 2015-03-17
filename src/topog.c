/*$*****************************************************************************
*
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
*****************************************************************************$*/
#ifdef HAVE_CONFIG_H
 #include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <topog.h>
#include <math.h>

#define V_TORAD(a) (a * 0.017453292519943  ) /*  pi / 180   */
#define V_TODEG(a) (a * 57.295779513082322 ) /*  180 / pi   */

/*-------------------------------------------------------------------------------------*/
double sphere_dist(double lat1, double lon1, double lat2, double lon2, double earthMeanR)
{
   double rD;
   /* must convert to radians... */
   lat1 = V_TORAD( lat1 );
   lat2 = V_TORAD( lat2 );
   lon1 = V_TORAD( lon1 );
   lon2 = V_TORAD( lon2 );
   rD = acos( sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon1 - lon2) );
   return (earthMeanR * rD );
} /* end  SphereDist */


/*-------------------------------------------------------------------------------------*/
int slope_aspect( DATATYPE* xgridb, DATATYPE* ygridb, DATATYPE* zgridb, 
                  char* mask, DATATYPE* sgridb, DATATYPE* agridb,
                  long lines, long samples, double earthsurad
                )
{
   /* -------------
     | nw | n | ne |
     | w  | c | e  |
     | sw | s | se |
      ------------- */
   double nw_x, n_x, ne_x, w_x, e_x, sw_x, s_x, se_x;
   double nw_y, n_y, ne_y, w_y, e_y, sw_y, s_y, se_y;
   double nw_z, n_z, ne_z, w_z, e_z, sw_z, s_z, se_z;
   char   nw_m, n_m, ne_m, w_m, c_m, e_m, sw_m, s_m, se_m;
   long ri, rim1, rip1;
   long i, j;
   double ew, ns, dx, dy;
   double slope, aspect;

   for(i = 1; i < (lines-1); i++)
   {
      /* set the row offset */
      rim1 = (i-1) * samples;
      ri = i * samples;
      rip1 = (i+1) * samples;

      for(j = 1; j < (samples-1); j++)
      {
         /* try to compute for this cell */
         c_m = mask[ri+j];
         if(c_m == 1)
         {
            /* localize info x */
            nw_x = (double)(xgridb[rim1+j-1]);
            w_x = (double)(xgridb[ri+j-1]);
            sw_x = (double)(xgridb[rip1+j-1]);
            n_x = (double)(xgridb[rim1+j]);
            s_x  = (double)(xgridb[rip1+j]);
            ne_x = (double)(xgridb[rim1+j+1]);
            e_x = (double)(xgridb[ri+j+1]);
            se_x = (double)(xgridb[rip1+j+1]);
            
            /* localize info y */
            nw_y = (double)(ygridb[rim1+j-1]);
            w_y = (double)(ygridb[ri+j-1]);
            sw_y = (double)(ygridb[rip1+j-1]);
            n_y = (double)(ygridb[rim1+j]);
            s_y  = (double)(ygridb[rip1+j]);
            ne_y = (double)(ygridb[rim1+j+1]);
            e_y = (double)(ygridb[ri+j+1]);
            se_y = (double)(ygridb[rip1+j+1]);
            
            /* localize info m */
            nw_m = mask[rim1+j-1];
            w_m = mask[ri+j-1];
            sw_m = mask[rip1+j-1];
            n_m = mask[rim1+j];
            s_m  = mask[rip1+j];
            ne_m = mask[rim1+j+1];
            e_m = mask[ri+j+1];
            se_m = mask[rip1+j+1];
            
            /* localize info z 
               Have to check the mask, bordering cell that 
               are masked out get nearest cell(s) z value
            */
            if(nw_m == 1) { nw_z = (double)(zgridb[rim1+j-1]); }
            else nw_z = (double)(zgridb[ri+j]); 

            if(w_m == 1) { w_z = (double)(zgridb[ri+j-1]); }
            else w_z =  (double)(zgridb[ri+j]); 

            if(sw_m == 1) { sw_z = (double)(zgridb[rip1+j-1]); }
            else sw_z =  (double)(zgridb[ri+j]); 

            if(n_m == 1) { n_z = (double)(zgridb[rim1+j]); }
            else n_z =  (double)(zgridb[ri+j]); 
            
            if(s_m == 1) { s_z = (double)(zgridb[rip1+j]); }
            else s_z =  (double)(zgridb[ri+j]); 

            if(ne_m == 1) { ne_z = (double)(zgridb[rim1+j+1]); }
            else ne_z =  (double)(zgridb[ri+j]); 

            if(e_m == 1) { e_z = (double)(zgridb[ri+j+1]); }
            else e_z =  (double)(zgridb[ri+j]); 

            if(se_m == 1) { se_z = (double)(zgridb[rip1+j+1]); }
            else se_z =  (double)(zgridb[ri+j]); 

            if(earthsurad < 0.0)
            {
               /* Compute the average distance across the cell to get a resolution 
                  (maybe this is redundant but adds versatilty for non-uniform grids) */
               dx = 0.5 * (( sqrt((ne_x-nw_x) * (ne_x-nw_x) + (ne_y-nw_y) * (ne_y-nw_y)) +
                             sqrt((e_x-nw_x) * (e_x-nw_x) + (e_y-w_y) * (e_y-w_y) ) + 
                             sqrt((se_x-sw_x) * (se_x-sw_x) + (se_y-sw_y) * (se_y-sw_y))) / 3.0);

               dy = 0.5 * (( sqrt((ne_x-se_x) * (ne_x-se_x) + (ne_y-se_y) * (ne_y-se_y)) + 
                             sqrt((n_x-s_x) * (n_x-s_x) + (n_y-s_y) * (n_y-s_y) ) + 
                             sqrt((nw_x-sw_x) * (nw_x-sw_x) + (nw_y-sw_y) * (nw_y-sw_y))) / 3.0);
            }
            else /* using spherical coords... estimate arc lengths */
            {
               dx = 0.5 * (( sphere_dist(ne_x, ne_y, nw_x, nw_y, earthsurad) +
                             sphere_dist(e_x, e_y, w_x, w_y, earthsurad) +
                             sphere_dist(se_x, se_y, sw_x, sw_y, earthsurad) ) / 3.0);
            
               dy = 0.5 * (( sphere_dist(ne_x, ne_y, se_x, se_y, earthsurad) + 
                             sphere_dist(n_x, n_y, s_x, s_y, earthsurad) + 
                             sphere_dist(nw_x, nw_y, sw_x, sw_y, earthsurad)  ) / 3.0);
            }
            /* slope = arctan[ sqrt( fx^2 + fy^2 ) ]
               where fx and fy are the gradients at E-W and N-S

               Compute the gradients, by third order finite difference 
               weighted by the reciprocal of squared distance (Horn 1981)
            */
            ns = (nw_z-sw_z + 2.0*(n_z-s_z) + ne_z-se_z) / (8.0 * dy);
            ew = (se_z-sw_z + 2.0*(e_z-w_z) + ne_z-nw_z) / (8.0 * dx); 

            /* compute slope */
            slope = V_TODEG(atan(sqrt(ew*ew + ns*ns)));

            /* aspect  = 270 + arctan[fx/fy] - 90 fy/|fy| */
            aspect = V_TODEG((V_TORAD(270.0) + atan(ew/ns) - V_TORAD(90.0) * ns / (sqrt(ns*ns)))); 

            /* drop the spin */
            while(aspect > 360.0)
               aspect -= 360.0;

           /* printf("ew %f dx %f ns %f dy %f slope %f aspect %f\n", ew, dx, ns, dy, slope, aspect); */

            if( isnan((float)slope) )
               sgridb[ri+j] = (float)0.0; 
            else
               sgridb[ri+j] = (float)slope; 

            if( isnan((float)aspect) )
               agridb[ri+j] = (float)180.0;
            else
               agridb[ri+j] = (float)aspect;

         }/* if mask */
      }
   }

   return 0;
}/* slope_aspect */


