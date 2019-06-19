//*****************************************************************************
//  File: environment.h
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


#ifndef ENVIRONMENT


#define DEF_SIB_ENVIRONMENT_PARAMS \
	color sib_environment_tex; float sib_environment_reflection_intensity;

#define SIB_ENVIRONMENT_PARAMS \
	sib_environment_tex, sib_environment_reflection_intensity


color sib_environment (DEF_SIB_ENVIRONMENT_PARAMS DEF_IMAGE_CLIP_PARAMS)
{
	extern normal N;
	extern vector I;
	color Cout = 0;
	normal Nf = faceforward(normalize(N), I);
	normal IN = normal normalize(I);
	extern float s, t;

	if(image_clip_Source_File != "") {
		vector D = reflect(IN, Nf);
		D = vtransform("world", D);
		float maxdim = image_clip_XRes;
		if (image_clip_YRes > maxdim){
			maxdim = image_clip_YRes;
		}
		Cout = color environment(image_clip_Source_File, D, "blur", image_clip_Radius*image_clip_Amount/maxdim);
	}

	return Cout;
}


#endif