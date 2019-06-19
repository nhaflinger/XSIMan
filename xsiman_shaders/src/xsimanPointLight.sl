light xsimanPointLight (
    float intensity = 1;
    color lightcolor = 1;
    color shadowcolor = 0;
    point from = point "shader" (0,0,0);	
    string sfpx = "";
    string sfnx = "";
    string sfpy = "";
    string sfny = "";
    string sfpz = "";
    string sfnz = "";
    float shadowblur = 0.01;
    float shadowsamples = 16;
    float shadowbias = 0.01;
    float falloffexponent = 0;    
    output float __nonspecular = 0;
    output float __nondiffuse = 0;
    output varying color __shadowatten = 0;
    output string __category = "";
)
{
    color lcol = lightcolor;
    float attenuation = 0.0;
    float Lx, Ly, Lz, AbsLx, AbsLy, AbsLz;
    vector Lrel;

    illuminate(from) {

        Lrel = vtransform("world", L);

        Lx = xcomp(Lrel);
        AbsLx = abs(Lx);
        Ly = ycomp(Lrel);
        AbsLy = abs(Ly);
        Lz = zcomp(Lrel);
        AbsLz = abs(Lz);

        if((AbsLx > AbsLy) && (AbsLx > AbsLz)) {
            if((Lx > 0.0)&&(sfpx != ""))
		    attenuation = shadow(sfpx, Ps, "samples", shadowsamples, "blur", shadowblur, "bias", shadowbias);
            else if (sfnx != "")
		    attenuation = shadow(sfnx, Ps, "samples", shadowsamples, "blur", shadowblur, "bias", shadowbias);
	  }
        else if((AbsLy > AbsLx) && (AbsLy > AbsLz)) {
            if((Ly > 0.0)&&(sfpy != ""))
		    attenuation = shadow(sfpy, Ps, "samples", shadowsamples, "blur", shadowblur, "bias", shadowbias);
            else if (sfny != "")
		    attenuation = shadow(sfny, Ps, "samples", shadowsamples, "blur", shadowblur, "bias", shadowbias);
	  }
        else if((AbsLz > AbsLy) && (AbsLz > AbsLx)) {
            if((Lz > 0.0)&&(sfpz != "")) 
		    attenuation = shadow(sfpz, Ps, "samples", shadowsamples, "blur", shadowblur, "bias", shadowbias);
            else if (sfnz != "") 
		    attenuation = shadow(sfnz, Ps, "samples", shadowsamples, "blur", shadowblur, "bias", shadowbias);
	  }
        __shadowatten = intensity;
        if (attenuation > 0) {
                lcol = mix(lcol, shadowcolor, attenuation);
        }
	      __shadowatten = intensity*(color(1) - mix(color(1.0), shadowcolor, attenuation));
        
        /* calculate light contribution  distance square fall off */
        float falloff = pow(length(L), falloffexponent);
        Cl = intensity * lcol / falloff;
    }
}
