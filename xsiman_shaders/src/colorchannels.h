//*****************************************************************************
//  File: colorchannels.h
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

#ifndef COLORCHANNELS


#define DEF_SIB_COLOR_COMBINE_PARAMS \
	float sib_color_combine_red; float sib_color_combine_green; float sib_color_combine_blue; \
	float sib_color_combine_alpha; 

#define SIB_COLOR_COMBINE_PARAMS \
	sib_color_combine_red, sib_color_combine_green, sib_color_combine_blue, sib_color_combine_alpha

#define DEF_SIB_HSVA_COMBINE_PARAMS \
	float sib_hsva_combine_hue; float sib_hsva_combine_sat; float sib_hsva_combine_val; \
	float sib_hsva_combine_alpha; 

#define SIB_HSVA_COMBINE_PARAMS \
	sib_hsva_combine_hue, sib_hsva_combine_sat, sib_hsva_combine_val, sib_hsva_combine_alpha

#define DEF_SIB_HLSA_COMBINE_PARAMS \
	float sib_hlsa_combine_hue; float sib_hlsa_combine_sat; float sib_hlsa_combine_lum; \
	float sib_hlsa_combine_alpha; 

#define SIB_HLSA_COMBINE_PARAMS \
	sib_hlsa_combine_hue, sib_hlsa_combine_sat, sib_hlsa_combine_lum, sib_hlsa_combine_alpha


color sib_color_combine(DEF_SIB_COLOR_COMBINE_PARAMS) 
{
	color Cout = color "rgb" (sib_color_combine_red,sib_color_combine_green,sib_color_combine_blue);
	return Cout;
}

color sib_hsva_combine(DEF_SIB_HSVA_COMBINE_PARAMS) 
{
	color Cout = color "hsv" (sib_hsva_combine_hue,sib_hsva_combine_sat,sib_hsva_combine_val);
	return Cout;
}

color sib_hlsa_combine(DEF_SIB_HLSA_COMBINE_PARAMS) 
{
	color Cout = color "hsl" (sib_hlsa_combine_hue,sib_hlsa_combine_sat,sib_hlsa_combine_lum);
	return Cout;
}

#endif