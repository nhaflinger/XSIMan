//*****************************************************************************
// File: xsiRibTokens.h
// Description: xsiRibTokens object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibTokens_H
#define xsiRibTokens_H

#include "xsiman.h"

//*****************************************************************************
//    class: xsiRibTokens xsiRibTokens.h
//*****************************************************************************

class xsiRibTokens
{
public:
	xsiRibTokens();
	~xsiRibTokens();
	int set(const char*, ParameterType, bool, bool, bool, unsigned int);
	char* getTokenName();
	void setDetailType(DetailType);
	DetailType getDetailType();
	const float* getTokenFloatArray();
	void assignFloat(unsigned int, float);
	void assignFloat(unsigned int, float, float, float);
	void assignFloat(unsigned int, float, float, float, float);
	void assignFloats(const float*);
	void assignString(const char*, unsigned int);
	void* getRtPointer();
	void getRiDeclare(char*);

private:
	float* tokenFloats;
	int tokenFloatsSize;
	int tokenFloatsArraySize;
	char* tokenString;
	DetailType tokenDetailType;
	ParameterType tokenParameterType;
	char tokenName[1024];
	bool isNurbs;
	bool isArray;
	bool isUArray;
	static const char* strTokenDetailType[];
	friend class xsiRibData;
};

#endif