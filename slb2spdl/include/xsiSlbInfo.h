//*****************************************************************************
// File: xsiSlbInfo.h
// Description: xsiSlbInfo object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiSlbInfo_H
#define xsiSlbInfo_H

#include "xsiman.h"

using namespace std;


//*****************************************************************************
//    class: xsiSlbInfo xsiSlbInfo.h
//*****************************************************************************

class xsiSlbInfo
{
public:
	xsiSlbInfo();
	~xsiSlbInfo();
	XSI::CStatus setShader(CString);
	XSI::CString getShaderName();
	long getNumArgs();
	void write(FILE*, XSI::CRef&);
	CString getArgType(ShaderArgType);
	CString getArgDef(ShaderArgType, void*);
	CString getPRManArgType(ShaderArgType);
	CString getDetailType(ShaderDetailType);
	ShaderArgType xsiSlbInfo::getSlbType(char*);
	ShaderDetailType xsiSlbInfo::getSlbDetail(char*);

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