//*****************************************************************************
//  File: combined.h
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


#ifndef TEXTURE


#define DEF_TXT2D_IMAGE_EXPLICIT_PARAMS \
	color txt2d_image_explicit_tex; vector txt2d_image_explicit_repeats; \
	vector txt2d_image_explicit_min; vector txt2d_image_explicit_max; \
	float txt2d_image_explicit_bump_inuse; float txt2d_image_explicit_factor; \
	string txt2d_image_explicit_tspace_id; float txt2d_image_explicit_tspace_id_u; \
	float txt2d_image_explicit_tspace_id_v;

#define TXT2D_IMAGE_EXPLICIT_PARAMS \
	txt2d_image_explicit_tex, txt2d_image_explicit_repeats, txt2d_image_explicit_min, \
	txt2d_image_explicit_max, txt2d_image_explicit_bump_inuse, txt2d_image_explicit_factor, \
	txt2d_image_explicit_tspace_id, txt2d_image_explicit_tspace_id_u, \
	txt2d_image_explicit_tspace_id_v

#define DEF_TXT2D_SCALARIMAGE_EXPLICIT_PARAMS \
	color txt2d_scalarimage_explicit_tex; vector txt2d_scalarimage_explicit_repeats; \
	vector txt2d_scalarimage_explicit_min; vector txt2d_scalarimage_explicit_max; \
	float txt2d_scalarimage_explicit_bump_inuse; float txt2d_scalarimage_explicit_factor; \
	string txt2d_scalarimage_explicit_tspace_id; float txt2d_scalarimage_explicit_tspace_id_u; \
	float txt2d_scalarimage_explicit_tspace_id_v;

#define TXT2D_SCALARIMAGE_EXPLICIT_PARAMS \
	txt2d_scalarimage_explicit_tex, txt2d_scalarimage_explicit_repeats, \
	txt2d_scalarimage_explicit_min, txt2d_scalarimage_explicit_max, \
	txt2d_scalarimage_explicit_bump_inuse, txt2d_scalarimage_explicit_factor, \
	txt2d_scalarimage_explicit_tspace_id, txt2d_scalarimage_explicit_tspace_id_u, \
	txt2d_scalarimage_explicit_tspace_id_v

#define DEF_IMAGE_CLIP_PARAMS \
	float image_clip_Display_Gamma; float image_clip_Display_Gamma_Affects_Rendering; \
	float image_clip_Brightness, image_clip_Gain, image_clip_Saturation, \
	image_clip_Hue, image_clip_Amount, image_clip_Radius, image_clip_XRes, image_clip_YRes, \
	image_clip_Flip_Horizontal, image_clip_Flip_Vertical, image_clip_X_Min, image_clip_X_Max, \
	image_clip_Y_Min, image_clip_Y_Max; string image_clip_Source_File; \
	float image_clip_invert_s; float image_clip_invert_t;

#define IMAGE_CLIP_PARAMS \
	image_clip_Display_Gamma, image_clip_Display_Gamma_Affects_Rendering, \
	image_clip_Brightness, image_clip_Gain, image_clip_Saturation, image_clip_Hue, \
  	image_clip_Amount, image_clip_Radius, image_clip_XRes, image_clip_YRes, \
	image_clip_Flip_Horizontal, image_clip_Flip_Vertical, image_clip_X_Min, image_clip_X_Max, \
	image_clip_Y_Min, image_clip_Y_Max, image_clip_Source_File, \
	image_clip_invert_s, image_clip_invert_t

#define DEF_TXT3D_CELLULAR_V3_PARAMS \
	color txt3d_cellular_v3_color1; color txt3d_cellular_v3_color2; \
	float txt3d_cellular_v3_alpha_output; float txt3d_cellular_v3_alpha_factor; \
	vector txt3d_cellular_v3_repeats; vector txt3d_cellular_v3_min; \
	vector txt3d_cellular_v3_max; float txt3d_cellular_v3_bump_inuse; \
	float txt3d_cellular_v3_factor; float txt3d_cellular_v3_alt_x; \
	float txt3d_cellular_v3_alt_y; float txt3d_cellular_v3_alt_z;

#define TXT3D_CELLULAR_V3_PARAMS \
	txt3d_cellular_v3_color1, txt3d_cellular_v3_color2, \
	txt3d_cellular_v3_alpha_output, txt3d_cellular_v3_alpha_factor, txt3d_cellular_v3_repeats, \
	txt3d_cellular_v3_min, txt3d_cellular_v3_max, txt3d_cellular_v3_bump_inuse, \
	txt3d_cellular_v3_factor, txt3d_cellular_v3_alt_x, txt3d_cellular_v3_alt_y, \
	txt3d_cellular_v3_alt_z

#define DEF_TXT3D_CHECKERBOARD_PARAMS \
	color txt3d_checkerboard_color1; color txt3d_checkerboard_color2; \
	float txt3d_checkerboard_alpha_output; float txt3d_checkerboard_alpha_factor; \
	float txt3d_checkerboard_xsize; float txt3d_checkerboard_ysize; float txt3d_checkerboard_zsize; \
	vector txt3d_checkerboard_repeats; vector txt3d_checkerboard_min; \
	vector txt3d_checkerboard_max; float txt3d_checkerboard_bump_inuse; \
	float txt3d_checkerboard_factor; float txt3d_checkerboard_alt_x; \
	float txt3d_checkerboard_alt_y; float txt3d_checkerboard_alt_z;

#define TXT3D_CHECKERBOARD_PARAMS \
	txt3d_checkerboard_color1, txt3d_checkerboard_color2, \
	txt3d_checkerboard_alpha_output, txt3d_checkerboard_alpha_factor, txt3d_checkerboard_xsize, \
	txt3d_checkerboard_ysize, txt3d_checkerboard_zsize, txt3d_checkerboard_repeats, \
	txt3d_checkerboard_min, txt3d_checkerboard_max, txt3d_checkerboard_bump_inuse, \
	txt3d_checkerboard_factor, txt3d_checkerboard_alt_x, txt3d_checkerboard_alt_y, \
	txt3d_checkerboard_alt_z

#define DEF_TXT3D_CLOUD_PARAMS \
	color txt3d_cloud_color1; color txt3d_cloud_color2; \
	float txt3d_cloud_alpha_output; float txt3d_cloud_alpha_factor; float txt3d_cloud_contrast; \
	float txt3d_cloud_noise_type; float txt3d_cloud_level_decay; \
	float txt3d_cloud_freq_mul; float txt3d_cloud_iter_max; \
	float txt3d_cloud_level_min; float txt3d_cloud_time; \
	vector txt3d_cloud_repeats; vector txt3d_cloud_min; \
	vector txt3d_cloud_max; float txt3d_cloud_bump_inuse; \
	float txt3d_cloud_factor; float txt3d_cloud_alt_x; \
	float txt3d_cloud_alt_y; float txt3d_cloud_alt_z; float txt3d_cloud_absolute;

#define TXT3D_CLOUD_PARAMS \
	txt3d_cloud_color1, txt3d_cloud_color2, \
	txt3d_cloud_alpha_output, txt3d_cloud_alpha_factor, txt3d_cloud_contrast, \
	txt3d_cloud_noise_type, txt3d_cloud_level_decay, \
	txt3d_cloud_freq_mul, txt3d_cloud_iter_max, \
	txt3d_cloud_level_min, txt3d_cloud_time, txt3d_cloud_repeats, \
	txt3d_cloud_min, txt3d_cloud_max, txt3d_cloud_bump_inuse, \
	txt3d_cloud_factor, txt3d_cloud_alt_x, txt3d_cloud_alt_y, \
	txt3d_cloud_alt_z, txt3d_cloud_absolute

#define DEF_TXT3D_FRACTAL_V3_PARAMS \
	color txt3d_fractal_v3_color1; color txt3d_fractal_v3_color2; float txt3d_fractal_v3_time; \
	float txt3d_fractal_v3_noise_type; float txt3d_fractal_v3_absolute; float txt3d_fractal_v3_iterations; \
	float txt3d_fractal_v3_level_min; float txt3d_fractal_v3_level_decay; float txt3d_fractal_v3_freq_mul; \
	float txt3d_fractal_v3_threshold; float txt3d_fractal_v3_upper_bound; float txt3d_fractal_v3_diffusion; \
	vector txt3d_fractal_v3_repeats; float txt3d_fractal_v3_alt_x; \
	float txt3d_fractal_v3_alt_y; float txt3d_fractal_v3_alt_z; vector txt3d_fractal_v3_min; \
	vector txt3d_fractal_v3_max; vector txt3d_fractal_v3_step; float txt3d_fractal_v3_factor; \
 	float txt3d_fractal_v3_bump_inuse; float txt3d_fractal_v3_alpha_output; float txt3d_fractal_v3_alpha_factor;  

#define TXT3D_FRACTAL_V3_PARAMS \
	txt3d_fractal_v3_color1, txt3d_fractal_v3_color2, txt3d_fractal_v3_time, \
	txt3d_fractal_v3_noise_type, txt3d_fractal_v3_absolute, txt3d_fractal_v3_iterations, \
	txt3d_fractal_v3_level_min, txt3d_fractal_v3_level_decay, txt3d_fractal_v3_freq_mul, \
	txt3d_fractal_v3_threshold, txt3d_fractal_v3_upper_bound, txt3d_fractal_v3_diffusion, \
	txt3d_fractal_v3_repeats, txt3d_fractal_v3_alt_x, \
	txt3d_fractal_v3_alt_y, txt3d_fractal_v3_alt_z, txt3d_fractal_v3_min, \
	txt3d_fractal_v3_max, txt3d_fractal_v3_step, txt3d_fractal_v3_factor, txt3d_fractal_v3_bump_inuse, \
	txt3d_fractal_v3_alpha_output, txt3d_fractal_v3_alpha_factor			

/* we need to do all the image_clip operations in the txt2d nodes */	
color image_clip(DEF_IMAGE_CLIP_PARAMS) 
{	
	color Cout = 0;

	return Cout;
}
	
color txt2d_image_explicit(DEF_TXT2D_IMAGE_EXPLICIT_PARAMS DEF_IMAGE_CLIP_PARAMS) 
{
	color Cout = txt2d_image_explicit_tex;

	float maxdim = image_clip_XRes;
	if (image_clip_YRes > maxdim){
		maxdim = image_clip_YRes;
	}

	float ss = txt2d_image_explicit_tspace_id_u;
      float tt = txt2d_image_explicit_tspace_id_v;

	// nurbs should be flipped
	if (image_clip_invert_s != 0) ss = 1 - ss;
  	if (image_clip_invert_t != 0) tt = 1 - tt;

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

	ss *= xcomp(point txt2d_image_explicit_repeats);
	tt *= ycomp(point txt2d_image_explicit_repeats);

	xmin = xcomp(point txt2d_image_explicit_min);
	xmax = xcomp(point txt2d_image_explicit_max);
	ymin = ycomp(point txt2d_image_explicit_min);
	ymax = ycomp(point txt2d_image_explicit_max);
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

	// bump map
	extern point P;
	extern normal N;
	extern normal Ng;
	if(txt2d_image_explicit_bump_inuse != 0) {       
	 	point PP = transform("shader", P);
       	normal Ns = normalize(ntransform("shader", N));
        	PP = transform("shader", "current", PP + txt2d_image_explicit_factor * vector(Cout) * Ns);
        	normal NN = calculatenormal(PP);
      	/* use shading normals */
		normal Nd = normalize(N) - normalize(Ng);
          	N = normalize(NN) + Nd;	
	}

	return Cout;
}

float txt2d_scalarimage_explicit(DEF_TXT2D_SCALARIMAGE_EXPLICIT_PARAMS DEF_IMAGE_CLIP_PARAMS) 
{
	extern point P;
	extern normal N;
	extern normal Ng;
	color Cout = txt2d_scalarimage_explicit_tex;

	float maxdim = image_clip_XRes;
	if (image_clip_YRes > maxdim){
		maxdim = image_clip_YRes;
	}

	float ss = txt2d_scalarimage_explicit_tspace_id_u;
      float tt = txt2d_scalarimage_explicit_tspace_id_v;

	// nurbs should be flipped
	if (image_clip_invert_s != 0) ss = 1 - ss;
  	if (image_clip_invert_t != 0) tt = 1 - tt;

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

	ss *= xcomp(point txt2d_scalarimage_explicit_repeats);
	tt *= ycomp(point txt2d_scalarimage_explicit_repeats);

	xmin = xcomp(point txt2d_scalarimage_explicit_min);
	xmax = xcomp(point txt2d_scalarimage_explicit_max);
	ymin = ycomp(point txt2d_scalarimage_explicit_min);
	ymax = ycomp(point txt2d_scalarimage_explicit_max);
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
    	Cout = ctransform("hsl", "rgb", Chsl);

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
    	Cout = ctransform("hsv", "rgb", Chsv);

	float Fout = (comp(Cout,0) + comp(Cout,1) + comp(Cout,2)) / 3;

	// bump map
	if(txt2d_scalarimage_explicit_bump_inuse != 0) {       
	 	point PP = transform("shader", P);
       	normal Ns = normalize(ntransform("shader", N));
        	PP = transform("shader", "current", PP + txt2d_scalarimage_explicit_factor * Fout * Ns);
        	normal NN = calculatenormal(PP);
      	/* use shading normals */
		normal Nd = normalize(N) - normalize(Ng);
          	N = normalize(NN) + Nd;	
	}

	return Fout;
}

//********************************************************************************************
//  Some of this code is borrowed from "Advanced RenderMan: Creating CGI for Motion Pictures"
//  by Anthony A. Apodaca & Larry Gritz
//********************************************************************************************
color txt3d_cellular_v3(DEF_TXT3D_CELLULAR_V3_PARAMS) 
{	
	extern float s, t;
	extern normal N;
	extern normal Ng;
	float ss, tt, ww, xmin, xmax, ymin, ymax, zmin, zmax;

	ss = s;
	tt = t;
	ww = 1;

	ss *= xcomp(point txt3d_cellular_v3_repeats);
	tt *= ycomp(point txt3d_cellular_v3_repeats);

	if (txt3d_cellular_v3_alt_x != 0) {
  		ss *= 0.5;
		ss -= 0.25;
	}
	if (txt3d_cellular_v3_alt_y != 0) {
		tt *= 0.5;
		tt += 0.25;
	}

	xmin = xcomp(point txt3d_cellular_v3_min);
	xmax = xcomp(point txt3d_cellular_v3_max);
	ymin = ycomp(point txt3d_cellular_v3_min);
	ymax = ycomp(point txt3d_cellular_v3_max);
	zmin = zcomp(point txt3d_cellular_v3_min);
	zmax = zcomp(point txt3d_cellular_v3_max);
	if (xmax == xmin) {
		xmin = 0;
		xmax = 1;
	}
	if (ymax == ymin) {
		ymin = 0;
		ymax = 1;
	}
	if (zmax == zmin) {
		zmin = 0;
		zmax = 1;
	}
	ss = xmin + ss * (xmax - xmin);
	tt = ymin + tt * (ymax - ymin);
	ww = zmin + ww * (zmax - zmin);

	float f1 = 1000;
	extern point P;
	point PP;
	setxcomp(PP, ss);
	setycomp(PP, tt);
	setzcomp(PP, ww);	
	// XSI texture space seems slightly larger
	PP = transform("shader", 1.5*PP);
	point thiscell = point (floor(xcomp(PP))+0.5, floor(ycomp(PP))+0.5, floor(zcomp(PP))+0.5);
	uniform float i, j, k;
	float jitter = 1.0;
	for (i = -1; i <= 1; i +=1) {
		for (j = -1; j <= 1; j +=1) {
			for (k = -1; k <= 1; k +=1) {
				point testcell = thiscell + vector(i,j,k);
				point pos = testcell + jitter*(vector cellnoise(testcell) - 0.5);
				vector offset = pos - PP;
				float dist = offset . offset;
				if (dist < f1) f1 = dist;
			}
		}
	}
	f1 = sqrt(f1);

	// make this result less constrasty to better match XSI
	f1 = 0.2 + f1 * (0.8 - 0.2);
	color Cout = mix(txt3d_cellular_v3_color1, txt3d_cellular_v3_color2, f1);

	if (txt3d_cellular_v3_alpha_output != 0) {
		float density = (comp(Cout,0) + comp(Cout,1) + comp(Cout,2)) / 3.0;
		Cout = txt3d_cellular_v3_alpha_factor * density;
	}

	// bump map
	if(txt3d_cellular_v3_bump_inuse != 0) {     
	 	point PP = transform("shader", P);
       	normal Ns = normalize(ntransform("shader", N));
		/* XSI bump seems approx 10x smaller */
		float bumpvalue = (1 - (comp(Cout,0) + comp(Cout,1) + comp(Cout,2)) / 3) / 10;
        	PP = transform("shader", "current", PP + txt3d_cellular_v3_factor * bumpvalue * Ns);
        	normal NN = calculatenormal(PP);

		/* use shading normals */
		normal Nd = normalize(N) - normalize(Ng);
          	N = normalize(NN) + Nd;
	}

	return Cout;
}

//********************************************************************************************
//  Some of this code is borrowed from Larry Gritz' LGAntialiasedChecks shader
//********************************************************************************************
color txt3d_checkerboard(DEF_TXT3D_CHECKERBOARD_PARAMS) 
{	
	extern float s, t;
	extern float du, dv;
	extern normal N;
	extern normal Ng;
	extern point P;
	extern vector I;	
	color Cout;
	float ss, tt, ww, xmin, xmax, ymin, ymax, zmin, zmax;

	ss = s;
	tt = t;
	ww = 1;

	ss *= xcomp(point txt3d_checkerboard_repeats);
	tt *= ycomp(point txt3d_checkerboard_repeats);

	if (txt3d_checkerboard_alt_x != 0) {
  		ss *= 0.5;
		ss -= 0.25;
	}
	if (txt3d_checkerboard_alt_y != 0) {
		tt *= 0.5;
		tt += 0.25;
	}

	xmin = xcomp(point txt3d_checkerboard_min);
	xmax = xcomp(point txt3d_checkerboard_max);
	ymin = ycomp(point txt3d_checkerboard_min);
	ymax = ycomp(point txt3d_checkerboard_max);
	zmin = zcomp(point txt3d_checkerboard_min);
	zmax = zcomp(point txt3d_checkerboard_max);

	ss = xmin + ss * (xmax - xmin);
	tt = ymin + tt * (ymax - ymin);
	ww = zmin + ww * (zmax - zmin);

	// filter width
  	float swidth = abs(Du(s)*du) + abs(Dv(s)*dv);
  	float twidth = abs(Du(t)*du) + abs(Dv(t)*dv);
	normal Nf = faceforward (normalize(N), I);
  	float sfuzz = swidth * xcomp(point txt3d_checkerboard_repeats);
  	float tfuzz = twidth * ycomp(point txt3d_checkerboard_repeats);
	float fuzzmax = max(sfuzz, tfuzz);

	float smod = mod(ss, 1);
	float tmod = mod(tt, 1);

  	if (fuzzmax <= 0.5) {
      	float x = ((smoothstep (txt3d_checkerboard_xsize,txt3d_checkerboard_xsize+sfuzz,smod)) + 
			(1 - smoothstep (0,sfuzz,smod)));
      	float y = ((smoothstep (1-txt3d_checkerboard_ysize,1-txt3d_checkerboard_ysize+tfuzz,tmod)) + 
			(1 - smoothstep (0,tfuzz,tmod)));
      	color checkcolor = mix (txt3d_checkerboard_color1, txt3d_checkerboard_color2, x*y + (1-x)*(1-y));
      	Cout = mix (checkcolor, (txt3d_checkerboard_color1+txt3d_checkerboard_color2)/2, smoothstep (.125, .5, fuzzmax));
	}
  	else { 
      	Cout = (txt3d_checkerboard_color1 + txt3d_checkerboard_color2) / 2;
    	}

	if (txt3d_checkerboard_alpha_output != 0) {
		float density = (comp(Cout,0) + comp(Cout,1) + comp(Cout,2)) / 3.0;
		Cout = txt3d_checkerboard_alpha_factor * density;
	}

	// bump map
	if(txt3d_checkerboard_bump_inuse != 0) {     
	 	point PP = transform("shader", P);
       	normal Ns = normalize(ntransform("shader", N));
		/* XSI bump seems approx 10x smaller */
		float bumpvalue = (1 - (comp(Cout,0) + comp(Cout,1) + comp(Cout,2)) / 3) / 10;
        	PP = transform("shader", "current", PP + txt3d_checkerboard_factor * bumpvalue * Ns);
        	normal NN = calculatenormal(PP);

		/* use shading normals */
		normal Nd = normalize(N) - normalize(Ng);
          	N = normalize(NN) + Nd;
	}

	return Cout;
}

color txt3d_cloud(DEF_TXT3D_CLOUD_PARAMS) 
{	
	extern float s, t;
	extern normal N;
	extern normal Ng;
	extern point P;
	extern vector I;	
	color Cout;
	float ss, tt, ww, xmin, xmax, ymin, ymax, zmin, zmax;

	ss = s;
	tt = t;
	ww = 1;

	ss *= xcomp(point txt3d_cloud_repeats);
	tt *= ycomp(point txt3d_cloud_repeats);

	if (txt3d_cloud_alt_x != 0) {
  		ss *= 0.5;
		ss -= 0.25;
	}
	if (txt3d_cloud_alt_y != 0) {
		tt *= 0.5;
		tt += 0.25;
	}

	xmin = xcomp(point txt3d_cloud_min);
	xmax = xcomp(point txt3d_cloud_max);
	ymin = ycomp(point txt3d_cloud_min);
	ymax = ycomp(point txt3d_cloud_max);
	zmin = zcomp(point txt3d_cloud_min);
	zmax = zcomp(point txt3d_cloud_max);

	ss = xmin + ss * (xmax - xmin);
	tt = ymin + tt * (ymax - ymin);
	ww = zmin + ww * (zmax - zmin);

	normal Nf = faceforward (normalize(N), I);

	point PP;
	setxcomp(PP, ss);
	setycomp(PP, tt);
	setzcomp(PP, ww + txt3d_cloud_time);
	PP = transform("shader", PP);
	
	float noiseval;
	if (txt3d_cloud_absolute != 0) {
		noiseval = turbulence(PP, txt3d_cloud_iter_max, txt3d_cloud_freq_mul, txt3d_cloud_level_decay, txt3d_cloud_level_min);
	}
	else {
		noiseval = fBm(PP, txt3d_cloud_iter_max, txt3d_cloud_freq_mul, txt3d_cloud_level_decay, txt3d_cloud_level_min);
	}

	noiseval = mix(0.5, noiseval, txt3d_cloud_contrast);

	if (txt3d_cloud_noise_type != 0) {
		Cout = mix(txt3d_cloud_color1, txt3d_cloud_color2, noiseval);
	}
	else {		
		Cout = mix(txt3d_cloud_color2, txt3d_cloud_color1, noiseval);
	}

	if (txt3d_cloud_alpha_output != 0) {
		Cout = txt3d_cloud_alpha_factor * noiseval;
	}

	// bump map
	if(txt3d_cloud_bump_inuse != 0) {     
	 	point PP = transform("shader", P);
       	normal Ns = normalize(ntransform("shader", N));
		/* XSI bump seems approx 10x smaller */
		float bumpvalue = (1 - (comp(Cout,0) + comp(Cout,1) + comp(Cout,2)) / 3) / 10;
        	PP = transform("shader", "current", PP + txt3d_cloud_factor * bumpvalue * Ns);
        	normal NN = calculatenormal(PP);

		/* use shading normals */
		normal Nd = normalize(N) - normalize(Ng);
          	N = normalize(NN) + Nd;
	}

	return Cout;
}

color txt3d_fractal_v3(DEF_TXT3D_FRACTAL_V3_PARAMS) 
{
	extern float s, t;
	extern normal N;
	extern normal Ng;
	extern point P;
	extern vector I;	
	color Cout = 0;
	float ss, tt, ww, xmin, xmax, ymin, ymax, zmin, zmax;

	ss = s;
	tt = t;
	ww = 1;

	ss *= xcomp(point txt3d_fractal_v3_repeats);
	tt *= ycomp(point txt3d_fractal_v3_repeats);

	if (txt3d_fractal_v3_alt_x != 0) {
  		ss *= 0.5;
		ss -= 0.25;
	}
	if (txt3d_fractal_v3_alt_y != 0) {
		tt *= 0.5;
		tt += 0.25;
	}

	xmin = xcomp(point txt3d_fractal_v3_min);
	xmax = xcomp(point txt3d_fractal_v3_max);
	ymin = ycomp(point txt3d_fractal_v3_min);
	ymax = ycomp(point txt3d_fractal_v3_max);
	zmin = zcomp(point txt3d_fractal_v3_min);
	zmax = zcomp(point txt3d_fractal_v3_max);

	ss = xmin + ss * (xmax - xmin);
	tt = ymin + tt * (ymax - ymin);
	ww = zmin + ww * (zmax - zmin);

	normal Nf = faceforward (normalize(N), I);

	point PP;
	setxcomp(PP, ss);
	setycomp(PP, tt);
	setzcomp(PP, ww + txt3d_fractal_v3_time);
	PP = transform("shader", PP);
	
	float noiseval;
	if (txt3d_fractal_v3_absolute != 0) {
		noiseval = turbulence(PP, txt3d_fractal_v3_iterations, txt3d_fractal_v3_freq_mul, txt3d_fractal_v3_level_decay, txt3d_fractal_v3_level_min);
	}
	else {
		noiseval = fBm(PP, txt3d_fractal_v3_iterations, txt3d_fractal_v3_freq_mul, txt3d_fractal_v3_level_decay, txt3d_fractal_v3_level_min);
	}
	
	// impose the clipping value
	if (noiseval > 2*txt3d_fractal_v3_upper_bound-1)
		noiseval = 2*txt3d_fractal_v3_upper_bound-1;

	// threshold the noise value
	noiseval = smoothstep(2*txt3d_fractal_v3_threshold-1, 1, noiseval);
	
	Cout = mix(txt3d_fractal_v3_color1, txt3d_fractal_v3_color2, noiseval);

	if (txt3d_fractal_v3_alpha_output != 0) {
		Cout = txt3d_fractal_v3_alpha_factor * noiseval;
	}

	// bump map
	if(txt3d_fractal_v3_bump_inuse != 0) { 
	 	point PP = transform("shader", P);   
       	normal Ns = normalize(ntransform("shader", N));
		/* XSI bump seems approx 10x smaller */
		float bumpvalue = (1 - (comp(Cout,0) + comp(Cout,1) + comp(Cout,2)) / 3) / 10;
        	PP = transform("shader", "current", PP + txt3d_fractal_v3_factor * bumpvalue * Ns);
        	normal NN = calculatenormal(PP);

		/* use shading normals */
		normal Nd = normalize(N) - normalize(Ng);
          	N = normalize(NN) + Nd;
	}

	return Cout;
}	

#endif


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


#ifndef HAIR


#define DEF_RH_RENDERER_PARAMS \
	float rh_renderer_TransparencyCrossoverRange; float rh_renderer_TransparencyCrossoverCenter; \
	color rh_renderer_TransparencyTip; color rh_renderer_TransparencyRoot; \
	float rh_renderer_TransparencyInuse; color rh_renderer_Irradiance; \
	float rh_renderer_SpecularDecay; color rh_renderer_Specular; \
	float rh_renderer_SpecularInuse; float rh_renderer_DiffuseCrossoverRange; \
	float rh_renderer_DiffuseCrossoverCenter; float rh_renderer_DiffuseVariation; \
	float rh_renderer_TipBalance; color rh_renderer_DiffuseTipB; \
	color rh_renderer_DiffuseTipA; color rh_renderer_DiffuseRoot; \
	float rh_renderer_FlatShadeRoot; float rh_renderer_FlatShadeTip; \
	color rh_renderer_Ambient; color rh_renderer_ambience; 

#define RH_RENDERER_PARAMS \
	rh_renderer_TransparencyCrossoverRange, rh_renderer_TransparencyCrossoverCenter, \
	rh_renderer_TransparencyTip, rh_renderer_TransparencyRoot, \
	rh_renderer_TransparencyInuse, rh_renderer_Irradiance, \
	rh_renderer_SpecularDecay, rh_renderer_Specular, \
	rh_renderer_SpecularInuse, rh_renderer_DiffuseCrossoverRange, \
	rh_renderer_DiffuseCrossoverCenter, rh_renderer_DiffuseVariation, \
	rh_renderer_TipBalance, rh_renderer_DiffuseTipB, \
	rh_renderer_DiffuseTipA, rh_renderer_DiffuseRoot, \
	rh_renderer_FlatShadeRoot, rh_renderer_FlatShadeTip, \
	rh_renderer_Ambient, rh_renderer_ambience


color rh_renderer (DEF_RH_RENDERER_PARAMS DEF2_GLOBAL_PARAMS DEF2_OUTPUT_PARAMS)
{
	color outputColor, outputOpacity;
	extern color Ci;	
	extern color Oi;
	extern color Cs;
	extern color Os;
	extern normal N;
	extern vector I;
	extern vector dPdv;
	extern float v;
	extern point P;

    	vector T = normalize(dPdv); 
    	vector V = -normalize(I);    
    	color Cspec = 0, Cdiff = 0;  
    	float cosang;

	ambientcolor = rh_renderer_Ambient;
	ambientcolor *= ambient() + rh_renderer_ambience;

	color speccolor = 1;
	if (rh_renderer_SpecularInuse != 0) speccolor = rh_renderer_Specular;
	else speccolor = 0;
	
	color tipcolor = mix(rh_renderer_DiffuseTipB, rh_renderer_DiffuseTipA, rh_renderer_TipBalance);
    
 	color Cflat = 0;
	illuminance (P) {
		cosang = cos(abs(acos(T.normalize(L)) - acos(-T.V)));
		Cspec += Cl * v * pow(cosang, rh_renderer_SpecularDecay);
		Cdiff += Cl * v;
		Cflat += Cl;
    	}

	color opacity = 1;
	float val = smoothstep(clamp(rh_renderer_TransparencyCrossoverCenter - 
		0.5*rh_renderer_TransparencyCrossoverRange,0,1), clamp(rh_renderer_TransparencyCrossoverCenter +
		0.5*rh_renderer_TransparencyCrossoverRange,0,1), v);
	if (rh_renderer_TransparencyInuse != 0) {
		opacity = mix(1-rh_renderer_TransparencyRoot, 1-rh_renderer_TransparencyTip, val);
	}

	val = smoothstep(clamp(rh_renderer_DiffuseCrossoverCenter -
		0.5*rh_renderer_DiffuseCrossoverRange,0,1), clamp(rh_renderer_DiffuseCrossoverCenter +
		0.5*rh_renderer_DiffuseCrossoverRange,0,1), v);

	color haircolor = mix(rh_renderer_DiffuseRoot, tipcolor, val);

	float root_flat_mix = mix(comp(Cdiff,0), 1.0, comp(Cflat,0)*rh_renderer_FlatShadeRoot/100.0);
	float tip_flat_mix = mix(comp(Cdiff,0), 1.0, comp(Cflat,0)*rh_renderer_FlatShadeTip/100.0);
	float flat_mix = mix(root_flat_mix, tip_flat_mix, val);

	specularcolor = speccolor * Cspec;
	diffusecolor = color(flat_mix,flat_mix,flat_mix);

    	color Chsv = ctransform("hsv", haircolor);
	float hue = comp(Chsv, 0); float sat = comp(Chsv, 1); float value = comp(Chsv, 2);
	hue = cellnoise(hair_id);
	sat = 1;
    	value = comp(Chsv, 2);
    	Chsv = color (hue, sat, value);
	color Crgb = ctransform("hsv", "rgb", Chsv);
	float color_variation = clamp(rh_renderer_DiffuseVariation / 10., 0, 1);
	haircolor = mix(haircolor, Crgb, color_variation);

	Oi = Os * opacity;
	Ci = Oi * (haircolor * (ambientcolor*ambient() + diffusecolor) + specularcolor);

#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	Ci += causticcolor;
#endif

	return Ci;
}

#endif


#ifndef ILLUMINATION


#define DEF_MATERIAL_PHONG_PARAMS \
	color material_phong_ambience; vector material_phong_bump; \
	float material_phong_diffuse_inuse; color material_phong_diffuse; \
	color material_phong_ambient; float material_phong_specular_inuse; \
	color material_phong_specular; float material_phong_shiny; \
	float material_phong_refract_inuse; color material_phong_transparency; \
	float material_phong_trans_glossy; uniform float material_phong_transparent_samples; \
	float material_phong_index_of_refraction; float material_phong_inverttrans; \
	float material_phong_usealphatrans; float material_phong_scaletrans; \
	float material_phong_reflect_inuse; color material_phong_reflectivity; \
	float material_phong_reflect_glossy; uniform float material_phong_reflect_samples; \
	float material_phong_invertrefl; float material_phong_usealpharefl; \
	float material_phong_scalerefl; color material_phong_radiance; \
	float material_phong_translucent_inuse; float material_phong_translucency; \
	float material_phong_incand_inuse; color material_phong_incandescence; \
	float material_phong_inc_inten; float material_phong_notrace; \
	color material_phong_environment;

#define MATERIAL_PHONG_PARAMS \
	material_phong_ambience, material_phong_bump, material_phong_diffuse_inuse, \
	material_phong_diffuse, material_phong_ambient, material_phong_specular_inuse, \
	material_phong_specular, material_phong_shiny, material_phong_refract_inuse, \
	material_phong_transparency, material_phong_trans_glossy, material_phong_transparent_samples, \
	material_phong_index_of_refraction, material_phong_inverttrans, \
	material_phong_usealphatrans, material_phong_scaletrans, material_phong_reflect_inuse, \
	material_phong_reflectivity, material_phong_reflect_glossy, material_phong_reflect_samples, \
	material_phong_invertrefl, material_phong_usealpharefl, \
	material_phong_scalerefl, material_phong_radiance, \
	material_phong_translucent_inuse, material_phong_translucency, material_phong_incand_inuse, \
	material_phong_incandescence, material_phong_inc_inten, material_phong_notrace, \
	material_phong_environment

#define DEF_MATERIAL_BLINN_PARAMS \
	color material_blinn_ambience; \
	float material_blinn_diffuse_inuse; color material_blinn_diffuse; \
	color material_blinn_ambient; float material_blinn_specular_inuse; \
	color material_blinn_specular; float material_blinn_roughness; \
	float material_blinn_refract_inuse; color material_blinn_transparency; \
	float material_blinn_trans_glossy; uniform float material_blinn_transparent_samples; \
	float material_blinn_index_of_refraction; float material_blinn_inverttrans; \
	float material_blinn_usealphatrans; float material_blinn_scaletrans; \
	float material_blinn_reflect_inuse; color material_blinn_reflectivity; \
	float material_blinn_reflect_glossy; uniform float material_blinn_reflect_samples; \
	float material_blinn_invertrefl; float material_blinn_usealpharefl; \
	float material_blinn_scalerefl; color material_blinn_radiance; \
	float material_blinn_translucent_inuse; float material_blinn_translucency; \
	float material_blinn_incand_inuse; color material_blinn_incandescence; \
	float material_blinn_inc_inten; float material_blinn_notrace; \
	color material_blinn_environment;

#define MATERIAL_BLINN_PARAMS \
	material_blinn_ambience, material_blinn_diffuse_inuse, \
	material_blinn_diffuse, material_blinn_ambient, material_blinn_specular_inuse, \
	material_blinn_specular, material_blinn_roughness, material_blinn_refract_inuse, \
	material_blinn_transparency, material_blinn_trans_glossy, material_blinn_transparent_samples, \
	material_blinn_index_of_refraction, material_blinn_inverttrans, \
	material_blinn_usealphatrans, material_blinn_scaletrans, material_blinn_reflect_inuse, \
	material_blinn_reflectivity, material_blinn_reflect_glossy, material_blinn_reflect_samples, \
	material_blinn_invertrefl, material_blinn_usealpharefl, \
	material_blinn_scalerefl, material_blinn_radiance, \
	material_blinn_translucent_inuse, material_blinn_translucency, material_blinn_incand_inuse, \
	material_blinn_incandescence, material_blinn_inc_inten, material_blinn_notrace, \
	material_blinn_environment

#define DEF_MATERIAL_LAMBERT_PARAMS \
	color material_lambert_ambience; vector material_lambert_bump; \
	float material_lambert_diffuse_inuse; color material_lambert_diffuse; \
	color material_lambert_ambient; \
	float material_lambert_refract_inuse; color material_lambert_transparency; \
	float material_lambert_trans_glossy; uniform float material_lambert_transparent_samples; \
	float material_lambert_index_of_refraction; float material_lambert_inverttrans; \
	float material_lambert_usealphatrans; float material_lambert_scaletrans; \
	float material_lambert_reflect_inuse; color material_lambert_reflectivity; \
	float material_lambert_reflect_glossy; uniform float material_lambert_reflect_samples; \
	float material_lambert_invertrefl; float material_lambert_usealpharefl; \
	float material_lambert_scalerefl; color material_lambert_radiance; \
	float material_lambert_translucent_inuse; float material_lambert_translucency; \
	float material_lambert_incand_inuse; color material_lambert_incandescence; \
	float material_lambert_inc_inten; float material_lambert_notrace; \
	color material_lambert_environment;

#define MATERIAL_LAMBERT_PARAMS \
	material_lambert_ambience, material_lambert_bump, material_lambert_diffuse_inuse, \
	material_lambert_diffuse, material_lambert_ambient, material_lambert_refract_inuse, \
	material_lambert_transparency, material_lambert_trans_glossy, material_lambert_transparent_samples, \
	material_lambert_index_of_refraction, material_lambert_inverttrans, \
	material_lambert_usealphatrans, material_lambert_scaletrans, material_lambert_reflect_inuse, \
	material_lambert_reflectivity, material_lambert_reflect_glossy, material_lambert_reflect_samples, \
	material_lambert_invertrefl, material_lambert_usealpharefl, \
	material_lambert_scalerefl, material_lambert_radiance, \
	material_lambert_translucent_inuse, material_lambert_translucency, material_lambert_incand_inuse, \
	material_lambert_incandescence, material_lambert_inc_inten, material_lambert_notrace, \
	material_lambert_environment

#define DEF_MATERIAL_COOKTORR_PARAMS \
	color material_cooktorr_ambience; \
	float material_cooktorr_diffuse_inuse; color material_cooktorr_diffuse; \
	color material_cooktorr_ambient; float material_cooktorr_specular_inuse; \
	color material_cooktorr_specular; float material_cooktorr_roughness; \
	float material_cooktorr_refract_inuse; color material_cooktorr_transparency; \
	float material_cooktorr_trans_glossy; uniform float material_cooktorr_transparent_samples; \
	float material_cooktorr_index_of_refraction; float material_cooktorr_inverttrans; \
	float material_cooktorr_usealphatrans; float material_cooktorr_scaletrans; \
	float material_cooktorr_reflect_inuse; color material_cooktorr_reflectivity; \
	float material_cooktorr_reflect_glossy; uniform float material_cooktorr_reflect_samples; \
	float material_cooktorr_invertrefl; float material_cooktorr_usealpharefl; \
	float material_cooktorr_scalerefl; color material_cooktorr_radiance; \
	float material_cooktorr_translucent_inuse; float material_cooktorr_translucency; \
	float material_cooktorr_incand_inuse; color material_cooktorr_incandescence; \
	float material_cooktorr_inc_inten; float material_cooktorr_notrace; \
	color material_cooktorr_environment;

#define MATERIAL_COOKTORR_PARAMS \
	material_cooktorr_ambience, material_cooktorr_diffuse_inuse, \
	material_cooktorr_diffuse, material_cooktorr_ambient, material_cooktorr_specular_inuse, \
	material_cooktorr_specular, material_cooktorr_roughness, material_cooktorr_refract_inuse, \
	material_cooktorr_transparency, material_cooktorr_trans_glossy, material_cooktorr_transparent_samples, \
	material_cooktorr_index_of_refraction, material_cooktorr_inverttrans, \
	material_cooktorr_usealphatrans, material_cooktorr_scaletrans, material_cooktorr_reflect_inuse, \
	material_cooktorr_reflectivity, material_cooktorr_reflect_glossy, material_cooktorr_reflect_samples, \
	material_cooktorr_invertrefl, material_cooktorr_usealpharefl, \
	material_cooktorr_scalerefl, material_cooktorr_radiance, \
	material_cooktorr_translucent_inuse, material_cooktorr_translucency, material_cooktorr_incand_inuse, \
	material_cooktorr_incandescence, material_cooktorr_inc_inten, material_cooktorr_notrace, \
	material_cooktorr_environment

#define DEF_MATERIAL_WARD_PARAMS \
	float material_ward_diffuse_inuse; color material_ward_diffuse; \
	color material_ward_ambient; float material_ward_specular_inuse; \
	color material_ward_glossy; float material_ward_shiny_u; float material_ward_shiny_v; \
	float material_ward_surfderiv_mode; color material_ward_image; \
	float material_ward_refract_inuse; color material_ward_transparency; \
	float material_ward_trans_glossy; uniform float material_ward_transparent_samples; \
	float material_ward_index_of_refraction; float material_ward_inverttrans; \
	float material_ward_usealphatrans; float material_ward_scaletrans; \
	float material_ward_reflect_inuse; color material_ward_reflectivity; \
	float material_ward_reflect_glossy; uniform float material_ward_reflect_samples; \
	float material_ward_invertrefl; float material_ward_usealpharefl; \
	float material_ward_scalerefl; color material_ward_radiance; \
	float material_ward_translucent_inuse; float material_ward_translucency; \
	float material_ward_incand_inuse; color material_ward_incandescence; \
	float material_ward_inc_inten; float material_ward_notrace; \
	color material_ward_environment;

#define MATERIAL_WARD_PARAMS \
	material_ward_diffuse_inuse, material_ward_diffuse, material_ward_ambient, material_ward_specular_inuse, \
	material_ward_glossy, material_ward_shiny_u, material_ward_shiny_v, \
	material_ward_surfderiv_mode, material_ward_image, material_ward_refract_inuse, \
	material_ward_transparency, material_ward_trans_glossy, material_ward_transparent_samples, \
	material_ward_index_of_refraction, material_ward_inverttrans, \
	material_ward_usealphatrans, material_ward_scaletrans, material_ward_reflect_inuse, \
	material_ward_reflectivity, material_ward_reflect_glossy, material_ward_reflect_samples, \
	material_ward_invertrefl, material_ward_usealpharefl, \
	material_ward_scalerefl, material_ward_radiance, \
	material_ward_translucent_inuse, material_ward_translucency, material_ward_incand_inuse, \
	material_ward_incandescence, material_ward_inc_inten, material_ward_notrace, \
	material_ward_environment

#define DEF_MATERIAL_CONSTANT_PARAMS \
	color material_constant_color; float material_constant_refract_inuse; color material_constant_transparency; \
	float material_constant_trans_glossy; uniform float material_constant_transparent_samples; \
	float material_constant_index_of_refraction; float material_constant_inverttrans; \
	float material_constant_usealphatrans; float material_constant_scaletrans; \
	float material_constant_reflect_inuse; color material_constant_reflectivity; \
	float material_constant_reflect_glossy; uniform float material_constant_reflect_samples; \
	float material_constant_invertrefl; float material_constant_usealpharefl; \
	float material_constant_scalerefl; color material_constant_radiance; \
	float material_constant_incand_inuse; color material_constant_incandescence; \
	float material_constant_inc_inten; float material_constant_notrace; \
	color material_constant_environment;

#define MATERIAL_CONSTANT_PARAMS \
	material_constant_color, material_constant_refract_inuse, \
	material_constant_transparency, material_constant_trans_glossy, material_constant_transparent_samples, \
	material_constant_index_of_refraction, material_constant_inverttrans, \
	material_constant_usealphatrans, material_constant_scaletrans, material_constant_reflect_inuse, \
	material_constant_reflectivity, material_constant_reflect_glossy, material_constant_reflect_samples, \
	material_constant_invertrefl, material_constant_usealpharefl, \
	material_constant_scalerefl, material_constant_radiance, \
	material_constant_incand_inuse, material_constant_incandescence, material_constant_inc_inten, \
	material_constant_notrace, material_constant_environment

#define DEF_MATERIAL_STRAUSS_PARAMS \
	color material_strauss_ambience; vector material_strauss_bump; \
	float material_strauss_diffuse_inuse; color material_strauss_diffuse; \
	float material_strauss_specular_inuse; float material_strauss_smoothness; float material_strauss_metalness; \
	float material_strauss_refract_inuse; color material_strauss_transparency; \
	float material_strauss_trans_glossy; uniform float material_strauss_transparent_samples; \
	float material_strauss_index_of_refraction; float material_strauss_inverttrans; \
	float material_strauss_usealphatrans; float material_strauss_scaletrans; \
	float material_strauss_reflect_inuse; color material_strauss_reflectivity; \
	float material_strauss_reflect_glossy; uniform float material_strauss_reflect_samples; \
	float material_strauss_invertrefl; float material_strauss_usealpharefl; \
	float material_strauss_scalerefl; color material_strauss_radiance; \
	float material_strauss_translucent_inuse; float material_strauss_translucency; \
	float material_strauss_incand_inuse; color material_strauss_incandescence; \
	float material_strauss_inc_inten; float material_strauss_notrace; \
	color material_strauss_environment;

#define MATERIAL_STRAUSS_PARAMS \
	material_strauss_ambience, material_strauss_bump, material_strauss_diffuse_inuse, \
	material_strauss_diffuse, material_strauss_specular_inuse, \
	material_strauss_smoothness, material_strauss_metalness, material_strauss_refract_inuse, \
	material_strauss_transparency, material_strauss_trans_glossy, material_strauss_transparent_samples, \
	material_strauss_index_of_refraction, material_strauss_inverttrans, \
	material_strauss_usealphatrans, material_strauss_scaletrans, material_strauss_reflect_inuse, \
	material_strauss_reflectivity, material_strauss_reflect_glossy, material_strauss_reflect_samples, \
	material_strauss_invertrefl, material_strauss_usealpharefl, \
	material_strauss_scalerefl, material_strauss_radiance, \
	material_strauss_translucent_inuse, material_strauss_translucency, material_strauss_incand_inuse, \
	material_strauss_incandescence, material_strauss_inc_inten, material_strauss_notrace, \
	material_strauss_environment

#define DEF_SIMPLE_PHONG_PARAMS \
	color simple_phong_ambience; color simple_phong_diffuse; \
	color simple_phong_ambient; color simple_phong_specular; \
	float simple_phong_shiny; color simple_phong_radiance; 

#define SIMPLE_PHONG_PARAMS \
	simple_phong_ambience, simple_phong_diffuse, simple_phong_ambient, \
	simple_phong_specular, simple_phong_shiny, simple_phong_radiance

#define DEF_SIMPLE_LAMBERT_PARAMS \
	color simple_lambert_ambience; color simple_lambert_diffuse; \
	color simple_lambert_ambient; color simple_lambert_radiance; 

#define SIMPLE_LAMBERT_PARAMS \
	simple_lambert_ambience, simple_lambert_diffuse, simple_lambert_ambient, simple_lambert_radiance

#define DEF_SIMPLE_BLINN_PARAMS \
	color simple_blinn_ambience; color simple_blinn_diffuse; \
	color simple_blinn_ambient; color simple_blinn_specular; \
	float simple_blinn_roughness; color simple_blinn_radiance; 

#define SIMPLE_BLINN_PARAMS \
	simple_blinn_ambience, simple_blinn_diffuse, simple_blinn_ambient, \
	simple_blinn_specular, simple_blinn_roughness, simple_blinn_radiance

#define DEF_SIMPLE_COOKTORR_PARAMS \
	color simple_cooktorr_ambience; color simple_cooktorr_diffuse; \
	color simple_cooktorr_ambient; color simple_cooktorr_specular; \
	float simple_cooktorr_roughness; color simple_cooktorr_radiance; 

#define SIMPLE_COOKTORR_PARAMS \
	simple_cooktorr_ambience, simple_cooktorr_diffuse, simple_cooktorr_ambient, \
	simple_cooktorr_specular, simple_cooktorr_roughness, simple_cooktorr_radiance

#define DEF_SIMPLE_WARD_PARAMS \
	color simple_ward_diffuse; color simple_ward_ambient; \
	color simple_ward_glossy; float simple_ward_shiny_u; float simple_ward_shiny_v; \
	float simple_ward_surfderiv_mode; color simple_ward_image; \
	color simple_ward_radiance; 

#define SIMPLE_WARD_PARAMS \
	simple_ward_diffuse, simple_ward_ambient, simple_ward_glossy, \
	simple_ward_shiny_u, simple_ward_shiny_v, \
	simple_ward_surfderiv_mode, simple_ward_image, simple_ward_radiance

#define DEF_SIMPLE_TRANSLUCENT_PARAMS \
	color simple_translucent_diffuse;

#define SIMPLE_TRANSLUCENT_PARAMS \
	simple_translucent_diffuse

#define DEF_XSIAMBIENTOCCLUSION_PARAMS \
	uniform float xsiambientocclusion_samples; color xsiambientocclusion_bright; \
	color xsiambientocclusion_dark; float xsiambientocclusion_spread; \
	uniform float xsiambientocclusion_max_distance; float xsiambientocclusion_reflective; \
	float xsiambientocclusion_output_mode;

#define XSIAMBIENTOCCLUSION_PARAMS \
	xsiambientocclusion_samples, xsiambientocclusion_bright, xsiambientocclusion_dark, \
	xsiambientocclusion_spread, xsiambientocclusion_max_distance, \
	xsiambientocclusion_reflective, xsiambientocclusion_output_mode

#define DEF_SIB_INCIDENCE_PARAMS \
	float sib_incidence_mode; float sib_incidence_exponent; vector sib_incidence_custom_vector;

#define SIB_INCIDENCE_PARAMS \
	sib_incidence_mode, sib_incidence_exponent, sib_incidence_custom_vector

#define DEF_SIB_ILLUM_SHADOWMATERIAL_PARAMS \
	float sib_illum_shadowmaterial_min; float sib_illum_shadowmaterial_max; float sib_illum_shadowmaterial_rgb; \
	float sib_illum_shadowmaterial_shadowvisible; 

#define SIB_ILLUM_SHADOWMATERIAL_PARAMS \
	sib_illum_shadowmaterial_min, sib_illum_shadowmaterial_max, sib_illum_shadowmaterial_rgb, \
	sib_illum_shadowmaterial_shadowvisible 

#define DEF_SIB_INCIDENCE_V2_PARAMS \
	float sib_incidence_v2_mode; vector sib_incidence_v2_custom_vector; float sib_incidence_v2_invert; \
	float sib_incidence_v2_bias; float sib_incidence_v2_gain;

#define SIB_INCIDENCE_V2_PARAMS \
	sib_incidence_v2_mode, sib_incidence_v2_custom_vector, sib_incidence_v2_invert, \
	sib_incidence_v2_bias, sib_incidence_v2_gain 

#define DEF_MISSS_FAST_SIMPLE_PHEN_PARAMS \
	float misss_fast_simple_phen_samples; float misss_fast_simple_phen_exponent; float misss_fast_simple_phen_scatter_bias; \
	float misss_fast_simple_phen_falloff; color misss_fast_simple_phen_overall_color; \
	color misss_fast_simple_phen_ambient; color misss_fast_simple_phen_specular; \
	color misss_fast_simple_phen_diffuse_color; float misss_fast_simple_phen_diffuse_weight; \
	color misss_fast_simple_phen_front_sss_color; float misss_fast_simple_phen_front_sss_weight;

#define MISSS_FAST_SIMPLE_PHEN_PARAMS \
	misss_fast_simple_phen_samples, misss_fast_simple_phen_exponent, misss_fast_simple_phen_scatter_bias, \
	misss_fast_simple_phen_falloff, misss_fast_simple_phen_overall_color, misss_fast_simple_phen_ambient, \
	misss_fast_simple_phen_specular, misss_fast_simple_phen_diffuse_color, misss_fast_simple_phen_diffuse_weight, \
	misss_fast_simple_phen_front_sss_color, misss_fast_simple_phen_front_sss_weight


color LocIllumWardAnisotropic (normal N;  vector V; vector xdir;  float xroughness, yroughness;)
{
    float sqr (float x) { return x*x; }

    float cos_theta_r = clamp (N.V, 0.0001, 1);
    vector X = xdir / xroughness;
    vector Y = (N ^ xdir) / yroughness;

    color C = 0;
    extern point P;
    illuminance (P, N, PI/2) {
        /* Must declare because extern L & Cl because we're in a function */
        extern vector L;  extern color Cl;
        float nonspec = 0;
        lightsource ("__nonspecular", nonspec);
        if (nonspec < 1) {
            vector LN = normalize (L);
            float cos_theta_i = LN . N;
            if (cos_theta_i > 0.0) {
                vector H = normalize (V + LN);
                float rho = exp (-2 * (sqr(X.H) + sqr(Y.H)) / (1 + H.N))
                    / sqrt (cos_theta_i * cos_theta_r);
                C += Cl * ((1-nonspec) * cos_theta_i * rho);
            }
        }
    }
    return C / (4 * xroughness * yroughness);
}

color fnc_specular(normal Nn; vector V; float roughness)
{
    extern point P;
    color C = 0;
    illuminance(P, Nn, PI/2) {
        float nonspec = 0;
        lightsource("__nonspecular", nonspec);
        vector H = normalize(normalize(L) + V);
        C += Cl * (1 - nonspec) * pow(max(0, Nn.H), 1/roughness);
    }
    return C;
}

color material_phong (DEF_MATERIAL_PHONG_PARAMS DEF2_GLOBAL_PARAMS DEF2_OUTPUT_PARAMS)
{
	color outputColor, outputOpacity;
	extern color Ci;	
	extern color Oi;
	extern color Cs;
	extern color Os;
	extern point P;	
	extern normal Ng;
	extern normal N;
	extern vector I;
	vector refrDir, reflDir;
	float kr, kt;
	normal Nf;
	normal Nn = normalize(N);
	vector In = normalize(I);

	// bump map
	float bumpvalue = length(material_phong_bump);
	if(bumpvalue != 0) {       
	 	point PP = transform("shader", P);
       	normal Ns = normalize(ntransform("shader", N));
        	PP += bumpvalue * Ns;
        	PP = transform("shader", "current", PP);
        	Nf = calculatenormal(PP);
            
		normal Nd = normalize(N) - normalize(Ng);
            Nf = normalize(Nf) + Nd;
	}
	else {
		Nf = N;
	}

	Nf = faceforward(normalize(Nf), I);

	vector V = normalize(-I);
	float roughness = material_phong_shiny;
	float eta = (In.Nn < 0) ? 1/material_phong_index_of_refraction : material_phong_index_of_refraction;
	fresnel(In, Nf, eta, kr, kt, reflDir, refrDir);
    	kt = 1 - kr;

	if (material_phong_diffuse_inuse != 0) diffusecolor = material_phong_diffuse;
	else diffusecolor = 0;

	if (material_phong_diffuse_inuse != 0) ambientcolor = material_phong_ambient;
	else ambientcolor = 0;

	if (material_phong_specular_inuse != 0) specularcolor = material_phong_specular;
	else specularcolor = 0;

	if (material_phong_refract_inuse != 0) transparencycolor = material_phong_transparency;
	else transparencycolor = 0;

	if (material_phong_inverttrans != 0) {
		transparencycolor = 1 - transparencycolor;
	}

	radiancecolor = material_phong_radiance;

	uniform float reflsamples = material_phong_reflect_samples;
	if (material_phong_reflect_samples == 0) reflsamples = 1;
	if (notrace == 0 && material_phong_notrace == 0 && material_phong_reflect_inuse != 0) {
#ifdef AIR
		reflcolor = trace(P, reflDir, "blur", material_phong_reflect_glossy, "samples", reflsamples);
#else
            color hitc = 0;
            gather("illuminance", P, reflDir, material_phong_reflect_glossy, reflsamples, "surface:Ci", hitc) {
			reflcolor += hitc;	
            }
            reflcolor /= material_phong_reflect_samples;
#endif
	}
	else if (material_phong_notrace != 0 && material_phong_reflect_inuse != 0) {
		reflcolor = material_phong_environment;
	}

	if (notrace == 0 && material_phong_refract_inuse != 0) {
#ifdef AIR
		refrcolor = trace(P, refrDir, "blur", material_phong_trans_glossy, "samples", material_phong_transparent_samples);
#else	
		color hitc = 0;
		gather("illuminance", P, refrDir, material_phong_trans_glossy, material_phong_transparent_samples, "surface:Ci", hitc) {
			if (kt > 0) {
				refrcolor += kt * hitc;
			}
		}
		refrcolor /= material_phong_transparent_samples;
#endif
	}

	if (material_phong_transparent_samples == 0) {
		outputOpacity = (1 - material_phong_scaletrans * transparencycolor) * Os;
	}
	else {
		outputOpacity = 1;
	}

	color occ = 0;
	if(notrace == 0 && doOcclusion != 0) {
#ifdef AIR
   		varying normal Nocc;
		occlusioncolor = 1 - occlusion(P, Nf, PI/2, Nocc, "samples", occlusion_samples, "maxdist", maxdist);
#else
      	occlusioncolor = 1 - occlusion(P, Nf, occlusion_samples, "maxdist", maxdist);
#endif
	}
	occ = occlusioncolor * radiancecolor * diffusecolor;

	if(notrace == 0 && doIndirect != 0) {
		indirectcolor = indirectdiffuse(P, Nf, indirect_samples) * radiancecolor;
	}


  	/* finally */
	diffusecolor *= diffuse(Nf);
	ambientcolor *= ambient() + material_phong_ambience;
	specularcolor *= phong(Nf, V, roughness);

	color translucentcolor;
	if (material_phong_translucent_inuse != 0) translucentcolor = material_phong_translucency;
	else translucentcolor = 0;

	color incandescentcolor;
	if (material_phong_incand_inuse != 0) incandescentcolor = material_phong_incandescence;
	else incandescentcolor = 0;
    	
	outputColor = (1 - material_phong_scaletrans * transparencycolor) * Cs * (ambientcolor + occ + diffusecolor + 
		indirectcolor + translucentcolor * material_phong_diffuse * diffuse(-Nf)) + specularcolor +
		material_phong_inc_inten * incandescentcolor; 

	outputColor = mix(outputColor, reflcolor, material_phong_scalerefl * material_phong_reflectivity);
	outputColor = mix(outputColor, refrcolor, material_phong_scaletrans * transparencycolor);
 
#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	outputColor += causticcolor;
#endif

	Ci = outputColor;
	Oi = outputOpacity;
	
	return Ci;
}

color material_blinn (DEF_MATERIAL_BLINN_PARAMS DEF2_GLOBAL_PARAMS DEF2_OUTPUT_PARAMS)
{
	color outputColor, outputOpacity;
	extern color Ci;	
	extern color Oi;
	extern color Cs;
	extern color Os;
	extern point P;	
	extern normal Ng;
	extern normal N;
	extern vector I;
	vector refrDir, reflDir;
	float kr, kt;
	normal Nf;
	normal Nn = normalize(N);
	vector In = normalize(I);
	Nf = faceforward(normalize(N), I);

	vector V = normalize(-I);
	float eta = (In.Nn < 0) ? 1/material_blinn_index_of_refraction : material_blinn_index_of_refraction;
	fresnel(In, Nf, eta, kr, kt, reflDir, refrDir);
    	kt = 1 - kr;

	if (material_blinn_diffuse_inuse != 0) diffusecolor = material_blinn_diffuse;
	else diffusecolor = 0;

	if (material_blinn_diffuse_inuse != 0) ambientcolor = material_blinn_ambient;
	else ambientcolor = 0;

	if (material_blinn_specular_inuse != 0) specularcolor = material_blinn_specular;
	else specularcolor = 0;

	if (material_blinn_refract_inuse != 0) transparencycolor = material_blinn_transparency;
	else transparencycolor = 0;

	if (material_blinn_inverttrans != 0) {
		transparencycolor = 1 - transparencycolor;
	}

	radiancecolor = material_blinn_radiance;

	uniform float reflsamples = material_blinn_reflect_samples;
	if (material_blinn_reflect_samples == 0) reflsamples = 1;
	if (notrace == 0 && material_blinn_notrace == 0 && material_blinn_reflect_inuse != 0) {
#ifdef AIR
		reflcolor = trace(P, reflDir, "blur", material_blinn_reflect_glossy, "samples", reflsamples);
#else
            color hitc = 0;
            gather("illuminance", P, reflDir, material_blinn_reflect_glossy, reflsamples, "surface:Ci", hitc) {
			reflcolor += hitc;	
            }
            reflcolor /= material_blinn_reflect_samples;
#endif
	}	
	else if (material_blinn_notrace != 0 && material_blinn_reflect_inuse != 0) {
		reflcolor = material_blinn_environment;
	}

	if (notrace == 0 && material_blinn_refract_inuse != 0) {
#ifdef AIR
		refrcolor = trace(P, refrDir, "blur", material_blinn_trans_glossy, "samples", material_blinn_transparent_samples);
#else	
		color hitc = 0;
		gather("illuminance", P, refrDir, material_blinn_trans_glossy, material_blinn_transparent_samples, "surface:Ci", hitc) {
			if (kt > 0) {
				refrcolor += kt * hitc;
			}
		}
		refrcolor /= material_blinn_transparent_samples;
#endif
	}

	if (material_blinn_transparent_samples == 0) {
		outputOpacity = (1 - material_blinn_scaletrans * transparencycolor) * Os;
	}
	else {
		outputOpacity = 1;
	}

	color occ = 0;
	if(notrace == 0 && doOcclusion != 0) {
#ifdef AIR
   		varying normal Nocc;
		occlusioncolor = 1 - occlusion(P, Nf, PI/2, Nocc, "samples", occlusion_samples, "maxdist", maxdist);
#else
      	occlusioncolor = 1 - occlusion(P, Nf, occlusion_samples, "maxdist", maxdist);
#endif
	}
	occ = occlusioncolor * radiancecolor * diffusecolor;

	if(notrace == 0 && doIndirect != 0) {
		indirectcolor = indirectdiffuse(P, Nf, indirect_samples) * radiancecolor;
	}


  	/* finally */
	diffusecolor *= diffuse(Nf);
	ambientcolor *= ambient() + material_blinn_ambience;

	color speccolor = 0;
      speccolor = material_blinn_specular * specular(Nf, V, material_blinn_roughness);
	/* multiply specular by diffuse to give more metallic appearance */
	specularcolor *= speccolor * material_blinn_diffuse;

	color translucentcolor;
	if (material_blinn_translucent_inuse != 0) translucentcolor = material_blinn_translucency;
	else translucentcolor = 0;

	color incandescentcolor;
	if (material_blinn_incand_inuse != 0) incandescentcolor = material_blinn_incandescence;
	else incandescentcolor = 0;

    	outputColor = (1 - material_blinn_scaletrans * transparencycolor) * Cs * (ambientcolor + occ + diffusecolor + 
		indirectcolor + translucentcolor * material_blinn_diffuse * diffuse(-Nf)) + specularcolor + 
		material_blinn_inc_inten * incandescentcolor; 

	outputColor = mix(outputColor, reflcolor, material_blinn_scalerefl * material_blinn_reflectivity);
	outputColor = mix(outputColor, refrcolor, material_blinn_scaletrans * transparencycolor);
 
#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	outputColor += causticcolor;
#endif

	Ci = outputColor;
	Oi = outputOpacity;

	return Ci;
}

color material_lambert (DEF_MATERIAL_LAMBERT_PARAMS DEF2_GLOBAL_PARAMS DEF2_OUTPUT_PARAMS)
{
	color outputColor, outputOpacity;
	extern color Ci;	
	extern color Oi;
	extern color Cs;
	extern color Os;
	extern point P;	
	extern normal Ng;
	extern normal N;
	extern vector I;
	vector refrDir, reflDir;
	float kr, kt;
	normal Nf;
	normal Nn = normalize(N);
	vector In = normalize(I);

	// bump map
	float bumpvalue = length(material_lambert_bump);
	if(bumpvalue != 0) {       
	 	point PP = transform("shader", P);
       	normal Ns = normalize(ntransform("shader", N));
        	PP += bumpvalue * Ns;
        	PP = transform("shader", "current", PP);
        	Nf = calculatenormal(PP);
            
		normal Nd = normalize(N) - normalize(Ng);
            Nf = normalize(Nf) + Nd;
           // P = PP;
	}
	else {
		Nf = N;
	}

	Nf = faceforward(normalize(Nf), I);

	vector V = normalize(-I);
	float eta = (In.Nn < 0) ? 1/material_lambert_index_of_refraction : material_lambert_index_of_refraction;
	fresnel(In, Nf, eta, kr, kt, reflDir, refrDir);
    	kt = 1 - kr;

	if (material_lambert_diffuse_inuse != 0) diffusecolor = material_lambert_diffuse;
	else diffusecolor = 0;

	if (material_lambert_diffuse_inuse != 0) ambientcolor = material_lambert_ambient;
	else ambientcolor = 0;

	if (material_lambert_refract_inuse != 0) transparencycolor = material_lambert_transparency;
	else transparencycolor = 0;

	if (material_lambert_inverttrans != 0) {
		transparencycolor = 1 - transparencycolor;
	}

	radiancecolor = material_lambert_radiance;

	uniform float reflsamples = material_lambert_reflect_samples;
	if (material_lambert_reflect_samples == 0) reflsamples = 1;
	if (notrace == 0 && material_lambert_notrace == 0 && material_lambert_reflect_inuse != 0) {
#ifdef AIR
		reflcolor = trace(P, reflDir, "blur", material_lambert_reflect_glossy, "samples", reflsamples);
#else
            color hitc = 0;
            gather("illuminance", P, reflDir, material_lambert_reflect_glossy, reflsamples, "surface:Ci", hitc) {
			reflcolor += hitc;	
            }
            reflcolor /= material_lambert_reflect_samples;
#endif
	}	
	else if (material_lambert_notrace != 0 && material_lambert_reflect_inuse != 0) {
		reflcolor = material_lambert_environment;
	}

	if (notrace == 0 && material_lambert_refract_inuse != 0) {	
#ifdef AIR
		refrcolor = trace(P, refrDir, "blur", material_lambert_trans_glossy, "samples", material_lambert_transparent_samples);
#else
		color hitc = 0;
		gather("illuminance", P, refrDir, material_lambert_trans_glossy, material_lambert_transparent_samples, "surface:Ci", hitc) {
			if (kt > 0) {
				refrcolor += kt * hitc;
			}
		}
		refrcolor /= material_lambert_transparent_samples;
#endif
	}

	if (material_lambert_transparent_samples == 0) {
		outputOpacity = (1 - material_lambert_scaletrans * transparencycolor) * Os;
	}
	else {
		outputOpacity = 1;
	}

	color occ = 0;
	if(notrace == 0 && doOcclusion != 0) {
#ifdef AIR
   		varying normal Nocc;
		occlusioncolor = 1 - occlusion(P, Nf, PI/2, Nocc, "samples", occlusion_samples, "maxdist", maxdist);
#else
      	occlusioncolor = 1 - occlusion(P, Nf, occlusion_samples, "maxdist", maxdist);
#endif
	}
	occ = occlusioncolor * radiancecolor * diffusecolor;

	if(notrace == 0 && doIndirect != 0) {
		indirectcolor = indirectdiffuse(P, Nf, indirect_samples) * radiancecolor;
	}


  	/* finally */
	diffusecolor *= diffuse(Nf);
	ambientcolor *= ambient() + material_lambert_ambience;

	color translucentcolor;
	if (material_lambert_translucent_inuse != 0) translucentcolor = material_lambert_translucency;
	else translucentcolor = 0;

	color incandescentcolor;
	if (material_lambert_incand_inuse != 0) incandescentcolor = material_lambert_incandescence;
	else incandescentcolor = 0;

    	outputColor = (1 - material_lambert_scaletrans * transparencycolor) * Cs * (ambientcolor + occ + diffusecolor + 
		indirectcolor + translucentcolor * material_lambert_diffuse * diffuse(-Nf)) +
		material_lambert_inc_inten * incandescentcolor; 

	outputColor = mix(outputColor, reflcolor, material_lambert_scalerefl * material_lambert_reflectivity);
	outputColor = mix(outputColor, refrcolor, material_lambert_scaletrans * transparencycolor);
 
#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	outputColor += causticcolor;
#endif

	Ci = outputColor;
	Oi = outputOpacity;

	return Ci;
}

color material_cooktorr (DEF_MATERIAL_COOKTORR_PARAMS DEF2_GLOBAL_PARAMS DEF2_OUTPUT_PARAMS)
{
	color outputColor, outputOpacity;
	extern color Ci;	
	extern color Oi;
	extern color Cs;
	extern color Os;
	extern point P;	
	extern normal Ng;
	extern normal N;
	extern vector I;
	vector refrDir, reflDir;
	float kr, kt;
	normal Nf;
	normal Nn = normalize(N);
	vector In = normalize(I);
	Nf = faceforward(normalize(N), I);

	vector V = normalize(-I);
	float eta = (In.Nn < 0) ? 1/material_cooktorr_index_of_refraction : material_cooktorr_index_of_refraction;
	fresnel(In, Nf, eta, kr, kt, reflDir, refrDir);
    	kt = 1 - kr;

	if (material_cooktorr_diffuse_inuse != 0) diffusecolor = material_cooktorr_diffuse;
	else diffusecolor = 0;

	if (material_cooktorr_diffuse_inuse != 0) ambientcolor = material_cooktorr_ambient;
	else ambientcolor = 0;

	if (material_cooktorr_specular_inuse != 0) specularcolor = material_cooktorr_specular;
	else specularcolor = 0;

	if (material_cooktorr_refract_inuse != 0) transparencycolor = material_cooktorr_transparency;
	else transparencycolor = 0;

	if (material_cooktorr_inverttrans != 0) {
		transparencycolor = 1 - transparencycolor;
	}

	radiancecolor = material_cooktorr_radiance;

	uniform float reflsamples = material_cooktorr_reflect_samples;
	if (material_cooktorr_reflect_samples == 0) reflsamples = 1;
	if (notrace == 0 && material_cooktorr_notrace == 0 && material_cooktorr_reflect_inuse != 0) {
#ifdef AIR
		reflcolor = trace(P, reflDir, "blur", material_cooktorr_reflect_glossy, "samples", reflsamples);
#else
            color hitc = 0;
            gather("illuminance", P, reflDir, material_cooktorr_reflect_glossy, reflsamples, "surface:Ci", hitc) {
			reflcolor += hitc;	
            }
            reflcolor /= material_cooktorr_reflect_samples;
#endif
	}	
	else if (material_cooktorr_notrace != 0 && material_cooktorr_reflect_inuse != 0) {
		reflcolor = material_cooktorr_environment;
	}

	if (notrace == 0 && material_cooktorr_refract_inuse != 0) {	
#ifdef AIR
		refrcolor = trace(P, refrDir, "blur", material_cooktorr_trans_glossy, "samples", material_cooktorr_transparent_samples);
#else
		color hitc = 0;
		gather("illuminance", P, refrDir, material_cooktorr_trans_glossy, material_cooktorr_transparent_samples, "surface:Ci", hitc) {
			if (kt > 0) {
				refrcolor += kt * hitc;
			}
		}
		refrcolor /= material_cooktorr_transparent_samples;
#endif
	}

	if (material_cooktorr_transparent_samples == 0) {
		outputOpacity = (1 - material_cooktorr_scaletrans * transparencycolor) * Os;
	}
	else {
		outputOpacity = 1;
	}

	color occ = 0;
	if(notrace == 0 && doOcclusion != 0) {
#ifdef AIR
   		varying normal Nocc;
		occlusioncolor = 1 - occlusion(P, Nf, PI/2, Nocc, "samples", occlusion_samples, "maxdist", maxdist);
#else
      	occlusioncolor = 1 - occlusion(P, Nf, occlusion_samples, "maxdist", maxdist);
#endif
	}
	occ = occlusioncolor * radiancecolor * diffusecolor;

	if(notrace == 0 && doIndirect != 0) {
		indirectcolor = indirectdiffuse(P, Nf, indirect_samples) * radiancecolor;
	}


  	/* finally */
	diffusecolor *= diffuse(Nf);
	ambientcolor *= ambient() + material_cooktorr_ambience;

	color spec = phong(Nf, V, material_cooktorr_roughness);
	specularcolor *= spec;

	color translucentcolor;
	if (material_cooktorr_translucent_inuse != 0) translucentcolor = material_cooktorr_translucency;
	else translucentcolor = 0;

	color incandescentcolor;
	if (material_cooktorr_incand_inuse != 0) incandescentcolor = material_cooktorr_incandescence;
	else incandescentcolor = 0;

    	outputColor = (1 - material_cooktorr_scaletrans * transparencycolor) * Cs * (ambientcolor + occ + diffusecolor + 
		indirectcolor + translucentcolor * material_cooktorr_diffuse * diffuse(-Nf)) + 
		specularcolor * specular(Nf, V, material_cooktorr_roughness) + 
		material_cooktorr_inc_inten * incandescentcolor; 

	outputColor = mix(outputColor, reflcolor, material_cooktorr_scalerefl * material_cooktorr_reflectivity);
	outputColor = mix(outputColor, refrcolor, material_cooktorr_scaletrans * transparencycolor);
 
#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	outputColor += causticcolor;
#endif

	Ci = outputColor;
	Oi = outputOpacity;

	return Ci;
}

color material_ward (DEF_MATERIAL_WARD_PARAMS DEF2_GLOBAL_PARAMS DEF2_OUTPUT_PARAMS)
{
	color outputColor, outputOpacity;
	extern color Ci;	
	extern color Oi;
	extern color Cs;
	extern color Os;
	extern point P;	
	extern normal Ng;
	extern normal N;
	extern vector I;
	extern vector dPdu, dPdv;
	vector refrDir, reflDir;
	float kr, kt;
	normal Nf;
	normal Nn = normalize(N);
	vector In = normalize(I);
	Nf = faceforward(normalize(N), I);

	vector V = normalize(-I);
	float eta = (In.Nn < 0) ? 1/material_ward_index_of_refraction : material_ward_index_of_refraction;
	fresnel(In, Nf, eta, kr, kt, reflDir, refrDir);
    	kt = 1 - kr;

	if (material_ward_diffuse_inuse != 0) diffusecolor = material_ward_diffuse;
	else diffusecolor = 0;

	if (material_ward_diffuse_inuse != 0) ambientcolor = material_ward_ambient;
	else ambientcolor = 0;

	if (material_ward_specular_inuse != 0) specularcolor = material_ward_glossy;
	else specularcolor = 0;

	if (material_ward_refract_inuse != 0) transparencycolor = material_ward_transparency;
	else transparencycolor = 0;

	if (material_ward_inverttrans != 0) {
		transparencycolor = 1 - transparencycolor;
	}

	radiancecolor = material_ward_radiance;

	uniform float reflsamples = material_ward_reflect_samples;
	if (material_ward_reflect_samples == 0) reflsamples = 1;
	if (notrace == 0 && material_ward_notrace == 0 && material_ward_reflect_inuse != 0) {
#ifdef AIR
		reflcolor = trace(P, reflDir, "blur", material_ward_reflect_glossy, "samples", reflsamples);
#else
            color hitc = 0;
            gather("illuminance", P, reflDir, material_ward_reflect_glossy, reflsamples, "surface:Ci", hitc) {
			reflcolor += hitc;	
            }
            reflcolor /= material_ward_reflect_samples;
#endif
	}	
	else if (material_ward_notrace != 0 && material_ward_reflect_inuse != 0) {
		reflcolor = material_ward_environment;
	}

	if (notrace == 0 && material_ward_refract_inuse != 0) {	
#ifdef AIR
		refrcolor = trace(P, refrDir, "blur", material_ward_trans_glossy, "samples", material_ward_transparent_samples);
#else
		color hitc = 0;
		gather("illuminance", P, refrDir, material_ward_trans_glossy, material_ward_transparent_samples, "surface:Ci", hitc) {
			if (kt > 0) {
				refrcolor += kt * hitc;
			}
		}
		refrcolor /= material_ward_transparent_samples;
#endif
	}

	if (material_ward_transparent_samples == 0) {
		outputOpacity = (1 - material_ward_scaletrans * transparencycolor) * Os;
	}
	else {
		outputOpacity = 1;
	}

	color occ = 0;
	if(notrace == 0 && doOcclusion != 0) {
#ifdef AIR
   		varying normal Nocc;
		occlusioncolor = 1 - occlusion(P, Nf, PI/2, Nocc, "samples", occlusion_samples, "maxdist", maxdist);
#else
      	occlusioncolor = 1 - occlusion(P, Nf, occlusion_samples, "maxdist", maxdist);
#endif
	}
	occ = occlusioncolor * radiancecolor * diffusecolor;

	if(notrace == 0 && doIndirect != 0) {
		indirectcolor = indirectdiffuse(P, Nf, indirect_samples) * radiancecolor;
	}


  	/* finally */
	diffusecolor *= diffuse(Nf);
	ambientcolor *= ambient();

	float imagedir = 2 * PI * (comp(material_ward_image,0) + comp(material_ward_image,1) + comp(material_ward_image,2)) / 3;
	vector xdir; 
	if (material_ward_surfderiv_mode == -5) {
		xdir = cos(imagedir) * normalize(dPdu) + sin(imagedir) * normalize(dPdv);
	}
	else {	
		xdir = -sin(imagedir) * normalize(dPdu) + cos(imagedir) * normalize(dPdv);
	}

	float roughness_u = 1 / material_ward_shiny_u;
	float roughness_v = 1 / material_ward_shiny_v;
	color spec = LocIllumWardAnisotropic(Nf, V, xdir, roughness_u, roughness_v);
	specularcolor *= spec;

	color translucentcolor;
	if (material_ward_translucent_inuse != 0) translucentcolor = material_ward_translucency;
	else translucentcolor = 0;

	color incandescentcolor;
	if (material_ward_incand_inuse != 0) incandescentcolor = material_ward_incandescence;
	else incandescentcolor = 0;

    	outputColor = (1 - material_ward_scaletrans * transparencycolor) * Cs * (ambientcolor + occ + diffusecolor + 
		indirectcolor + translucentcolor * material_ward_diffuse * diffuse(-Nf)) + specularcolor +
		material_ward_inc_inten * incandescentcolor; 

	outputColor = mix(outputColor, reflcolor, material_ward_scalerefl * material_ward_reflectivity);
	outputColor = mix(outputColor, refrcolor, material_ward_scaletrans * transparencycolor);
 
#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	outputColor += causticcolor;
#endif

	Ci = outputColor;
	Oi = outputOpacity;

	return Ci;
}

color material_constant (DEF_MATERIAL_CONSTANT_PARAMS DEF2_GLOBAL_PARAMS DEF2_OUTPUT_PARAMS)
{
	color outputColor, outputOpacity;
	extern color Ci;	
	extern color Oi;
	extern color Cs;
	extern color Os;
	extern point P;	
	extern normal Ng;
	extern normal N;
	extern vector I;
	vector refrDir, reflDir;
	float kr, kt;
	normal Nf;
	normal Nn = normalize(N);
	vector In = normalize(I);
	Nf = faceforward(normalize(N), I);

	vector V = normalize(-I);
	float eta = (In.Nn < 0) ? 1/material_constant_index_of_refraction : material_constant_index_of_refraction;
	fresnel(In, Nf, eta, kr, kt, reflDir, refrDir);
    	kt = 1 - kr;

	if (material_constant_refract_inuse != 0) transparencycolor = material_constant_transparency;
	else transparencycolor = 0;

	if (material_constant_inverttrans != 0) {
		transparencycolor = 1 - transparencycolor;
	}

	radiancecolor = material_constant_radiance;

	uniform float reflsamples = material_constant_reflect_samples;
	if (material_constant_reflect_samples == 0) reflsamples = 1;
	if (notrace == 0 && material_constant_notrace == 0 && material_constant_reflect_inuse != 0) {
#ifdef AIR
		reflcolor = trace(P, reflDir, "blur", material_constant_reflect_glossy, "samples", reflsamples);
#else
            color hitc = 0;
            gather("illuminance", P, reflDir, material_constant_reflect_glossy, reflsamples, "surface:Ci", hitc) {
			reflcolor += hitc;	
            }
            reflcolor /= material_constant_reflect_samples;
#endif
	}	
	else if (material_constant_notrace != 0 && material_constant_reflect_inuse != 0) {
		reflcolor = material_constant_environment;
	}

	if (notrace == 0 && material_constant_refract_inuse != 0) {	
#ifdef AIR
		refrcolor = trace(P, refrDir, "blur", material_constant_trans_glossy, "samples", material_constant_transparent_samples);
#else
		color hitc = 0;
		gather("illuminance", P, refrDir, material_constant_trans_glossy, material_constant_transparent_samples, "surface:Ci", hitc) {
			if (kt > 0) {
				refrcolor += kt * hitc;
			}
		}
		refrcolor /= material_constant_transparent_samples;
#endif
	}

	if (material_constant_transparent_samples == 0) {
		outputOpacity = (1 - material_constant_scaletrans * transparencycolor) * Os;
	}
	else {
		outputOpacity = 1;
	}

	color occ = 0;
	if(notrace == 0 && doOcclusion != 0) {
#ifdef AIR
   		varying normal Nocc;
		occlusioncolor = 1 - occlusion(P, Nf, PI/2, Nocc, "samples", occlusion_samples, "maxdist", maxdist);
#else
      	occlusioncolor = 1 - occlusion(P, Nf, occlusion_samples, "maxdist", maxdist);
#endif
	}
	occ = occlusioncolor * radiancecolor * diffusecolor;

	if(notrace == 0 && doIndirect != 0) {
		indirectcolor = indirectdiffuse(P, Nf, indirect_samples) * radiancecolor;
	}


  	/* finally */
	diffusecolor = material_constant_color;

	color incandescentcolor;
	if (material_constant_incand_inuse != 0) incandescentcolor = material_constant_incandescence;
	else incandescentcolor = 0;

    	outputColor = (1 - material_constant_scaletrans * transparencycolor) * Cs * (occ + diffusecolor + 
		indirectcolor) + material_constant_inc_inten * incandescentcolor; 

	outputColor = mix(outputColor, reflcolor, material_constant_scalerefl * material_constant_reflectivity);
	outputColor = mix(outputColor, refrcolor, material_constant_scaletrans * transparencycolor);
 
#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	outputColor += causticcolor;
#endif

	Ci = outputColor;
	Oi = outputOpacity;

	return Ci;
}

color material_strauss (DEF_MATERIAL_STRAUSS_PARAMS DEF2_GLOBAL_PARAMS DEF2_OUTPUT_PARAMS)
{
	color outputColor, outputOpacity;
	extern color Ci;	
	extern color Oi;
	extern color Cs;
	extern color Os;
	extern point P;	
	extern normal Ng;
	extern normal N;
	extern vector I;
	vector refrDir, reflDir;
	float kr, kt;
	normal Nf;
	normal Nn = normalize(N);
	vector In = normalize(I);

	// bump map
	float bumpvalue = length(material_strauss_bump);
	if(bumpvalue != 0) {       
	 	point PP = transform("shader", P);
       	normal Ns = normalize(ntransform("shader", N));
        	PP += bumpvalue * Ns;
        	PP = transform("shader", "current", PP);
        	Nf = calculatenormal(PP);
            
		normal Nd = normalize(N) - normalize(Ng);
            Nf = normalize(Nf) + Nd;
           // P = PP;
	}
	else {
		Nf = N;
	}

	Nf = faceforward(normalize(Nf), I);

	vector V = normalize(-I);	
	float eta = (In.Nn < 0) ? 1/material_strauss_index_of_refraction : material_strauss_index_of_refraction;
	fresnel(In, Nf, eta, kr, kt, reflDir, refrDir);
    	kt = 1 - kr;

	if (material_strauss_diffuse_inuse != 0) diffusecolor = material_strauss_diffuse;
	else diffusecolor = 0;

	if (material_strauss_refract_inuse != 0) transparencycolor = material_strauss_transparency;
	else transparencycolor = 0;

	if (material_strauss_inverttrans != 0) {
		transparencycolor = 1 - transparencycolor;
	}

	radiancecolor = material_strauss_radiance;

	uniform float reflsamples = material_strauss_reflect_samples;
	if (material_strauss_reflect_samples == 0) reflsamples = 1;
	if (notrace == 0 && material_strauss_notrace == 0 && material_strauss_reflect_inuse != 0) {
#ifdef AIR
		reflcolor = trace(P, reflDir, "blur", material_strauss_reflect_glossy, "samples", reflsamples);
#else
            color hitc = 0;
            gather("illuminance", P, reflDir, material_strauss_reflect_glossy, reflsamples, "surface:Ci", hitc) {
			reflcolor += hitc;	
            }
            reflcolor /= material_strauss_reflect_samples;
#endif
	}	
	else if (material_strauss_notrace != 0 && material_strauss_reflect_inuse != 0) {
		reflcolor = material_strauss_environment;
	}

	if (notrace == 0 && material_strauss_refract_inuse != 0) {	
#ifdef AIR
		refrcolor = trace(P, refrDir, "blur", material_strauss_trans_glossy, "samples", material_strauss_transparent_samples);
#else
		color hitc = 0;
		gather("illuminance", P, refrDir, material_strauss_trans_glossy, material_strauss_transparent_samples, "surface:Ci", hitc) {
			if (kt > 0) {
				refrcolor += kt * hitc;
			}
		}
		refrcolor /= material_strauss_transparent_samples;
#endif
	}

	if (material_strauss_transparent_samples == 0) {
		outputOpacity = (1 - material_strauss_scaletrans * transparencycolor) * Os;
	}
	else {
		outputOpacity = 1;
	}

	color occ = 0;
	if(notrace == 0 && doOcclusion != 0) {
#ifdef AIR
   		varying normal Nocc;
		occlusioncolor = 1 - occlusion(P, Nf, PI/2, Nocc, "samples", occlusion_samples, "maxdist", maxdist);
#else
      	occlusioncolor = 1 - occlusion(P, Nf, occlusion_samples, "maxdist", maxdist);
#endif
	}
	occ = occlusioncolor * radiancecolor * diffusecolor;

	if(notrace == 0 && doIndirect != 0) {
		indirectcolor = indirectdiffuse(P, Nf, indirect_samples) * radiancecolor;
	}

	float diffuseAdj = clamp(1 - material_strauss_smoothness*material_strauss_metalness, 0, 1);
	float diffuseRef = clamp(1 - (pow(material_strauss_smoothness,3)), 0, 1);

  	/* finally */
	ambientcolor = diffusecolor * (ambient() + material_strauss_ambience)*clamp(1 - material_strauss_smoothness, 0, 1);
	diffusecolor = diffusecolor * diffuse(Nf) * diffuseAdj * diffuseRef;
	
	if (material_strauss_specular_inuse != 0) {
		float specularExp = 3. / clamp(1 - material_strauss_smoothness, 0, 1);
		color diffuseTrm = diffuse(Nf) * diffuseAdj * diffuseRef;
		color specularAdj = 1 - diffuseTrm;		
		color speccolor = mix(color(1), material_strauss_diffuse, clamp(material_strauss_metalness,0,1));
		specularcolor = specularAdj * phong(Nf, V, specularExp) * speccolor;
	}
	else {
		specularcolor = 0;
	}

	color translucentcolor;
	if (material_strauss_translucent_inuse != 0) translucentcolor = material_strauss_translucency;
	else translucentcolor = 0;

	color incandescentcolor;
	if (material_strauss_incand_inuse != 0) incandescentcolor = material_strauss_incandescence;
	else incandescentcolor = 0;

    	outputColor = (1 - material_strauss_scaletrans * transparencycolor) * Cs * (ambientcolor + occ + diffusecolor + 
		indirectcolor + translucentcolor * material_strauss_diffuse * diffuse(-Nf)) + specularcolor + 
		material_strauss_inc_inten * incandescentcolor; 

	outputColor = mix(outputColor, reflcolor, material_strauss_scalerefl * material_strauss_reflectivity);
	outputColor = mix(outputColor, refrcolor, material_strauss_scaletrans * transparencycolor);
 
#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	outputColor += causticcolor;
#endif

	Ci = outputColor;
	Oi = outputOpacity;

	return Ci;
}

color simple_phong (DEF_SIMPLE_PHONG_PARAMS DEF2_GLOBAL_PARAMS DEF2_OUTPUT_PARAMS)
{
	color outputColor, outputOpacity;
	extern color Ci;	
	extern color Oi;
	extern color Cs;
	extern color Os;
	extern point P;
	extern normal N;
	extern vector I;
	normal Nf;
	Nf = faceforward(normalize(N), I);

	vector V = normalize(-I);
	float roughness = simple_phong_shiny;

	diffusecolor = simple_phong_diffuse;

	ambientcolor = simple_phong_ambient;

	specularcolor = simple_phong_specular;

	radiancecolor = simple_phong_radiance;

	color occ = 0;
	if(notrace == 0 && doOcclusion != 0) {
#ifdef AIR
   		varying normal Nocc;
		occlusioncolor = 1 - occlusion(P, Nf, PI/2, Nocc, "samples", occlusion_samples, "maxdist", maxdist);
#else
      	occlusioncolor = 1 - occlusion(P, Nf, occlusion_samples, "maxdist", maxdist);
#endif
	}
	occ = occlusioncolor * radiancecolor * diffusecolor;

	if(notrace == 0 && doIndirect != 0) {
		indirectcolor = indirectdiffuse(P, Nf, indirect_samples) * radiancecolor;
	}

  	/* finally */
	diffusecolor *= diffuse(Nf);	
	ambientcolor *= ambient() + simple_phong_ambience;
	specularcolor *= phong(Nf, V, roughness);

    	outputColor = Cs * (ambientcolor + occ + diffusecolor + indirectcolor) + specularcolor; 
 
#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	outputColor += causticcolor;
#endif

	Ci = outputColor;
	Oi = Os;

	return Ci;
}


color simple_lambert (DEF_SIMPLE_LAMBERT_PARAMS DEF2_GLOBAL_PARAMS DEF2_OUTPUT_PARAMS)
{
	color outputColor, outputOpacity;
	extern color Ci;	
	extern color Oi;
	extern color Cs;
	extern color Os;
	extern point P;
	extern normal N;
	extern vector I;
	normal Nf;
	Nf = faceforward(normalize(N), I);

	diffusecolor = simple_lambert_diffuse;

	ambientcolor = simple_lambert_ambient;

	radiancecolor = simple_lambert_radiance;

	color occ = 0;
	if(notrace == 0 && doOcclusion != 0) {
#ifdef AIR
   		varying normal Nocc;
		occlusioncolor = 1 - occlusion(P, Nf, PI/2, Nocc, "samples", occlusion_samples, "maxdist", maxdist);
#else
      	occlusioncolor = 1 - occlusion(P, Nf, occlusion_samples, "maxdist", maxdist);
#endif
	}
	occ = occlusioncolor * radiancecolor * diffusecolor;

	if(notrace == 0 && doIndirect != 0) {
		indirectcolor = indirectdiffuse(P, Nf, indirect_samples) * radiancecolor;
	}

  	/* finally */
	diffusecolor *= diffuse(Nf);	
	ambientcolor *= ambient() + simple_lambert_ambience;

    	outputColor = Cs * (ambientcolor + occ + diffusecolor + indirectcolor); 
 
#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	outputColor += causticcolor;
#endif

	Ci = outputColor;
	Oi = Os;

	return Ci;
}

color simple_blinn (DEF_SIMPLE_BLINN_PARAMS DEF2_GLOBAL_PARAMS DEF2_OUTPUT_PARAMS)
{
	color outputColor, outputOpacity;
	extern color Ci;	
	extern color Oi;
	extern color Cs;
	extern color Os;
	extern point P;
	extern normal N;
	extern vector I;
	normal Nf;
	Nf = faceforward(normalize(N), I);

	vector V = normalize(-I);

	diffusecolor = simple_blinn_diffuse;

	ambientcolor = simple_blinn_ambient;

	specularcolor = simple_blinn_specular;

	radiancecolor = simple_blinn_radiance;

	color occ = 0;
	if(notrace == 0 && doOcclusion != 0) {
#ifdef AIR
   		varying normal Nocc;
		occlusioncolor = 1 - occlusion(P, Nf, PI/2, Nocc, "samples", occlusion_samples, "maxdist", maxdist);
#else
      	occlusioncolor = 1 - occlusion(P, Nf, occlusion_samples, "maxdist", maxdist);
#endif
	}
	occ = occlusioncolor * radiancecolor * diffusecolor;

	if(notrace == 0 && doIndirect != 0) {
		indirectcolor = indirectdiffuse(P, Nf, indirect_samples) * radiancecolor;
	}

  	/* finally */
	diffusecolor *= diffuse(Nf);	
	ambientcolor *= ambient() + simple_blinn_ambience;
		
	color speccolor = 0;
      speccolor = simple_blinn_specular * specular(Nf, V, simple_blinn_roughness);
	/* multiply specular by diffuse to give more metallic appearance */
	specularcolor *= speccolor * simple_blinn_diffuse;

    	outputColor = Cs * (ambientcolor + occ + diffusecolor + indirectcolor) + specularcolor; 
 
#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	outputColor += causticcolor;
#endif

	Ci = outputColor;
	Oi = Os;

	return Ci;
}

color simple_cooktorr (DEF_SIMPLE_COOKTORR_PARAMS DEF2_GLOBAL_PARAMS DEF2_OUTPUT_PARAMS)
{
	color outputColor, outputOpacity;
	extern color Ci;	
	extern color Oi;
	extern color Cs;
	extern color Os;
	extern point P;
	extern normal N;
	extern vector I;
	normal Nf;
	Nf = faceforward(normalize(N), I);

	vector V = normalize(-I);

	diffusecolor = simple_cooktorr_diffuse;

	ambientcolor = simple_cooktorr_ambient;

	specularcolor = simple_cooktorr_specular;

	radiancecolor = simple_cooktorr_radiance;

	color occ = 0;
	if(notrace == 0 && doOcclusion != 0) {
#ifdef AIR
   		varying normal Nocc;
		occlusioncolor = 1 - occlusion(P, Nf, PI/2, Nocc, "samples", occlusion_samples, "maxdist", maxdist);
#else
      	occlusioncolor = 1 - occlusion(P, Nf, occlusion_samples, "maxdist", maxdist);
#endif
	}
	occ = occlusioncolor * radiancecolor * diffusecolor;

	if(notrace == 0 && doIndirect != 0) {
		indirectcolor = indirectdiffuse(P, Nf, indirect_samples) * radiancecolor;
	}

  	/* finally */
	diffusecolor *= diffuse(Nf);	
	ambientcolor *= ambient() + simple_cooktorr_ambience;
      specularcolor *= phong(Nf, V, simple_cooktorr_roughness);

    	outputColor = Cs * (ambientcolor + occ + diffusecolor + indirectcolor) + 
		specularcolor* specular(Nf, V, simple_cooktorr_roughness); 
 
#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	outputColor += causticcolor;
#endif

	Ci = outputColor;
	Oi = Os;

	return Ci;
}

color simple_ward (DEF_SIMPLE_WARD_PARAMS DEF2_GLOBAL_PARAMS DEF2_OUTPUT_PARAMS)
{
	color outputColor, outputOpacity;
	extern color Ci;	
	extern color Oi;
	extern color Cs;
	extern color Os;
	extern point P;
	extern normal N;
	extern vector I;
	extern vector dPdu, dPdv;
	normal Nf;
	Nf = faceforward(normalize(N), I);

	vector V = normalize(-I);

	diffusecolor = simple_ward_diffuse;

	ambientcolor = simple_ward_ambient;

	specularcolor = simple_ward_glossy;

	radiancecolor = simple_ward_radiance;

	color occ = 0;
	if(notrace == 0 && doOcclusion != 0) {
#ifdef AIR
   		varying normal Nocc;
		occlusioncolor = 1 - occlusion(P, Nf, PI/2, Nocc, "samples", occlusion_samples, "maxdist", maxdist);
#else
      	occlusioncolor = 1 - occlusion(P, Nf, occlusion_samples, "maxdist", maxdist);
#endif
	}
	occ = occlusioncolor * radiancecolor * diffusecolor;

	if(notrace == 0 && doIndirect != 0) {
		indirectcolor = indirectdiffuse(P, Nf, indirect_samples) * radiancecolor;
	}


  	/* finally */
	diffusecolor *= diffuse(Nf);
	ambientcolor *= ambient();

	float imagedir = 2 * PI * (comp(simple_ward_image,0) + comp(simple_ward_image,1) + comp(simple_ward_image,2)) / 3;
	vector xdir; 
	if (simple_ward_surfderiv_mode == -5) {
		xdir = cos(imagedir) * normalize(dPdu) + sin(imagedir) * normalize(dPdv);
	}
	else {	
		xdir = -sin(imagedir) * normalize(dPdu) + cos(imagedir) * normalize(dPdv);
	}

	float roughness_u = 1 / simple_ward_shiny_u;
	float roughness_v = 1 / simple_ward_shiny_v;
	color spec = LocIllumWardAnisotropic(Nf, V, xdir, roughness_u, roughness_v);
	specularcolor *= spec;

    	outputColor = Cs * (ambientcolor + occ + diffusecolor + indirectcolor) + specularcolor; 
 
#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	outputColor += causticcolor;
#endif

	Ci = outputColor;
	Oi = Os;

	return Ci;
}

color simple_translucent (DEF_SIMPLE_TRANSLUCENT_PARAMS DEF2_GLOBAL_PARAMS DEF2_OUTPUT_PARAMS)
{
	color outputColor, outputOpacity;
	extern color Ci;	
	extern color Oi;
	extern color Cs;
	extern color Os;
	extern normal N;
	extern vector I;
	normal Nf;
	Nf = faceforward(normalize(N), I);

	diffusecolor = simple_translucent_diffuse;

  	/* finally */
	diffusecolor *= diffuse(Nf);

    	outputColor = Cs * (ambientcolor + indirectcolor + simple_translucent_diffuse * diffuse(-Nf)); 

	Ci = outputColor;
	Oi = Os;

	return Ci;
}

color xsiambientocclusion (DEF_XSIAMBIENTOCCLUSION_PARAMS DEF2_GLOBAL_PARAMS)
{
	extern normal N;
	extern vector I;
	extern point P;
	extern color Oi, Os;
	normal Ns;
	color Cout = 1;
	uniform float max_dist = 1e30;
	float occ = 0;
	normal Nf = faceforward(normalize(N), I);
	normal Nr = normal normalize(reflect(I, Nf));
	
	if (xsiambientocclusion_max_distance <= 0)
		max_dist = 1e30;
	else max_dist = xsiambientocclusion_max_distance;
	
	if (xsiambientocclusion_reflective == 0) 
		Ns = Nf;
	else
		Ns = Nr;

	if(notrace == 0 && xsiambientocclusion_output_mode < 2) {
		if (xsiambientocclusion_reflective == 0) {
   			varying normal Nocc;
			color occl = occlusion(P, Ns, xsiambientocclusion_spread, Nocc, "samples", 
				xsiambientocclusion_samples, "maxdist", max_dist);
			occ = (comp(occl,0) + comp(occl,1) + comp(occl,2))/3;
			Cout = mix(xsiambientocclusion_bright, xsiambientocclusion_dark, occ);
		}
		else {
			uniform float samples = xsiambientocclusion_samples;
        		float hits = 0;
      		gather("illuminance", P, Ns, xsiambientocclusion_spread*PI/2, samples, "maxdist", max_dist) {
         			hits += 1;
        		}
			occ = hits/samples;
			Cout = mix(xsiambientocclusion_bright, xsiambientocclusion_dark, occ);
		}
	}

    	if(notrace == 0 && xsiambientocclusion_output_mode >= 2) {
		vector bentNormal = 0;
        	vector dir = 0;
        	float hits = 0;

		uniform float samples = xsiambientocclusion_samples;
      	gather("illuminance", P, Ns, xsiambientocclusion_spread*PI/2, samples, "maxdist", max_dist, "ray:direction", dir) {
         		hits += 1;
        	} else { /* ray miss */
          		bentNormal += dir;
        	}
        	
		bentNormal = normalize(bentNormal);
		setcomp(Cout, 0, (xcomp(bentNormal) + 1)/2);
        	setcomp(Cout, 1, (ycomp(bentNormal) + 1)/2);
        	setcomp(Cout, 2, 1-(zcomp(bentNormal) + 1)/2);
    	}
		
	Oi = Os;

	return Cout;
}

float sib_incidence (DEF_SIB_INCIDENCE_PARAMS DEF2_GLOBAL_PARAMS)
{
	extern normal N;
	extern vector I;
	float Fout = 0;
	vector cusvector;
	normal Nf = faceforward(normalize(N), I);

	if (sib_incidence_mode == 0) {
		cusvector = vector (1,0,0);
	}
	else if (sib_incidence_mode == 1) {
		cusvector = vector (-1,0,0);
	}
	else if (sib_incidence_mode == 2) {
		cusvector = vector (0,1,0);
	}
	else if (sib_incidence_mode == 3) {
		cusvector = vector (0,-1,0);
	}
	else if (sib_incidence_mode == 4) {
		cusvector = vector (0,0,1);
	}
	else if (sib_incidence_mode == 5) {
		cusvector = vector (0,0,-1);
	}
	else if (sib_incidence_mode == 6) {
		cusvector = -normalize(I);
	}
	else if (sib_incidence_mode == 7) {
		cusvector = sib_incidence_custom_vector;
	}
	
	Fout = pow(Nf.cusvector, sib_incidence_exponent);	

	return Fout;
}

color sib_illum_shadowmaterial (DEF_SIB_ILLUM_SHADOWMATERIAL_PARAMS DEF2_GLOBAL_PARAMS)
{
	extern point P; extern normal N; extern color Oi; extern vector I;
	varying color shadatten = 0;
	color shadowing = 0;

	if (sib_illum_shadowmaterial_shadowvisible == -1) {
    		illuminance ("__shadowatten", P, N, PI) {
			lightsource("__shadowatten", shadatten);	
			shadowing += shadatten;
		}
	}
	else {    		
		illuminance ("__shadowatten", P, N, PI/2) {
			lightsource("__shadowatten", shadatten);
			shadowing += shadatten;
		}
	}
    	illuminance ("-__shadowatten", P, N, PI/2) {
		color shadatten2 = 0;
		lightsource("__shadowatten", shadatten2);
		float mag = (comp(shadowing,0) + comp(shadowing,1) + comp(shadowing,2));
		if (mag > 0) {
			normal Nf = faceforward(normalize(N), I);
			shadowing -= shadatten2 * diffuse(Nf);
		}
	}

	setcomp(shadowing, 0, clamp(comp(shadowing,0), sib_illum_shadowmaterial_min, sib_illum_shadowmaterial_max));
	setcomp(shadowing, 1, clamp(comp(shadowing,1), sib_illum_shadowmaterial_min, sib_illum_shadowmaterial_max));
	setcomp(shadowing, 2, clamp(comp(shadowing,2), sib_illum_shadowmaterial_min, sib_illum_shadowmaterial_max));

	Oi *= shadowing;

	if (sib_illum_shadowmaterial_rgb == 0) {    		
		shadowing = 0;
	}

	return shadowing;
}

float sib_incidence_v2 (DEF_SIB_INCIDENCE_V2_PARAMS DEF2_GLOBAL_PARAMS)
{
	extern normal N;
	extern vector I;
	float Fout = 0;
	vector cusvector;
	normal Nf = faceforward(normalize(N), I);

	if (sib_incidence_v2_mode == 0) {
		cusvector = vector (1,0,0);
	}
	else if (sib_incidence_v2_mode == 1) {
		cusvector = vector (-1,0,0);
	}
	else if (sib_incidence_v2_mode == 2) {
		cusvector = vector (0,1,0);
	}
	else if (sib_incidence_v2_mode == 3) {
		cusvector = vector (0,-1,0);
	}
	else if (sib_incidence_v2_mode == 4) {
		cusvector = vector (0,0,-1);
	}
	else if (sib_incidence_v2_mode == 5) {
		cusvector = vector (0,0,1);
	}
	else if (sib_incidence_v2_mode == 6) {
		cusvector = -normalize(I);
	}
	else if (sib_incidence_v2_mode == 7) {
		cusvector = sib_incidence_v2_custom_vector;
	}
	
	Fout = Nf.cusvector;

	if (sib_incidence_v2_mode != 6) {
		Fout = sib_incidence_v2_bias + sib_incidence_v2_gain * Fout;
	}

	if (sib_incidence_v2_invert == -1) {
		Fout = 1.0 - Fout;
	}

	return Fout;
}

color misss_fast_simple_phen (DEF_MISSS_FAST_SIMPLE_PHEN_PARAMS DEF2_GLOBAL_PARAMS DEF2_OUTPUT_PARAMS)
{
    extern color Ci;
    extern color Oi;
    extern color Cs;
    extern color Os;
    extern point P;	
    extern normal Ng;
    extern normal N;
    extern vector I;
    color rad = 0, irrad;
    normal Nn = normalize(N);
    vector Vn = -normalize(I);
    uniform string raylabel;

#ifdef DELIGHT
    rayinfo("type", raylabel); 
#elif AIR   
    rayinfo("type", raylabel);
#else
    rayinfo("label", raylabel);
#endif

    color Ka = misss_fast_simple_phen_ambient;
    float Kd = misss_fast_simple_phen_diffuse_weight;
    if (raylabel == "subsurface") { 
        Ci = (Ka*ambient() + Kd*misss_fast_simple_phen_diffuse_color*diffuse(Nn)) * Cs;
    }
    else { 
        color raycolor = 0, sum = 0;
        float dist = 0;
        float roughness = misss_fast_simple_phen_exponent;
        Ci = (Ka*ambient() + Kd*misss_fast_simple_phen_diffuse_color*diffuse(Nn)) * Cs + misss_fast_simple_phen_specular*phong(Nn,Vn,roughness);
    
#ifdef DELIGHT
        Ci += subsurface(P);
#else    
        float Ksub = misss_fast_simple_phen_front_sss_weight;
        if (Ksub > 0) {

            gather("illuminance", P, -Nn, PI/2, misss_fast_simple_phen_samples, "label", "subsurface", "surface:Ci", raycolor, "ray:length", dist) {
                sum += exp(-(1.0/misss_fast_simple_phen_falloff)*dist) * raycolor;
            }
            irrad = sum / misss_fast_simple_phen_samples;
            Ci += Ksub*misss_fast_simple_phen_front_sss_color * irrad * Cs;
        }
#endif
    }

    Ci *= misss_fast_simple_phen_overall_color * Os;
    return Ci;
}


#endif


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


#ifndef MATH


#define DEF_SIB_COLOR_MATH_BASIC_PARAMS \
	color sib_color_math_basic_input1; color sib_color_math_basic_input2; float sib_color_math_basic_op;

#define SIB_COLOR_MATH_BASIC_PARAMS \
	sib_color_math_basic_input1, sib_color_math_basic_input2, sib_color_math_basic_op

#define DEF_SIB_COLOR_MATH_EXPONENT_PARAMS \
	color sib_color_math_exponent_input; color sib_color_math_exponent_factor; float sib_color_math_exponent_op;

#define SIB_COLOR_MATH_EXPONENT_PARAMS \
	sib_color_math_exponent_input, sib_color_math_exponent_factor, sib_color_math_exponent_op

#define DEF_SIB_COLOR_MATH_UNARY_PARAMS \
	color sib_color_math_unary_input; float sib_color_math_unary_op;

#define SIB_COLOR_MATH_UNARY_PARAMS \
	sib_color_math_unary_input, sib_color_math_unary_op

#define DEF_SIB_COLOR_MATH_LOGIC_PARAMS \
	color sib_color_math_logic_input1; color sib_color_math_logic_input2; float sib_color_math_logic_op;

#define SIB_COLOR_MATH_LOGIC_PARAMS \
	sib_color_math_logic_input1, sib_color_math_logic_input2, sib_color_math_logic_op

#define DEF_MIB_COLOR_AVERAGE_PARAMS \
	color mib_color_average_input; float mib_color_average_factor;

#define MIB_COLOR_AVERAGE_PARAMS \
	mib_color_average_input, mib_color_average_factor

#define DEF_SIB_INTERP_LINEAR_PARAMS \
	float sib_interp_linear_input; float sib_interp_linear_oldrange_min; float sib_interp_linear_oldrange_max; \
	float sib_interp_linear_newrange_min; float sib_interp_linear_newrange_max;

#define SIB_INTERP_LINEAR_PARAMS \
	sib_interp_linear_input, sib_interp_linear_oldrange_min, sib_interp_linear_oldrange_max, \
	sib_interp_linear_newrange_min, sib_interp_linear_newrange_max

#define DEF_SIB_SCALAR_MATH_BASIC_PARAMS \
	float sib_scalar_math_basic_input1; float sib_scalar_math_basic_input2; float sib_scalar_math_basic_op;

#define SIB_SCALAR_MATH_BASIC_PARAMS \
	sib_scalar_math_basic_input1, sib_scalar_math_basic_input2, sib_scalar_math_basic_op

#define DEF_SIB_SCALAR_MATH_EXPONENT_PARAMS \
	float sib_scalar_math_exponent_input; float sib_scalar_math_exponent_factor; float sib_scalar_math_exponent_op;

#define SIB_SCALAR_MATH_EXPONENT_PARAMS \
	sib_scalar_math_exponent_input, sib_scalar_math_exponent_factor, sib_scalar_math_exponent_op

#define DEF_SIB_SCALAR_MATH_UNARY_PARAMS \
	float sib_scalar_math_unary_input; float sib_scalar_math_unary_op;

#define SIB_SCALAR_MATH_UNARY_PARAMS \
	sib_scalar_math_unary_input, sib_scalar_math_unary_op

#define DEF_SIB_SCALAR_MATH_LOGIC_PARAMS \
	float sib_scalar_math_logic_input1; float sib_scalar_math_logic_input2; float sib_scalar_math_logic_op;

#define SIB_SCALAR_MATH_LOGIC_PARAMS \
	sib_scalar_math_logic_input1, sib_scalar_math_logic_input2, sib_scalar_math_logic_op


color sib_color_math_basic (DEF_SIB_COLOR_MATH_BASIC_PARAMS) 
{
	color Cout = 0;

	if (sib_color_math_basic_op == 0) {
		Cout = sib_color_math_basic_input1 + sib_color_math_basic_input2;
	}
	else if (sib_color_math_basic_op == 1) {
		Cout = sib_color_math_basic_input1 - sib_color_math_basic_input2;
	}
	else if (sib_color_math_basic_op == 2) {
		Cout = sib_color_math_basic_input1 * sib_color_math_basic_input2;
	}
	else if (sib_color_math_basic_op == 3) {
		Cout = sib_color_math_basic_input1 / sib_color_math_basic_input2;
	}

	return Cout;
}

color sib_color_math_exponent (DEF_SIB_COLOR_MATH_EXPONENT_PARAMS) 
{
	color Cout = 0;

	if (sib_color_math_exponent_op == 0) {
		setcomp(Cout, 0, pow(comp(sib_color_math_exponent_input,0), comp(sib_color_math_exponent_factor,0)));
		setcomp(Cout, 1, pow(comp(sib_color_math_exponent_input,1), comp(sib_color_math_exponent_factor,1)));
		setcomp(Cout, 2, pow(comp(sib_color_math_exponent_input,2), comp(sib_color_math_exponent_factor,2)));
	}
	else if (sib_color_math_exponent_op == 1) {
		setcomp(Cout, 0, 1/pow(comp(sib_color_math_exponent_input,0), comp(sib_color_math_exponent_factor,0)));
		setcomp(Cout, 1, 1/pow(comp(sib_color_math_exponent_input,1), comp(sib_color_math_exponent_factor,1)));
		setcomp(Cout, 2, 1/pow(comp(sib_color_math_exponent_input,2), comp(sib_color_math_exponent_factor,2)));
	}
	else if (sib_color_math_exponent_op == 2) {
		setcomp(Cout, 0, mix(0.5, comp(sib_color_math_exponent_input,0), 0.5 + comp(sib_color_math_exponent_factor,0)));
		setcomp(Cout, 1, mix(0.5, comp(sib_color_math_exponent_input,1), 0.5 + comp(sib_color_math_exponent_factor,1)));
		setcomp(Cout, 2, mix(0.5, comp(sib_color_math_exponent_input,2), 0.5 + comp(sib_color_math_exponent_factor,2)));
	}
	else if (sib_color_math_exponent_op == 3) {
		setcomp(Cout, 0, comp(sib_color_math_exponent_input,0) * comp(sib_color_math_exponent_factor,0));
		setcomp(Cout, 1, comp(sib_color_math_exponent_input,1) * comp(sib_color_math_exponent_factor,1));
		setcomp(Cout, 2, comp(sib_color_math_exponent_input,2) * comp(sib_color_math_exponent_factor,2));
	}

	return Cout;
}

color sib_color_math_unary (DEF_SIB_COLOR_MATH_UNARY_PARAMS) 
{
	color Cout = 0;

	if (sib_color_math_unary_op == 0) {
		setcomp(Cout, 0, abs(comp(sib_color_math_unary_input,0)));
		setcomp(Cout, 1, abs(comp(sib_color_math_unary_input,1)));
		setcomp(Cout, 2, abs(comp(sib_color_math_unary_input,2)));
	}
	else if (sib_color_math_unary_op == 1) {
		setcomp(Cout, 0, -comp(sib_color_math_unary_input,0));
		setcomp(Cout, 1, -comp(sib_color_math_unary_input,1));
		setcomp(Cout, 2, -comp(sib_color_math_unary_input,2));
	}
	else if (sib_color_math_unary_op == 2) {
		setcomp(Cout, 0, 1-comp(sib_color_math_unary_input,0));
		setcomp(Cout, 1, 1-comp(sib_color_math_unary_input,1));
		setcomp(Cout, 2, 1-comp(sib_color_math_unary_input,2));
	}

	return Cout;
}

float sib_color_math_logic (DEF_SIB_COLOR_MATH_LOGIC_PARAMS) 
{
	float Fout = 0;

	if (sib_color_math_logic_op == 0) {
		if (comp(sib_color_math_logic_input1, 0) == comp(sib_color_math_logic_input2, 0) &&
			comp(sib_color_math_logic_input1, 1) == comp(sib_color_math_logic_input2, 1) &&
			comp(sib_color_math_logic_input1, 2) == comp(sib_color_math_logic_input2, 2))
			Fout = 0;
	}
	else if (sib_color_math_logic_op == 1) {
		if (comp(sib_color_math_logic_input1, 0) < comp(sib_color_math_logic_input2, 0) &&
			comp(sib_color_math_logic_input1, 1) < comp(sib_color_math_logic_input2, 1) &&
			comp(sib_color_math_logic_input1, 2) < comp(sib_color_math_logic_input2, 2))
			Fout = 1;
	}
	else if (sib_color_math_logic_op == 2) {
		if (comp(sib_color_math_logic_input1, 0) > comp(sib_color_math_logic_input2, 0) &&
			comp(sib_color_math_logic_input1, 1) > comp(sib_color_math_logic_input2, 1) &&
			comp(sib_color_math_logic_input1, 2) > comp(sib_color_math_logic_input2, 2))
			Fout = 1;
	}
	else if (sib_color_math_logic_op == 3) {
		if (comp(sib_color_math_logic_input1, 0) <= comp(sib_color_math_logic_input2, 0) &&
			comp(sib_color_math_logic_input1, 1) <= comp(sib_color_math_logic_input2, 1) &&
			comp(sib_color_math_logic_input1, 2) <= comp(sib_color_math_logic_input2, 2))
			Fout = 1;
	}
	else if (sib_color_math_logic_op == 4) {
		if (comp(sib_color_math_logic_input1, 0) >= comp(sib_color_math_logic_input2, 0) &&
			comp(sib_color_math_logic_input1, 1) >= comp(sib_color_math_logic_input2, 1) &&
			comp(sib_color_math_logic_input1, 2) >= comp(sib_color_math_logic_input2, 2))
			Fout = 1;
	}

	return Fout;
}

color mib_color_average (DEF_MIB_COLOR_AVERAGE_PARAMS) 
{
	color Cout = 0;

	Cout = mib_color_average_factor * (comp(mib_color_average_input,0) + comp(mib_color_average_input,1) + 
		comp(mib_color_average_input,2)) / 3.;

	return Cout;
}

float sib_interp_linear (DEF_SIB_INTERP_LINEAR_PARAMS) 
{
	float Fout = 0;
	
	Fout = sib_interp_linear_newrange_min + (sib_interp_linear_input - sib_interp_linear_oldrange_min) * 
		(sib_interp_linear_newrange_max - sib_interp_linear_newrange_min) / 
		(sib_interp_linear_oldrange_max - sib_interp_linear_oldrange_min); 
			

	return Fout;
}

float sib_scalar_math_basic (DEF_SIB_SCALAR_MATH_BASIC_PARAMS) 
{
	float Fout = 0;

	if (sib_scalar_math_basic_op == 0) {
		Fout = sib_scalar_math_basic_input1 + sib_scalar_math_basic_input2;
	}
	else if (sib_scalar_math_basic_op == 1) {
		Fout = sib_scalar_math_basic_input1 - sib_scalar_math_basic_input2;
	}
	else if (sib_scalar_math_basic_op == 2) {
		Fout = sib_scalar_math_basic_input1 * sib_scalar_math_basic_input2;
	}
	else if (sib_scalar_math_basic_op == 3) {
		Fout = sib_scalar_math_basic_input1 / sib_scalar_math_basic_input2;
	}
	else if (sib_scalar_math_basic_op == 4) {
		Fout = MIN(sib_scalar_math_basic_input1, sib_scalar_math_basic_input2);
	}
	else if (sib_scalar_math_basic_op == 5) {
		Fout = MAX(sib_scalar_math_basic_input1, sib_scalar_math_basic_input2);
	}

	return Fout;
}

float sib_scalar_math_exponent (DEF_SIB_SCALAR_MATH_EXPONENT_PARAMS) 
{
	float Fout = 0;

	if (sib_scalar_math_exponent_op == 0) 
		Fout = pow(sib_scalar_math_exponent_input, sib_scalar_math_exponent_factor);
	else if (sib_scalar_math_exponent_op == 1) 
		Fout = 1/pow(sib_scalar_math_exponent_input, sib_scalar_math_exponent_factor);
	else if (sib_scalar_math_exponent_op == 2) 
		Fout = mix(0.5, sib_scalar_math_exponent_input, 0.5 + sib_scalar_math_exponent_factor);
	else if (sib_scalar_math_exponent_op == 3) 
		Fout = sib_scalar_math_exponent_input * sib_scalar_math_exponent_factor;

	return Fout;
}

float sib_scalar_math_unary (DEF_SIB_SCALAR_MATH_UNARY_PARAMS) 
{
	float Fout = 0;

	if (sib_scalar_math_unary_op == 0) 
		Fout = abs(sib_scalar_math_unary_input);
	else if (sib_scalar_math_unary_op == 1) 
		Fout = -sib_scalar_math_unary_input;
	else if (sib_scalar_math_unary_op == 2) 
		Fout = 1 - sib_scalar_math_unary_input;

	return Fout;
}

float sib_scalar_math_logic (DEF_SIB_SCALAR_MATH_LOGIC_PARAMS) 
{
	float Fout = 0;

	if (sib_scalar_math_logic_op == 0) {
		if (sib_scalar_math_logic_input1 == sib_scalar_math_logic_input2)
			Fout = 0;
	}
	else if (sib_scalar_math_logic_op == 1) {
		if (sib_scalar_math_logic_input1 < sib_scalar_math_logic_input2)
			Fout = 1;
	}
	else if (sib_scalar_math_logic_op == 2) {
		if (sib_scalar_math_logic_input1 > sib_scalar_math_logic_input2) 
			Fout = 1;
	}
	else if (sib_scalar_math_logic_op == 3) {
		if (sib_scalar_math_logic_input1 <= sib_scalar_math_logic_input2)
			Fout = 1;
	}
	else if (sib_scalar_math_logic_op == 4) {
		if (sib_scalar_math_logic_input1 >= sib_scalar_math_logic_input2)
			Fout = 1;
	}

	return Fout;
}


#endif


#ifndef CONVERSION


#define DEF_SIB_COLOR_2MIX_PARAMS \
	color sib_color_2mix_base_color; color sib_color_2mix_color1; color sib_color_2mix_weight1; \
	float sib_color_2mix_mode1;

#define SIB_COLOR_2MIX_PARAMS \
	sib_color_2mix_base_color, sib_color_2mix_color1, sib_color_2mix_weight1, sib_color_2mix_mode1


color sib_color_2mix(DEF_SIB_COLOR_2MIX_PARAMS) 
{
	color Cout;

	if (sib_color_2mix_mode1 == 2) {
		Cout = sib_color_2mix_color1 * sib_color_2mix_weight1 + sib_color_2mix_base_color;
	}
	else if (sib_color_2mix_mode1 == 4) {
		Cout = sib_color_2mix_color1 * sib_color_2mix_base_color * sib_color_2mix_weight1;
	}
	else if (sib_color_2mix_mode1 == 5) {
		Cout = sib_color_2mix_color1 * sib_color_2mix_base_color * sib_color_2mix_weight1;
	}
	else if (sib_color_2mix_mode1 == 9) {
		Cout = sib_color_2mix_base_color - sib_color_2mix_color1 * sib_color_2mix_weight1;
		setcomp(Cout, 0, abs(comp(Cout,0)));
		setcomp(Cout, 1, abs(comp(Cout,1)));
		setcomp(Cout, 2, abs(comp(Cout,2)));
	}
	else if (sib_color_2mix_mode1 == 16) {
		Cout = sib_color_2mix_color1 * sib_color_2mix_base_color * sib_color_2mix_weight1;
	}
	else { 
		Cout = sib_color_2mix_color1 * sib_color_2mix_weight1 + sib_color_2mix_base_color * 
			(1 - sib_color_2mix_weight1);
	}

	return Cout;
}


#endif


#ifndef RAYTRACING


#define DEF_MIB_REFLECT_PARAMS \
	color mib_reflect_input; color mib_reflect_reflect; float mib_reflect_notrace; 

#define MIB_REFLECT_PARAMS \
	mib_reflect_input, mib_reflect_reflect, mib_reflect_notrace

#define DEF_SIB_DIFFUSE_REFLECTION_PARAMS \
	float sib_diffuse_reflection_enabled; color sib_diffuse_reflection_input; \
	color sib_diffuse_reflection_reflect; float sib_diffuse_reflection_notrace; \
	float sib_diffuse_reflection_invert; float sib_diffuse_reflection_scale; \
	float sib_diffuse_reflection_max_depth; float sib_diffuse_reflection_glossy; \
	float sib_diffuse_reflection_samples;

#define SIB_DIFFUSE_REFLECTION_PARAMS \
	sib_diffuse_reflection_enabled, sib_diffuse_reflection_input, \
	sib_diffuse_reflection_reflect, sib_diffuse_reflection_notrace, \
	sib_diffuse_reflection_invert, sib_diffuse_reflection_scale, \
	sib_diffuse_reflection_max_depth, sib_diffuse_reflection_glossy, \
	sib_diffuse_reflection_samples

#define DEF_MIB_REFRACT_PARAMS \
	color mib_refract_input; color mib_refract_refract; float mib_refract_ior; 

#define MIB_REFRACT_PARAMS \
	mib_refract_input, mib_refract_refract, mib_refract_ior

#define DEF_SIB_DIFFUSE_REFRACTION_PHEN_PARAMS \
	float sib_diffuse_refraction_phen_enabled; color sib_diffuse_refraction_phen_input; \
	color sib_diffuse_refraction_phen_refract; float sib_diffuse_refraction_phen_ior; \
	float sib_diffuse_refraction_phen_invert; float sib_diffuse_refraction_phen_scale; \
	float sib_diffuse_refraction_phen_max_depth; float sib_diffuse_refraction_phen_glossy; \
	float sib_diffuse_refraction_phen_samples;

#define SIB_DIFFUSE_REFRACTION_PHEN_PARAMS \
	sib_diffuse_refraction_phen_enabled, sib_diffuse_refraction_phen_input, \
	sib_diffuse_refraction_phen_refract, sib_diffuse_refraction_phen_ior, \
	sib_diffuse_refraction_phen_invert, sib_diffuse_refraction_phen_scale, \
	sib_diffuse_refraction_phen_max_depth, sib_diffuse_refraction_phen_glossy, \
	sib_diffuse_refraction_phen_samples

#define DEF_MIB_OPACITY_PARAMS \
	color mib_opacity_input; color mib_opacity_opacity;

#define MIB_OPACITY_PARAMS \
	mib_opacity_input, mib_opacity_opacity

#define DEF_SIB_TRANSPARENCY_PARAMS \
	color sib_transparency_input; color sib_transparency_transp;

#define SIB_TRANSPARENCY_PARAMS \
	sib_transparency_input, sib_transparency_transp

#define DEF_SIB_DIELECTRIC_IOR_PARAMS \
	float sib_dielectric_ior_mode; float sib_dielectric_ior_refract; \
	float sib_dielectric_ior_reflect; float sib_dielectric_ior_ior; \
	float sib_dielectric_ior_exponent;

#define SIB_DIELECTRIC_IOR_PARAMS \
	sib_dielectric_ior_mode, sib_dielectric_ior_refract, \
	sib_dielectric_ior_reflect, sib_dielectric_ior_ior, \
	sib_dielectric_ior_exponent


color mib_reflect (DEF_MIB_REFLECT_PARAMS DEF2_GLOBAL_PARAMS)
{
	color outputcolor;
	extern point P;
	extern normal N;
	extern vector I;
	color reflcolor = 0;

	normal Nf = faceforward(normalize(N), I);
	normal Nn = normalize(N);
	vector In = normalize(I);
	vector refldir = reflect(In, Nn);

	if (notrace == 0 && mib_reflect_notrace == 0) {
            reflcolor = trace(P, refldir);
	}

	outputcolor = reflcolor * mib_reflect_reflect + (1 - mib_reflect_reflect) * mib_reflect_input;

	return outputcolor;
}

color sib_diffuse_reflection (DEF_SIB_DIFFUSE_REFLECTION_PARAMS DEF2_GLOBAL_PARAMS)
{
	color outputcolor;
	extern point P;
	extern normal N;
	extern vector I;
	color reflcolor = 0;

	normal Nf = faceforward(normalize(N), I);
	normal Nn = normalize(N);
	vector In = normalize(I);
	vector refldir = reflect(In, Nn);

	float glossy = sib_diffuse_reflection_glossy;
	uniform float reflsamples = sib_diffuse_reflection_samples;
	if (sib_diffuse_reflection_samples == 0) reflsamples = 1;

	float depth;
	rayinfo("depth", depth);
	if (depth >= sib_diffuse_reflection_max_depth) {
		glossy = 0;
		reflsamples = 1;
	}

	if (notrace == 0 && sib_diffuse_reflection_enabled != 0 && sib_diffuse_reflection_notrace == 0) {
		color hitc = 0;
		gather("illuminance", P, refldir, glossy, reflsamples, "surface:Ci", hitc) {
			reflcolor += hitc;	
		}
		reflcolor /= reflsamples;
	}

	color sdr_reflect;
	if (sib_diffuse_reflection_invert != 0) 
		sdr_reflect = 1 - sib_diffuse_reflection_reflect;
	else
		sdr_reflect = sib_diffuse_reflection_reflect;

	outputcolor = reflcolor * sib_diffuse_reflection_scale * sdr_reflect + 
		(1 - sib_diffuse_reflection_scale * sdr_reflect) * sib_diffuse_reflection_input;

	return outputcolor;
}

color mib_refract (DEF_MIB_REFRACT_PARAMS DEF2_GLOBAL_PARAMS)
{
	vector refldir, refrdir;
	float kr, kt;
	color outputcolor;
	extern point P;
	extern normal N;
	extern vector I;
	color refrcolor = 0;

	normal Nf = faceforward(normalize(N), I);
	normal Nn = normalize(N);
	vector In = normalize(I);
	
	float eta = (In.Nn < 0) ? 1/mib_refract_ior : mib_refract_ior;
	fresnel(In, Nf, eta, kr, kt, refldir, refrdir);
    	kt = 1 - kr;

	if (notrace == 0) {
            refrcolor = kt * trace(P, refrdir);
	}

	outputcolor = refrcolor * mib_refract_refract + (1 - mib_refract_refract) * mib_refract_input;

	return outputcolor;
}

color sib_diffuse_refraction_phen (DEF_SIB_DIFFUSE_REFRACTION_PHEN_PARAMS DEF2_GLOBAL_PARAMS)
{
	float kr, kt;
	vector refldir, refrdir;
	color outputcolor;
	extern point P;
	extern normal N;
	extern vector I;
	color refrcolor = 0;

	normal Nf = faceforward(normalize(N), I);
	normal Nn = normalize(N);
	vector In = normalize(I);

	float eta = (In.Nn < 0) ? 1/sib_diffuse_refraction_phen_ior : sib_diffuse_refraction_phen_ior;
	fresnel(In, Nf, eta, kr, kt, refldir, refrdir);
    	kt = 1 - kr;

	float glossy = sib_diffuse_refraction_phen_glossy;
	uniform float refrsamples = sib_diffuse_refraction_phen_samples;
	if (sib_diffuse_refraction_phen_samples == 0) refrsamples = 1;

	float depth;
	rayinfo("depth", depth);
	if (depth >= sib_diffuse_refraction_phen_max_depth) {
		glossy = 0;
		refrsamples = 1;
	}

	if (notrace == 0 && sib_diffuse_refraction_phen_enabled != 0) {
		color hitc = 0;
		gather("illuminance", P, refrdir, glossy, refrsamples, "surface:Ci", hitc) {
			refrcolor += kt * hitc;	
		}
		refrcolor /= refrsamples;
	}

	color sdr_refract;
	if (sib_diffuse_refraction_phen_invert != 0) 
		sdr_refract = 1 - sib_diffuse_refraction_phen_refract;
	else
		sdr_refract = sib_diffuse_refraction_phen_refract;

	outputcolor = refrcolor * sib_diffuse_refraction_phen_scale * sdr_refract + 
		(1 - sib_diffuse_refraction_phen_scale * sdr_refract) * sib_diffuse_refraction_phen_input;

	return outputcolor;
}

color mib_opacity (DEF_MIB_OPACITY_PARAMS)
{
	color outputopacity;
	extern vector I;
	extern point P;

	vector In = normalize(I);
	color transcolor = trace(P, I);
	
	outputopacity = transcolor * (1 - mib_opacity_opacity) + mib_opacity_opacity * mib_opacity_input;

	return outputopacity;
}

color sib_transparency (DEF_SIB_TRANSPARENCY_PARAMS)
{
	color outputopacity;
	extern vector I;
	extern point P;

	vector In = normalize(I);
	color transcolor = trace(P, I);

	outputopacity = transcolor * sib_transparency_transp + (1 - sib_transparency_transp) * sib_transparency_input;
	
	return outputopacity;
}

color sib_dielectric_ior (DEF_SIB_DIELECTRIC_IOR_PARAMS) 
{
	float kr, kt;
	float eta;
	float dielectricval;
	vector refldir, refrdir;
	extern vector I;
	extern normal N;
	normal Nf = faceforward(normalize(N), I);
	normal Nn = normalize(N);
	vector In = normalize(I);

	if (sib_dielectric_ior_mode == 0) { 
		refldir = reflect(In, Nn);
	}
	else {
		eta = sib_dielectric_ior_ior;
		fresnel(In, Nf, eta, kr, kt, refldir, refrdir);
	}

	if (sib_dielectric_ior_mode == 0) 
		dielectricval = pow(1-(Nf . refldir), sib_dielectric_ior_exponent) * sib_dielectric_ior_reflect;
	else 
		dielectricval = pow((-Nf . refrdir), sib_dielectric_ior_exponent) * sib_dielectric_ior_refract;

	return color(dielectricval);
}


#endif


#ifndef STATE


#define DEF_SIB_STATE_SCALAR_PARAMS \
	float sib_state_scalar_mode; 

#define SIB_STATE_SCALAR_PARAMS \
	sib_state_scalar_mode

#define DEF_SIB_STATE_VECTOR_PARAMS \
	float sib_state_vector_mode; 

#define SIB_STATE_VECTOR_PARAMS \
	sib_state_vector_mode



float sib_state_scalar(DEF_SIB_STATE_SCALAR_PARAMS)
{
	extern vector I;
	extern normal N;
	extern point P;
	extern uniform float dtime;
	extern float s, t;
	float Fout;

	if (sib_state_scalar_mode == 0) {
		point Pndc = transform("raster", P);
		Fout = xcomp(Pndc);
	}
	else if (sib_state_scalar_mode == 1) {
		point Pndc = transform("raster", P);
		Fout = ycomp(Pndc);
	}
	else if (sib_state_scalar_mode == 2) {
		Fout = dtime;
	}
	else if (sib_state_scalar_mode == 5) {
		normal Nf = faceforward(normalize(N), I);
		vector V = -normalize(I);
		Fout = Nf . V;
	}
	else if (sib_state_scalar_mode == 6) {
		float success = rayinfo("length", Fout);
		if (success == 0) {
			Fout = length(I);
		}
	}
	else if (sib_state_scalar_mode == 7) {
		Fout = s;
	}
	else if (sib_state_scalar_mode == 8) {
		Fout = t;
	}
	else {
		Fout = 1.0;
	}

	return Fout;
}

vector sib_state_vector(DEF_SIB_STATE_VECTOR_PARAMS)
{
	extern vector I;
	extern normal N;
	extern normal Ng;
	extern point P;
#ifndef DELIGHT
	extern vector dPdtime;
#endif
	extern uniform point E;
	vector Vout;
	
	if (sib_state_vector_mode == 0) {
		vector V = normalize(I);
		Vout = vtransform("shader", V);
	}
	else if (sib_state_vector_mode == 1) {
		point PE = transform("shader", E);
		Vout = vector PE;
	}
	else if (sib_state_vector_mode == 2) {
		point Pint = transform("shader", P);
		Vout = vector Pint;
	}
	else if (sib_state_vector_mode == 3) {
		vector NV = vtransform("shader", vector N);
		Vout = normalize(NV);
	}
	else if (sib_state_vector_mode == 4) {
		vector NV = vtransform("shader", vector Ng);
		Vout = normalize(NV);
	}
	else if (sib_state_vector_mode == 5) {
#ifndef DELIGHT
		vector V = normalize(dPdtime);
		Vout = vtransform("shader", V);
#endif
	}
	else if (sib_state_vector_mode == 6) {
		Vout = 0;
	}

	return Vout;
}



#endif


#ifndef SWITCH


#define DEF_MIB_TWOSIDED_PARAMS \
	color mib_twosided_front; color mib_twosided_back; 

#define MIB_TWOSIDED_PARAMS \
	mib_twosided_front, mib_twosided_back

#define DEF_SIB_SCALAR_SWITCH_PARAMS \
	float sib_scalar_switch_input; float sib_scalar_switch_scalar1; float sib_scalar_switch_scalar2;

#define SIB_SCALAR_SWITCH_PARAMS \
	sib_scalar_switch_input, sib_scalar_switch_scalar1, sib_scalar_switch_scalar2

#define DEF_SIB_VECTOR_SWITCH_PARAMS \
	float sib_vector_switch_input; vector sib_vector_switch_vector1; vector sib_vector_switch_vector2;

#define SIB_VECTOR_SWITCH_PARAMS \
	sib_vector_switch_input, sib_vector_switch_vector1, sib_vector_switch_vector2

#define DEF_SIB_COLOR_SWITCH_PARAMS \
	float sib_color_switch_switch; color sib_color_switch_input1; color sib_color_switch_input2;

#define SIB_COLOR_SWITCH_PARAMS \
	sib_color_switch_switch, sib_color_switch_input1, sib_color_switch_input2


color mib_twosided(DEF_MIB_TWOSIDED_PARAMS) 
{
	extern vector I;
	extern normal N;
	color Cout;
	
	vector V = -I;
	if (N.V >= 0) Cout = mib_twosided_front;
	else Cout = mib_twosided_back;

	return Cout;
}

float sib_scalar_switch(DEF_SIB_SCALAR_SWITCH_PARAMS)
{
	float Fout;
	
	if (sib_scalar_switch_input != 0) Fout = sib_scalar_switch_scalar2;
	else Fout = sib_scalar_switch_scalar1;

	return Fout;
}

vector sib_vector_switch(DEF_SIB_VECTOR_SWITCH_PARAMS)
{
	vector Vout;
	
	if (sib_vector_switch_input != 0) Vout = sib_vector_switch_vector2;
	else Vout = sib_vector_switch_vector1;

	return Vout;
}

color sib_color_switch(DEF_SIB_COLOR_SWITCH_PARAMS)
{
	color Cout;
	
	if (sib_color_switch_switch != 0) Cout = sib_color_switch_input2;
	else Cout = sib_color_switch_input1;

	return Cout;
}


#endif


#ifndef VOLUME


#define DEF_DENSE_VOL_PARAMS \
	float dense_vol_density; color dense_vol_transmit; float dense_vol_profile; \
	float dense_vol_one_to_one; float dense_vol_one_to_ten; float dense_vol_one_to_hundred;

#define DENSE_VOL_PARAMS \
	dense_vol_density, dense_vol_transmit, dense_vol_profile, \
	dense_vol_one_to_one, dense_vol_one_to_ten, dense_vol_one_to_hundred

#define DEF_SOFT_FOG_PARAMS \
	float soft_fog_start; float soft_fog_stop; float soft_fog_rate; \
	color soft_fog_transmit; float soft_fog_lightrays;

#define SOFT_FOG_PARAMS \
	soft_fog_start, soft_fog_stop, soft_fog_rate, soft_fog_transmit, soft_fog_lightrays

	
color dense_vol(DEF_DENSE_VOL_PARAMS) 
{
	extern vector I;
	extern color Ci;
	extern point P;
	color Cout = 0;	

	illuminance (P) {
		float distance = length(L);
		if (dense_vol_one_to_one != 0) distance *= 1;
		if (dense_vol_one_to_ten != 0) distance *= 10;
		else if (dense_vol_one_to_hundred != 0) distance *= 100;
		float raylength;
		rayinfo("length", raylength);
		float d = dense_vol_density * pow(raylength/distance, 1/dense_vol_profile);
		Cout += mix(Ci, dense_vol_transmit, d);
	}

	return Cout;
}

color soft_fog(DEF_SOFT_FOG_PARAMS) 
{
	extern vector I;
	extern color Ci;

	float d = clamp((zcomp(I) - soft_fog_start)/(soft_fog_stop - soft_fog_start), 0, 1);
	color Cout = mix(Ci, soft_fog_transmit, 0.1 * d * (1 - soft_fog_rate));

	return Cout;
}


#endif
