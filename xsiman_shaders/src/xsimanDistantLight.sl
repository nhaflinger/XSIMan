light xsimanDistantLight (
    float intensity = 1;
    color lightcolor = 1;
    color shadowcolor = 0;
    point from = point "shader" (0,0,0);
    point to = point "shader" (0,0,1);
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
    solar(to - from, 0.0) {
        __shadowatten = intensity;
	  if (shadowname != "") {
		float shadatten = shadow(shadowname, Ps, "samples", shadowsamples,
			"blur", shadowblur, "bias", shadowbias);
            if (shadatten > 0) {
                lcol = mix(lcol, shadowcolor, shadatten);
		}
		__shadowatten = intensity*(color(1) - mix(color(1), shadowcolor, shadatten));
	  }
	  Cl = intensity * lcol;
    }
}
