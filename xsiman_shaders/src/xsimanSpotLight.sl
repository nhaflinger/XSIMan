light xsimanSpotLight (
    float intensity = 1;
    color lightcolor = 1;
    color shadowcolor = 0;
    point from = point "shader" (0,0,0);	
    point to = point "shader" (0,0,1);
    float coneangle = radians(30);
    float conedeltaangle = radians(5);
    float beamdistribution = 2;
    float falloffexponent = 0;
    string shadowname = "";
    float shadowsamples = 16;
    float shadowblur = 0.01;
    float shadowbias = 0.01;
    output float __nonspecular = 0;
    output float __nondiffuse = 0;
    output varying color __shadowatten = 0;
    output string __category = "";
)
{
    color lcol = lightcolor;
    float atten, cosangle, falloff;
    uniform vector A = (to - from) / length(to - from);
    uniform float cosoutside = cos(coneangle),
		  cosinside = cos(coneangle - conedeltaangle);
    if (conedeltaangle >= coneangle)
        cosinside = 1;

    illuminate(from, A, coneangle) {
	cosangle = L.A / length(L); 
	falloff = pow(length(L) , falloffexponent);
	atten = pow(cosangle, beamdistribution) / falloff;
	atten *= smoothstep(cosoutside, cosinside, cosangle);
	__shadowatten = atten * intensity;
	if (shadowname != "") {
		float shadatten = shadow(shadowname, Ps, "samples", shadowsamples,
			"blur", shadowblur, "bias", shadowbias);
		if (shadatten > 0) {
			lcol = mix(lcol, shadowcolor, shadatten);
		}
		__shadowatten = intensity*(color(1) - mix(color(1.0), shadowcolor, shadatten));
	}	    
	Cl = atten * intensity * lcol;
    }
}
