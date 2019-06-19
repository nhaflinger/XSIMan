//*****************************************************************************
//  File: math.h
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

#ifndef MATH


#define DEF_SIB_COLOR_MATH_BASIC_PARAMS \
	color sib_color_math_basic_input1; color sib_color_math_basic_input2; float sib_color_math_basic_op;

#define SIB_COLOR_MATH_BASIC_PARAMS \
	sib_color_math_basic_input1, sib_color_math_basic_input2, sib_color_math_basic_op

#define DEF_SIB_COLOR_MATH_EXPONENT_PARAMS \
	color sib_color_math_exponent_input; color sib_color_math_exponent_factor; float sib_color_math_exponent_op;

#define SIB_COLOR_MATH_EXPONENT_PARAMS \
	sib_color_math_exponent_input, sib_color_math_exponent_factor, sib_color_math_exponent_op

#define DEF_SIB_COLOR_MATH_UNARY_PARAMS \
	color sib_color_math_unary_input; float sib_color_math_unary_op;

#define SIB_COLOR_MATH_UNARY_PARAMS \
	sib_color_math_unary_input, sib_color_math_unary_op

#define DEF_SIB_COLOR_MATH_LOGIC_PARAMS \
	color sib_color_math_logic_input1; color sib_color_math_logic_input2; float sib_color_math_logic_op;

#define SIB_COLOR_MATH_LOGIC_PARAMS \
	sib_color_math_logic_input1, sib_color_math_logic_input2, sib_color_math_logic_op

#define DEF_MIB_COLOR_AVERAGE_PARAMS \
	color mib_color_average_input; float mib_color_average_factor;

#define MIB_COLOR_AVERAGE_PARAMS \
	mib_color_average_input, mib_color_average_factor

#define DEF_SIB_INTERP_LINEAR_PARAMS \
	float sib_interp_linear_input; float sib_interp_linear_oldrange_min; float sib_interp_linear_oldrange_max; \
	float sib_interp_linear_newrange_min; float sib_interp_linear_newrange_max;

#define SIB_INTERP_LINEAR_PARAMS \
	sib_interp_linear_input, sib_interp_linear_oldrange_min, sib_interp_linear_oldrange_max, \
	sib_interp_linear_newrange_min, sib_interp_linear_newrange_max

#define DEF_SIB_SCALAR_MATH_BASIC_PARAMS \
	float sib_scalar_math_basic_input1; float sib_scalar_math_basic_input2; float sib_scalar_math_basic_op;

#define SIB_SCALAR_MATH_BASIC_PARAMS \
	sib_scalar_math_basic_input1, sib_scalar_math_basic_input2, sib_scalar_math_basic_op

#define DEF_SIB_SCALAR_MATH_EXPONENT_PARAMS \
	float sib_scalar_math_exponent_input; float sib_scalar_math_exponent_factor; float sib_scalar_math_exponent_op;

#define SIB_SCALAR_MATH_EXPONENT_PARAMS \
	sib_scalar_math_exponent_input, sib_scalar_math_exponent_factor, sib_scalar_math_exponent_op

#define DEF_SIB_SCALAR_MATH_UNARY_PARAMS \
	float sib_scalar_math_unary_input; float sib_scalar_math_unary_op;

#define SIB_SCALAR_MATH_UNARY_PARAMS \
	sib_scalar_math_unary_input, sib_scalar_math_unary_op

#define DEF_SIB_SCALAR_MATH_LOGIC_PARAMS \
	float sib_scalar_math_logic_input1; float sib_scalar_math_logic_input2; float sib_scalar_math_logic_op;

#define SIB_SCALAR_MATH_LOGIC_PARAMS \
	sib_scalar_math_logic_input1, sib_scalar_math_logic_input2, sib_scalar_math_logic_op


color sib_color_math_basic (DEF_SIB_COLOR_MATH_BASIC_PARAMS) 
{
	color Cout = 0;

	if (sib_color_math_basic_op == 0) {
		Cout = sib_color_math_basic_input1 + sib_color_math_basic_input2;
	}
	else if (sib_color_math_basic_op == 1) {
		Cout = sib_color_math_basic_input1 - sib_color_math_basic_input2;
	}
	else if (sib_color_math_basic_op == 2) {
		Cout = sib_color_math_basic_input1 * sib_color_math_basic_input2;
	}
	else if (sib_color_math_basic_op == 3) {
		Cout = sib_color_math_basic_input1 / sib_color_math_basic_input2;
	}

	return Cout;
}

color sib_color_math_exponent (DEF_SIB_COLOR_MATH_EXPONENT_PARAMS) 
{
	color Cout = 0;

	if (sib_color_math_exponent_op == 0) {
		setcomp(Cout, 0, pow(comp(sib_color_math_exponent_input,0), comp(sib_color_math_exponent_factor,0)));
		setcomp(Cout, 1, pow(comp(sib_color_math_exponent_input,1), comp(sib_color_math_exponent_factor,1)));
		setcomp(Cout, 2, pow(comp(sib_color_math_exponent_input,2), comp(sib_color_math_exponent_factor,2)));
	}
	else if (sib_color_math_exponent_op == 1) {
		setcomp(Cout, 0, 1/pow(comp(sib_color_math_exponent_input,0), comp(sib_color_math_exponent_factor,0)));
		setcomp(Cout, 1, 1/pow(comp(sib_color_math_exponent_input,1), comp(sib_color_math_exponent_factor,1)));
		setcomp(Cout, 2, 1/pow(comp(sib_color_math_exponent_input,2), comp(sib_color_math_exponent_factor,2)));
	}
	else if (sib_color_math_exponent_op == 2) {
		setcomp(Cout, 0, mix(0.5, comp(sib_color_math_exponent_input,0), 0.5 + comp(sib_color_math_exponent_factor,0)));
		setcomp(Cout, 1, mix(0.5, comp(sib_color_math_exponent_input,1), 0.5 + comp(sib_color_math_exponent_factor,1)));
		setcomp(Cout, 2, mix(0.5, comp(sib_color_math_exponent_input,2), 0.5 + comp(sib_color_math_exponent_factor,2)));
	}
	else if (sib_color_math_exponent_op == 3) {
		setcomp(Cout, 0, comp(sib_color_math_exponent_input,0) * comp(sib_color_math_exponent_factor,0));
		setcomp(Cout, 1, comp(sib_color_math_exponent_input,1) * comp(sib_color_math_exponent_factor,1));
		setcomp(Cout, 2, comp(sib_color_math_exponent_input,2) * comp(sib_color_math_exponent_factor,2));
	}

	return Cout;
}

color sib_color_math_unary (DEF_SIB_COLOR_MATH_UNARY_PARAMS) 
{
	color Cout = 0;

	if (sib_color_math_unary_op == 0) {
		setcomp(Cout, 0, abs(comp(sib_color_math_unary_input,0)));
		setcomp(Cout, 1, abs(comp(sib_color_math_unary_input,1)));
		setcomp(Cout, 2, abs(comp(sib_color_math_unary_input,2)));
	}
	else if (sib_color_math_unary_op == 1) {
		setcomp(Cout, 0, -comp(sib_color_math_unary_input,0));
		setcomp(Cout, 1, -comp(sib_color_math_unary_input,1));
		setcomp(Cout, 2, -comp(sib_color_math_unary_input,2));
	}
	else if (sib_color_math_unary_op == 2) {
		setcomp(Cout, 0, 1-comp(sib_color_math_unary_input,0));
		setcomp(Cout, 1, 1-comp(sib_color_math_unary_input,1));
		setcomp(Cout, 2, 1-comp(sib_color_math_unary_input,2));
	}

	return Cout;
}

float sib_color_math_logic (DEF_SIB_COLOR_MATH_LOGIC_PARAMS) 
{
	float Fout = 0;

	if (sib_color_math_logic_op == 0) {
		if (comp(sib_color_math_logic_input1, 0) == comp(sib_color_math_logic_input2, 0) &&
			comp(sib_color_math_logic_input1, 1) == comp(sib_color_math_logic_input2, 1) &&
			comp(sib_color_math_logic_input1, 2) == comp(sib_color_math_logic_input2, 2))
			Fout = 0;
	}
	else if (sib_color_math_logic_op == 1) {
		if (comp(sib_color_math_logic_input1, 0) < comp(sib_color_math_logic_input2, 0) &&
			comp(sib_color_math_logic_input1, 1) < comp(sib_color_math_logic_input2, 1) &&
			comp(sib_color_math_logic_input1, 2) < comp(sib_color_math_logic_input2, 2))
			Fout = 1;
	}
	else if (sib_color_math_logic_op == 2) {
		if (comp(sib_color_math_logic_input1, 0) > comp(sib_color_math_logic_input2, 0) &&
			comp(sib_color_math_logic_input1, 1) > comp(sib_color_math_logic_input2, 1) &&
			comp(sib_color_math_logic_input1, 2) > comp(sib_color_math_logic_input2, 2))
			Fout = 1;
	}
	else if (sib_color_math_logic_op == 3) {
		if (comp(sib_color_math_logic_input1, 0) <= comp(sib_color_math_logic_input2, 0) &&
			comp(sib_color_math_logic_input1, 1) <= comp(sib_color_math_logic_input2, 1) &&
			comp(sib_color_math_logic_input1, 2) <= comp(sib_color_math_logic_input2, 2))
			Fout = 1;
	}
	else if (sib_color_math_logic_op == 4) {
		if (comp(sib_color_math_logic_input1, 0) >= comp(sib_color_math_logic_input2, 0) &&
			comp(sib_color_math_logic_input1, 1) >= comp(sib_color_math_logic_input2, 1) &&
			comp(sib_color_math_logic_input1, 2) >= comp(sib_color_math_logic_input2, 2))
			Fout = 1;
	}

	return Fout;
}

color mib_color_average (DEF_MIB_COLOR_AVERAGE_PARAMS) 
{
	color Cout = 0;

	Cout = mib_color_average_factor * (comp(mib_color_average_input,0) + comp(mib_color_average_input,1) + 
		comp(mib_color_average_input,2)) / 3.;

	return Cout;
}

float sib_interp_linear (DEF_SIB_INTERP_LINEAR_PARAMS) 
{
	float Fout = 0;
	
	Fout = sib_interp_linear_newrange_min + (sib_interp_linear_input - sib_interp_linear_oldrange_min) * 
		(sib_interp_linear_newrange_max - sib_interp_linear_newrange_min) / 
		(sib_interp_linear_oldrange_max - sib_interp_linear_oldrange_min); 
			

	return Fout;
}

float sib_scalar_math_basic (DEF_SIB_SCALAR_MATH_BASIC_PARAMS) 
{
	float Fout = 0;

	if (sib_scalar_math_basic_op == 0) {
		Fout = sib_scalar_math_basic_input1 + sib_scalar_math_basic_input2;
	}
	else if (sib_scalar_math_basic_op == 1) {
		Fout = sib_scalar_math_basic_input1 - sib_scalar_math_basic_input2;
	}
	else if (sib_scalar_math_basic_op == 2) {
		Fout = sib_scalar_math_basic_input1 * sib_scalar_math_basic_input2;
	}
	else if (sib_scalar_math_basic_op == 3) {
		Fout = sib_scalar_math_basic_input1 / sib_scalar_math_basic_input2;
	}
	else if (sib_scalar_math_basic_op == 4) {
		Fout = MIN(sib_scalar_math_basic_input1, sib_scalar_math_basic_input2);
	}
	else if (sib_scalar_math_basic_op == 5) {
		Fout = MAX(sib_scalar_math_basic_input1, sib_scalar_math_basic_input2);
	}

	return Fout;
}

float sib_scalar_math_exponent (DEF_SIB_SCALAR_MATH_EXPONENT_PARAMS) 
{
	float Fout = 0;

	if (sib_scalar_math_exponent_op == 0) 
		Fout = pow(sib_scalar_math_exponent_input, sib_scalar_math_exponent_factor);
	else if (sib_scalar_math_exponent_op == 1) 
		Fout = 1/pow(sib_scalar_math_exponent_input, sib_scalar_math_exponent_factor);
	else if (sib_scalar_math_exponent_op == 2) 
		Fout = mix(0.5, sib_scalar_math_exponent_input, 0.5 + sib_scalar_math_exponent_factor);
	else if (sib_scalar_math_exponent_op == 3) 
		Fout = sib_scalar_math_exponent_input * sib_scalar_math_exponent_factor;

	return Fout;
}

float sib_scalar_math_unary (DEF_SIB_SCALAR_MATH_UNARY_PARAMS) 
{
	float Fout = 0;

	if (sib_scalar_math_unary_op == 0) 
		Fout = abs(sib_scalar_math_unary_input);
	else if (sib_scalar_math_unary_op == 1) 
		Fout = -sib_scalar_math_unary_input;
	else if (sib_scalar_math_unary_op == 2) 
		Fout = 1 - sib_scalar_math_unary_input;

	return Fout;
}

float sib_scalar_math_logic (DEF_SIB_SCALAR_MATH_LOGIC_PARAMS) 
{
	float Fout = 0;

	if (sib_scalar_math_logic_op == 0) {
		if (sib_scalar_math_logic_input1 == sib_scalar_math_logic_input2)
			Fout = 0;
	}
	else if (sib_scalar_math_logic_op == 1) {
		if (sib_scalar_math_logic_input1 < sib_scalar_math_logic_input2)
			Fout = 1;
	}
	else if (sib_scalar_math_logic_op == 2) {
		if (sib_scalar_math_logic_input1 > sib_scalar_math_logic_input2) 
			Fout = 1;
	}
	else if (sib_scalar_math_logic_op == 3) {
		if (sib_scalar_math_logic_input1 <= sib_scalar_math_logic_input2)
			Fout = 1;
	}
	else if (sib_scalar_math_logic_op == 4) {
		if (sib_scalar_math_logic_input1 >= sib_scalar_math_logic_input2)
			Fout = 1;
	}

	return Fout;
}


#endif
