//*****************************************************************************
//  File: hair.h
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