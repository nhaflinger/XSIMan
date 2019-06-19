//*****************************************************************************
//  File: imageprocessing.h
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

#ifndef IMAGEPROCESSING


#define DEF_MIB_COLOR_INTENSITY_PARAMS \
	float mib_color_intensity_factor; color mib_color_intensity_input;

#define MIB_COLOR_INTENSITY_PARAMS \
	mib_color_intensity_factor, mib_color_intensity_input

#define DEF_SIB_COLOR_INVERT_PARAMS \
	color sib_color_invert_input;

#define SIB_COLOR_INVERT_PARAMS \
	sib_color_invert_input

#define DEF_SIB_COLOR_CORRECTION_PARAMS \
	color sib_color_correction_input; float sib_color_correction_gamma; \
	float sib_color_correction_contrast; float sib_color_correction_hue; \
	float sib_color_correction_saturation; float sib_color_correction_level;

#define SIB_COLOR_CORRECTION_PARAMS \
	sib_color_correction_input, sib_color_correction_gamma, \
	sib_color_correction_contrast, sib_color_correction_hue, \
	sib_color_correction_saturation, sib_color_correction_level 
 
	
color mib_color_intensity(DEF_MIB_COLOR_INTENSITY_PARAMS)
{	
	float intensity = (comp(mib_color_intensity_input,0) + comp(mib_color_intensity_input,1) +
		comp(mib_color_intensity_input,2)) / 3;
	color Cout = mib_color_intensity_factor * intensity;
	return Cout;
}

color sib_color_invert(DEF_SIB_COLOR_INVERT_PARAMS)
{	
	color Cout;
	setcomp(Cout, 0, 1-comp(sib_color_invert_input,0));
	setcomp(Cout, 1, 1-comp(sib_color_invert_input,1));
	setcomp(Cout, 2, 1-comp(sib_color_invert_input,2));
	return Cout;
}

color sib_color_correction(DEF_SIB_COLOR_CORRECTION_PARAMS)
{
	color Cout;
	float gamma;
	if (sib_color_correction_gamma < EPS) gamma = EPS;
	else gamma = sib_color_correction_gamma;
	setcomp(Cout, 0, pow(comp(sib_color_correction_input,0), 1/gamma));
	setcomp(Cout, 1, pow(comp(sib_color_correction_input,1), 1/gamma));
	setcomp(Cout, 2, pow(comp(sib_color_correction_input,2), 1/gamma));
	
	Cout = mix(color(0.5), Cout, 0.5+sib_color_correction_contrast);
	
	/* Convert to "hsl" space */
    	color Chsl = ctransform("hsl", Cout);
    	float hue = comp(Chsl, 0); 
	float sat = comp(Chsl, 1); 
	float level = comp(Chsl, 2);
	hue += sib_color_correction_hue / 360;
	if (hue > 1) {
		hue -= 1;
	}
	sat += sib_color_correction_saturation;
    	level += sib_color_correction_level;
    	Chsl = color (hue, sat, level);
    	Cout = ctransform("hsl", "rgb", Chsl);

	return Cout;
}


#endif