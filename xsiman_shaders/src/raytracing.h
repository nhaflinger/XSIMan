//*****************************************************************************
//  File: raytracing.h
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