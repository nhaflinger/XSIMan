surface
xsimanHair(
	float hair_id = 0;
	float TransparencyCrossoverRange = 0; 
	float TransparencyCrossoverCenter = 0;
	float TransparencyTip = 0;
 	string TransparencyTipmap = "";
	float TransparencyTipmapblur = 0;
	float TransparencyRoot = 0;
 	string TransparencyRootmap = "";
	float TransparencyRootmapblur = 0;
	float TransparencyInuse = 0; 
	color Irradiance = 0;
	float SpecularDecay = 0; 
	color Specular = 1;
 	string Specularmap = "";
	float Specularmapblur = 0;
	float SpecularInuse = 0; 
	float DiffuseCrossoverRange = 0;
	float DiffuseCrossoverCenter = 0; 
	float DiffuseVariation = 0;
	float TipBalance = 0; 
	color DiffuseTipB = 1;
 	string DiffuseTipBmap = "";
	float DiffuseTipBmapblur = 0;
	color DiffuseTipA = 1;
 	string DiffuseTipAmap = "";
	float DiffuseTipAmapblur = 0;
	float FlatShadeRoot = 0;
     	float FlatShadeTip = 0;
	color DiffuseRoot = 1;
 	string DiffuseRootmap = "";
	float DiffuseRootmapblur = 0;
	color Ambient = 1; 
 	string Ambientmap = "";
	float Ambientmapblur = 0;
	color ambienceval = 1;
	color radiance = 1;
	string radiancemap = "";
	float radiancemapblur = 0;
	output varying color diffusecolor = 1;
	output varying color ambientcolor = 1;
	output varying color specularcolor = 1;
	output varying color transparencycolor = 0;	
	output varying color refrcolor = 0;
	output varying color reflcolor = 0; 
	output varying color occlusioncolor = 0;
	output varying color indirectcolor = 0;
	output varying color causticcolor = 0;
	output varying color shadowcolor = 0;
	float invert_s = 0;
	float invert_t = 0;
	float notrace = 1;
	float doOcclusion = 0;
	float occlusion_samples = 16;
	float maxdist = 1000000;
	float doIndirect = 0;
	float indirect_samples = 16;
// these parameters are placeholders
	float SelfShadow = 0;
	float BspDepth = 0;
	float BspMaxLeaf = 0;
	float BspMaxTree = 0;
	float BspMaxMemory = 0;
	color Material = 0;
	float DiffuseModel = 0;
	float NormalBlend = 0;
)
{
	float ss, tt;
	color radiancecolor = 1;
	color transparencytip = 0;
	color transparencyroot = 0;
	color diffusetipa = 1;
	color diffusetipb = 1;
	color diffuseroot = 1;
	extern color Ci;	
	extern color Oi;
	extern color Cs;
	extern color Os;
	extern normal N;
	extern vector I;

	if (invert_s != 0) ss = 1 - s;
	else ss = s;
  	if (invert_t != 0) tt = 1 - t;
 	else tt = t;

    	vector T = normalize(dPdv); 
    	vector V = -normalize(I);    
    	color Cspec = 0, Cdiff = 0;  
    	float cosang;

	if (TransparencyTipmap != "") {
		transparencytip *= texture(TransparencyTipmap, ss, tt, "blur", TransparencyTipmapblur);
	}
	else {
		transparencytip = color TransparencyTip;
	}

	if (TransparencyRootmap != "") {
		transparencyroot *= texture(TransparencyRootmap, ss, tt, "blur", TransparencyRootmapblur);
	}
	else {
		transparencyroot = color TransparencyRoot;
	}

	if (Specularmap != "") {
		specularcolor *= texture(Specularmap, ss, tt, "blur", Specularmapblur);
	}
	else {
		specularcolor = Specular;
	}

	if (DiffuseTipAmap != "") {
		diffusetipa *= texture(DiffuseTipAmap, ss, tt, "blur", DiffuseTipAmapblur);
	}
	else {
		diffusetipa = DiffuseTipA;
	}

	if (DiffuseTipBmap != "") {
		diffusetipb *= texture(DiffuseTipBmap, ss, tt, "blur", DiffuseTipBmapblur);
	}
	else {
		diffusetipb = DiffuseTipB;
	}
	if (DiffuseRootmap != "") {
		diffuseroot *= texture(DiffuseRootmap, ss, tt, "blur", DiffuseRootmapblur);
	}
	else {
		diffuseroot = DiffuseRoot;
	}

	if (Ambientmap != "") {
		ambientcolor *= texture(Ambientmap, ss, tt, "blur", Ambientmapblur);
	}
	else {
		ambientcolor = Ambient;
	}

	if (radiancemap != "") {
		radiancecolor *= texture(radiancemap, ss, tt, "blur", radiancemapblur);
	}
	else {
		radiancecolor = radiance;
	}

	ambientcolor = Ambient;
	ambientcolor *= ambient() + ambienceval;

	color speccolor = 1;
	if (SpecularInuse != 0) speccolor = specularcolor;
	else speccolor = 0;
	
	color tipcolor = mix(diffusetipb, diffusetipa, TipBalance);
    
    	color Cflat = 0;
	illuminance (P) {
		cosang = cos(abs(acos(T.normalize(L)) - acos(-T.V)));
		Cspec += Cl * v * pow(cosang, SpecularDecay);
		Cdiff += Cl * v;
		Cflat += Cl;
    	}

	color opacity = 1;
	float val = smoothstep(clamp(TransparencyCrossoverCenter - 
		0.5*TransparencyCrossoverRange,0,1), clamp(TransparencyCrossoverCenter +
		0.5*TransparencyCrossoverRange,0,1), v);
	if (TransparencyInuse != 0) {
		opacity = mix(1-transparencyroot, 1-transparencytip, val);
	}

	val = smoothstep(clamp(DiffuseCrossoverCenter -
		0.5*DiffuseCrossoverRange,0,1), clamp(DiffuseCrossoverCenter +
		0.5*DiffuseCrossoverRange,0,1), v);

	color haircolor = mix(diffuseroot, tipcolor, val);

	float root_flat_mix = mix(comp(Cdiff,0), 1.0, comp(Cflat,0)*FlatShadeRoot/100.0);
	float tip_flat_mix = mix(comp(Cdiff,0), 1.0, comp(Cflat,0)*FlatShadeTip/100.0);
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
	float color_variation = clamp(DiffuseVariation / 10., 0, 1);
	haircolor = mix(haircolor, Crgb, color_variation);

	Oi = Os * opacity;
	Ci = Oi * (haircolor * (ambientcolor*ambient() + diffusecolor) + specularcolor);

#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	Ci += causticcolor;
#endif
}


