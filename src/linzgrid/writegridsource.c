
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linzgrid.h"
#include "linzgridload.h"


void write_grid_source( lnzGrid *grid, char *funcname, char *filename )
{
    int i,j;
    float *gv;
    char *srcfile=(char *) malloc( strlen(filename)+8);
    FILE *f;
    strcpy(srcfile,filename);
    strcat(srcfile,".c");
    f = fopen(srcfile,"w");
    if( ! f )
    {
        fprintf(stderr,"Cannot open %s for writing\n",srcfile);
        return;
    }
    fprintf(f,"#include <stdlib.h>\n\n");
    fprintf(f,"#include <string.h>\n\n");
    fprintf(f,"#include \"linzgrid.h\"\n\n");
    for( i=0; i<grid->nlat; i++ )
    {
       fprintf(f,"static float row%d[%d]={",i,2*grid->nlon); 
       gv=grid->grid[i];
       for(j=0; j<2*grid->nlon; j++, gv++ )
       {
           fprintf(f,"%s    %.8f",j ? ",\n" : "\n",*gv);
       }
       fprintf(f,"\n    };\n\n");
    }

   fprintf(f,"static float *griddata[%d]={",grid->nlat);
   for( i=0; i<grid->nlat; i++ )
   {
       fprintf(f,"%s    row%d",i ? ",\n" : "\n", i );

   }
   fprintf(f,"\n     };\n\n");

   fprintf(f,"static lnzGrid gridmeta = {\n");
   fprintf(f,"    %.8f,%.8f,%.8f,\n",grid->lat0,grid->lat1,grid->dlat);
   fprintf(f,"    %.8f,%.8f,%.8f,\n",grid->lon0,grid->lon1,grid->dlon);
   fprintf(f,"    %d,%d,%d,%d,\n",grid->nlat,grid->nlon,grid->gridok,0);
   fprintf(f,"    \"%s\",\"%s\",\"\",\n",grid->datum0,grid->datum1);
   fprintf(f,"    griddata};\n\n");

   fprintf(f,"lnzGrid *%s()\n{\n",funcname);
   fprintf(f,"    lnzGrid *grid=(lnzGrid *) malloc(sizeof(lnzGrid));\n");
   fprintf(f,"    memcpy(grid,&gridmeta,sizeof(lnzGrid));\n");
   fprintf(f,"    return grid;\n");
   fprintf(f,"}\n");
   fclose(f);
   
    strcpy(srcfile,filename);
    strcat(srcfile,".h");
    f = fopen(srcfile,"w");
    if( ! f )
    {
        fprintf(stderr,"Cannot open %s for writing\n",srcfile);
        return;
    }
    fprintf(f,"#include \"linzgrid.h\"\n\n");
    fprintf(f,"lnzGrid *%s();\n",funcname);
    fclose(f);
   
    strcpy(srcfile,"test");
    strcat(srcfile,filename);
    strcat(srcfile,".c");
    f = fopen(srcfile,"w");
    if( ! f )
    {
        fprintf(stderr,"Cannot open %s for writing\n",srcfile);
        return;
    }
    fprintf(f,"#include <stdio.h>\n");
    fprintf(f,"#include <string.h>\n");
    fprintf(f,"#include <stdlib.h>\n");
    fprintf(f,"#include \"%s.h\"\n\n",filename);
    fprintf(f,"int main( int argc, char *argv[] ) {\n");
    fprintf(f,"   char *datafile;\n");
    fprintf(f,"   char *outfile;\n");
    fprintf(f,"   lnzGrid *grid;\n");
    fprintf(f,"   FILE *d;\n");
    fprintf(f,"   FILE *o;\n");
    fprintf(f,"   char buf[80];\n");
    fprintf(f,"   int reverse;\n");
    fprintf(f,"   int quiet;\n");
    fprintf(f,"   double lt0, ln0;\n");
    fprintf(f,"   double lt1, ln1;\n");
    fprintf(f,"\n");
    fprintf(f,"   reverse = 0;\n");
    fprintf(f,"   quiet = 0;\n");
    fprintf(f,"\n");
    fprintf(f,"   while( argc > 1 && argv[1][0] == '-' ) {\n");
    fprintf(f,"      if( strcmp(argv[1],\"-r\") == 0 ) {\n");
    fprintf(f,"         reverse = 1;\n");
    fprintf(f,"         }\n");
    fprintf(f,"      else if (strcmp(argv[1],\"-q\") == 0 ) {\n");
    fprintf(f,"         quiet = 1;\n");
    fprintf(f,"         }\n");
    fprintf(f,"      else {\n");
    fprintf(f,"         fprintf(stderr,\"Invalid switch %%s to linzgrid\\n\",argv[1]);\n");
    fprintf(f,"         return 1;\n");
    fprintf(f,"         }\n");
    fprintf(f,"      argc--;\n");
    fprintf(f,"      argv++;\n");
    fprintf(f,"      }\n");
    fprintf(f,"\n");
    fprintf(f,"   if( argc != 2 && argc != 3 ) {\n");
    fprintf(f,"      fprintf(stderr,\"Syntax: linzgrid [-r] [-q] data_file [output_file]\\n\");\n");
    fprintf(f,"      return 1;\n");
    fprintf(f,"      }\n");
    fprintf(f,"\n");
    fprintf(f,"   datafile = argv[1];\n");
    fprintf(f,"   outfile = NULL;\n");
    fprintf(f,"   if( argc == 3 ) outfile = argv[2];\n");
    fprintf(f,"   \n");
    fprintf(f,"   grid = %s();\n",funcname);
    fprintf(f,"   if( ! lnzGridIsOk( grid ) ) {\n");
    fprintf(f,"       fprintf(stderr,\"Error: %%s\\n\", lnzGridLastError(grid) );\n");
    fprintf(f,"       lnzGridDestroy( grid );\n");
    fprintf(f,"       return 2;    \n");
    fprintf(f,"       }\n");
    fprintf(f," \n");
    fprintf(f,"   if( ! quiet ) {\n");
    fprintf(f,"      printf(\"Grid definition\\n\");\n");
    fprintf(f,"      printf(\"  lat0 = %%.5f\\n\", grid->lat0);\n");
    fprintf(f,"      printf(\"  lat1 = %%.5f\\n\", grid->lat1);\n");
    fprintf(f,"      printf(\"  dlat = %%.5f\\n\", grid->dlat);\n");
    fprintf(f,"      printf(\"  lon0 = %%.5f\\n\", grid->lon0);\n");
    fprintf(f,"      printf(\"  lon1 = %%.5f\\n\", grid->lon1);\n");
    fprintf(f,"      printf(\"  dlon = %%.5f\\n\", grid->dlon);\n");
    fprintf(f,"      printf(\"  nlat = %%d\\n\", grid->nlat);\n");
    fprintf(f,"      printf(\"  nlon = %%d\\n\", grid->nlon);\n");
    fprintf(f,"      printf(\"  datum0 = %%s\\n\", grid->datum0);\n");
    fprintf(f,"      printf(\"  datum1 = %%s\\n\", grid->datum1);\n");
    fprintf(f,"      }\n");
    fprintf(f,"   \n");
    fprintf(f,"   if( outfile && strcmp(outfile,\"-\") != 0 ) {\n");
    fprintf(f,"      o = fopen(outfile,\"w\");\n");
    fprintf(f,"      if( ! o ) {\n");
    fprintf(f,"         fprintf(stderr,\"Error: Cannot open output file %%s\\n\", outfile );\n");
    fprintf(f,"         }\n");
    fprintf(f,"      }    \n");
    fprintf(f,"   else {\n");
    fprintf(f,"      o = stdout;\n");
    fprintf(f,"      }\n");
    fprintf(f,"\n");
    fprintf(f,"   if( strcmp(datafile,\"-\") != 0 ) {\n");
    fprintf(f,"      d = fopen(datafile,\"r\");\n");
    fprintf(f,"      if( ! d ) {\n");
    fprintf(f,"          fprintf(stderr,\"Error: Cannot open data file %%s\\n\", datafile );\n");
    fprintf(f,"          }\n");
    fprintf(f,"      }\n");
    fprintf(f,"   else {\n");
    fprintf(f,"      d = stdin;\n");
    fprintf(f,"      }\n");
    fprintf(f,"\n");
    fprintf(f,"   if( d && o ) {\n");
    fprintf(f,"       while( fgets(buf,80,d) ) {\n");
    fprintf(f,"           if( sscanf(buf,\"%%lf%%lf\",&lt0,&ln0) != 2 ) {\n");
    fprintf(f,"               fprintf(o,\"Error: Invalid input data\\n\");\n");
    fprintf(f,"               }\n");
    fprintf(f,"           else if( lnzGridTransform( grid, lt0, ln0, &lt1, &ln1, reverse ) ) {\n");
    fprintf(f,"               fprintf(o,\"%%12.8f %%12.8f\\n\",lt1,ln1);\n");
    fprintf(f,"               }\n");
    fprintf(f,"           else {\n");
    fprintf(f,"               fprintf(o,\"Error: %%s\\n\",lnzGridLastError(grid) );\n");
    fprintf(f,"               }\n");
    fprintf(f,"           }\n");
    fprintf(f,"       }\n");
    fprintf(f,"\n");
    fprintf(f,"   if( d && d != stdin ) fclose(d);\n");
    fprintf(f,"   if( o && o != stdout ) fclose(o);\n");
    fprintf(f,"\n");
    fprintf(f,"   lnzGridDestroy( grid );\n");
    fprintf(f,"\n");
    fprintf(f,"   return 0;\n");
    fprintf(f,"   }\n");
    fprintf(f,"\n");
    fclose(f);
}

int main( int argc, char *argv[] )
{
    char *gridfile=argc > 1 ? argv[1] : "nzgd2kgrid9911.asc";
    char *funcname=argc > 2 ? argv[2] : "lnzGridNzgd2k";
    char *srcfile=argc > 3 ? argv[3] : "linzgridnzgd2k";
    lnzGrid *grid;
    grid = lnzGridCreateFromNTv2Asc( gridfile );
    if( ! lnzGridIsOk(grid) )
    {
        fprintf(stderr,"Source grid %s not valid\n%s\n",
                gridfile, lnzGridLastError(grid), funcname, srcfile );
        return 1;
    }
    write_grid_source( grid, funcname, srcfile );
    return 0;
}
