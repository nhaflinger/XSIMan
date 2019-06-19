//*****************************************************************************
//  File: mixers.h
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

#ifndef CONVERSION


#define DEF_SIB_COLOR_2MIX_PARAMS \
	color sib_color_2mix_base_color; color sib_color_2mix_color1; color sib_color_2mix_weight1; \
	float sib_color_2mix_mode1;

#define SIB_COLOR_2MIX_PARAMS \
	sib_color_2mix_base_color, sib_color_2mix_color1, sib_color_2mix_weight1, sib_color_2mix_mode1


color sib_color_2mix(DEF_SIB_COLOR_2MIX_PARAMS) 
{
	color Cout;

	if (sib_color_2mix_mode1 == 2) {
		Cout = sib_color_2mix_color1 * sib_color_2mix_weight1 + sib_color_2mix_base_color;
	}
	else if (sib_color_2mix_mode1 == 4) {
		Cout = sib_color_2mix_color1 * sib_color_2mix_base_color * sib_color_2mix_weight1;
	}
	else if (sib_color_2mix_mode1 == 5) {
		Cout = sib_color_2mix_color1 * sib_color_2mix_base_color * sib_color_2mix_weight1;
	}
	else if (sib_color_2mix_mode1 == 9) {
		Cout = sib_color_2mix_base_color - sib_color_2mix_color1 * sib_color_2mix_weight1;
		setcomp(Cout, 0, abs(comp(Cout,0)));
		setcomp(Cout, 1, abs(comp(Cout,1)));
		setcomp(Cout, 2, abs(comp(Cout,2)));
	}
	else if (sib_color_2mix_mode1 == 16) {
		Cout = sib_color_2mix_color1 * sib_color_2mix_base_color * sib_color_2mix_weight1;
	}
	else { 
		Cout = sib_color_2mix_color1 * sib_color_2mix_weight1 + sib_color_2mix_base_color * 
			(1 - sib_color_2mix_weight1);
	}

	return Cout;
}


#endif