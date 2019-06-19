//*****************************************************************************
// File: xsiSloInfo.cpp
// Description: xsiSloInfo object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#include "xsiSloInfo.h"
#include "xsiUtilities.h"
#include <xsi_application.h>
#include <xsi_comapihandler.h>
#include <xsi_context.h>

#ifdef DELIGHT
#include <slo.h>
#endif

xsiSloInfo::xsiSloInfo()
{
}

xsiSloInfo::~xsiSloInfo()
{
	// cleanup
	numArgs = 0;
	argName.clear();
	argType.clear();
	argDetail.clear();
	argArraySize.clear();
	for (unsigned i = 0; i < argDefault.size(); i++) {
		delete argDefault[i];
	}
}

CStatus xsiSloInfo::setShader(CString shaderFilePath) 
{
	Application app;
	long slen = shaderFilePath.Length();
	CString fileExt = subString(shaderFilePath, slen-3, slen-1);

	// we seem to need to replace back slashes with forward slashes?
	CString nuShaderFilePath = subString(shaderFilePath, 0, slen-5);
	nuShaderFilePath = replaceSubString(nuShaderFilePath, L"\\", L"/");

#ifdef DELIGHT 
	if (fileExt != CString(L"sdl")) { 
		app.LogMessage(L"This is not a 3Delight shader!");
		return CStatus::Fail;
	}

	char* shaderfilepath = NULL;
	XSIW2A(&shaderfilepath, nuShaderFilePath.GetWideString());
	int status = Slo_SetShader(shaderfilepath);

	if (status != 0) {
		app.LogMessage(L"Error finding shader " + nuShaderFilePath);
		return CStatus::Fail;
	}
 
	// the 3Delight version of Slo_GetName returns the full path if that is how Slo_SetShader is set!
	const char* shadername;
	shadername = Slo_GetName();
	wchar_t* wshadername;
	XSIA2W(&wshadername, shadername);
	shaderName = CValue(wshadername).GetAsText(); 
	shaderType = (ShaderArgType)Slo_GetType();
	numArgs = Slo_GetNArgs();

	for (int i = 1; i <= numArgs; i++) {
		SLO_VISSYMDEF *arg;
		arg = Slo_GetArgById(i);
		if (arg->svd_valisvalid) {

			wchar_t* svd_name; 
			XSIA2W(&svd_name, arg->svd_name);
			argName.push_back(CValue(svd_name).GetAsText());
			argType.push_back((ShaderArgType)arg->svd_type);
			argArraySize.push_back(arg->svd_arraylen);
			argDetail.push_back((ShaderDetailType)arg->svd_detail);

			switch (arg->svd_type) {
				case SLO_TYPE_STRING: {
					char *slostr = new char[strlen(arg->svd_default.stringval) + 1];
					strcpy(slostr, arg->svd_default.stringval);
					argDefault.push_back((void*)slostr);
					break;
				}
				case SLO_TYPE_SCALAR: {
					if (arg->svd_arraylen > 0) {
						SLO_VISSYMDEF *elem;
						float *slofloat = new float[arg->svd_arraylen];
						for (int j = 0; j < arg->svd_arraylen; j++) {
							elem = Slo_GetArrayArgElement(arg, j);
							slofloat[j] = *elem->svd_default.scalarval;
						}
						argDefault.push_back((void*)slofloat);
					} 
					else {
						float *slofloat = new float[1];
						slofloat[0] = *arg->svd_default.scalarval;
						argDefault.push_back((void*)slofloat);
					}
					break;
				}
				case SLO_TYPE_COLOR:
				case SLO_TYPE_POINT:
				case SLO_TYPE_VECTOR:
				case SLO_TYPE_NORMAL: {
					float *slopnt = new float[3];
					slopnt[0] = arg->svd_default.pointval->xval;
					slopnt[1] = arg->svd_default.pointval->yval;
					slopnt[2] = arg->svd_default.pointval->zval;
					argDefault.push_back((void*)slopnt);
					break;
				}
				case SLO_TYPE_MATRIX: {
					float *slomat = new float[16];
					const char* svd_spacename = arg->svd_spacename;
					slomat[0] = arg->svd_default.matrixval[0];
					slomat[1] = arg->svd_default.matrixval[1];
					slomat[2] = arg->svd_default.matrixval[2];
					slomat[3] = arg->svd_default.matrixval[3];
					slomat[4] = arg->svd_default.matrixval[4];
					slomat[5] = arg->svd_default.matrixval[5];
					slomat[6] = arg->svd_default.matrixval[6];
					slomat[7] = arg->svd_default.matrixval[7];
					slomat[8] = arg->svd_default.matrixval[8];
					slomat[9] = arg->svd_default.matrixval[9];
					slomat[10] = arg->svd_default.matrixval[10];
					slomat[11] = arg->svd_default.matrixval[11];
					slomat[12] = arg->svd_default.matrixval[12];
					slomat[13] = arg->svd_default.matrixval[13];
					slomat[14] = arg->svd_default.matrixval[14];
					slomat[15] = arg->svd_default.matrixval[15];
					argDefault.push_back((void*)slomat);
					break;
				}
				default:
					argDefault.push_back(NULL);
					break;
			}
		}
		else {
		}
	}

	Slo_EndShader();
#endif

	return CStatus::OK;
}

CString xsiSloInfo::getShaderName() 
{
	return shaderName;
}

long xsiSloInfo::getNumArgs()
{
	return numArgs;
}

void xsiSloInfo::write(FILE* stream, CRef& in_ref)
{
	Application app;
	Context ctxt(in_ref); 
	CValue guid, arg;
	
	// Using the COMAPIHandler
	CComAPIHandler xsifactory;
	xsifactory.CreateInstance(L"XSI.Factory");

	fwprintf(stream, L"# SPDL Generated by XSIMan\n");
	fwprintf(stream, L"SPDL\n");
	fwprintf(stream, L"Version = \"2.0.0.2\";\n");

	xsifactory.Call(L"CreateGuid", guid);
	fwprintf(stream, CString(L"Reference = \"" + guid.GetAsText() + L"\";\n").GetWideString());

	fwprintf(stream, CString(L"PropertySet \"" + shaderName + L"_params\"\n").GetWideString());
	fwprintf(stream, L"{\n");

	fwprintf(stream, L"\tParameter \"out\" output\n");

	fwprintf(stream, L"\t{\n");
	xsifactory.Call(L"CreateGuid", guid);
	fwprintf(stream, L"\t\ttitle = \"Output\";\n");
	fwprintf(stream, CString(L"\t\tguid = \"" + guid.GetAsText() + L"\";\n").GetWideString());

	if (getArgType(shaderType) == L"material" || getArgType(shaderType) == L"light" || 
		getArgType(shaderType) == L"volume" || getArgType(shaderType) == L"imager") {
		fwprintf(stream, L"\t\ttype = color;\n");
	}
	if (getArgType(shaderType) == L"displace") { 
		fwprintf(stream, L"\t\ttype = scalar;\n");
	}

	fwprintf(stream, L"\t\tflags  = 0;\n");
	fwprintf(stream, L"\t}\n");

	if (getArgType(shaderType) == L"material") {
		fwprintf(stream, L"\tParameter \"Cs\" input\n");
		fwprintf(stream, L"\t{\n");
		xsifactory.Call(L"CreateGuid", guid);
		fwprintf(stream, L"\t\ttitle = \"Surface Color\";\n");
		fwprintf(stream, CString(L"\t\tguid = \"" + guid.GetAsText() + L"\";\n").GetWideString());
		fwprintf(stream, L"\t\ttype = color;\n");
		fwprintf(stream, L"\t\tvalue  = 1.0 1.0 1.0 1.0;\n");
		fwprintf(stream, L"\t\tflags  = 0;\n");
		fwprintf(stream, L"\t}\n");
		fwprintf(stream, L"\tParameter \"Os\" input\n");
		fwprintf(stream, L"\t{\n");
		xsifactory.Call(L"CreateGuid", guid);
		fwprintf(stream, L"\t\ttitle = \"Surface Opacity\";\n");
		fwprintf(stream, CString(L"\t\tguid = \"" + guid.GetAsText() + L"\";\n").GetWideString());
		fwprintf(stream, L"\t\ttype = color;\n");
		fwprintf(stream, L"\t\tvalue  = 1.0 1.0 1.0 1.0;\n");
		fwprintf(stream, L"\t\tflags  = 0;\n");
		fwprintf(stream, L"\t}\n");
		fwprintf(stream, L"\tParameter \"DisplacementBound\" input\n");
		fwprintf(stream, L"\t{\n");
		xsifactory.Call(L"CreateGuid", guid);
		fwprintf(stream, L"\t\ttitle = \"Displacement Bound\";\n");
		fwprintf(stream, CString(L"\t\tguid = \"" + guid.GetAsText() + L"\";\n").GetWideString());
		fwprintf(stream, L"\t\ttype = scalar;\n");
		fwprintf(stream, L"\t\tvalue  = 0.0;\n");
		fwprintf(stream, L"\t\tflags  = 0;\n");
		fwprintf(stream, L"\t}\n");
	}

	if (getArgType(shaderType) == L"displace") {
		fwprintf(stream, L"\tParameter \"DisplacementBound\" input\n");
		fwprintf(stream, L"\t{\n");
		xsifactory.Call(L"CreateGuid", guid);
		fwprintf(stream, L"\t\ttitle = \"Displacement Bound\";\n");
		fwprintf(stream, CString(L"\t\tguid = \"" + guid.GetAsText() + L"\";\n").GetWideString());
		fwprintf(stream, L"\t\ttype = scalar;\n");
		fwprintf(stream, L"\t\tvalue  = 0.0;\n");
		fwprintf(stream, L"\t\tflags  = 0;\n");
		fwprintf(stream, L"\t}\n");
	}

	vector<CString>::iterator iter1 = argName.begin();
	vector<ShaderArgType>::iterator iter2 = argType.begin();
	vector<void*>::iterator iter3 = argDefault.begin();
	while (iter1 != argName.end()) {
		CString argname = (*iter1);
		ShaderArgType argtype = (*iter2);
		void* argdef = (*iter3);

		fwprintf(stream, CString(L"\tParameter \"" + argname + L"\" input\n").GetWideString());
		fwprintf(stream, L"\t{\n");
		fwprintf(stream, L"\t\ttitle = \"\";\n");
		xsifactory.Call(L"CreateGuid", guid);
		fwprintf(stream, CString(L"\t\tguid = \"" + guid.GetAsText() + L"\";\n").GetWideString());
		fwprintf(stream, CString(L"\t\ttype = " + getArgType(argtype) + L";\n").GetWideString());
		fwprintf(stream, CString(L"\t\tvalue = " + getArgDef(argtype, argdef) + L";\n").GetWideString());
		fwprintf(stream, L"\t\twritable = on;\n"); 
		fwprintf(stream, L"\t\tanimatable = on;\n"); 
		fwprintf(stream, L"\t\tpersistable = on;\n"); 
		fwprintf(stream, L"\t\treadable = on;\n"); 
		fwprintf(stream, L"\t\tinspectable = on;\n");
		fwprintf(stream, L"\t}\n");

		++iter1; ++iter2; ++iter3;
	}
	fwprintf(stream, L"}\n\n");

	fwprintf(stream, CString(L"MetaShader \"" + shaderName + L"_declare\"\n").GetWideString());
	fwprintf(stream, L"{\n");
	fwprintf(stream, CString(L"\tName = \"" + shaderName + L"\";\n").GetWideString());
	fwprintf(stream, CString(L"\tType = " + getArgType(shaderType) + L";\n").GetWideString());

	fwprintf(stream, L"\tRenderer \"mental ray\"\n");
	fwprintf(stream, L"\t{\n");
	fwprintf(stream, CString(L"\t\tName = \"" + shaderName + L"\";\n").GetWideString());
	fwprintf(stream, L"\t\tOptions\n");
	fwprintf(stream, L"\t\t{\n");
	fwprintf(stream, L"\t\t}\n");
	fwprintf(stream, L"\t}\n");

	fwprintf(stream, L"\tRenderer \"3delight\"\n");
	fwprintf(stream, L"\t{\n");
	fwprintf(stream, CString(L"\t\tName = \"" + shaderName + L"\";\n").GetWideString());
	fwprintf(stream, CString(L"\t\tFileName = \"" + shaderName + L"\";\n").GetWideString());

	fwprintf(stream, L"\t\tOptions\n");
	fwprintf(stream, L"\t\t{\n");

	fwprintf(stream, CString(L"\t\t\t\"Type\" = \"" + getArgType(shaderType) + L"\";\n").GetWideString());

	iter1 = argName.begin();
	iter2 = argType.begin();
	vector<ShaderDetailType>::iterator iter4 = argDetail.begin();
	while (iter1 != argName.end()) {
		CString argname = (*iter1);
		ShaderArgType argtype = (*iter2);
		ShaderDetailType argdetail = (*iter4);

		fwprintf(stream, CString(L"\t\t\t\"" + argname + L"\" = \"" + getDetailType(argdetail) + L" " + 
			getPRManArgType(argtype) + L"\";\n").GetWideString());

		++iter1; ++iter2; ++iter4;
	}

	fwprintf(stream, L"\t\t}\n");

	fwprintf(stream, L"\t}\n");
	fwprintf(stream, L"}\n\n");

	fwprintf(stream, CString(L"BEGIN \"{24810490-56FF-11d2-BF23-00A0C982CE5D}\"\n").GetWideString());
	fwprintf(stream, L"Defaults\n");
	fwprintf(stream, L"{\n");

	if (getArgType(shaderType) == L"material") {
		fwprintf(stream, L"\tCs\n");
		fwprintf(stream, L"\t{\n");
		fwprintf(stream, L"\t\tName = \"Cs\";\n");
		fwprintf(stream, L"\t\tDescription = \"\";\n");
		fwprintf(stream, L"\t\tUIType = \"rgba\",4;\n");
		fwprintf(stream, L"\t}\n");
		fwprintf(stream, L"\tOs\n");
		fwprintf(stream, L"\t{\n");
		fwprintf(stream, L"\t\tName = \"Os\";\n");
		fwprintf(stream, L"\t\tDescription = \"\";\n");
		fwprintf(stream, L"\t\tUIType = \"rgba\",4;\n");
		fwprintf(stream, L"\t}\n");
		fwprintf(stream, L"\tDisplacementBound\n");
		fwprintf(stream, L"\t{\n");
		fwprintf(stream, L"\t\tName = \"DisplacementBound\";\n");
		fwprintf(stream, L"\t\tDescription = \"\";\n");
		fwprintf(stream, L"\t}\n");
	}

	if (getArgType(shaderType) == L"displace") {
		fwprintf(stream, L"\tDisplacementBound\n");
		fwprintf(stream, L"\t{\n");
		fwprintf(stream, L"\t\tName = \"DisplacementBound\";\n");
		fwprintf(stream, L"\t\tDescription = \"\";\n");
		fwprintf(stream, L"\t}\n");
	}

	iter1 = argName.begin();
	iter2 = argType.begin();
	while (iter1 != argName.end()) {
		CString argname = (*iter1);
		ShaderArgType argtype = (*iter2);
		fwprintf(stream, CString(L"\t" + argname + L"\n").GetWideString());
		fwprintf(stream, L"\t{\n");
		fwprintf(stream, CString(L"\t\tName = \"" + argname + L"\";\n").GetWideString());
		fwprintf(stream, L"\t\tDescription = \"\";\n");
		if (getArgType(argtype) == L"color") {
			fwprintf(stream, L"\t\tUIType = \"rgba\",4;\n");
		}
		if (getArgType(argtype) == L"vector") {
			fwprintf(stream, L"\t\tUIType = \"vector\";\n");
		}
		if (getArgType(argtype) == L"matrix") {
			fwprintf(stream, L"\t\tUIType = \"SRTTexture.SRTTexture.1\";\n");
		}
		//if (getArgType(argtype) == L"string") {
			//fwprintf(stream, L"\t\tUIType = \"ImageBrowser\";\n");
		//}
		fwprintf(stream, L"\t}\n");

		++iter1; ++iter2;
	}

	fwprintf(stream, L"}\n");

	fwprintf(stream, L"Layout \"Default\"\n"); 
	fwprintf(stream, L"{\n");

	if (getArgType(shaderType) == L"material") {
		fwprintf(stream, L"\tGroup \"Shader Properties\"\n");
		fwprintf(stream, L"\t{\n");
		fwprintf(stream, L"\t\tCs;\n");
		fwprintf(stream, L"\t\tOs;\n");
		fwprintf(stream, L"\t\tDisplacementBound;\n");
		fwprintf(stream, L"\t}\n");
	}
	if (getArgType(shaderType) == L"displace") {
		fwprintf(stream, L"\tGroup \"Shader Properties\"\n");
		fwprintf(stream, L"\t{\n");
		fwprintf(stream, L"\t\tDisplacementBound;\n");
		fwprintf(stream, L"\t}\n");
	}

	iter1 = argName.begin();
	while (iter1 != argName.end()) {
		CString argname = (*iter1);
		fwprintf(stream, CString(L"\t" + argname + L";\n").GetWideString());
		++iter1;
	}
	fwprintf(stream, L"}\n");
	fwprintf(stream, L"Logic\n");
	fwprintf(stream, L"{\n");
	fwprintf(stream, L"}\n");

	fwprintf(stream, CString(L"END\n").GetWideString());
}

CString xsiSloInfo::getArgType(ShaderArgType argtype) 
{
	//wchar_t* shaderArgType[14] = {L"unknown", L"point", L"color", L"float", L"string", L"surface", 
	//L"light", L"displacement", L"volume", L"transformation", L"imager", L"vector", L"normal", L"matrix"};
    wchar_t* shaderArgType[14] = {L"unknown", L"vector", L"color", L"scalar", L"string", L"material", L"light", 
		L"displace", L"volume", L"transformation", L"imager", L"vector", L"vector", L"matrix"};
	return CValue(shaderArgType[argtype]).GetAsText();
}

CString xsiSloInfo::getArgDef(ShaderArgType argtype, void* argdef) 
{
	CString rval;
	
	switch (argtype) {
		case UnknownShaderArg:
			break;		
		case ColorShaderArg: {
			float* carg = (float*)argdef;
			wchar_t cargstr[50];
			swprintf(cargstr, L"%0.4f %0.4f %0.4f 1.0000", carg[0], carg[1], carg[2]);
			rval = CString(cargstr);
			break;
		}
		case ScalarShaderArg: {
			float* farg = (float*)argdef;
			rval = CValue(farg[0]).GetAsText();
			break;
		}
		case StringShaderArg: {
			wchar_t* wcarg;
			XSIA2W(&wcarg, (char*)argdef);
			rval = CString(L"\"" + CValue(wcarg).GetAsText() + L"\"");
			break;
		}
		case PointShaderArg:
		case VectorShaderArg:
		case NormalShaderArg: {
			float* parg = (float*)argdef;
			wchar_t pargstr[50];
			swprintf(pargstr, L"%0.4f %0.4f %0.4f", parg[0], parg[1], parg[2]);
			rval = CString(pargstr);
			break;
		}
		case MatrixShaderArg:
			float* marg = (float*)argdef;
			wchar_t margstr[120];
			swprintf(margstr, L"%0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f", 
				marg[0], marg[1], marg[2], marg[3], marg[4], marg[5], marg[6], marg[7], marg[8], marg[9], marg[10], marg[11], marg[12], marg[13], marg[14], marg[15]);
			rval = CString(margstr);
			break;
	}

	return rval;
}

CString xsiSloInfo::getPRManArgType(ShaderArgType argtype) 
{
	wchar_t* shaderArgType[14] = {L"unknown", L"point", L"color", L"float", L"string", L"material", L"light", 
		L"displace", L"volume", L"transformation", L"imager", L"vector", L"normal", L"matrix"};
	return CValue(shaderArgType[argtype]).GetAsText();
}

CString xsiSloInfo::getDetailType(ShaderDetailType detailtype) 
{
	wchar_t* shaderDetailType[3] = {L"unknown", L"varying", L"uniform",};
	return CValue(shaderDetailType[detailtype]).GetAsText();
}
