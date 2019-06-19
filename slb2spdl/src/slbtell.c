/*
   slbtell.c

   Prints a RIB shader declaration for a .slb compiled
   shader file

   (c) 2000 SiTex Graphics.  All rights reserved.

   Last Modified:  July 14, 2000

*/
/*#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "slb.h"

#pragma hdrstop

#ifdef __BORLANDC
#pragma argsused
#endif

int main(int argc, char* argv[])
{
   int i,j,n,n2,old;
   ShaderString ss;
   char *fname, *typ, *clss;
   char *tk[MAXNSHADERTOKENS];
   char *t2[4];
   char drive[64],dir[260],name[260],ext[64],slbname[260];

   if (argc<2) {
	  printf("Version: 2.2\n");
      printf("Usage:  slbtell [-o] <shader.slb>\n");
   } else {
      fname=argv[1];
      old=(strcmp(fname,"-o")==0);
      if (old) {
         if (argc<3) { printf("Missing file name"); return 1;};
         fname=argv[2];
      }
	  _splitpath(fname,drive,dir,name,ext);
	  _makepath(slbname,drive,dir,name,".slb");

      if (SLB_LoadShader(slbname,ss,1)) {
         n=SLB_Tokenize(ss,MAXNSHADERTOKENS,&(tk[0]));
         if (n>1) {
            if (old) {
               i=2;
               typ="surface";
               if (strcmp(tk[0],"Displacement")==0) typ="displacement";
               if (strcmp(tk[0],"Atmosphere")==0) typ="volume";
               if (strcmp(tk[0],"Deformation")==0) typ="transformation";

               if (strcmp(tk[0],"Procedure")==0) typ="procedure";
               if (strcmp(tk[0],"LightSource")==0) {typ="light"; i=3;};
               printf("%s \"%s\"\n",typ,name);      // shader type & name
               while ((i+1)<n) {
                  n2=SLB_Tokenize(tk[i],4,&(t2[0]));
                  j=0;
                  if (strcmp(t2[0],"output")==0) j=1;
                  clss=t2[j];
                  if (strcmp(clss,"varying")==0) {
                     j++;
                  } else {
                     clss="uniform";
                  }
                  typ=t2[j];
                  printf("    \"%s\" \"%s %s\"\n",t2[j+1],clss,typ);
                  printf("\t\tDefault value: ");
                  i++;
                  if (strcmp(typ,"color")==0) {
                     printf("\"rgb\" [%s]",tk[i]);
                  } else if ((strcmp(typ,"point")==0)|(strcmp(typ,"vector")==0)|(strcmp(typ,"normal")==0)) {
                     printf("\"current\" [%s]",tk[i]);
                  } else if ((strcmp(typ,"matrix")==0)|(strchr(typ,'[')!=NULL)){
                     printf("[%s]",tk[i]);
                  } else {
                     printf("%s",tk[i]);
                  }
                  printf("\n");
                  i++;
               }
            } else {
               printf("%s \"%s\"",tk[0],name);      // shader type & name
               i=2;
               if (strcmp(tk[0],"LightSource")==0) { // print light handle
                  printf(" "); printf(tk[2]); i=3;
               }
               printf("\n");
               while ((i+1)<n) {
                  printf("  \"%s\" [%s]\n",tk[i],tk[i+1]);
                  i+=2;
               }
            }
         }
      } else {
         printf("Unable to load .slb file\n");
         return 1;
      }
   }
   return 0;
}*/

//---------------------------------------------------------------------------
