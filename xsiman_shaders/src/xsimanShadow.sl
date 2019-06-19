surface 
xsimanShadow(
	float min = 0;
	float max = 1;
	float rgb = 0;
	float shadowvisible = 0;
	float notrace = 1;
	float doOcclusion = 0;
	float occlusion_samples = 16;
	float maxdist = 1.0e20;
	float doIndirect = 0;
	float indirect_samples = 16;
	float invert_s = 0;
	float invert_t = 0;
)
{
	varying color shadatten = 0;
    	color shadowing = 0;

	if (shadowvisible == 1) {
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
		float mag = (comp(shadowing,0)+comp(shadowing,1)+comp(shadowing,2));
		if (mag > 0) {
			normal Nf = faceforward(normalize(N), I);
			shadowing -= shadatten2 * diffuse(Nf);
		}
  	}

	setcomp(shadowing, 0, clamp(comp(shadowing,0), min, max));
	setcomp(shadowing, 1, clamp(comp(shadowing,1), min, max));
	setcomp(shadowing, 2, clamp(comp(shadowing,2), min, max));

	Oi = shadowing;
	if (rgb != 0) {
    		Ci = shadowing;
	}
	else {    		
		Ci = 0;
	}
}


