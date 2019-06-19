/*
   SLB.h

   Routines for obtaining the parameters for
   shaders compiled to .slb files for AIR

   (c) 2000 SiTex Graphics.  All rights reserved.

   Last Modified:  July 14, 2000

*/

#define MAXSHADERLENGTH 160000
#define MAXNSHADERARGS 4000
#define MAXNSHADERTOKENS (MAXNSHADERARGS*2+3)

//enum ShaderType {shSurface,shDisplace,shVolume,shImager,shLight,shTransform,shProcedure};

typedef char ShaderString [MAXSHADERLENGTH];

int SLB_LoadShader(char *fname, char *s, int lightid);

/* Returns a fully qualified shader declaration
   in the buffer pointed to by s

   For Light shaders lightid is inserted as the 3rd
   token in the string.

   The returned string is a self-contained RIB
   shader declaration */


int SLB_Tokenize(char *s, int maxt, char **t);

/* "Tokenizes" a shader declaration in buffer s
   Returns the number of tokens in s.
   t[i] points to the ith token
   s is modified. */

// N.B.:  SLB_Tokenize can be called again on each parameter value string
//        to obtain its elements
 