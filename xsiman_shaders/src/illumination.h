//*****************************************************************************
//  File: illumination.h
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
	extern point P;
	extern normal N;
	extern vector I;
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
#ifdef AIR
   		varying normal Nocc;
		color occl = occlusion(P, Ns, xsiambientocclusion_spread, Nocc, "samples", 
			xsiambientocclusion_samples, "maxdist", max_dist);
		occ = (comp(occl,0) + comp(occl,1) + comp(occl,2))/3;
#elif DELIGHT
      	occ = occlusion(P, Ns, xsiambientocclusion_samples, "maxdist", max_dist, "angle", xsiambientocclusion_spread);
#else
		occ = occlusion(P, Ns, xsiambientocclusion_samples, "maxdist", max_dist, "coneangle", xsiambientocclusion_spread);
#endif
		Cout = mix(xsiambientocclusion_bright, xsiambientocclusion_dark, occ);
	}

    	if(notrace == 0 && xsiambientocclusion_output_mode >= 2) {
		vector bentNormal = 0;
        	vector dir = 0;
        	float hits = 0;
		uniform float samples = xsiambientocclusion_samples;
#ifdef DELIGHT
		occlusion(P, Ns, xsiambientocclusion_samples, "maxdist", max_dist, "angle", xsiambientocclusion_spread,
			"environmentdir", bentNormal);
#else
      	gather("illuminance", P, Ns, xsiambientocclusion_spread*PI/2, samples, 
			"distribution", "cosine", "maxdist", max_dist, "ray:direction", dir) {
         		hits += 1;
        	} else { /* ray miss */
          		bentNormal += dir;
        	}
#endif
        	
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

	// remap contrast and brightness
	Fout = 0.5 + 0.5 * Fout;

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
