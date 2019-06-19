//*****************************************************************************
// File: slo2spdl.cpp
// Description: Defines the entry point for the DLL application.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#include <xsi_ref.h>
#include <xsi_value.h>
#include <xsi_status.h>
#include <xsi_pluginregistrar.h>
#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_plugin.h>
#include <xsi_pluginitem.h>
#include <xsi_command.h>
#include <xsi_comapihandler.h>
#include "xsiSloInfo.h"
#include "xsiUtilities.h"

#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif
#ifdef LINUX 
#include <unistd.h>
#endif

using namespace XSI;

CStatus GetUserSelectedShader(CString&, CString&);


#ifdef unix
extern "C"
#endif
CStatus XSILoadPlugin( XSI::PluginRegistrar& in_reg )
{
	in_reg.PutAuthor( L"Graphic Primitives LLC" );
	in_reg.PutName( L"slo2spdl" );
	in_reg.PutVersion( 1, 0 );

	// register a command plugin item.
	in_reg.RegisterCommand( L"slo2spdl", L"slo2spdl" );

	return XSI::CStatus::OK;	
}


#ifdef unix
extern "C"
#endif
XSI::CStatus XSIUnloadPlugin( const XSI::PluginRegistrar& in_reg )
{
	Application app;
	app.LogMessage( in_reg.GetName() + L" has been unloaded.");

	return XSI::CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus slo2spdl_Execute( XSI::CRef& in_context )
{
	Application app;
	Context ctxt(in_context);


	CValue index; CValueArray args;
	CStatus st = CStatus::OK;

	CString shaderFilePath, shaderBaseName;
	st = GetUserSelectedShader(shaderFilePath, shaderBaseName);

	if (shaderFilePath == L"") {
		return CStatus::Fail;
	}

	// get RenderMan shader info
	xsiSloInfo rmanShaderInfo;
	if (rmanShaderInfo.setShader(shaderFilePath) == CStatus::Fail) return CStatus::Fail;
	//CString rmanShader = rmanShaderInfo.getShaderName();
	CString rmanShader = shaderBaseName;
	
	// spdl file path;
	CString installPath = app.GetInstallationPath(siUserPath);
	CString spdlPath = CString(installPath + L"/Application/spdl/");
	spdlPath = replaceSubString(spdlPath, L"\\", L"/");

	// try to make directory if it doesn't exist
	char* spath = NULL;
	XSIW2A(&spath, spdlPath.GetWideString());
	if (access(spath, 0)) {
		mkdir(spath);

		if (access(spath, 0)) {
			app.LogMessage(L"WARNING: SPDL directory " + spdlPath + L" does not exist!");
			return CStatus::Fail;
		}
	}

	spdlPath += CString(rmanShader + L".spdl");

	// save SPDL file
	FILE* stream;
	if ((stream = _wfopen(spdlPath.GetWideString(), L"w")) == NULL)
		app.LogMessage(L"File " + spdlPath + L" was not opened!");
	else
		rmanShaderInfo.write(stream, in_context);
	fclose(stream);

	// Register the SPDL
	CComAPIHandler xsiutils;
	xsiutils.CreateInstance(L"XSI.Utils");
	xsiutils.Call(L"RegisterSPDL", index, spdlPath, true);

	return st;
}


CStatus GetUserSelectedShader(CString& pathName, CString& baseName)
{
	// Using the COMAPIHandler
	CComAPIHandler xsiuitoolkit;
    xsiuitoolkit.CreateInstance(L"XSI.UIToolkit");
	CValue rval = xsiuitoolkit.GetProperty(L"FileBrowser");
	CComAPIHandler filebrowser(rval);
	filebrowser.PutProperty(L"DialogTitle", CValue(L"Select Shader"));
	filebrowser.PutProperty(L"InitialDirectory", CValue(L"C:/"));
	filebrowser.PutProperty(L"Filter", CValue(L"PRMan Shader (*.slo)|*.slo||"));
	CValue index; CValueArray args;
	filebrowser.Call(L"ShowOpen", index, args);
	pathName = filebrowser.GetProperty(L"FilePathName");
	baseName = filebrowser.GetProperty(L"FileBaseName");

	return CStatus::OK;
}
