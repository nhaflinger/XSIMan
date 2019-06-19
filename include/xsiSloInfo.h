//*****************************************************************************
// File: xsiSloInfo.h
// Description: xsiSloInfo object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiSloInfo_H
#define xsiSloInfo_H

#include "xsiman.h"
#include <ri.h>
#ifdef PRMAN
#include <slo.h>
#endif

using namespace std;


//*****************************************************************************
//    class: xsiSloInfo xsiSloInfo.h
//*****************************************************************************

class xsiSloInfo
{
public:
	xsiSloInfo();
	~xsiSloInfo();
	XSI::CStatus setShader(CString);
	XSI::CString getShaderName();
	long getNumArgs();
	void write(FILE*, XSI::CRef&);
	CString getArgType(ShaderArgType);
	CString getArgDef(ShaderArgType, void*);
	CString getPRManArgType(ShaderArgType);
	CString getDetailType(ShaderDetailType);

private:
	CString shaderName;
	ShaderArgType shaderType;
	long numArgs;
	vector<CString> argName;
	vector<ShaderArgType> argType;
	vector<ShaderDetailType> argDetail;
	vector<int> argArraySize;
	vector<void*> argDefault;
};
					

#endif