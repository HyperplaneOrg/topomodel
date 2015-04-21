/*$*****************************************************************************
*
* FILE: 
*  main.c
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
#ifdef HAVE_CONFIG_H
 #include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <topog.h>

#ifdef HAVE_GETOPT_H 
 #include <getopt.h>
#else
 #ifdef HAVE_UNISTD_H 
  #include <unistd.h> 
 #endif
#endif

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 

#define PROGNAME "topomodel"

static const char* SRC_FILE = __FILE__;

/*------------------------------------------------------------------------------
*/
static void print_usage()
{
   fprintf(stdout, "\n   Usage: %s [ OPTIONS ] -x <file> -y <file> -z <file> -m <file> -d <lines,samples> -s <file> -a <file>\n\n", PROGNAME);
   fprintf(stdout, "   -x | --xgrid <file> :: Grid containing the x plane info.\n");
   fprintf(stdout, "   -y | --ygrid <file> :: Grid containing the y plane info.\n");
   fprintf(stdout, "   -z | --zgrid <file> :: Grid containing the z plane info. Units are meters\n");
   fprintf(stdout, "   All above grids should be IEEE float(%zu). Note that the x and y planes should not be masked\n", sizeof(DATATYPE)*8);
   fprintf(stdout, "   or should contain at least two cells outside the mask border. It's ok for the z grid to be masked.\n");
   fprintf(stdout, "   -m | --mask <file> :: Mask grid. Values: 0=>exclude,1=>include, %zu bit integers\n", sizeof(char)*8);
   fprintf(stdout, "   -d | --dims <lines,samples> :: The dimensions of the grid\n");
   fprintf(stdout, "   -s | --slope <file> :: Output slope grid name\n");
   fprintf(stdout, "   -a | --aspect <file> :: Output aspect grid name\n");
   fprintf(stdout, "   -e |--earths-rad <value>:: The mean radius of the spherical earth (meters). If\n");
   fprintf(stdout, "          this parameter is set then: x=>lon, y=lat; units should be decimal degrees.\n");
   fprintf(stdout, "   -h | --help :: This help message.\n");
   fprintf(stdout, "   -v | --verbose :: Run in verbose mode.\n");
   fprintf(stdout, "   See the manpages for more information.\n");
   fprintf(stdout, "   Author: Andrew Michaelis, amac at hyperplane dot org\n");
   fprintf(stdout, "\n");
}/* print_usage */

/*------------------------------------------------------------------------------
*/
void parse_arg_commas(char* strp, long* v0, long* v1, long* v2)
{
   char* strngb = NULL;
   char* ssv = NULL;

   if( (strp == NULL) || (v0 == NULL) || (v1 == NULL) )
      return;
   
   *v0 = 0; *v1 = 0; 

   if( (ssv = strtok_r(strp, ", ", &strngb)) == NULL)
      return;
   *v0 = atol(ssv);

   if( (ssv = strtok_r(NULL, ", ", &strngb)) == NULL)
      return;
   *v1 = atol(ssv);

   if( v2 != NULL )
   {
      *v2 = 0;
      if( (ssv = strtok_r(NULL, ", ", &strngb)) == NULL)
         return;
      *v2 = atol(ssv);
   }
}/* parse_arg_commas */

/*------------------------------------------------------------------------------
*/
void* mmapfile(const char* fname, int proto, size_t bytes)
{
   int opflag;
   int fd;
   mode_t mode = S_IRUSR;
   void* bf = NULL;

   if(proto == PROT_WRITE)
   {
      opflag = O_RDWR | O_CREAT | O_TRUNC;
      mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
   }
   else
      opflag = O_RDONLY;

   if( (fd = open(fname, opflag, mode)) < 0)
   {
      fprintf(stderr, "%s @ %d : %s : %s\n", SRC_FILE, __LINE__, fname, strerror(errno));
      return NULL;
   }

   if(proto == PROT_WRITE) /* init output */
   {
      proto = PROT_WRITE; 
      if( ftruncate(fd, (off_t)bytes) < 0)
      {
         fprintf(stderr, "%s @ %d : trunc %s : %s\n", SRC_FILE, __LINE__, fname, strerror(errno));
         close(fd);
         return NULL;
      }
      if( lseek(fd, (off_t)0 , SEEK_SET ) < 0)
      {
         fprintf(stderr, "%s @ %d : lseek %s : %s\n", SRC_FILE, __LINE__, fname, strerror(errno));
         close(fd);
         return NULL;
      }
   }
 
   if( (bf = mmap(0, bytes, proto, MAP_SHARED, fd, (off_t)0)) == MAP_FAILED )
   {
      fprintf(stderr, "%s @ %d : mmap %s : %s\n", SRC_FILE, __LINE__, fname, strerror(errno));
      close(fd);
      return NULL;
   }
   close(fd);

   return bf;
}/* mmapfile */

/*------------------------------------------------------------------------------
*/
int munmapfile(void* st, size_t bytes, const char* fname)
{
   if( munmap(st, bytes) < 0)
   {
      fprintf(stderr, "%s @ %d : munmap %s : %s\n", SRC_FILE, __LINE__, fname, strerror(errno));
      return -1;
   }
   return 0;
}/* munmapfile */

/*------------------------------------------------------------------------------
*/
int main(int argc, char** argv)
{
   int c, verbose = 0, status = 0;
   int option_index = 0;
   static struct option long_options[] =
   {
      {"help", 0, 0, 0},   
      {"verbose", 0, 0, 0},   
      {"xgrid", 1, 0, 0},   
      {"ygrid", 1, 0, 0},   
      {"zgrid", 1, 0, 0},   
      {"mask", 1, 0, 0},   
      {"dims", 1, 0, 0},   
      {"slope", 1, 0, 0},   
      {"aspect", 1, 0, 0},   
      {"earths-rad", 1, 0, 0},   
      {0, 0, 0, 0}         /* terminate */
   };

   char* xgridnm = NULL;
   char* ygridnm = NULL;
   char* zgridnm = NULL;
   char* masknm = NULL;
   char* slopenm = NULL;
   char* aspectnm = NULL;
   FILE* verbout = stdout;

   long lines = 0, samples = 0;
   char dimargs[32];
   double earthsurad = -1.0;
   
   DATATYPE* xgridb = NULL;
   DATATYPE* ygridb = NULL;
   DATATYPE* zgridb = NULL;
   DATATYPE* sgridb = NULL;
   DATATYPE* agridb = NULL;
   char* mask = NULL;
   
   dimargs[31] = '\0';

   while( (c = getopt_long(argc, argv, "hvx:y:z:m:d:s:a:e:", long_options, &option_index)) != -1 )
   {
      switch (c)
      {
         case 0:
            if(option_index == 0)
            {
   			   print_usage();
   			   exit(0);
            }
            else if(option_index == 1)
               verbose += 1;
            else if(option_index == 2)
               xgridnm = optarg;
            else if(option_index == 3)
               ygridnm = optarg;
            else if(option_index == 4)
               zgridnm = optarg;
            else if(option_index == 5)
               masknm = optarg;
            else if(option_index == 6)
            {
               strncpy(dimargs, optarg, 31);
               parse_arg_commas(dimargs, &lines, &samples, NULL);
            }
            else if(option_index == 7)
               slopenm = optarg;
            else if(option_index == 8)
               aspectnm = optarg;
            else if(option_index == 9)
               earthsurad = atof(optarg);
            break;
   		case 'v':
            verbose += 1;
   			break;
   		case 'x':
            xgridnm = optarg;
   			break;
   		case 'y':
            ygridnm = optarg;
   			break;
   		case 'z':
            zgridnm = optarg;
   			break;
   		case 'm':
            masknm = optarg;
   			break;
   		case 's':
            slopenm = optarg;
            break;
   		case 'e':
            earthsurad = atof(optarg);
            break;
   		case 'a':
            aspectnm = optarg;
            break;
   		case 'd':
            strncpy(dimargs, optarg, 31);
            parse_arg_commas(dimargs, &lines, &samples, NULL);
   			break;
   		case 'h':
   		   print_usage();
   		   exit(0);
         default:
            fprintf(stderr, "Try -h for help.\n");
   		   exit(0);
      }
   }

   if( (xgridnm == NULL) || ( ygridnm == NULL) || (zgridnm == NULL) || (masknm == NULL) )
   {
      fprintf(stderr, "You didn't specify one of the input file names, Try -h for help.\n");
      exit(1);
   }
   
   if( (slopenm == NULL) || ( aspectnm == NULL) )
   {
      fprintf(stderr, "You didn't specify one of the output file names, Try -h for help.\n");
      exit(1);
   }

   if( (lines <= 0) || (samples <= 0) )
   {
      fprintf(stderr, "Bad lines or sample args, Try -h for help.\n");
      exit(1);
   }
      
   if(verbose >= 1)
   {
      fprintf(verbout, "lines = %ld\nsamples = %ld\n", lines, samples);
      fprintf(verbout, "xgrid = \"%s\"\nygrid = \"%s\"\n", xgridnm, ygridnm);
      fprintf(verbout, "zgrid = \"%s\"\nmask = \"%s\"\n", zgridnm, masknm);
      fprintf(verbout, "slope = \"%s\"\naspect = \"%s\"\n", slopenm, aspectnm);
      if(earthsurad > 0.0)
         fprintf(verbout, "Earths mean radius = %f\n", earthsurad);
   }

   /* Open files so we can mmap them... */
   if( (xgridb = (DATATYPE*) mmapfile(xgridnm, PROT_READ, lines*samples*sizeof(DATATYPE))) == NULL)
      exit(1);
   if( (ygridb = (DATATYPE*) mmapfile(ygridnm, PROT_READ, lines*samples*sizeof(DATATYPE))) == NULL)
      exit(1);
   if( (zgridb = (DATATYPE*) mmapfile(zgridnm, PROT_READ, lines*samples*sizeof(DATATYPE))) == NULL)
      exit(1);
   if( (mask = (char*) mmapfile(masknm, PROT_READ, lines*samples*sizeof(char))) == NULL)
      exit(1);
   if( (sgridb = (DATATYPE*) mmapfile(slopenm, PROT_WRITE, lines*samples*sizeof(DATATYPE))) == NULL)
      exit(1);
   if( (agridb = (DATATYPE*) mmapfile(aspectnm, PROT_WRITE, lines*samples*sizeof(DATATYPE))) == NULL)
      exit(1);

   /* do some work */

   status = slope_aspect(xgridb, ygridb, zgridb, mask, sgridb, agridb, lines, samples, earthsurad);

   /* done some clean up */
   
   if( munmapfile(xgridb, lines*samples*sizeof(DATATYPE), xgridnm) < 0)
      exit(1);
   if( munmapfile(ygridb, lines*samples*sizeof(DATATYPE), ygridnm) < 0)
      exit(1);
   if( munmapfile(zgridb, lines*samples*sizeof(DATATYPE), zgridnm) < 0)
      exit(1);
   if( munmapfile(mask, lines*samples*sizeof(char), masknm) < 0)
      exit(1); 
   if( munmapfile(agridb, lines*samples*sizeof(DATATYPE), aspectnm) < 0)
      exit(1);
   if( munmapfile(sgridb, lines*samples*sizeof(DATATYPE), slopenm) < 0)
      exit(1);

   exit(0);
}/* end main */


