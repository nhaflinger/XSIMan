//*****************************************************************************
// File: xsiUtilities.cpp
// Description: utility functions
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#include "xsiUtilities.h"
#include <xsi_application.h>
#include <xsi_comapihandler.h>



CString replaceSubString(CString oldString, CString wold, CString wnew)
{
	long strlen = oldString.Length();
	long olen = wold.Length();
	CString newString;
	wchar_t curChar;
	for (long i = 0; i < strlen; i++) {
		curChar = oldString[i];
		CString substr = subString(oldString, i, i+olen-1);
		if (substr == wold) {newString += wnew; i += olen-1;}
		else newString += curChar;
	}
	return newString;
}

bool stringHasChar(CString bstr, CString lstr)
{
	long blen = bstr.Length();
	long llen = lstr.Length();
	bool hasChar = false;
	for (long i = 0; i < blen; i++) {
		CString substr = subString(bstr, i, i+llen-1);
		if (substr == lstr) {
			hasChar = true;
			break;
		}
	}

	return hasChar;
}

CString subString(CString oldString, long index1, long index2)
{
	long strlen = oldString.Length();
	CString newString;
	wchar_t curChar;
	for (long i = index1; i <= index2; i++) {
		if (i > strlen-1) break;
		curChar = oldString[i];
		newString += curChar;
	}
	return newString;
}

void getCompoundParameter(Parameter param, float vec3[3])
{
	CParameterRefArray compoundParameterRefArray(param.GetParameters());
	for(long k = 0; k < compoundParameterRefArray.GetCount(); k++) {
		Parameter compoundparam(compoundParameterRefArray[k]);
		if (compoundparam.GetName() == L"x") vec3[0] = compoundparam.GetValue();
		else if (compoundparam.GetName() == L"y") vec3[1] = compoundparam.GetValue();
		else if (compoundparam.GetName() == L"z") vec3[2] = compoundparam.GetValue();
	}
}






