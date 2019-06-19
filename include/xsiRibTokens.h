//*****************************************************************************
// File: xsiRibTokens.h
// Description: xsiRibTokens object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibTokens_H
#define xsiRibTokens_H

#include "xsiman.h"
#include <ri.h>

//*****************************************************************************
//    class: xsiRibTokens xsiRibTokens.h
//*****************************************************************************

class xsiRibTokens
{
public:
	xsiRibTokens();
	~xsiRibTokens();
	int put(const char*, ParameterType, bool, bool, bool, unsigned int);
	char* getTokenName();
	void putDetailType(DetailType);
	DetailType getDetailType();
	const RtFloat* getTokenFloatArray();
	void assignFloat(unsigned int, RtFloat);
	void assignFloat(unsigned int, RtFloat, RtFloat, RtFloat);
	void assignFloat(unsigned int, RtFloat, RtFloat, RtFloat, RtFloat);
	void assignFloat(unsigned int, RtFloat, RtFloat, RtFloat, RtFloat, RtFloat, RtFloat, RtFloat, RtFloat, 
		RtFloat, RtFloat, RtFloat, RtFloat, RtFloat, RtFloat, RtFloat, RtFloat);
	void assignFloats(const RtFloat*);
	void assignString(const char*, unsigned int);
	RtPointer getRtPointer();
	void getRiDeclare(char*);

private:
	RtFloat* tokenFloats;
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