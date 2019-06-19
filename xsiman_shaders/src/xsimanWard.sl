color
LocIllumWardAnisotropic (normal N;  vector V;
                         vector xdir;  float xroughness, yroughness;)
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

surface 
xsimanWard(
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
	color ambient = 0.3;
	string ambientmap = "";		
	float ambientmapblur = 0;	
	output varying color ambientcolor = 1;
	color image = 0;
	string imagemap = "";
	float imagemapblur = 0;
	float specular_inuse = 1;
	color glossy = 0;
	string glossymap = "";
	float glossymapblur = 0;
	float shiny_u = 4;
	string shiny_umap = "";
	float shiny_umapblur = 0;
	float shiny_v = 4;
	string shiny_vmap = "";
	float shiny_vmapblur = 0;
	float surfderiv_mode = -5;	
	output varying color glossycolor = 1;
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
	color imagecolor = 1;
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
		diffusecolor = diffuse_inuse* diffuse;
	}
	
	if (ambientmap != "") {
		ambientcolor *= diffuse_inuse * texture(ambientmap, ss, tt, "blur", ambientmapblur);
	}
	else {
		ambientcolor = diffuse_inuse* ambient;
	}

	if (glossymap != "") {
		glossycolor *= specular_inuse * texture(glossymap, ss, tt, "blur", glossymapblur);
	}
	else {
		glossycolor = specular_inuse * glossy;
	}

	if (imagemap != "") {
		imagecolor *= texture(imagemap, ss, tt, "blur", imagemapblur);
	}
	else {
		imagecolor = image;
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

	float shiny_uval = shiny_u;
	if (shiny_umap != "") {
		shiny_uval *= texture(shiny_umap, ss, tt, "blur", shiny_umapblur);
	}

	float shiny_vval = shiny_v;
	if (shiny_vmap != "") {
		shiny_vval *= texture(shiny_vmap, ss, tt, "blur", shiny_vmapblur);
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
		refrcolor = trace(P, refrDir, "blur", trans_glossy, "samples", transparent_samples);
#else
		gather("illuminance", P, refrDir, trans_glossy, transparent_samples, "surface:Ci", hitc) {
			if (kt > 0) {
				refrcolor += kt * hitc;
			}
		}
		refrcolor /= transparent_samples;
#endif
	}
#endif

	float imagedir = 2 * PI * (comp(imagecolor,0) + comp(imagecolor,1) + comp(imagecolor,2)) / 3;
	vector xdir; 
	if (surfderiv_mode == -5) {
		xdir = cos(imagedir) * normalize(dPdu) + sin(imagedir) * normalize(dPdv);
	}
	else {	
		xdir = -sin(imagedir) * normalize(dPdu) + cos(imagedir) * normalize(dPdv);
	}

	float roughness_u = 1 / shiny_uval;
	float roughness_v = 1 / shiny_vval;
	color spec = LocIllumWardAnisotropic(Nf, V, xdir, roughness_u, roughness_v);
	glossycolor *= spec;

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

	if(notrace == 0 && doIndirect != 0) {
		indirectcolor = indirectdiffuse(P, Nf, indirect_samples) * radiancecolor;
	}
#endif

  	/* finally */
	diffusecolor *= diffuse(Nf);
	ambientcolor *= ambient() + ambienceval;

    	Ci = (1 - scaletrans * transparencycolor) * Cs * (ambientcolor + occ + diffusecolor + 
		indirectcolor + translucent_inuse * translucency * diffuse * diffuse(-Nf)) + 
		glossycolor + incand_inuse * inc_inten * incandescence;  

	Ci = mix(Ci, reflcolor, scalerefl * reflectivity);
	Ci = mix(Ci, refrcolor, scaletrans * transparencycolor);
 
#ifndef DELIGHT
	/* caustics */
    	causticcolor = radiancecolor * caustic(P, N);
	Ci += causticcolor;
#endif
}

