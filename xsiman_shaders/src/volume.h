//*****************************************************************************
//  File: volume.h
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
	extern point P;
	extern color Ci;
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