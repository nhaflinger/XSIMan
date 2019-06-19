surface xsimanParticle(
	float shape = 0;
	float normalType = 0;
	float falloffType = 0;
	float falloffX = 0.5;
	float falloffY = 0.5;
	float exponent = 3;)
{
	float dist, maxdist, falloffExponent, atten;
	normal Nf = faceforward(normalize(N), I);

	maxdist = 0.5;
	dist = sqrt((u - falloffX)*(u - falloffX) + (v - falloffY)*(v - falloffY));

	falloffExponent = falloffType;
	if (falloffType >= 3) {
		falloffExponent = exponent;
	}

	atten = clamp(1 - pow(dist/maxdist, falloffExponent), 0, 1);

	if(falloffType == 0) atten = 1;

	Oi = atten * Os;
	Ci = atten * Cs * diffuse(Nf);
}