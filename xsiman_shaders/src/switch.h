//*****************************************************************************
//  File: switch.h
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

#ifndef SWITCH


#define DEF_MIB_TWOSIDED_PARAMS \
	color mib_twosided_front; color mib_twosided_back; 

#define MIB_TWOSIDED_PARAMS \
	mib_twosided_front, mib_twosided_back

#define DEF_SIB_SCALAR_SWITCH_PARAMS \
	float sib_scalar_switch_input; float sib_scalar_switch_scalar1; float sib_scalar_switch_scalar2;

#define SIB_SCALAR_SWITCH_PARAMS \
	sib_scalar_switch_input, sib_scalar_switch_scalar1, sib_scalar_switch_scalar2

#define DEF_SIB_VECTOR_SWITCH_PARAMS \
	float sib_vector_switch_input; vector sib_vector_switch_vector1; vector sib_vector_switch_vector2;

#define SIB_VECTOR_SWITCH_PARAMS \
	sib_vector_switch_input, sib_vector_switch_vector1, sib_vector_switch_vector2

#define DEF_SIB_COLOR_SWITCH_PARAMS \
	float sib_color_switch_switch; color sib_color_switch_input1; color sib_color_switch_input2;

#define SIB_COLOR_SWITCH_PARAMS \
	sib_color_switch_switch, sib_color_switch_input1, sib_color_switch_input2


color mib_twosided(DEF_MIB_TWOSIDED_PARAMS) 
{
	extern vector I;
	extern normal N;
	color Cout;
	
	vector V = -I;
	if (N.V >= 0) Cout = mib_twosided_front;
	else Cout = mib_twosided_back;

	return Cout;
}

float sib_scalar_switch(DEF_SIB_SCALAR_SWITCH_PARAMS)
{
	float Fout;
	
	if (sib_scalar_switch_input != 0) Fout = sib_scalar_switch_scalar2;
	else Fout = sib_scalar_switch_scalar1;

	return Fout;
}

vector sib_vector_switch(DEF_SIB_VECTOR_SWITCH_PARAMS)
{
	vector Vout;
	
	if (sib_vector_switch_input != 0) Vout = sib_vector_switch_vector2;
	else Vout = sib_vector_switch_vector1;

	return Vout;
}

color sib_color_switch(DEF_SIB_COLOR_SWITCH_PARAMS)
{
	color Cout;
	
	if (sib_color_switch_switch != 0) Cout = sib_color_switch_input2;
	else Cout = sib_color_switch_input1;

	return Cout;
}


#endif