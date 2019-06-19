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


void assignRibTokensV(std::vector<xsiRibTokens>* ribTokens, RtToken tokens[], RtPointer pointers[])
{
	char declare[1024];
	unsigned i = 0;
	std::vector<xsiRibTokens>::iterator iter = ribTokens->begin();	
	while (iter != ribTokens->end()) {
		tokens[i] = iter->getTokenName();
		pointers[i] = iter->getRtPointer();
		iter->getRiDeclare(declare);
		RiDeclare(tokens[i], declare);
		++iter;
		i++;
	}
}

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

/* rotate matrix */
void rotateMatrix(RtMatrix mo, RtMatrix mi, float angle, int axis) 
{
    RtMatrix a;

    identityMatrix(a);

    if (axis == 0) {
		a[1][1] = cos(angle);
		a[1][2] = -sin(angle);
		a[2][1] = sin(angle);
		a[2][2] = cos(angle);
    }
    else if(axis == 1) {
		a[0][0] = cos(angle);
		a[0][2] = sin(angle);
		a[2][0] = -sin(angle);
		a[2][2] = cos(angle);
    }
    else {
		a[0][0] = cos(angle);
		a[0][1] = -sin(angle);
		a[1][0] = sin(angle);
		a[1][1] = cos(angle);
    }

    multMatrix(mo, mi, a);
}

/* identity matrix */
void identityMatrix(RtMatrix a) 
{
    int i, j;

    for(j=0; j<4; j++) {
        for(i=0; i<4; i++) {
            a[i][j] = (i == j) ? 1.f : 0.f;
        }
    }
}

/* matrix multiplication */
void multMatrix(RtMatrix a, RtMatrix b, RtMatrix c) 
{
    int i, j, k;

    for(i=0; i<4; i++) {
        for(j=0; j<4; j++) {
	    a[i][j] = 0;
            for(k=0; k<4; k++) {
                a[i][j] += b[i][k] * c[k][j];
            }
        }
    }
}

// transfer properties from operator 
CValue TransferAllPropertiesAcrossGenOp(const CValue& in_operator, const CValue& in_object, const CString& in_propertytransfertype, bool in_shareproperty)
{ 
	Application app;
	CValueArray args(4); 
	CValue retval; 
	long i(0); 

	args[i++]= in_operator; 
	args[i++]= in_object; 
	args[i++]= in_propertytransfertype; 
	args[i++]= in_shareproperty; 

	CStatus st = app.ExecuteCommand(L"TransferAllPropertiesAcrossGenOp", args, retval); 

	return retval; 
} 

CRef getOperator(CustomProperty& oProp, CString in_op) 
{ 
	X3DObject oX3D(oProp.GetParent()); 
	Primitive oPrim(oX3D.GetActivePrimitive()); 

	CValue oReturn; 
	CValueArray args(0); 
	CComAPIHandler comPrimitive(oPrim.GetRef()); 
	comPrimitive.Invoke(L"ConstructionHistory", CComAPIHandler::PropertyGet, oReturn, args); 
	CComAPIHandler comConstructionHistory(oReturn); 
	comConstructionHistory.Call(L"Find", oReturn, in_op); 
	CRef oRefOp(oReturn); 
	if(oRefOp.GetClassID() == siCustomOperatorID) 
		return oRefOp; 
	else 
		return CRef(); 
} 



