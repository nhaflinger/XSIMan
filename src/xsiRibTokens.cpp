//*****************************************************************************
// File: xsiRibTokens.cpp
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

#include "xsiRibTokens.h"
#include <xsi_application.h>


const char* xsiRibTokens::strTokenDetailType[] = {"constant", "uniform", "varying", "vertex", "facevarying"};


xsiRibTokens::xsiRibTokens()
{
	tokenFloats = NULL;
	tokenFloatsSize = 0;
	tokenFloatsArraySize = 0;
	tokenString = NULL;
}

xsiRibTokens::~xsiRibTokens()
{
	// these lines crash XSI
	// anyways, this stuff is being cleaned up in the xsiRibData destructor
	//if (tokenFloats) {delete tokenFloats; tokenFloats = NULL;}
	//if (tokenString) {delete tokenString; tokenString = NULL;}
}

int xsiRibTokens::put(const char* name, ParameterType type, bool useNurbs, bool useArray, bool useUArray, unsigned int arraySize)
{
	strcpy(tokenName, name);
	tokenParameterType = type;
	isNurbs = useNurbs;
	isArray = useArray;
	isUArray = useUArray;
	tokenFloatsArraySize = arraySize;

	// cleanup from previous set
	if (tokenString) {delete tokenString; tokenString = NULL;}
	if (tokenFloats) {delete tokenFloats; tokenFloats = NULL;}

	switch(tokenParameterType) {
		case FloatType:
			tokenFloatsSize = 1;
			break;
		case PointType:
			if (isNurbs) tokenFloatsSize = 4;
			else tokenFloatsSize = 3;
			break;
		case NormalType:
		case VectorType:
		case ColorType:
			tokenFloatsSize = 3;
			break;
		case MatrixType:
			tokenFloatsSize = 16;
			break;
		case HpointType:
			tokenFloatsSize = 0;
			break;
	}

	// assign memory
	if (useArray || useUArray) {
		tokenFloats = new RtFloat[arraySize * tokenFloatsSize];
	}
	else {
		tokenFloats = new RtFloat[tokenFloatsSize];
	}

	return 1;
}

char* xsiRibTokens::getTokenName()
{
	return tokenName;
}

void xsiRibTokens::putDetailType(DetailType type) 
{
	tokenDetailType = type;
}

DetailType xsiRibTokens::getDetailType() 
{
	return tokenDetailType;
}

const RtFloat* xsiRibTokens::getTokenFloatArray( )
{
	return tokenFloats;
}

void xsiRibTokens::assignFloat(unsigned int i, RtFloat x)
{
	tokenFloats[i] = x;
}

void xsiRibTokens::assignFloat(unsigned int i, RtFloat x, RtFloat y , RtFloat z)
{
	tokenFloats[3*i + 0] = x;
	tokenFloats[3*i + 1] = y;
	tokenFloats[3*i + 2] = z;
}

void xsiRibTokens::assignFloat(unsigned int i, RtFloat x, RtFloat y , RtFloat z, RtFloat w)
{
	tokenFloats[4*i + 0] = x;
	tokenFloats[4*i + 1] = y;
	tokenFloats[4*i + 2] = z;
	tokenFloats[4*i + 3] = w;
}

void xsiRibTokens::assignFloat(unsigned int i, RtFloat m0, RtFloat m1 , RtFloat m2, RtFloat m3, 
							   RtFloat m4, RtFloat m5 , RtFloat m6, RtFloat m7, 
							   RtFloat m8, RtFloat m9 , RtFloat m10, RtFloat m11, 
							   RtFloat m12, RtFloat m13 , RtFloat m14, RtFloat m15)
{
	tokenFloats[16*i + 0] = m0;
	tokenFloats[16*i + 1] = m1;
	tokenFloats[16*i + 2] = m2;
	tokenFloats[16*i + 3] = m3;
	tokenFloats[16*i + 4] = m4;
	tokenFloats[16*i + 5] = m5;
	tokenFloats[16*i + 6] = m6;
	tokenFloats[16*i + 7] = m7;
	tokenFloats[16*i + 8] = m8;
	tokenFloats[16*i + 9] = m9;
	tokenFloats[16*i + 10] = m10;
	tokenFloats[16*i + 11] = m11;
	tokenFloats[16*i + 12] = m12;
	tokenFloats[16*i + 13] = m13;
	tokenFloats[16*i + 14] = m14;
	tokenFloats[16*i + 15] = m15;
}

void xsiRibTokens::assignFloats(const RtFloat* floats)
{
	if (isArray || isUArray) {
		memcpy(tokenFloats, floats, tokenFloatsArraySize * tokenFloatsSize * sizeof(RtFloat));
	}
	else {
		memcpy(tokenFloats, floats, tokenFloatsSize * sizeof(RtFloat));
	}
}

void xsiRibTokens::assignString(const char* chars, unsigned strlen)
{
	tokenString = new char[strlen + 1];
	strcpy(tokenString, chars);
}

RtPointer xsiRibTokens::getRtPointer()
{
	RtPointer retptr;

	switch(tokenParameterType) {
		case StringType:
			retptr = (RtPointer)&tokenString;
			break;
		case FloatType:
		case PointType:
		case VectorType:
		case NormalType:
		case HpointType:
		case MatrixType:
		case ColorType:
			retptr = (RtPointer)tokenFloats;
			break;
	}
	return retptr;
}
	
void xsiRibTokens::getRiDeclare(char* declare)
{
	switch(tokenParameterType) {
		case StringType:
			sprintf(declare, "%s string", strTokenDetailType[tokenDetailType]);
			break;
		case FloatType:
			if (isUArray) sprintf(declare, "%s float[%d]", strTokenDetailType[tokenDetailType], tokenFloatsArraySize);
			else sprintf(declare, "%s float", strTokenDetailType[tokenDetailType]);
			break;
		case PointType:
			if (isNurbs) {
				if (isArray) sprintf(declare, "%s hpoint", strTokenDetailType[tokenDetailType]); 
				else sprintf(declare, "%s point", strTokenDetailType[tokenDetailType]);
			}
			else {
				sprintf(declare, "%s point", strTokenDetailType[tokenDetailType]);
			}
			break;
		case VectorType:
			sprintf(declare, "%s vector", strTokenDetailType[tokenDetailType]);
			break;
		case NormalType:
			sprintf(declare, "%s normal", strTokenDetailType[tokenDetailType]);
			break;
		case MatrixType:
			sprintf(declare, "%s matrix", strTokenDetailType[tokenDetailType]);
			break;
		case HpointType:
			break;
		case ColorType:
			sprintf(declare, "%s color", strTokenDetailType[tokenDetailType]);
			break;
	}
}
