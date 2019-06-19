//*****************************************************************************
//  File: utility.h
//  Description: xsiman shader code.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef UTILITY


#define EPS 1.e-06
#define snoise(x) (2*noise(x)-1)
#define vsnoise(x) (2*(vector noise(x))-1)
#define luminance(c) comp(c,0)*0.299 + comp(c,1)*0.587 + comp(c,2)*0.114
#define MIN(a,b) (a < b) ? a : b
#define MAX(a,b) (a >= b) ? a : b

#define DEF_GLOBAL_PARAMS \
	float invert_s = 0; float invert_t = 0; float doOcclusion = 0; float occlusion_samples = 0; \
	float maxdist = 0; float doIndirect = 0; float indirect_samples = 0; float notrace = 0; \
	float hair_id = 0;

#define DEF2_GLOBAL_PARAMS \
	float invert_s; float invert_t; float doOcclusion; float occlusion_samples; \
	float maxdist; float doIndirect; float indirect_samples; float notrace; float hair_id;

#define GLOBAL_PARAMS \
	invert_s, invert_t, doOcclusion, occlusion_samples, \
	maxdist, doIndirect, indirect_samples, notrace, hair_id

#define DEF_OUTPUT_PARAMS \
	output color diffusecolor = 0; output color ambientcolor = 0; \
	output color specularcolor = 0; output color radiancecolor = 0; \
	output color transparencycolor = 0; output color refrcolor = 0;\
	output color reflcolor = 0; output color occlusioncolor = 0; \
	output color indirectcolor = 0; output color causticcolor = 0; \
	output color shadowcolor = 0; 

#define DEF2_OUTPUT_PARAMS \
	output color diffusecolor; output color ambientcolor; output color specularcolor; \
	output color radiancecolor; output color transparencycolor; output color refrcolor; \
	output color reflcolor; output color occlusioncolor; output color indirectcolor; \
	output color causticcolor; output color shadowcolor;

#define OUTPUT_PARAMS \
	diffusecolor, ambientcolor, specularcolor, radiancecolor, transparencycolor, \
	refrcolor, reflcolor, occlusioncolor, indirectcolor, causticcolor, shadowcolor

#define DEF_OUTPUT_VARYING_PARAMS \
	output varying color __diffusecolor = 0; output varying color __ambientcolor = 0; \
	output varying color __specularcolor = 0; output varying color __radiancecolor = 0; \
	output varying color __transparencycolor = 0; output varying color __refrcolor = 0;\
	output varying color __reflcolor = 0; output varying color __occlusioncolor = 0; \
	output varying color __indirectcolor = 0; output varying color __causticcolor = 0; \
	output varying color __shadowcolor = 0; 

#define DEF_TEXTURE_PARAMS \
	float invert_s = 0; float invert_t = 0; 

#define DEF_UNIMPLEMENTED_PARAMS \
	color unimplemented_input;

#define UNIMPLEMENTED_PARAMS \
	unimplemented_input


/* trap unimplemented function calls */
color unimplemented(DEF_UNIMPLEMENTED_PARAMS)
{
	return unimplemented_input;
}

/* displacement function */
void fnc_displace(float dispvalue) {
	extern point P;
	extern normal N;
	extern normal Ng;
	point PP = transform("shader", P);
	normal Ns = normalize(ntransform("shader", N));
	PP = transform("shader", "current", PP + dispvalue * Ns);
	normal NN = calculatenormal(PP);

	/* use shading normals */
	normal Nd = normalize(N) - normalize(Ng);
	NN = normalize(NN) + Nd;

   	N = NN;
	P = PP;

#ifdef AIR
	N = calculatenormal(PP);
#endif
}


float fBm(point p; float octaves, lacunarity, gain, minlevel)
{
	varying float sum = 0, amp = 1;
	varying point pp = p;
	uniform float i;
	
	for(i=0;i<octaves;i+=1)
	{
		if (amp > minlevel) {
			sum += amp*snoise(pp);
			amp *= gain;
			pp *= lacunarity;
		}
	}
	return sum;
}

vector vfBm(point p; float octaves, lacunarity, gain, minlevel)
{
	varying float amp = 1;
	varying point pp = p;
	varying vector sum = 0;
	uniform float i;
	
	for(i=0;i<octaves;i+=1)
	{
		if (amp > minlevel) {
			sum += amp*vsnoise(pp);
			amp *= gain;
			pp *= lacunarity;
		}
	}
	return sum;
}

float turbulence(point p; float octaves, lacunarity, gain, minlevel)
{
	varying float sum = 0, amp = 1;
	varying point pp = p;
	uniform float i;
	
	for(i=0;i<octaves;i+=1)
	{
		if (amp > minlevel) {
			sum += amp*abs(snoise(pp));
			amp *= gain;
			pp *= lacunarity;
		}
	}
	return sum;
}


#endif



