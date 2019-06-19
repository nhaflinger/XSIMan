#include <string.h>
#include <stdio.h>
#include "slb.h"

enum ShaderType {shSurface,shDisplace,shVolume,shImager,shLight,shTransform,shProcedure};

enum SLBParameterType {pString,pFloat,pColor,pPoint,pVector,pNormal,pMatrix};

typedef struct {
   char ptype;
   char ptspc;
   short paddr, pnarr;
   short pflags;
   char pname[24];
} SLBParameter;

typedef struct {
   long nextShader;
   short shsize;
   char type;
   char opt;
   char name[48];
   unsigned short nparm,ncode,ndata,localbase;
   short codix,dataix;
   char vary[32],output[32],cachevar[32];
   short minvary,maxvary,minoutput,maxoutput;
   short shaderflags;
   short ix[8];
   long rsrved[8];
   long code, data;
   SLBParameter parm[MAXNSHADERARGS];
   char buf[128000];
} ShaderHeader;

typedef struct {
   short magic,size;
} MagicHeader;


void appendf(char *s, float f)
{
   char t[255];
   int i;

   sprintf(t,"%f",f);
   i=strlen(t)-1;
   if (strchr(t,'.'))
      {
      while (t[i]=='0') {
         t[i]='\0'; i--;
      };
      if (t[i]=='.') t[i]='\0';
      }
   strcat(s,t);
}

int SLB_Tokenize(char *s, int maxn, char **t) {
   int i=0, n=0;
   int b,q;

   while (1) {
      while ((s[i]<=' ') && (s[i]>'\00')) i++;
      if (s[i]=='\0') return n;
      b=(s[i]=='[');
      if (b) {
         i++; *t=&(s[i]); t++;
         while ((s[i]!=']') && (s[i]>'\0')) i++;
         n++;
         if (s[i]=='\0') return n;
         s[i]='\0'; i++;
      } else {
         q=(s[i]=='\"');
         if (q) {
            i++; *t=&(s[i]); t++;
            while ((s[i]!='\"') && (s[i]!='\0')) i++;
            n++;
            if (s[i]=='\"') {
               s[i]='\0'; i++; }
            else
               return n;
         } else {
            *t=&(s[i]); t++;
            while (s[i]>' ') i++;
            n++;
            if (s[i]=='\0') return n;
            s[i]='\0'; i++;
         };
      };
      if (n>=maxn) return n;
   }
}

int SLB_LoadShader(char *fname, char *s, int lightid) {
MagicHeader h1;
ShaderHeader h;
unsigned short i,j,k,na;
unsigned int jj;
char *t;
char ts[9];
float *fp;
FILE *f;
   s[0]='\0';
     f=fopen(fname,"rb");
     if (f) {
         t=(char *)&h1;
         if (fread(t,1,4,f)==4) {
            t=(char *)&h;
            jj=h1.size;
            if (jj>sizeof(h)) jj=sizeof(h);

            if (fread(t,1,sizeof(h),f)>0) {
				switch(h.type) {
               case shSurface:
                  strcpy(s,"Surface"); break;
               case shDisplace:
                  strcpy(s,"Displacement"); break;
               case shVolume:
                  strcpy(s,"Atmosphere"); break;
               case shImager:
                  strcpy(s,"Imager"); break;
               case shLight:
                  strcpy(s,"LightSource"); break;
               case shTransform:
                  strcpy(s,"Deformation"); break;
               case shProcedure:
                  strcpy(s,"Procedure");
               };
               strcat(s," \"");
/*               t=fname;
               while (*fname!='\0') {
                  if (*fname=='\\') {t=fname; t++;}
                  fname++;
               };
               fname=strchr(t,'.');
               if (fname!=NULL) {
                  ch=*fname; *fname='\0';
                  strcat(s,t); *fname=ch;
               } else
                  strcat(s,t);
*/
			   strcat(s,h.name);
               strcat(s,"\"");

               if (h.type==4) {
                  sprintf(ts," %d",lightid);
                  strcat(s,ts);
               };

               for (i=0;i<h.nparm;i++) {

                  strcat(s," \"");
                //  j=i/8;
                //  if ((h.vary[j] && (1>>((i-j*8)))))
                //     strcat(s,"varying ");
				  j=h.parm[i].pflags;
				  if ((j==2) || (j==3)) strcat(s,"output ");
				  if ((j==1) || (j==3)) strcat(s,"varying ");
                  switch (h.parm[i].ptype) {
                  case pString: strcat(s,"string"); break;
                  case pFloat: strcat(s,"float"); break;
                  case pColor: strcat(s,"color"); break;
                  case pPoint: strcat(s,"point"); break;
                  case pVector: strcat(s,"vector"); break;
                  case pNormal: strcat(s,"normal"); break;
                  case pMatrix: strcat(s,"matrix");
                  };

                  na=h.parm[i].pnarr;
                  if (na>1) {
                     strcat(s,"[");
                     sprintf(ts,"%d",na);
                     strcat(s,ts);
                     strcat(s,"]");
                  };

                  strcat(s," ");
                  strcat(s,h.parm[i].pname);
                  strcat(s,"\" [");
                  t=(char *)(&h);
				  t+=sizeof(h)-(MAXNSHADERARGS-h.nparm)*sizeof(SLBParameter)-128000+h.ncode*4;
                  //t+=h.dataix+h.parm[i].paddr*4-200;
				  t+=h.parm[i].paddr*4-200;
                  fp=(float *)t;
                  for (j=0;j<na;j++) {
                     switch (h.parm[i].ptype) {
                     case pString:
                        strcat(s,"\""); strcat(s,t); strcat(s,"\"");
                        t+=160; break;
                     case pFloat:
                        appendf(s,*fp); fp++; break;
                     case pColor:
                     case pPoint:
                     case pVector:
                     case pNormal:
                        appendf(s,*fp); fp++; strcat(s," ");
                        appendf(s,*fp); fp++; strcat(s," ");
                        appendf(s,*fp); fp++; break;
                     case pMatrix:
                        appendf(s,*fp); fp++;
                        for (k=1;k<16;k++) {
                          strcat(s," "); appendf(s,*fp); fp++;
						}
                     };
                     if ((na>1) && ((j+1)<na)) strcat(s," ");
                  };
                  strcat(s,"]");
               };
            };
         };
         fclose(f);

         return 1;
     };
   return 0;
}
