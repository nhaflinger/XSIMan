

surface 
xsimanStrauss(
	color lights = 0;
	color ambienceval = 0;
	color bumpval = 0;
	string bumpmap = "";
	float bumpmapblur = 0;
	float usenormals = 0;
	float truedisplacement = 0;
	float diffuse_inuse = 1;
	color diffuse = 0.7;
	string diffusemap = "";	
	float diffusemapblur = 0;	
	output varying color diffusecolor = 1;
	float specular_inuse = 1;
	color specular = 1.0;
	string specularmap = "";	
	float specularmapblur = 0;	
	output varying color specularcolor = 1;
	float smoothness = .1;
	float metalness = .1;
	float refract_inuse = 1;
	color transparency = 0;
	string transparencymap = "";	
	float transparencymapblur = 0;	
	output varying color transparencycolor = 0;
	float trans_glossy = 0;
	string trans_glossymap = "";	
	float trans_glossymapblur = 0;
	float transparent_samples = 0;
	float ior = 1;
	float index_of_refraction = 1;
	string index_of_refractionmap = "";	
	float index_of_refractionmapblur = 0;
	float inverttrans = 0;	
	output varying color refrcolor = 0;
	float usealphatrans = 0;
	float scaletrans = 1;
	float reflect_inuse = 1;
	color reflectivity = 0;
	string reflectivitymap = "";	
	float reflectivitymapblur = 0;	
	output varying color reflcolor = 0;
	float reflect_glossy = 0;
	float reflect_samples = 0;
	float invertrefl = 0;
	float usealpharefl = 0;
	float scalerefl = 1;
	float notrace = 1;
	color radiance = 1;
	string radiancemap = "";	
	float radiancemapblur = 0;
	float radiance_filter = 0;
	float translucent_inuse = 1;
	float translucency = 0;
	float incand_inuse = 1;
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

	if (specularmap != "") {
		specularcolor *= specular_inuse* texture(specularmap, ss, tt, "blur", specularmapblur);
	}
	else {
		specularcolor = specular_inuse* specular;
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

	if (radiancemap != "") {
		radiancecolor *= texture(radiancemap, ss, tt, "blur", radiancemapblur);
	}
	else {
		radiancecolor = radiance;
	}

	float trans_glossyval = trans_glossy;
	if (trans_glossymap != "") {
		trans_glossyval *= texture(trans_glossymap, ss, tt, "blur", trans_glossymapblur);
	}

	if (notrace == 0 && reflect_inuse != 0) {
            color hitc = 0;
            gather("illuminance", P, reflDir, reflect_glossy, reflect_samples, "surface:Ci", hitc) {
			reflcolor += hitc;	
            }
            reflcolor /= reflect_samples;
	}

	color hitc = 0;
	if (refract_inuse != 0) {
		gather("illuminance", P, refrDir, trans_glossyval, transparent_samples, "surface:Ci", hitc) {
			if (kt > 0) {
				refrcolor += kt * hitc;
			}
		}
		refrcolor /= transparent_samples;
	}

	color speccolor = 0;
      speccolor = specular(Nf, V, smoothness);
	/* multiply specular by diffuse to give more metallic appearance */
	specularcolor *= speccolor * diffuse;

	if (transparent_samples == 0) {
		Oi = (1 - scaletrans * transparencycolor) * Os;
	}
	else {
		Oi = 1;
	}

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
#endif

#ifndef RDC
	if(notrace == 0 && doIndirect != 0) {
		indirectcolor = indirectdiffuse(P, Nf, indirect_samples) * radiancecolor;
	}
#endif

  	/* finally */

	float diffuseAdj = clamp(1 - smoothness*metalness, 0, 1);
	float diffuseRef = clamp(1 - (pow(smoothness,3)), 0, 1);

  	/* finally */
	color ambientcolor = diffuse_inuse * diffuse * (ambient() + ambienceval)*clamp(1 - smoothness, 0, 1);
	diffusecolor = diffusecolor * diffuse(Nf) * diffuseAdj * diffuseRef;
	
	if (specular_inuse != 0) {
		float specularExp = 3. / clamp(1 - smoothness, 0, 1);
		color diffuseTrm = diffuse(Nf) * diffuseAdj * diffuseRef;
		color specularAdj = 1 - diffuseTrm;	
		color speccolor = mix(color(1), diffuse, clamp(metalness,0,1));
		specularcolor = specularAdj * phong(Nf, V, specularExp) * speccolor;
	}
	else {
		specularcolor = 0;
	}
	
    	Ci = (1 - scaletrans * transparencycolor) * Cs * (ambientcolor + occ + diffusecolor + 		
		indirectcolor + translucent_inuse * translucency * diffuse * diffuse(-Nf)) + 
		specularcolor + incand_inuse * inc_inten * incandescence;  

	Ci = mix(Ci, reflcolor, scalerefl * reflectivity);
	Ci = mix(Ci, refrcolor, scaletrans * transparencycolor);
 
#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	Ci += causticcolor;
#endif
}

