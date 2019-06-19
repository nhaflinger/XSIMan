//*****************************************************************************
// File: xsiSlbInfo.cpp
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

#include "xsiSlbInfo.h"
#include "xsiUtilities.h"
#include <xsi_application.h>
#include <xsi_comapihandler.h>
#include <xsi_context.h>

#ifdef AIR
#include <slb.c>
#endif

xsiSlbInfo::xsiSlbInfo()
{
}

xsiSlbInfo::~xsiSlbInfo()
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

CStatus xsiSlbInfo::setShader(CString shaderFilePath) 
{
	Application app;
	int i,j,n,n2;
	ShaderString ss;   
	char *fname, *typ, *clss;
	char *tk[MAXNSHADERTOKENS];
	char *t2[4];
	char drive[64],dir[260],name[260],ext[64],slbname[260];

#ifdef AIR 
	char* shaderfilepath = NULL;
	XSIW2A(&shaderfilepath, shaderFilePath.GetWideString());
	fname = shaderfilepath;
	_splitpath(fname,drive,dir,name,ext);
	_makepath(slbname,drive,dir,name,".slb");

	if (SLB_LoadShader(slbname, ss, 1)) {
		n=SLB_Tokenize(ss,MAXNSHADERTOKENS,&(tk[0]));
		if (n>1) {
			i=2;
			typ="surface";
			shaderType = SurfaceShader;
			if (strcmp(tk[0],"Displacement")==0) {
				typ="displacement";
				shaderType = DisplacementShader;
			}
			if (strcmp(tk[0],"Atmosphere")==0) {
				typ="volume";
				shaderType = VolumeShader;
			}
			if (strcmp(tk[0],"Deformation")==0) {
				typ="transformation";
				shaderType = TransformationShader;
			}
			if (strcmp(tk[0],"Procedure")==0) {
				typ="procedure";
				shaderType = ProcedureShader;
			}
			if (strcmp(tk[0],"LightSource")==0) {
				typ="light"; 
				shaderType = LightShader;
				i=3;
			};

			wchar_t* wshadername;
			XSIA2W(&wshadername, name);
			shaderName = CValue(wshadername).GetAsText(); 

			numArgs = n - (i + 1);

			while ((i+1)<n) {
				n2=SLB_Tokenize(tk[i],4,&(t2[0]));
				j=0;
				if (strcmp(t2[0],"output")==0) j=1;
				clss=t2[j];
				if (strcmp(clss,"varying")==0) {
					j++;
				} else {
					clss="uniform";
				}
				typ=t2[j];
				i++;
				
				ShaderArgType argtype = getSlbType(typ);
				ShaderDetailType argdetail = getSlbDetail(clss);
				
				wchar_t* wt2; XSIA2W(&wt2, t2[j+1]);
				argName.push_back(CString(wt2));
				argType.push_back(argtype);
				//argArraySize.push_back(arg->svd_arraylen);
				argDetail.push_back(argdetail);

				switch (argtype) {
					case StringShaderArg: {
						char *slostr = new char[strlen(tk[i]) + 1];
						strcpy(slostr, tk[i]);
						argDefault.push_back((void*)slostr);
						break;
					}
					case ScalarShaderArg: {
						float *slofloat = new float[1];
						slofloat[0] = (float)atof(tk[i]);
						argDefault.push_back((void*)slofloat);
						break;
					}
					case ColorShaderArg:
					case PointShaderArg:
					case VectorShaderArg:
					case NormalShaderArg: {
						float *slopnt = new float[3];
						wchar_t* wtk; XSIA2W(&wtk, tk[i]);
						float xval, yval, zval;
						swscanf(wtk, L"%f %f %f", &xval, &yval, &zval);
						slopnt[0] = xval;
						slopnt[1] = yval;
						slopnt[2] = zval;
						argDefault.push_back((void*)slopnt);
						break;
					}
					case MatrixShaderArg: {
						float *slomat = new float[16];
						wchar_t* wtk; XSIA2W(&wtk, tk[i]);
						float matrixval[16];
						swscanf(wtk, L"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &matrixval[0], &matrixval[1],
							&matrixval[2], &matrixval[3], &matrixval[4], &matrixval[5], &matrixval[6], &matrixval[7], 
							&matrixval[8], &matrixval[9], &matrixval[10], &matrixval[11], &matrixval[12], &matrixval[13], 
							&matrixval[14], &matrixval[15]);
						slomat[0] = matrixval[0];
						slomat[1] = matrixval[1];
						slomat[2] = matrixval[2];
						slomat[3] = matrixval[3];
						slomat[4] = matrixval[4];
						slomat[5] = matrixval[5];
						slomat[6] = matrixval[6];
						slomat[7] = matrixval[7];
						slomat[8] = matrixval[8];
						slomat[9] = matrixval[9];
						slomat[10] = matrixval[10];
						slomat[11] = matrixval[11];
						slomat[12] = matrixval[12];
						slomat[13] = matrixval[13];
						slomat[14] = matrixval[14];
						slomat[15] = matrixval[15];
						argDefault.push_back((void*)slomat);
						break;
					}
					default:
						argDefault.push_back(NULL);
						break;
				}
				i++;
			}
		}
	}
	else {
		app.LogMessage(L"Error finding shader " + shaderFilePath);
		return CStatus::Fail;
	}

#endif

	return CStatus::OK;
}

CString xsiSlbInfo::getShaderName() 
{
	return shaderName;
}

long xsiSlbInfo::getNumArgs()
{
	return numArgs;
}

void xsiSlbInfo::write(FILE* stream, CRef& in_ref)
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

	if (getArgType(shaderType) == L"material" || getArgType(shaderType) == L"light" || getArgType(shaderType) == L"volume") {
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

	fwprintf(stream, L"\tRenderer \"Air\"\n");
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

CString xsiSlbInfo::getArgType(ShaderArgType argtype) 
{
    wchar_t* shaderArgType[15] = {L"unknown", L"vector", L"color", L"scalar", L"string", L"material", L"light", 
		L"displace", L"volume", L"transformation", L"imager",  L"procedure", L"vector", L"vector", L"matrix"};
	return CValue(shaderArgType[argtype]).GetAsText();
}

CString xsiSlbInfo::getArgDef(ShaderArgType argtype, void* argdef) 
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
			rval = CString(CValue(wcarg).GetAsText());
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

CString xsiSlbInfo::getPRManArgType(ShaderArgType argtype) 
{
	wchar_t* shaderArgType[15] = {L"unknown", L"point", L"color", L"float", L"string", L"material", L"light", 
		L"displace", L"volume", L"transformation", L"imager", L"procedure", L"vector", L"normal", L"matrix"};
	return CValue(shaderArgType[argtype]).GetAsText();
}

CString xsiSlbInfo::getDetailType(ShaderDetailType detailtype) 
{
	wchar_t* shaderDetailType[3] = {L"unknown", L"varying", L"uniform",};
	return CValue(shaderDetailType[detailtype]).GetAsText();
}

ShaderArgType xsiSlbInfo::getSlbType(char* argtype)
{
	ShaderArgType argType;
	
	if (!strcmp(argtype, "point")) argType = PointShaderArg;
	else if (!strcmp(argtype, "vector")) argType = VectorShaderArg;
	else if (!strcmp(argtype, "normal")) argType = NormalShaderArg;
	else if (!strcmp(argtype, "color")) argType = ColorShaderArg;
	else if (!strcmp(argtype, "float")) argType = ScalarShaderArg;
	else if (!strcmp(argtype, "string")) argType = StringShaderArg;
	else if (!strcmp(argtype, "float[16]")) argType = MatrixShaderArg;
	else argType = UnknownShaderArg;

	return argType;
} 

ShaderDetailType xsiSlbInfo::getSlbDetail(char* argdetail)
{
	ShaderDetailType detailType;

	if (!strcmp(argdetail, "varying")) detailType = VaryingShaderDetail;
	else if (!strcmp(argdetail, "uniform")) detailType = UniformShaderDetail;
	else detailType = UnknownShaderDetail;

	return detailType;
} 

