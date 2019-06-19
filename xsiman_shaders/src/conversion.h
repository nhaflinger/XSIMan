//*****************************************************************************
//  File: conversion.h
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

#ifndef CONVERSION


#define DEF_SIB_COLOR_TO_SCALAR_PARAMS \
	float sib_color_to_scalar_alpha; color sib_color_to_scalar_input; 

#define SIB_COLOR_TO_SCALAR_PARAMS \
	sib_color_to_scalar_alpha, sib_color_to_scalar_input 

#define DEF_SIB_COLOR_RGB_TO_HSV_PARAMS \
	color sib_color_rgb_to_hsv_rgba;

#define SIB_COLOR_RGB_TO_HSV_PARAMS \
	sib_color_rgb_to_hsv_rgba

#define DEF_SIB_COLOR_HSV_TO_RGB_PARAMS \
	color sib_color_hsv_to_rgb_hsva;

#define SIB_COLOR_HSV_TO_RGB_PARAMS \
	sib_color_hsv_to_rgb_hsva

#define DEF_SIB_COLOR_TO_VECTOR_PARAMS \
	float sib_color_to_vector_modeR, sib_color_to_vector_modeG, sib_color_to_vector_modeB, \
	sib_color_to_vector_modeA; color sib_color_to_vector_input; 

#define SIB_COLOR_TO_VECTOR_PARAMS \
	sib_color_to_vector_modeR, sib_color_to_vector_modeG, sib_color_to_vector_modeB, \
	sib_color_to_vector_modeA, sib_color_to_vector_input

#define DEF_SIB_VECTOR_TO_COLOR_PARAMS \
	float sib_vector_to_color_modeX, sib_vector_to_color_modeY, \
	sib_vector_to_color_modeZ; vector sib_vector_to_color_input; 

#define SIB_VECTOR_TO_COLOR_PARAMS \
	sib_vector_to_color_modeX, sib_vector_to_color_modeY, sib_vector_to_color_modeZ, \
	sib_vector_to_color_input
	
#define DEF_SIB_INTEGER_TO_SCALAR_PARAMS \
	float sib_integer_to_scalar_input;

#define SIB_INTEGER_TO_SCALAR_PARAMS \
	sib_integer_to_scalar_input

#define DEF_SIB_SCALAR_TO_COLOR_PARAMS \
	float sib_scalar_to_color_input, sib_scalar_to_color_alpha;

#define SIB_SCALAR_TO_COLOR_PARAMS \
	sib_scalar_to_color_input, sib_scalar_to_color_alpha
	
#define DEF_SIB_SCALAR_TO_INTEGER_PARAMS \
	float sib_scalar_to_integer_input;

#define SIB_SCALAR_TO_INTEGER_PARAMS \
	sib_scalar_to_integer_input

#define DEF_SIB_SCALARS_TO_VECTOR_PARAMS \
	float sib_scalars_to_vector_modeX, sib_scalars_to_vector_modeY, \
	sib_scalars_to_vector_modeZ, sib_scalars_to_vector_inputX, \
	sib_scalars_to_vector_inputY, sib_scalars_to_vector_inputZ; 

#define SIB_SCALARS_TO_VECTOR_PARAMS \
	sib_scalars_to_vector_modeX, sib_scalars_to_vector_modeY, sib_scalars_to_vector_modeZ, \
	sib_scalars_to_vector_inputX, sib_scalars_to_vector_inputY, sib_scalars_to_vector_inputZ

#define DEF_SIB_VECTOR_TO_SCALAR_PARAMS \
	vector sib_vector_to_scalar_input; float sib_vector_to_scalar_component; 

#define SIB_VECTOR_TO_SCALAR_PARAMS \
	sib_vector_to_scalar_input, sib_vector_to_scalar_component
		
	
	
float sib_color_to_scalar(DEF_SIB_COLOR_TO_SCALAR_PARAMS) 
{
	float Fout = (comp(sib_color_to_scalar_input,0) + comp(sib_color_to_scalar_input,1) +
		comp(sib_color_to_scalar_input,2)) / 3;
	return Fout;
}

color sib_color_rgb_to_hsv(DEF_SIB_COLOR_RGB_TO_HSV_PARAMS) 
{
	color Cout = ctransform("hsv", sib_color_rgb_to_hsv_rgba);
	return Cout;
}

color sib_color_hsv_to_rgb(DEF_SIB_COLOR_HSV_TO_RGB_PARAMS) 
{
	color Cout = ctransform("hsv", "rgb", sib_color_hsv_to_rgb_hsva);
	return Cout;
}

vector sib_color_to_vector(DEF_SIB_COLOR_TO_VECTOR_PARAMS)
{
	color Vout = sib_color_to_vector_input;
	setcomp(Vout, sib_color_to_vector_modeR - 1, comp(sib_color_to_vector_input, 0));
	setcomp(Vout, sib_color_to_vector_modeG - 1, comp(sib_color_to_vector_input, 1));
	setcomp(Vout, sib_color_to_vector_modeB - 1, comp(sib_color_to_vector_input, 2));
	//if (sib_color_to_vector_modeA > 0 && ncomps > 3) {
		//setcomp(Vout, sib_color_to_vector_modeA - 1, comp(sib_color_to_vector_input, ncomps-1));
	//}

	return vector(Vout);
}

color sib_vector_to_color(DEF_SIB_VECTOR_TO_COLOR_PARAMS)
{
	color Cout = color(sib_vector_to_color_input);
	setcomp(Cout, sib_vector_to_color_modeX - 1, comp(sib_vector_to_color_input, 0));
	setcomp(Cout, sib_vector_to_color_modeY - 1, comp(sib_vector_to_color_input, 1));
	setcomp(Cout, sib_vector_to_color_modeZ - 1, comp(sib_vector_to_color_input, 2));

	return Cout;
}

float sib_integer_to_scalar(DEF_SIB_INTEGER_TO_SCALAR_PARAMS)
{
	return sib_integer_to_scalar_input;
}

color sib_scalar_to_color(DEF_SIB_SCALAR_TO_COLOR_PARAMS) 
{
	color Cout;
	setcomp(Cout, 0, sib_scalar_to_color_input);
	setcomp(Cout, 1, sib_scalar_to_color_input);
	setcomp(Cout, 2, sib_scalar_to_color_input);
	return Cout;
}

float sib_scalar_to_integer(DEF_SIB_SCALAR_TO_INTEGER_PARAMS)
{
	return sib_scalar_to_integer_input;
}

vector sib_scalars_to_vector(DEF_SIB_SCALARS_TO_VECTOR_PARAMS)
{
	point Vout = point(sib_scalars_to_vector_inputX,sib_scalars_to_vector_inputY,sib_scalars_to_vector_inputZ);
	setcomp(Vout, sib_scalars_to_vector_modeX - 1, sib_scalars_to_vector_inputX);
	setcomp(Vout, sib_scalars_to_vector_modeY - 1, sib_scalars_to_vector_inputY);
	setcomp(Vout, sib_scalars_to_vector_modeZ - 1, sib_scalars_to_vector_inputZ);

	return vector(Vout);
}

float sib_vector_to_scalar(DEF_SIB_VECTOR_TO_SCALAR_PARAMS)
{
	float Fout = comp(sib_vector_to_scalar_input, sib_vector_to_scalar_component);
	return Fout;
}

#endif