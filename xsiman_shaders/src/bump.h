//*****************************************************************************
//  File: bump.h
//  Description: xsiman shader code.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005. All Rights Reserved 
//         (except where noted otherwise)
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef BUMP


#define DEF_SIB_BUMPMAP_GEN_PARAMS \
	color sib_bumpmap_gen_input; vector sib_bumpmap_gen_repeats; \
	vector sib_bumpmap_gen_min; vector sib_bumpmap_gen_max; \
	float sib_bumpmap_gen_inuse; float sib_bumpmap_gen_factor;

#define SIB_BUMPMAP_GEN_PARAMS \
	sib_bumpmap_gen_input, sib_bumpmap_gen_repeats, sib_bumpmap_gen_min, \
	sib_bumpmap_gen_max, sib_bumpmap_gen_inuse, sib_bumpmap_gen_factor

	
vector sib_bumpmap_gen(DEF_SIB_BUMPMAP_GEN_PARAMS DEF_IMAGE_CLIP_PARAMS) 
{
	extern float s, t;
	color Cout = sib_bumpmap_gen_input;

	float maxdim = image_clip_XRes;
	if (image_clip_YRes > maxdim){
		maxdim = image_clip_YRes;
	}

	// nurbs should be flipped
	float ss, tt;
	if (image_clip_invert_s != 0) ss = 1 - s;
	else ss = s;
  	if (image_clip_invert_t != 0) tt = 1 - t;
 	else tt = t;

	if (image_clip_Flip_Horizontal != 0) ss = 1 - ss;
	if (image_clip_Flip_Vertical != 0) tt = 1 - tt;

	float xmin = image_clip_X_Min;
	float xmax = image_clip_X_Max;
	float ymin = image_clip_Y_Min;
	float ymax = image_clip_Y_Max;
	if (xmax <= xmin) {
		xmin = 0;
		xmax = 1;
	}
	if (ymax <= ymin) {
		ymin = 0;
		ymax = 1;
	}
	ss = xmin + ss * (xmax - xmin);
	tt = ymin + tt * (ymax - ymin);

	ss *= xcomp(point sib_bumpmap_gen_repeats);
	tt *= ycomp(point sib_bumpmap_gen_repeats);

	xmin = xcomp(point sib_bumpmap_gen_min);
	xmax = xcomp(point sib_bumpmap_gen_max);
	ymin = ycomp(point sib_bumpmap_gen_min);
	ymax = ycomp(point sib_bumpmap_gen_max);
	if (xmax == xmin) {
		xmin = 0;
		xmax = 1;
	}
	if (ymax == ymin) {
		ymin = 0;
		ymax = 1;
	}
	ss = xmin + ss * (xmax - xmin);
	tt = ymin + tt * (ymax - ymin);

	Cout = texture(image_clip_Source_File, ss, tt, "blur", image_clip_Radius*image_clip_Amount/maxdim);
    	
	/* apply display gamma */
	if(image_clip_Display_Gamma_Affects_Rendering != 0) {
 		setcomp(Cout, 0, pow(comp(Cout,0), 1/image_clip_Display_Gamma));
 		setcomp(Cout, 1, pow(comp(Cout,1), 1/image_clip_Display_Gamma));
 		setcomp(Cout, 2, pow(comp(Cout,2), 1/image_clip_Display_Gamma));	
	}

	/* Convert to "hsl" space */
    	color Chsl = ctransform("hsl", Cout);
    	float hue = comp(Chsl, 0), sat = comp(Chsl, 1), lightness = comp(Chsl, 2);
    	lightness *= (image_clip_Brightness + 100) / 100;
    	Chsl = color (hue, sat, clamp(lightness, 0, 1));
    	Cout = ctransform("hsl", "rgb", clamp(Chsl, color 0, color 1));

	/* Convert to "hsv" space */
    	color Chsv = ctransform("hsv", Cout);
    	hue = comp(Chsv, 0); sat = comp(Chsv, 1); float val = comp(Chsv, 2);
	hue += image_clip_Hue / 360;
	if (hue > 1) {
		hue -= 1;
	}
	sat *= image_clip_Saturation / 100;
    	val *= image_clip_Gain / 100;
    	Chsv = color (hue, sat, val);
    	Cout = ctransform("hsv", "rgb", clamp(Chsv, color 0, color 1));

	Cout *= sib_bumpmap_gen_factor;

	return vector(Cout);
}


#endif