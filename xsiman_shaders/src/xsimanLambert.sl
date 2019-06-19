surface 
xsimanLambert(
	color lights = 0;
	color ambienceval = 0;
	color bumpval = 0;
	string bumpmap = "";
	float bumpmapblur = 0;
	float usenormals = 0;
	float truedisplacement = 0;
	float diffuse_inuse = 0;
	color diffuse = 0.7;
	string diffusemap = "";	
	float diffusemapblur = 0;
	output varying color diffusecolor = 1;
	color ambient = 0.3;
	string ambientmap = "";		
	float ambientmapblur = 0;
	output varying color ambientcolor = 1;
	float refract_inuse = 0;
	color transparency = 0;
	string transparencymap = "";	
	float transparencymapblur = 0;
	float trans_glossy = 0;
	string trans_glossymap = "";	
	float trans_glossymapblur = 0;
	float transparent_samples = 0;
	float index_of_refraction = 1;
	string index_of_refractionmap = "";	
	float index_of_refractionmapblur = 0;
	float inverttrans = 0;
	float usealphatrans = 0;
	float scaletrans = 1;
	output varying color transparencycolor = 0;
	float reflect_inuse = 0;
	color reflectivity = 0;
	string reflectivitymap = "";	
	float reflectivitymapblur = 0;
	float reflect_glossy = 0;
	float reflect_samples = 0;
	output varying color reflcolor = 0;
	output varying color refrcolor = 0;
	float invertrefl = 0;
	float usealpharefl = 0;
	float scalerefl = 1;
	float notrace = 0;
	color radiance = 1;
	string radiancemap = "";	
	float radiancemapblur = 0;
	float radiance_filter = 0;
	float translucent_inuse = 0;
	float translucency = 0;
	float incand_inuse = 0;
	color incandescence = 1;
	string incandescencemap = "";	
	float incandescencemapblur = 0;
	float inc_inten = 0;
	float invert_s = 0;
	float invert_t = 0;
	float doOcclusion = 0;
	float occlusion_samples = 16;
	float maxdist = 1000000;
	float doIndirect = 0;
	float indirect_samples = 16;
	output varying color occlusioncolor = 0;
	output varying color indirectcolor = 0;
	output varying color causticcolor = 0;
	output varying color shadowcolor = 0;
)
{
	normal Nf;
	color radiancecolor = 1;
	vector refrDir, reflDir;
	float kr, kt;
	normal Nn = normalize(N);
	vector In = normalize(I);

	float ss, tt;
	if (invert_s != 0) ss = 1 - s;
	else ss = s;
  	if (invert_t != 0) tt = 1 - t;
 	else tt = t;

	// bump map
	if(bumpmap != "") {
        	float bumpvalue = float texture(bumpmap, ss, tt, "blur", bumpmapblur);
		bumpvalue *= length(vector bumpval);       
	 	point PP = transform("shader", P);
       	normal Ns = normalize(ntransform("shader", N));
        	PP += bumpvalue * Ns;
        	PP = transform("shader", "current", PP);
        	Nf = calculatenormal(PP);

        	if(usenormals != 0) {
            	normal Nd = normalize(N) - normalize(Ng);
            	Nf = normalize(Nf) + Nd;
        	}

        	if(truedisplacement != 0) {
            	P = PP;
        	}
	}
	else {
		Nf = N;
	}

	Nf = faceforward(normalize(Nf), I);
	vector V = normalize(-I);

	float index_of_refractionval = index_of_refraction;
	if (index_of_refractionmap != "") {
		index_of_refractionval *= texture(index_of_refractionmap , ss, tt, "blur", index_of_refractionmapblur);
	}

	float eta = (In.Nn < 0) ? 1/index_of_refractionval : index_of_refractionval;
	fresnel(In, Nf, eta, kr, kt, reflDir, refrDir);
    	kt = 1 - kr;

	if (diffusemap != "") {
		diffusecolor *= diffuse_inuse * texture(diffusemap, ss, tt, "blur", diffusemapblur);
	}
	else {
		diffusecolor = diffuse_inuse * diffuse;
	}

	if (ambientmap != "") {
		ambientcolor *= diffuse_inuse* texture(ambientmap, ss, tt, "blur", ambientmapblur);
	}
	else {
		ambientcolor = diffuse_inuse* ambient;
	}

	if (radiancemap != "") {
		radiancecolor *= texture(radiancemap, ss, tt, "blur", radiancemapblur);
	}
	else {
		radiancecolor = radiance;
	}

	if (transparencymap != "") {
		transparencycolor *= texture(transparencymap, ss, tt, "blur", transparencymapblur);
	}
	else {
		transparencycolor = transparency;
	}
	if (inverttrans != 0) {
		transparencycolor = 1 - transparencycolor;
	}

	float trans_glossyval = trans_glossy;
	if (trans_glossymap != "") {
		trans_glossyval *= texture(trans_glossymap, ss, tt, "blur", trans_glossymapblur);
	}

#ifndef RDC
	uniform float reflsamples = reflect_samples;
	if (reflect_samples == 0) reflsamples = 1;
	if (notrace == 0 && reflect_inuse != 0) {
#ifdef AIR
		reflcolor = trace(P, reflDir, "blur", reflect_glossy, "samples", reflsamples);
#else
            color hitc = 0;
            gather("illuminance", P, reflDir, reflect_glossy, reflsamples, "surface:Ci", hitc) {
			reflcolor += hitc;	
            }
            reflcolor /= reflect_samples;
#endif
	}
#endif

#ifndef RDC
	color hitc = 0;
	if (notrace == 0 && refract_inuse != 0) {
#ifdef AIR
		refrcolor = trace(P, refrDir, "blur", trans_glossyval, "samples", transparent_samples);
#else
		gather("illuminance", P, refrDir, trans_glossyval, transparent_samples, "surface:Ci", hitc) {
			if (kt > 0) {
				refrcolor += kt * hitc;
			}
		}
		refrcolor /= transparent_samples;
#endif
	}
#endif

	color occ = 0;
#ifndef RDC
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
#endif

	if (transparent_samples == 0) {
		Oi = (1 - scaletrans * transparencycolor) * Os;
	}
	else {
		Oi = 1;
	}

  	/* finally */
	diffusecolor *= diffuse(Nf);
	ambientcolor *= ambient();

    	Ci = (1 - scaletrans * transparencycolor) *  Cs * (ambientcolor + occ + diffusecolor + indirectcolor + 
            translucent_inuse * translucency * diffuse * diffuse(-Nf)) + incand_inuse * inc_inten * incandescence;  

	Ci = mix(Ci, reflcolor, scalerefl * reflectivity);
	Ci = mix(Ci, refrcolor, scaletrans * transparencycolor);

#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	Ci += causticcolor;
#endif
}

