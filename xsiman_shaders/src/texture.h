//*****************************************************************************
//  File: texture.h
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
	float txt2d_scalarimage_explicit_bump_inuse; float txt2d_scalarimage_explicit_factor;\
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
	txt3d_cloud_freq_mul, txt3d_cloud_iter_max, txt3d_cloud_level_min, \
	txt3d_cloud_time, txt3d_cloud_repeats, \
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

	if (ss > 0 && tt > 0) {
	 	Cout = texture(image_clip_Source_File, ss, tt, "blur", image_clip_Radius*image_clip_Amount/maxdim);
	}
    
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

	if (ss > 0 && tt > 0) {
		Cout = texture(image_clip_Source_File, ss, tt, "blur", image_clip_Radius*image_clip_Amount/maxdim);
	}
      
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