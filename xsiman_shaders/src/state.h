//*****************************************************************************
//  File: state.h
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

#ifndef STATE


#define DEF_SIB_STATE_SCALAR_PARAMS \
	float sib_state_scalar_mode; 

#define SIB_STATE_SCALAR_PARAMS \
	sib_state_scalar_mode

#define DEF_SIB_STATE_VECTOR_PARAMS \
	float sib_state_vector_mode; 

#define SIB_STATE_VECTOR_PARAMS \
	sib_state_vector_mode



float sib_state_scalar(DEF_SIB_STATE_SCALAR_PARAMS)
{
	extern vector I;
	extern normal N;
	extern point P;
	extern uniform float dtime;
	extern float s, t;
	float Fout;

	if (sib_state_scalar_mode == 0) {
		point Pndc = transform("raster", P);
		Fout = xcomp(Pndc);
	}
	else if (sib_state_scalar_mode == 1) {
		point Pndc = transform("raster", P);
		Fout = ycomp(Pndc);
	}
	else if (sib_state_scalar_mode == 2) {
		Fout = dtime;
	}
	else if (sib_state_scalar_mode == 5) {
		normal Nf = faceforward(normalize(N), I);
		vector V = -normalize(I);
		Fout = Nf . V;
	}
	else if (sib_state_scalar_mode == 6) {
		float success = rayinfo("length", Fout);
		if (success == 0) {
			Fout = length(I);
		}
	}
	else if (sib_state_scalar_mode == 7) {
		Fout = s;
	}
	else if (sib_state_scalar_mode == 8) {
		Fout = t;
	}
	else {
		Fout = 1.0;
	}

	return Fout;
}

vector sib_state_vector(DEF_SIB_STATE_VECTOR_PARAMS)
{
	extern vector I;
	extern normal N;
	extern normal Ng;
	extern point P;
	extern uniform point E;
#ifndef DELIGHT
	extern vector dPdtime;
#endif
	vector Vout;
	
	if (sib_state_vector_mode == 0) {
		vector V = normalize(I);
		Vout = vtransform("shader", V);
	}
	else if (sib_state_vector_mode == 1) {
		point PE = transform("shader", E);
		Vout = vector PE;
	}
	else if (sib_state_vector_mode == 2) {
		point Pint = transform("shader", P);
		Vout = vector Pint;
	}
	else if (sib_state_vector_mode == 3) {
		vector NV = vtransform("shader", vector N);
		Vout = normalize(NV);
	}
	else if (sib_state_vector_mode == 4) {
		vector NV = vtransform("shader", vector Ng);
		Vout = normalize(NV);
	}
	else if (sib_state_vector_mode == 5) {
#ifndef DELIGHT
		vector V = normalize(dPdtime);
		Vout = vtransform("shader", V);
#endif
	}
	else if (sib_state_vector_mode == 6) {
		Vout = 0;
	}

	return Vout;
}



#endif