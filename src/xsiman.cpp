//*****************************************************************************
// File: XSIMan.cpp
// Description: Defines the entry point for the DLL application.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#include "stdafx.h"
#include <xsi_value.h>
#include <xsi_status.h>
#include <xsi_application.h>
#include <xsi_plugin.h>
#include <xsi_pluginitem.h>
#include <xsi_pluginregistrar.h>
#include <xsi_pluginitem.h>
#include <xsi_command.h>
#include <xsi_argument.h>
#include <xsi_context.h>
#include <xsi_menuitem.h>
#include <xsi_menu.h>
#include <xsi_customproperty.h>
#include <xsi_ppglayout.h>
#include <xsi_ppgeventcontext.h>
#include <xsi_selection.h>
#include <xsi_comapihandler.h>
#include <xsi_userdatamap.h>
#include <xsi_project.h>
#include <xsi_point.h>
#include <xsi_cluster.h>
#include <xsi_userdatamap.h>
#include <xsi_uitoolkit.h>
#include <xsi_griddata.h>
#include <xsi_gridwidget.h>
#include <xsi_sample.h>
#include <xsi_nurbssample.h>
#include <xsi_nurbssurface.h>
#include <xsi_controlpoint.h>

#include "xsiRibTranslator.h"
#include "xsiSloInfo.h"
#include "xsiUtilities.h"
#include "helper.h"

#include <string>
#include <fstream>
#include <iostream>

#include "md5.h"
#include "zlib_timeops.h"
#include "getmacaddr.h"

#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif
#ifdef LINUX 
#include <unistd.h>
#endif

//#define EXPTIME 1233112703
#define EXPTIME 1523517197

// magic string added to mac address before encrypting,
// make it harder for someone to simply call MD5 to get their own key.
#define MAGIC "gprimrulez"

static const int nltrace = 0;
static char hexmap[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

using namespace XSI;

CString GetUserSelectedObject();
CStatus GetUserSelectedShader(CString&, CString&);
CValue InspectObj(const CString&, const CString&, const CString&, const CValue&, bool);
void DeleteObj(const CValue& in_inputobj);
static void nlmacaddr(unsigned char addr[/*6*/]);
static void nlencrypt(const char *str, const unsigned char macaddr[], unsigned char encrypted[]);
static bool nlcheck(const char*, unsigned int, const char*);
bool nlcheck(const char *licfile, const char *progname);


#ifdef unix
extern "C"
#endif
CStatus XSILoadPlugin(XSI::PluginRegistrar& in_reg)
{
	Application app;

	// get XSIMAN environment variable
	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
		
	// get XSIMAN_LICENSE environment variable
	wchar_t* xsimanlic;
	xsimanlic = _wgetenv(L"XSIMAN_LICENSE");

	CString licenseFile = replaceSubString(CString(xsimanhome), L"\\", L"/") + L"/license.dat";
	CString progName = L"XSIMan1.0";
	char* licensefile = NULL;
	char* progname = NULL;
	XSIW2A(&licensefile, licenseFile.GetWideString());
	XSIW2A(&progname, progName.GetWideString());
	bool licenseok1 = nlcheck(licensefile, progname);

	licenseFile = replaceSubString(CString(xsimanlic), L"\\", L"/") + L"/license.dat";
	XSIW2A(&licensefile, licenseFile.GetWideString());
	bool licenseok2 = nlcheck(licensefile, progname);

	if (licenseok1 || licenseok2) {
		app.LogMessage(L"Existing license for " + progName + L" is OK");
	}
	else {
		app.LogMessage(L"Did not find valid license. Using built-in license.");

		// check timeout
		int timeout = zlib_timeops::expire(EXPTIME);
		if (timeout < 0) {
			app.LogMessage(L"XSIMan license has expired");
			return XSI::CStatus::Fail;
		}
		char* exptime = zlib_timeops::ctime(EXPTIME);
		wchar_t* wexptime;
		XSIA2W(&wexptime, exptime); 
		if (timeout < (60*60*24*90)) {
			app.LogMessage(L"Evaluation license will expire on " + CValue(wexptime).GetAsText());
		}
	}

	in_reg.PutAuthor(L"Graphic Primitives LLC");
	in_reg.PutName(L"XSIMan");	
	in_reg.PutVersion(1, 0);
	in_reg.PutURL(L"http://www.graphicprimitives.com");
	in_reg.PutEmail(L"info@graphicprimitives.com");

	in_reg.PutHelp(CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIMan.htm").GetAsText());

	// register the commands plugin item.
	in_reg.RegisterCommand(L"XSIManRender", L"XSIManRender");

	// register the menu for the export
	in_reg.RegisterMenu(siMenuMainTopLevelID, L"XSIMan", false, false);

	// register the properties for export
	in_reg.RegisterProperty(L"XSIManOptions");
	in_reg.RegisterProperty(L"XSIManShadows");
	in_reg.RegisterProperty(L"XSIManArchive");
	in_reg.RegisterProperty(L"XSIManRibBox");
	in_reg.RegisterProperty(L"XSIManScriptBox");
	in_reg.RegisterProperty(L"XSIManTraceSet");
	in_reg.RegisterProperty(L"XSIManLODControls");
	in_reg.RegisterProperty(L"XSIManDetailRange");
	in_reg.RegisterProperty(L"XSIManShadowShading");
	in_reg.RegisterProperty(L"XSIManParticles");
	in_reg.RegisterProperty(L"XSIManSamples");
	in_reg.RegisterProperty(L"XSIManWidth");
	in_reg.RegisterProperty(L"XSIManSubdiv");
	in_reg.RegisterProperty(L"XSIManCSGPrimitive");
	in_reg.RegisterProperty(L"XSIManCSGDifference");
	in_reg.RegisterProperty(L"XSIManCSGIntersection");
	in_reg.RegisterProperty(L"XSIManCSGUnion");
	in_reg.RegisterProperty(L"XSIManMatte");
	in_reg.RegisterProperty(L"XSIManEnvmap");
	in_reg.RegisterProperty(L"XSIManShadingModel");
	in_reg.RegisterProperty(L"XSIManPhotonMap");
	in_reg.RegisterProperty(L"XSIManAirOptions");
	in_reg.RegisterProperty(L"XSIManDelightOptions");
	in_reg.RegisterProperty(L"XSIManMiscOptions");

	return XSI::CStatus::OK;	
}

#ifdef unix
extern "C"
#endif
XSI::CStatus XSIUnloadPlugin(const XSI::PluginRegistrar& in_reg)
{
	Application app;
#ifdef _DEBUG
	app.LogMessage(in_reg.GetName() + L" has been unloaded.");
#endif
	return XSI::CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus XSIManRender_Init(const XSI::CRef& in_context)
{
	Context ctx(in_context);
	Command cmd(ctx.GetSource());

	Application app;
	app.LogMessage(L"Defining: " + cmd.GetName());

	ArgumentArray args = cmd.GetArguments();

	args.Add(L"RibName", L"");
	args.Add(L"ImageName", L"");
	args.Add(L"ExportCamera", L"");
	args.Add(L"DisplayDriver", L"");
	args.Add(L"StartFrame", L"");
	args.Add(L"EndFrame", L"");
	args.Add(L"StepSize", L"");
	args.Add(L"ShadingRate", L"");
	args.Add(L"PixelSamplesX", L"");
	args.Add(L"PixelSamplesY", L"");
	args.Add(L"ResolutionX", L"");
	args.Add(L"ResolutionY", L"");
	args.Add(L"BatchMode", L"");
	args.Add(L"RibPath", L"");
	args.Add(L"JobPath", L"");
	args.Add(L"PixPath", L"");
	args.Add(L"TexPath", L"");
	args.Add(L"ShdPath", L"");
	args.Add(L"ShaderPath", L"");

	return XSI::CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus XSIManRender_Execute(XSI::CRef& in_context)
{
	Application app;
	Property prop;
	Context ctxt(in_context);

	// reset environment (this is so simulations will run correctly)
	Model root = app.GetActiveSceneRoot();
	CRef propRef = root.GetProperties().GetItem(L"XSIManOptions");
	if (propRef.IsValid()) prop = Property(propRef);
	else prop = root.AddProperty(L"XSIManOptions");

	Parameter RenderFrames = prop.GetParameters().GetItem(L"RenderFrames");
	bool renderFrames = RenderFrames.GetValue();
	if (renderFrames) {
		CValueArray args; CValue index;
		app.ExecuteCommand(L"FirstFrame", args, index);
		app.ExecuteCommand(L"Refresh", args, index);
	}

	Command cmd(ctxt.GetSource());
	ArgumentArray arglist = cmd.GetArguments();
	CString ribName = arglist.GetItem(L"RibName").GetValue();
	CString imageName = arglist.GetItem(L"ImageName").GetValue();
	CString exportCamera = arglist.GetItem(L"ExportCamera").GetValue();
	CString displayDriver = arglist.GetItem(L"DisplayDriver").GetValue();
	long startFrame = arglist.GetItem(L"StartFrame").GetValue();
	long endFrame = arglist.GetItem(L"EndFrame").GetValue();
	long stepSize = arglist.GetItem(L"StepSize").GetValue();
	float shadingRate = (float)arglist.GetItem(L"ShadingRate").GetValue();
	long pixelSamplesX = arglist.GetItem(L"PixelSamplesX").GetValue();
	long pixelSamplesY = arglist.GetItem(L"PixelSamplesY").GetValue();
	long resolutionX = arglist.GetItem(L"ResolutionX").GetValue();
	long resolutionY = arglist.GetItem(L"ResolutionY").GetValue();
	bool batchMode = arglist.GetItem(L"BatchMode").GetValue();
	CString ribPath = arglist.GetItem(L"RibPath").GetValue();
	CString jobPath = arglist.GetItem(L"JobPath").GetValue();
	CString pixPath = arglist.GetItem(L"PixPath").GetValue();
	CString texPath = arglist.GetItem(L"TexPath").GetValue();
	CString shdPath = arglist.GetItem(L"ShdPath").GetValue();
	CString shaderPath = arglist.GetItem(L"ShaderPath").GetValue();

	commandLineOptions* options = new commandLineOptions;

	options->RibName = ribName;
	options->ImageName = imageName;
	options->ExportCamera = exportCamera;
	options->DisplayDriver = displayDriver;
	options->StartFrame = startFrame;
	options->EndFrame = endFrame;
	options->StepSize = stepSize;
	options->ShadingRate = shadingRate;
	options->PixelSamplesX = pixelSamplesX;
	options->PixelSamplesY = pixelSamplesY;
	options->ResolutionX = resolutionX;
	options->ResolutionY = resolutionY;
	options->BatchMode = batchMode;
	options->RibPath = ribPath;
	options->JobPath = jobPath;
	options->PixPath = pixPath;
	options->TexPath = texPath;
	options->ShdPath = shdPath;
	options->ShaderPath = shaderPath;

	xsiRibTranslator XSIMan;
	CStatus st = XSIMan.ExportFile(options, false);

	// cleanup
	delete options;

	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus XSIMan_Init(XSI::CRef& in_ref)
{
	Menu menu, subMenu1, subMenu2, subMenu3, subMenu4, subMenu5, subMenu6, subMenu7, subMenu8, subMenu9, subMenu10, 
		subMenu11, subMenu12, subMenu13, subMenu14, subMenu15, subMenu16;
	MenuItem item1, item2, item3, item4, item5, item6, item7, item8, item9, item10, item11, item12, item13, 
		item14, item15, item16, item17, item18, item19, item20, item21, item22, item23, item24, item25, item26;
	CStatus st;

	Context ctxt = in_ref;
	menu = ctxt.GetSource();

	menu.AddCallbackItem(L"Render", L"OnXSIRender", item1);

	menu.AddCallbackItem(L"Render Settings", L"OnXSIRenderSettings", item2);

	menu.AddItem(L"Attributes", siMenuItemSubmenu, item3);
	subMenu1 = item3;

	subMenu1.AddItem(L"Curve Rendering", siMenuItemSubmenu, item4);
	subMenu2 = item4;
	subMenu2.AddCallbackItem(L"Attach", L"OnXSIConstantWidthAttach", item4);
	subMenu2.AddCallbackItem(L"Detach", L"OnXSIConstantWidthDetach", item4);

	subMenu1.AddSeparatorItem();

	subMenu1.AddItem(L"Subdivision Rendering", siMenuItemSubmenu, item5);
	subMenu3 = item5;
	subMenu3.AddCallbackItem(L"Attach", L"OnXSISubdivisionAttach", item5);
	subMenu3.AddCallbackItem(L"Detach", L"OnXSISubdivisionDetach", item5);
	
	subMenu1.AddSeparatorItem();

	subMenu1.AddItem(L"Matte Object", siMenuItemSubmenu, item5);
	subMenu10 = item5;
	subMenu10.AddCallbackItem(L"Attach", L"OnXSIMatteAttach", item17);
	subMenu10.AddCallbackItem(L"Detach", L"OnXSIMatteDetach", item17);

	subMenu1.AddSeparatorItem();

	subMenu1.AddItem(L"CSG", siMenuItemSubmenu, item11);
	subMenu5 = item11;
	subMenu5.AddCallbackItem(L"Primitive", L"OnXSICSGPrimitive", item11);
	subMenu5.AddCallbackItem(L"Difference", L"OnXSICSGDifference", item11);
	subMenu5.AddCallbackItem(L"Intersection", L"OnXSICSGIntersection", item11);
	subMenu5.AddCallbackItem(L"Union", L"OnXSICSGUnion", item11);
	subMenu5.AddCallbackItem(L"Detach", L"OnXSICSGDetach", item11);

	subMenu1.AddSeparatorItem();

	subMenu1.AddItem(L"Level of Detail", siMenuItemSubmenu, item13);
	subMenu7 = item13;
	subMenu7.AddSeparatorItem();
	subMenu7.AddCallbackItem(L"LOD Controls", L"OnXSILODControls", item13);
	subMenu7.AddCallbackItem(L"Detail Range", L"OnXSIDetailRange", item13);

	subMenu1.AddSeparatorItem();

	subMenu1.AddItem(L"Motion Samples", siMenuItemSubmenu, item12);
	subMenu6 = item12;
	subMenu6.AddCallbackItem(L"Attach", L"OnXSISamplesAttach", item12);
	subMenu6.AddCallbackItem(L"Detach", L"OnXSISamplesDetach", item12);

	subMenu1.AddSeparatorItem();
	
	subMenu1.AddItem(L"Shadow Shading", siMenuItemSubmenu, item25);
	subMenu11 = item25;
	subMenu11.AddCallbackItem(L"Attach", L"OnXSIShadowShadingAttach", item25);
	subMenu11.AddCallbackItem(L"Detach", L"OnXSIShadowShadingDetach", item25);

	subMenu1.AddSeparatorItem();

	subMenu1.AddItem(L"Particle Types", siMenuItemSubmenu, item15);
	subMenu9 = item15;
	subMenu9.AddCallbackItem(L"Attach", L"OnXSIParticlesAttach", item15);
	subMenu9.AddCallbackItem(L"Detach", L"OnXSIParticlesDetach", item15);

	subMenu1.AddSeparatorItem();

	subMenu1.AddItem(L"Photon Shading Model", siMenuItemSubmenu, item18);
	subMenu11 = item18;
	subMenu11.AddCallbackItem(L"Attach", L"OnXSIShadingModelAttach", item18);
	subMenu11.AddCallbackItem(L"Detach", L"OnXSIShadingModelDetach", item18);

	subMenu1.AddSeparatorItem();

	subMenu1.AddItem(L"Misc. Options", siMenuItemSubmenu, item22);
	subMenu14 = item22;
	subMenu14.AddCallbackItem(L"Attach", L"OnXSIMiscOptionsAttach", item22);
	subMenu14.AddCallbackItem(L"Detach", L"OnXSIMiscOptionsDetach", item22);

	subMenu1.AddSeparatorItem();
	
	subMenu1.AddItem(L"Air Options", siMenuItemSubmenu, item23);
	subMenu15 = item23;
	subMenu15.AddCallbackItem(L"Attach", L"OnXSIAirOptionsAttach", item23);
	subMenu15.AddCallbackItem(L"Detach", L"OnXSIAirOptionsDetach", item23);
	
	subMenu1.AddSeparatorItem();
	
	subMenu1.AddItem(L"3Delight Options", siMenuItemSubmenu, item26);
	subMenu16 = item26;
	subMenu16.AddCallbackItem(L"Attach", L"OnXSIDelightOptionsAttach", item26);
	subMenu16.AddCallbackItem(L"Detach", L"OnXSIDelightOptionsDetach", item26);

	menu.AddItem(L"Helpers", siMenuItemSubmenu, item6);
	subMenu4 = item6;

	subMenu4.AddCallbackItem(L"Shadow Map Controls", L"OnXSIShadowControls", item6);
	subMenu4.AddCallbackItem(L"Environment Map Controls", L"OnXSIEnvmapControls", item6);
	subMenu4.AddCallbackItem(L"RIB Archive Controls", L"OnXSIArchiveControls", item6);
	subMenu4.AddCallbackItem(L"Photon Map Controls", L"OnXSIPhotonMapControls", item6);
	subMenu4.AddCallbackItem(L"RIB Box", L"OnXSIRibBox", item6);
	subMenu4.AddCallbackItem(L"Script Box", L"OnXSIScriptBox", item6);
	subMenu4.AddCallbackItem(L"Trace Set", L"OnXSITraceSet", item6);

	menu.AddItem(L"Primitive Variables", siMenuItemSubmenu, item14);
	subMenu8 = item14;

	subMenu8.AddItem(L"Pref Geometry", siMenuItemSubmenu, item19);
	subMenu12 = item19;
	subMenu12.AddCallbackItem(L"Attach Pref", L"OnXSIPrefAttach", item19);
	subMenu12.AddCallbackItem(L"Detach Pref", L"OnXSIPrefDetach", item19);

	menu.AddCallbackItem(L"Register Shader", L"OnXSIRegisterShader", item10);

	menu.AddCallbackItem(L"Export as Archive", L"OnXSIExportAsArchive", item24);

	menu.AddCallbackItem(L"Help", L"OnXSIHelp", item21);

	menu.AddCallbackItem(L"About", L"OnXSIAbout", item16);

	return CStatus::OK;	
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIRender(XSI::CRef& in_ref)
{
	Application app;
	Property prop;
	CStatus st = CStatus::Fail;

	Model root = app.GetActiveSceneRoot();
	CRef propRef = root.GetProperties().GetItem(L"XSIManOptions");

	if (!propRef.IsValid()) prop = root.AddProperty(L"XSIManOptions");
	else prop = Property(propRef);

	// reset environment if multi-frame (this is so simulations will run correctly)
	Parameter RenderFrames = prop.GetParameters().GetItem(L"RenderFrames");
	bool renderFrames = RenderFrames.GetValue();
	if (renderFrames) {
		CValueArray args; CValue index;
		app.ExecuteCommand(L"FirstFrame", args, index);
		app.ExecuteCommand(L"Refresh", args, index);
	}

	//Launch the command 
	commandLineOptions* options = NULL;
	xsiRibTranslator XSIMan;
	st = XSIMan.ExportFile(options, false);

	//cleanup
	delete options;

	if(st != CStatus::OK) {
		app.LogMessage(L"Could not find XSIManOptions property.");
		return st;
	}
	
	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIExportAsArchive(XSI::CRef& in_ref)
{
	Application app;
	Property prop;
	CStatus st = CStatus::Fail;

	Model root = app.GetActiveSceneRoot();
	CRef propRef = root.GetProperties().GetItem(L"XSIManOptions");

	if (!propRef.IsValid()) prop = root.AddProperty(L"XSIManOptions");
	else prop = Property(propRef);

	// reset environment if multi-frame (this is so simulations will run correctly)
	Parameter RenderFrames = prop.GetParameters().GetItem(L"RenderFrames");
	bool renderFrames = RenderFrames.GetValue();
	if (renderFrames) {
		CValueArray args; CValue index;
		app.ExecuteCommand(L"FirstFrame", args, index);
		app.ExecuteCommand(L"Refresh", args, index);
	}

	//Launch the command 
	commandLineOptions* options = NULL;
	xsiRibTranslator XSIMan;
	st = XSIMan.ExportFile(options, true);

	//cleanup
	delete options;

	if(st != CStatus::OK) {
		app.LogMessage(L"Could not find XSIManOptions property.");
		return st;
	}
	
	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIRenderSettings(XSI::CRef& in_ref)
{	
	Application app;
	Property prop;
	CStatus st(CStatus::OK);

	Model root = app.GetActiveSceneRoot();
	CRef propRef = root.GetProperties().GetItem(L"XSIManOptions");
	if (!propRef.IsValid()) {
		prop = root.AddProperty(L"XSIManOptions");
	
		// check for configuration file
		wchar_t* xsimanhome;
		xsimanhome = _wgetenv(L"XSIMAN");
		if (CString(xsimanhome) != L"") {
			CString scriptName = replaceSubString(CString(xsimanhome), L"\\", L"/") + L"/xsiman.js";
			char* scriptname = NULL;
			XSIW2A(&scriptname, scriptName.GetWideString());
			if (!access(scriptname, 0)) {
				// Using the COMAPIHandler
				CValue retval; CValueArray args(4);
				args[0] = CValue(scriptName);
				CComAPIHandler xsiapp;
				xsiapp.CreateInstance(L"XSI.Application");
				CStatus stat = xsiapp.Call(L"ExecuteScript", retval, args);
				if (stat != CStatus::OK) app.LogMessage(scriptName + L" did not execute correctly!");
			} 
			else app.LogMessage(L"Cannot find " + scriptName);
		}
	}

	// InspectObj Returns true if the command was cancelled otherwise it returns false. 
	bool ret = InspectObj(L"XSIManOptions", CValue(), L"XSIMan", (long)4, true);

	// rename object before saving
	prop.PutName(L"XSIManOptions");

	return st;	
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIConstantWidthAttach(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManWidth");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding curve width property to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManWidth");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIConstantWidthDetach(XSI::CRef& in_ref)
{
	Application app;
	CStatus st(CStatus::OK);
	CValue retval;

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManWidth");
		if (propRef.IsValid()) {
			DeleteObj(propRef);
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManWidth_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"width", CValue::siDouble, siAnimatable, L"Curve Width", L"width", 0.1, 0.0, 100., 0., 10., param);
	
	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManWidth_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	item = oLayout.AddItem(L"width", L"");

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManAttributes.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSISubdivisionAttach(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManSubdiv");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding subdivsion property to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManSubdiv");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSISubdivisionDetach(XSI::CRef& in_ref)
{
	Application app;
	CStatus st(CStatus::OK);
	CValue retval;

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManSubdiv");
		if (propRef.IsValid()) {
			DeleteObj(propRef);
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManSubdiv_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"subdiv", CValue::siBool, siAnimatable, L"Subdivision Surface", L"", true, param);
	prop.AddParameter(L"interpBoundary", CValue::siBool, siAnimatable, L"Interpolate Boundary", L"", true, param);	
	prop.AddParameter(L"subdivScheme", CValue::siString, siPersistable, L"Subdivision Scheme", L"", dft, param);
	prop.PutParameterValue(L"subdivScheme", CValue(L"catmull-clark"));
	
	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManSubdiv_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	item = oLayout.AddItem(L"subdiv", L"");
	
	item = oLayout.AddItem(L"interpBoundary", L"");
		
	CValueArray comboItems1(4);
	comboItems1[0] = L"catmull-clark"; comboItems1[1] = L"catmull-clark";
	comboItems1[2] = L"loop"; comboItems1[3] = L"loop";
	item = oLayout.AddEnumControl(L"subdivScheme", comboItems1, L"", siControlCombo);
	item.PutLabelPercentage(90);

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManAttributes.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIMatteAttach(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManMatte");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding matte object property to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManMatte");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIMatteDetach(XSI::CRef& in_ref)
{
	Application app;
	CStatus st(CStatus::OK);
	CValue retval;

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManMatte");
		if (propRef.IsValid()) {
			DeleteObj(propRef);
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManMatte_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"matte", CValue::siBool, siAnimatable, L"Matte Object", L"", true, param);
	
	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManMatte_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	item = oLayout.AddItem(L"matte", L"");

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManAttributes.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSICSGPrimitive(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManCSGPrimitive");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding CSG primitive property to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManCSGPrimitive");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManCSGPrimitive_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"csgPrim", CValue::siBool, siAnimatable, L"CSG Primitive", L"", true, param);
	
	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManCSGPrimitive_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	item = oLayout.AddItem(L"csgPrim", L"");

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManAttributes.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSICSGDifference(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		
		if (!SIObject(sel[i]).IsA(siModelID)) {
			app.LogMessage(SceneItem(sel[i]).GetName() + L" is not a model object!");
			continue;
		}

		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManCSGDifference");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding CSG difference property to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManCSGDifference");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManCSGDifference_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"csgDiff", CValue::siBool, siAnimatable, L"CSG Difference", L"", true, param);
	
	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManCSGDifference_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	item = oLayout.AddItem(L"csgDiff", L"");

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManAttributes.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSICSGIntersection(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		
		if (!SIObject(sel[i]).IsA(siModelID)) {
			app.LogMessage(SceneItem(sel[i]).GetName() + L" is not a model object!");
			continue;
		}

		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManCSGIntersection");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding CSG intersection property to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManCSGIntersection");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManCSGIntersection_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"csgInt", CValue::siBool, siAnimatable, L"CSG Intersection", L"", true, param);
	
	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManCSGIntersection_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	item = oLayout.AddItem(L"csgInt", L"");

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManAttributes.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSICSGUnion(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		
		if (!SIObject(sel[i]).IsA(siModelID)) {
			app.LogMessage(SceneItem(sel[i]).GetName() + L" is not a model object!");
			continue;
		}

		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManCSGUnion");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding CSG union property to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManCSGUnion");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManCSGUnion_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"csgUnion", CValue::siBool, siAnimatable, L"CSG Union", L"", true, param);
	
	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManCSGUnion_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	item = oLayout.AddItem(L"csgUnion", L"");

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManAttributes.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSICSGDetach(XSI::CRef& in_ref)
{
	Application app;
	CStatus st(CStatus::OK);
	CValue retval;

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRefArray propRefArray = SceneItem(sel[i]).GetProperties();
		for (long j = 0; j < propRefArray.GetCount(); ++j) {
			if ((SIObject(propRefArray[j]).GetName() == L"XSIManCSGPrimitive") ||
				(SIObject(propRefArray[j]).GetName() == L"XSIManCSGDifference") ||
				(SIObject(propRefArray[j]).GetName() == L"XSIManCSGIntersection") ||
				(SIObject(propRefArray[j]).GetName() == L"XSIManCSGUnion")) {
				DeleteObj(propRefArray[j]);
			}
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSISamplesAttach(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManSamples");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding motion samples property to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManSamples");			
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSISamplesDetach(XSI::CRef& in_ref)
{
	Application app;
	CStatus st(CStatus::OK);
	CValue retval;

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManSamples");
		if (propRef.IsValid()) {
			DeleteObj(propRef);
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManSamples_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"samples", CValue::siInt2, siAnimatable, L"Motion Samples", L"samples", 1., 1., 5., 1., 5., param);

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManSamples_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	item = oLayout.AddItem(L"samples", L"");

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManAttributes.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIShadowShadingAttach(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManShadowShading");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding shadow shading control to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManShadowShading");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIShadowShadingDetach(XSI::CRef& in_ref)
{
	Application app;
	CStatus st(CStatus::OK);
	CValue retval;

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManShadowShading");
		if (propRef.IsValid()) {
			DeleteObj(propRef);
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManShadowShading_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"ShadowShading", CValue::siString, siPersistable, L"Shadow Shading", L"", dft, param);

	prop.PutParameterValue(L"ShadowShading", CValue(L"opaque"));
	
	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManShadowShading_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	CValueArray comboItems1(6);
	comboItems1[0] = L"opaque"; comboItems1[1] = L"opaque";
	comboItems1[2] = L"Os"; comboItems1[3] = L"Os";
	comboItems1[4] = L"shade"; comboItems1[5] = L"shade";
	item = oLayout.AddEnumControl(L"ShadowShading", comboItems1, L"", siControlCombo);
	item.PutLabelPercentage(90);

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManAttributes.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIParticlesAttach(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManParticles");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding particle type control to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManParticles");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIParticlesDetach(XSI::CRef& in_ref)
{
	Application app;
	CStatus st(CStatus::OK);
	CValue retval;

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManParticles");
		if (propRef.IsValid()) {
			DeleteObj(propRef);
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManParticles_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"ParticleType", CValue::siString, siPersistable, L"Particle Type", L"", dft, param);

	prop.PutParameterValue(L"ParticleType", CValue(L"Sprite"));
	
	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManParticles_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	CValueArray comboItems1(8);
	comboItems1[0] = L"Point"; comboItems1[1] = 0l;
	comboItems1[2] = L"Sphere"; comboItems1[3] = 1l;
	comboItems1[4] = L"Blobby"; comboItems1[5] = 2l;
	comboItems1[6] = L"Sprite"; comboItems1[7] = 3l;
	item = oLayout.AddEnumControl(L"ParticleType", comboItems1, L"", siControlCombo) ;
	item.PutLabelPercentage(90);

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManAttributes.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIShadowControls(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CValue dft;	// Used for arguments we don't want to set
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {

		if (!SIObject(sel[i]).IsA(siLightID)) {
			app.LogMessage(SceneItem(sel[i]).GetName() + L" is not a light object!");
			continue;
		}

		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManShadows");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding shadow map controls property to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManShadows");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManShadows_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"ReuseShadowMap", CValue::siBool, siAnimatable, L"Reuse Shadow Map", L"", false, param);
	prop.AddParameter(L"ShadowCamera", CValue::siString, caps, L"Shadow Camera", L"", dft, param);
	prop.AddParameter(L"WriteFrequency", CValue::siString, siPersistable, L"Write Frequency", L"", dft, param);
	prop.AddParameter(L"ShadowMapType", CValue::siString, siPersistable, L"Shadow Map Type", L"", dft, param);
	prop.AddParameter(L"ShadowVolInterp", CValue::siString, siPersistable, L"Shadow Volume Interpretation", L"", dft, param);
	prop.AddParameter(L"ShadowPixelSamples", CValue::siInt4, caps, L"Shadow Pixel Samples", L"", 1., 0., 100., dft, dft, param);
	prop.AddParameter(L"ShadowMapBlur", CValue::siBool, siAnimatable, L"Motion Blur", L"", false, param);
	prop.AddParameter(L"RayTraceShadows", CValue::siBool, siAnimatable, L"Ray Trace", L"", false, param);

	prop.PutParameterValue(L"ShadowMapType", CValue(L"min"));
	prop.PutParameterValue(L"ShadowVolInterp", CValue(L"discrete"));
	prop.PutParameterValue(L"WriteFrequency", CValue(L"Every Frame"));

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManShadows_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	item = oLayout.AddItem(L"ReuseShadowMap", L"");

	item = oLayout.AddItem(L"ShadowCamera", L"Shadow Camera", siControlEdit);
	item.PutLabelPercentage(90);

	CValueArray comboItems1(4);
	comboItems1[0] = L"Every Frame"; comboItems1[1] = L"Every Frame";
	comboItems1[2] = L"Once Per Job"; comboItems1[3] = L"Once Per Job";
	item = oLayout.AddEnumControl(L"WriteFrequency", comboItems1, L"", siControlCombo) ;
	item.PutLabelPercentage(90);

	CValueArray comboItems2(10);
	comboItems2[0] = L"min"; comboItems2[1] = L"min";
	comboItems2[2] = L"max"; comboItems2[3] = L"max";
	comboItems2[4] = L"minmax"; comboItems2[5] = L"minmax";
	comboItems2[6] = L"midpoint"; comboItems2[7] = L"midpoint";
	comboItems2[8] = L"deepshad"; comboItems2[9] = L"deepshad";
	item = oLayout.AddEnumControl(L"ShadowMapType", comboItems2, L"", siControlCombo) ;
	item.PutLabelPercentage(90);

	CValueArray comboItems3(4);
	comboItems3[0] = L"discrete"; comboItems3[1] = L"discrete";
	comboItems3[2] = L"continuous"; comboItems3[3] = L"continuous";
	item = oLayout.AddEnumControl(L"ShadowVolInterp", comboItems3, L"", siControlCombo) ;
	item.PutLabelPercentage(90);

	item = oLayout.AddItem(L"ShadowPixelSamples", L"");
	item.PutAttribute(siUINoSlider, true);
	item.PutAttribute(siUIThumbWheel, true);
	item.PutWidthPercentage(83);
	
	item = oLayout.AddItem(L"ShadowMapBlur", L"");

	item = oLayout.AddItem(L"RayTraceShadows", L"");

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManHelpersShadows.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIEnvmapControls(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CValue dft;	// Used for arguments we don't want to set
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {

		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManEnvmap");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding environment map controls property to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManEnvmap");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManEnvmap_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set
	
	prop.AddParameter(L"envmap", CValue::siBool, siAnimatable, L"Create Environment Map", L"", true, param);
	prop.AddParameter(L"ReuseEnvMap", CValue::siBool, siAnimatable, L"Reuse Environment Map", L"", false, param);
	prop.AddParameter(L"WriteFrequency", CValue::siString, siPersistable, L"Write Frequency", L"", dft, param);
	prop.AddParameter(L"EnvmapSize", CValue::siInt4, caps, L"Map Size", L"", 256., 0., 10240., dft, dft, param);
	
	prop.PutParameterValue(L"WriteFrequency", CValue(L"Every Frame"));

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManEnvmap_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();
	
	item = oLayout.AddItem(L"envmap", L"");

	item = oLayout.AddItem(L"ReuseEnvmap", L"");

	CValueArray comboItems1(4);
	comboItems1[0] = L"Every Frame"; comboItems1[1] = L"Every Frame";
	comboItems1[2] = L"Once Per Job"; comboItems1[3] = L"Once Per Job";
	item = oLayout.AddEnumControl(L"WriteFrequency", comboItems1, L"", siControlCombo) ;
	item.PutLabelPercentage(90);

	item = oLayout.AddItem(L"EnvmapSize", L"");
	item.PutAttribute(siUINoSlider, true);
	item.PutAttribute(siUIThumbWheel, true);
	item.PutWidthPercentage(83);

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManHelpersEnvmap.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIPhotonMapControls(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CValue dft;	// Used for arguments we don't want to set
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {

		if (!SIObject(sel[i]).IsA(siLightID)) {
			app.LogMessage(SceneItem(sel[i]).GetName() + L" is not a light object!");
			continue;
		}

		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManPhotonMap");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding photon map controls property to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManPhotonMap");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManPhotonMap_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set
	
	prop.AddParameter(L"ReusePhotonMap", CValue::siBool, siAnimatable, L"Reuse Photon Map", L"", false, param);
	prop.AddParameter(L"WriteFrequency", CValue::siString, siPersistable, L"Write Frequency", L"", dft, param);
	prop.AddParameter(L"Estimator", CValue::siInt4, caps, L"Estimator", L"", 50., 0., 10000., dft, dft, param);
	
	prop.PutParameterValue(L"WriteFrequency", CValue(L"Every Frame"));

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManPhotonMap_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();
	
	item = oLayout.AddItem(L"ReusePhotonMap", L"");

	CValueArray comboItems1(4);
	comboItems1[0] = L"Every Frame"; comboItems1[1] = L"Every Frame";
	comboItems1[2] = L"Once Per Job"; comboItems1[3] = L"Once Per Job";
	item = oLayout.AddEnumControl(L"WriteFrequency", comboItems1, L"", siControlCombo) ;
	item.PutLabelPercentage(90);

	item = oLayout.AddItem(L"Estimator", L"");
	item.PutAttribute(siUINoSlider, true);
	item.PutAttribute(siUIThumbWheel, true);
	item.PutWidthPercentage(83);

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManHelpersPhotonMap.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIRegisterShader(XSI::CRef& in_ref)
{
	Application app;
	Property prop;
	CValue index, retval; 
	CValueArray args;
	CStatus st = CStatus::OK;

	Model root = app.GetActiveSceneRoot();
	CString renderer = L"PRMan";
	CRef propRef = root.GetProperties().GetItem(L"XSIManOptions");
	if (propRef.IsValid()) {
		Property XSIManOptions(propRef);
		Parameter Renderer = XSIManOptions.GetParameters().GetItem(L"Renderer");
		renderer = Renderer.GetValue();
	}

	if (renderer == L"PRMan") st = app.ExecuteCommand(L"slo2spdl", args, retval);
	else if (renderer == L"3Delight") st = app.ExecuteCommand(L"sdl2spdl", args, retval);
	else if (renderer == L"Air") st = app.ExecuteCommand(L"slb2spdl", args, retval);
	//else if (renderer == L"RenderDotC") st = app.ExecuteCommand(L"so2spdl", args, retval);

	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIAbout(XSI::CRef& in_ref)
{
	Application app;
	UIToolkit uiToolKit = app.GetUIToolkit();
	CStatus st;
	long outResult;
	CString aboutMessage(L"    XSIMan\n    by Doug Creel\n    Graphic Primitives, LLC\n    Copyright (C) 2005-2006. All Rights Reserved");
	CString aboutCaption(L"XSIMan");
	st = uiToolKit.MsgBox(aboutMessage, siMsgOkOnly, aboutCaption, outResult);
	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIHelp(XSI::CRef& in_ref)
{
	Application app;

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	CString homePage = CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIMan.htm").GetAsText();

	CValue index; CValueArray args(1);
	args[0] = CValue(homePage);
	app.ExecuteCommand(L"OpenNetView", args, index);

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIArchiveControls(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CValue dft;	// Used for arguments we don't want to set
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {

		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManArchive");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding archive control to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManArchive");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManArchive_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"ReuseArchive", CValue::siBool, siAnimatable, L"Reuse RIB Archive", L"", false, param);
	prop.AddParameter(L"WriteFrequency", CValue::siString, siPersistable, L"Write Frequency", L"", dft, param);
	prop.AddParameter(L"ArchiveType", CValue::siString, siPersistable, L"RIB Archive Type", L"", dft, param);
	prop.AddParameter(L"ArchivePass", CValue::siInt4, siPersistable, L"Archive Pass", L"", 277l, 0l, 277l, dft, dft, param);
	prop.AddParameter(L"ArchiveContents", CValue::siInt4, siPersistable, L"Archive Contents", L"", 5l, 0l, 5l, dft, dft, param);
	prop.AddParameter(L"BBoxPad", CValue::siFloat, caps, L"BBox pad", L"", 0.0, 0.0, 100.0, dft, dft, param);
	
	prop.PutParameterValue(L"ArchiveType", CValue(L"Read"));
	prop.PutParameterValue(L"WriteFrequency", CValue(L"Every Frame"));

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManArchive_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	item = oLayout.AddItem(L"ReuseArchive", L"");

	CValueArray comboItems1(4);
	comboItems1[0] = L"Read"; comboItems1[1] = L"Read";
	comboItems1[2] = L"Delayed Read"; comboItems1[3] = L"Delayed Read";
	item = oLayout.AddEnumControl(L"ArchiveType", comboItems1, L"", siControlCombo) ;
	item.PutLabelPercentage(90);

	CValueArray comboItems2(4);
	comboItems2[0] = L"Every Frame"; comboItems2[1] = L"Every Frame";
	comboItems2[2] = L"Once Per Job"; comboItems2[3] = L"Once Per Job";
	item = oLayout.AddEnumControl(L"WriteFrequency", comboItems2, L"", siControlCombo) ;
	item.PutLabelPercentage(90);

	CValueArray bitmaskItems1(8) ;
	bitmaskItems1[0] = L"Beauty"; bitmaskItems1[1] = 1l ;
	bitmaskItems1[2] = L"Shadow"; bitmaskItems1[3] = 4l ;
	bitmaskItems1[4] = L"Environment"; bitmaskItems1[5] = 16l ;
	bitmaskItems1[6] = L"Photon"; bitmaskItems1[7] = 256l ;
	item = oLayout.AddEnumControl(L"ArchivePass", bitmaskItems1, L"", siControlCheck);
	item.PutLabelPercentage(90);

	CValueArray bitmaskItems2(4) ;
	bitmaskItems2[0] = L"Shaders"; bitmaskItems2[1] = 1l ;
	bitmaskItems2[2] = L"Lights"; bitmaskItems2[3] = 4l ;
	item = oLayout.AddEnumControl(L"ArchiveContents", bitmaskItems2, L"", siControlCheck);
	item.PutLabelPercentage(90);

	item = oLayout.AddItem(L"BBoxPad", L"");
	item.PutAttribute(siUIThumbWheel, true); 
	item.PutAttribute(siUINoSlider, true);
	item.PutAttribute(siUIDecimals, L"2");

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManHelpersArchive.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIRibBox(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);
	int caps = siPersistable;
	CValue dft;	

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManRibBox");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding RIB box to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManRibBox");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManRibBox_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"RIBBox", CValue::siString, caps, L"RIB Box", L"", dft, param);
	prop.AddParameter(L"RIBBoxReplace", CValue::siBool, siAnimatable, L"Replace Object", L"", false, param);
	prop.AddParameter(L"RIBBoxPass", CValue::siInt4, siPersistable, L"RIB Box Pass", L"", 277l, 0l, 277l, dft, dft, param);

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManRibBox_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	item = oLayout.AddItem(L"RIBBox", L"");

	CValueArray bitmaskItems(8) ;
	bitmaskItems[0] = L"Beauty"; bitmaskItems[1] = 1l ;
	bitmaskItems[2] = L"Shadow"; bitmaskItems[3] = 4l ;
	bitmaskItems[4] = L"Environment"; bitmaskItems[5] = 16l ;
	bitmaskItems[6] = L"Photon"; bitmaskItems[7] = 256l ;
	item = oLayout.AddEnumControl(L"RIBBoxPass", bitmaskItems, L"", siControlCheck);
	item.PutLabelPercentage(90);

	item = oLayout.AddItem(L"RIBBoxReplace", L"");

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManHelpersRibBox.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIScriptBox(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);
	int caps = siPersistable;
	CValue dft;	

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManScriptBox");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding Script box to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManScriptBox");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManScriptBox_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"ScriptBox", CValue::siString, caps, L"Script Box", L"", dft, param);
	prop.AddParameter(L"ScriptBoxPass", CValue::siInt4, siPersistable, L"Script Box Pass", L"", 277l, 0l, 277l, dft, dft, param);

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManScriptBox_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	item = oLayout.AddItem(L"ScriptBox", L"");

	CValueArray bitmaskItems(8) ;
	bitmaskItems[0] = L"Beauty"; bitmaskItems[1] = 1l;
	bitmaskItems[2] = L"Shadow"; bitmaskItems[3] = 4l;
	bitmaskItems[4] = L"Environment"; bitmaskItems[5] = 16l;
	bitmaskItems[6] = L"Photon"; bitmaskItems[7] = 256l;
	item = oLayout.AddEnumControl(L"ScriptBoxPass", bitmaskItems, L"", siControlCheck);
	item.PutLabelPercentage(90);

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManHelpersScriptBox.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSITraceSet(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);
	int caps = siPersistable;
	CValue dft;	

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManTraceSet");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding Trace Set to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManTraceSet");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManTraceSet_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"TraceSet", CValue::siString, caps, L"Trace Set", L"", dft, param);

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManTraceSet_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	item = oLayout.AddItem(L"TraceSet", L"");

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManHelpersTraceSet.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSILODControls(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CValue dft;	// Used for arguments we don't want to set
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {

		if (!SIObject(sel[i]).IsA(siModelID)) {
			app.LogMessage(SceneItem(sel[i]).GetName() + L" is not a model object!");
			continue;
		}

		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManLODControls");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding level of detail control to " + objName);
			Property prop;
			prop = SceneItem(sel[i]).AddProperty(L"XSIManLODControls");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManLODControls_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"LevelOfDetail", CValue::siBool, siAnimatable, L"Level of Detail", L"", true, param);
	prop.AddParameter(L"DetailSizeMinX", CValue::siFloat, caps, L"Min", L"", -1.0, dft, dft, dft, dft, param);
	prop.AddParameter(L"DetailSizeMinY", CValue::siFloat, caps, L"", L"", -1.0, dft, dft, dft, dft, param);
	prop.AddParameter(L"DetailSizeMinZ", CValue::siFloat, caps, L"", L"", -1.0, dft, dft, dft, dft, param);
	prop.AddParameter(L"DetailSizeMaxX", CValue::siFloat, caps, L"Max", L"", 1.0, dft, dft, dft, dft, param);
	prop.AddParameter(L"DetailSizeMaxY", CValue::siFloat, caps, L"", L"", 1.0, dft, dft, dft, dft, param);
	prop.AddParameter(L"DetailSizeMaxZ", CValue::siFloat, caps, L"", L"", 1.0, dft, dft, dft, dft, param);
	
	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManLODControls_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	item = oLayout.AddItem(L"LevelOfDetail", L"");

	oLayout.AddGroup(L"Detail Min Size");
		
		oLayout.AddRow();

			item = oLayout.AddItem(L"DetailSizeMinX", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"2");
			item.PutAttribute(siUINoLabel, true);

			item = oLayout.AddItem(L"DetailSizeMinY", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"2"); 
			item.PutAttribute(siUINoLabel, true);

			item = oLayout.AddItem(L"DetailSizeMinZ", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"2"); 
			item.PutAttribute(siUINoLabel, true);

		oLayout.EndRow();

	oLayout.EndGroup();

	oLayout.AddGroup(L"Detail Max Size");

		oLayout.AddRow();

			item = oLayout.AddItem(L"DetailSizeMaxX", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"2"); 
			item.PutAttribute(siUINoLabel, true);

			item = oLayout.AddItem(L"DetailSizeMaxY", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"2"); 
			item.PutAttribute(siUINoLabel, true);

			item = oLayout.AddItem(L"DetailSizeMaxZ", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"2"); 
			item.PutAttribute(siUINoLabel, true);

		oLayout.EndRow();

	oLayout.EndGroup();

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManAttributes.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIDetailRange(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CValue dft;	// Used for arguments we don't want to set
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {

		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManArchive");
		if (!propRef.IsValid()) {
			app.LogMessage(objName + L" does not a rib archive property!");
			return CStatus::Fail;
		}

		propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManDetailRange");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding detail range control to " + objName);
			Property prop;
			prop = SceneItem(sel[i]).AddProperty(L"XSIManDetailRange");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManDetailRange_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"DetailStart", CValue::siFloat, caps, L"Min", L"", 0.0, dft, dft, dft, dft, param);
	prop.AddParameter(L"DetailLow", CValue::siFloat, caps, L"", L"", 0.0, dft, dft, dft, dft, param);
	prop.AddParameter(L"DetailHigh", CValue::siFloat, caps, L"", L"", 2.0, dft, dft, dft, dft, param);
	prop.AddParameter(L"DetailEnd", CValue::siFloat, caps, L"Max", L"", 4.0, dft, dft, dft, dft, param);
	
	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManDetailRange_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	oLayout.AddGroup(L"Detail Range");
		
		oLayout.AddRow();

			item = oLayout.AddItem(L"DetailStart", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"2");
			item.PutAttribute(siUINoLabel, true);

			item = oLayout.AddItem(L"DetailLow", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"2"); 
			item.PutAttribute(siUINoLabel, true);

			item = oLayout.AddItem(L"DetailHigh", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"2"); 
			item.PutAttribute(siUINoLabel, true);

			item = oLayout.AddItem(L"DetailEnd", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"2"); 
			item.PutAttribute(siUINoLabel, true);

		oLayout.EndRow();

	oLayout.EndGroup();

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManAttributes.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIPrefAttach(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		
		Property playControl = app.GetActiveProject().GetProperties().GetItem(L"Play Control");
		float cframe = playControl.GetParameterValue(L"current");

		X3DObject x3dobject(sel[i]);
		Geometry geom(x3dobject.GetActivePrimitive().GetGeometry(cframe));

		objName = SIObject(sel[i]).GetName();
		app.LogMessage(L"Adding Pref user data map to " + objName);

		CLongArray pointIndexArray;
		MATH::CVector3Array positionArray;
		long numPoints;

		// get points at current frame
		CPointRefArray pointRefArray(geom.GetPoints());
		positionArray = pointRefArray.GetPositionArray();
		pointIndexArray = pointRefArray.GetIndexArray();
		numPoints = positionArray.GetCount();
	
		// what type of object are we dealing with?
		/*switch (geom.GetRef().GetClassID()) {
			case siNurbsSurfaceMeshID: 
				NurbsSurfaceMesh nurbSurfaceMesh(geom);
				CNurbsSurfaceDataArray nurbsSurfaceDataArray;
				nurbSurfaceMesh.Get(siIGESNurbs, nurbsSurfaceDataArray);

				for (long i = 0; i < nurbsSurfaceDataArray.GetCount(); ++i) {
					long nu = nurbsSurfaceDataArray[i].m_lNbUControlPoints;
					long nv = nurbsSurfaceDataArray[i].m_lNbVControlPoints;
							
					unsigned k = 0;
					for (long j = 0; j < (nu * nv); j++) {
						positionArray[i].PutX(nurbsSurfaceDataArray[i].m_aControlPoints[j].GetX());
						positionArray[i].PutY(nurbsSurfaceDataArray[i].m_aControlPoints[j].GetY());
						positionArray[i].PutZ(nurbsSurfaceDataArray[i].m_aControlPoints[j].GetZ());
						k += 4;
					}
				}

				break;
		}*/

		// Create a cluster
		Cluster prefCluster;
		geom.AddCluster(siVertexCluster, L"", pointIndexArray, prefCluster);
		SceneItem prefClusterSceneItem(prefCluster);

		// Add the user data map
		UserDataMap prefMap;
		prefClusterSceneItem.AddProperty(L"UserDataMap", false, L"rmanPPref", prefMap);

		// Initialize the user data map to default values
		struct vec3 {
			float x;
			float y;
			float z;
		};
		vec3 prefValues;
		for (long i = 0 ; i < prefMap.GetCount() ; i++) {
			prefValues.x = (float)positionArray[i].GetX();
			prefValues.y = (float)positionArray[i].GetY();
			prefValues.z = (float)positionArray[i].GetZ();
			prefMap.PutItemValue(i, (unsigned char*)&prefValues, sizeof(vec3));
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIPrefDetach(XSI::CRef& in_ref)
{
	Application app;
	CStatus st(CStatus::OK);
	CValue retval;

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		X3DObject x3dobject(sel[i]);
		Geometry geom(x3dobject.GetActivePrimitive().GetGeometry());

		CRefArray clsRefArray = geom.GetClusters();
		for (long j = 0; j < clsRefArray.GetCount(); ++j) {
			Cluster cls(clsRefArray[j]);
			UserDataMap udm = cls.GetProperties().GetItem(L"rmanPPref");
			if (udm.IsValid()) {
				DeleteObj(clsRefArray[j]);
				break;
			}
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIShadingModelAttach(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManShadingModel");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding shading model control to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManShadingModel");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIShadingModelDetach(XSI::CRef& in_ref)
{
	Application app;
	CStatus st(CStatus::OK);
	CValue retval;

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManShadingModel");
		if (propRef.IsValid()) {
			DeleteObj(propRef);
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManShadingModel_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"ShadingModel", CValue::siString, siPersistable, L"Shading Model", L"", dft, param);
	
	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManShadingModel_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	CValueArray comboItems1(12);
	comboItems1[0] = L""; comboItems1[1] = L"";
	comboItems1[2] = L"glass"; comboItems1[3] = L"glass";
	comboItems1[4] = L"water"; comboItems1[5] = L"water";
	comboItems1[6] = L"chrome"; comboItems1[7] = L"chrome";
	comboItems1[8] = L"matte"; comboItems1[9] = L"matte";
	comboItems1[10] = L"transparent"; comboItems1[11] = L"transparent";
	item = oLayout.AddEnumControl(L"ShadingModel", comboItems1, L"", siControlCombo);
	item.PutLabelPercentage(90);

	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManAttributes.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManOptions_Define(const CRef & in_Ctx)
{
	Application app;
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	// Display tab parameters
	prop.AddParameter(L"ExportCamera", CValue::siString, caps, L"Export Camera", L"", dft, param);
	prop.AddParameter(L"RibName", CValue::siString, caps, L"RIB Name", L"", dft, param);
	prop.AddParameter(L"FramePadding", CValue::siInt4, caps, L"Frame Padding", L"", 1., 1., 4., dft, dft, param);
	prop.AddParameter(L"ResolutionX", CValue::siInt4, caps, L"Resolution", L"", 640., 0., 16384., dft, dft, param);
	prop.AddParameter(L"ResolutionY", CValue::siInt4, caps, L"", L"", 486., 0., 16384., dft, dft, param);
	prop.AddParameter(L"PixelAspectRatio", CValue::siFloat, caps, L"Pixel Aspect Ratio", L"", 1.0, 0.0, 10.0, dft, dft, param);
	prop.AddParameter(L"DisplayDriver", CValue::siString, siPersistable, L"Display Driver", L"", dft, param);
	prop.AddParameter(L"DisplayMode", CValue::siString, siPersistable, L"Display Mode", L"", dft, param);
	prop.AddParameter(L"Quantization", CValue::siString, siPersistable, L"Quantize", L"", dft, param);
	prop.AddParameter(L"QuantizeOne", CValue::siInt4, caps, L"Quantize One", L"", 255., 0., 1000000., dft, dft, param);
	prop.AddParameter(L"QuantizeMin", CValue::siInt4, caps, L"Quantize Min", L"", 0., 0., 1000000., dft, dft, param);
	prop.AddParameter(L"QuantizeMax", CValue::siInt4, caps, L"Quantize Max", L"", 255., 0., 1000000., dft, dft, param);
	prop.AddParameter(L"QuantizeDither", CValue::siFloat, caps, L"Quantize Dither", L"", 0.5, 0., 1., dft, dft, param);
	prop.AddParameter(L"ExposureGain", CValue::siFloat, caps, L"Exposure", L"", 1., 0.0, 20.0, dft, dft, param);
	prop.AddParameter(L"ExposureGamma", CValue::siFloat, caps, L"", L"", 1., 0.0, 20.0, dft, dft, param);
	prop.AddParameter(L"PrimaryDisplay", CValue::siBool, siPersistable, L"Primary Display Only", L"", dft, param);

	// AOVs tab parameters
	Parameter gridParam1 = prop.AddGridParameter(L"AOVList");
	GridData gridData1 = gridParam1.GetValue();
	gridData1.PutColumnCount(11);
	gridData1.PutRowCount(32);
	gridData1.PutColumnLabel(0, L"Name");
	gridData1.PutColumnLabel(1, L"Type");
	gridData1.PutColumnLabel(2, L"Mode");
	gridData1.PutColumnLabel(3, L"PixelFilter");
	gridData1.PutColumnLabel(4, L"FilterSizeX");
	gridData1.PutColumnLabel(5, L"FilterSizeY");
	gridData1.PutColumnLabel(6, L"QuantizeZero");
	gridData1.PutColumnLabel(7, L"QuantizeOne");
	gridData1.PutColumnLabel(8, L"QuantizeMin");
	gridData1.PutColumnLabel(9, L"QuantizeMax");
	gridData1.PutColumnLabel(10, L"QuantizeDither");
	for (long i = 0; i < 32; i++) gridData1.PutRowLabel(i, CValue(i+1).GetAsText());

	prop.AddParameter(L"AOVName", CValue::siString, caps, L"Name", L"", dft, param);
	prop.AddParameter(L"AOVType", CValue::siString, siPersistable, L"Type", L"", dft, param);
	prop.AddParameter(L"AOVMode", CValue::siString, siPersistable, L"Mode", L"", dft, param);
	prop.AddParameter(L"AOVModeList", CValue::siString, siPersistable, L"", L"", dft, param);
	prop.AddParameter(L"AOVPixelFilter", CValue::siString, siPersistable, L"Pixel Filter", L"", dft, param);
	prop.AddParameter(L"AOVFilterSizeX", CValue::siInt2, siPersistable, L"Filter Size", L"", 2., 1., 100., dft, dft, param);
	prop.AddParameter(L"AOVFilterSizeY", CValue::siInt2, siPersistable, L"", L"", 2., 1., 256., dft, dft, param);
	prop.AddParameter(L"AOVQuantization", CValue::siString, siPersistable, L"Quantize", L"", dft, param);
	prop.AddParameter(L"AOVQuantizeZero", CValue::siInt4, caps, L"Quantize Zero", L"", 0., 0., 1000000., dft, dft, param);
	prop.AddParameter(L"AOVQuantizeOne", CValue::siInt4, caps, L"Quantize One", L"", 255., 0., 1000000., dft, dft, param);
	prop.AddParameter(L"AOVQuantizeMin", CValue::siInt4, caps, L"Quantize Min", L"", 0., 0., 1000000., dft, dft, param);
	prop.AddParameter(L"AOVQuantizeMax", CValue::siInt4, caps, L"Quantize Max", L"", 255., 0., 1000000., dft, dft, param);
	prop.AddParameter(L"AOVQuantizeDither", CValue::siFloat, caps, L"Quantize Dither", L"", 0.5, 0., 1., dft, dft, param);

	// Camera tab parameters
	prop.AddParameter(L"ImageName", CValue::siString, caps, L"Image Name", L"", dft, param);
	prop.AddParameter(L"fStop", CValue::siFloat, siAnimatable|siPersistable, L"fStop", L"", 0., 0.0, 100.0, dft, dft, param);
	prop.AddParameter(L"FocalDistance", CValue::siFloat, siAnimatable|siPersistable, L"Focal Distance", L"", 0., 0.0, 1000.0, dft, dft, param);
	prop.AddParameter(L"MotionBlur", CValue::siBool, siPersistable, L"Motion Blur", L"", dft, param);
	prop.AddParameter(L"DeformBlur", CValue::siBool, siPersistable, L"Deform Blur", L"", dft, param);
	prop.AddParameter(L"CameraBlur", CValue::siBool, siPersistable, L"Camera Blur", L"", dft, param);
	prop.AddParameter(L"ShutterAngle", CValue::siFloat, siAnimatable|siPersistable, L"Shutter Angle", L"", 180., 0.0, 1000.0, dft, dft, param);
	prop.AddParameter(L"ShutterTiming", CValue::siString, siPersistable, L"Shutter Timing", L"", dft, param);
	prop.AddParameter(L"FrontplaneGeom", CValue::siBool, siPersistable, L"Front Plane Geometry", L"", dft, param);
	prop.AddParameter(L"BackplaneGeom", CValue::siBool, siPersistable, L"Back Plane Geometry", L"", dft, param);

	// Reyes tab parameters
	prop.AddParameter(L"BinaryDicing", CValue::siBool, siPersistable, L"Binary Dicing", L"", dft, param);
	prop.AddParameter(L"ShadingRate", CValue::siFloat, caps, L"Shading Rate", L"", 5.0, 0.0, 100.0, dft, dft, param);
	prop.AddParameter(L"PixelSamplesX", CValue::siInt2, caps, L"Pixel Samples", L"", 3., 1., 100., dft, dft, param);
	prop.AddParameter(L"PixelSamplesY", CValue::siInt2, caps, L"", L"", 3., 1., 100., dft, dft, param);
	prop.AddParameter(L"Eyesplits", CValue::siInt2, caps, L"Eye Splits", L"", 10., 0., 10., dft, dft, param);
	prop.AddParameter(L"PixelFilter", CValue::siString, siPersistable, L"Pixel Filter", L"", dft, param);
	prop.AddParameter(L"FilterSizeX", CValue::siInt2, siPersistable, L"Filter Size", L"", 2., 1., 100., dft, dft, param);
	prop.AddParameter(L"FilterSizeY", CValue::siInt2, siPersistable, L"", L"", 2., 1., 256., dft, dft, param);
	prop.AddParameter(L"BucketSizeX", CValue::siInt2, siPersistable, L"Bucket Size", L"", 16., 1., 100., dft, dft, param);
	prop.AddParameter(L"BucketSizeY", CValue::siInt2, siPersistable, L"", L"", 16., 1., 100., dft, dft, param);
	prop.AddParameter(L"GridSize", CValue::siInt4, siPersistable, L"Grid Size", L"", 256., 1., 65536., dft, dft, param);
	prop.AddParameter(L"TextureMemory", CValue::siInt4, siPersistable, L"Texture Memory", L"", 8192., 1., 1000000., dft, dft, param);
	prop.AddParameter(L"ShadingInterpolation", CValue::siBool, siPersistable, L"Shading Interpolation", L"", dft, param);
	prop.AddParameter(L"ShadowBias", CValue::siFloat, caps, L"Shadow Bias", L"", 1.0, 0.0, 100.0, dft, dft, param);
	prop.AddParameter(L"MotionFactor", CValue::siFloat, caps, L"Motion Factor", L"", 1.0, 0.0, 20.0, dft, dft, param);
	prop.AddParameter(L"DicingStrategy", CValue::siString, siPersistable, L"Dicing Strategy", L"", dft, param);
	prop.AddParameter(L"DicingCamera", CValue::siString, siPersistable, L"Dicing Camera", L"", dft, param);
	prop.AddParameter(L"RasterOriented", CValue::siBool, siPersistable, L"Raster Oriented", L"", dft, param);
	prop.AddParameter(L"OpacityThresholdR", CValue::siFloat, caps, L"", L"", 1.0, 0.0, 1.0, 0.0, 1.0, param);
	prop.AddParameter(L"OpacityThresholdG", CValue::siFloat, caps, L"", L"", 1.0, 0.0, 1.0, 0.0, 1.0, param);
	prop.AddParameter(L"OpacityThresholdB", CValue::siFloat, caps, L"", L"", 1.0, 0.0, 1.0, 0.0, 1.0, param);
	prop.AddParameter(L"OpacityCullingR", CValue::siFloat, caps, L"", L"", 0.996, 0.0, 1.0, 0.0, 1.0, param);
	prop.AddParameter(L"OpacityCullingG", CValue::siFloat, caps, L"", L"", 0.996, 0.0, 1.0, 0.0, 1.0, param);
	prop.AddParameter(L"OpacityCullingB", CValue::siFloat, caps, L"", L"", 0.996, 0.0, 1.0, 0.0, 1.0, param);

	// Ray tracing tab parameters
	prop.AddParameter(L"EnableRayTracing", CValue::siBool, siPersistable, L"Enable Ray Tracing", L"", dft, param);
	prop.AddParameter(L"RayTraceBias", CValue::siFloat, caps, L"Ray Trace Bias", L"", .05, 0.0, 100.0, dft, dft, param);
	prop.AddParameter(L"MaxRayDepth", CValue::siInt2, siPersistable, L"Max Ray Depth", L"", 10., 1., 100., dft, dft, param);
	prop.AddParameter(L"RayTraceBreadthFactor", CValue::siFloat, caps, L"Breadth Factor", L"", 1., 0.0, 100.0, dft, dft, param);
	prop.AddParameter(L"RayTraceDepthFactor", CValue::siFloat, caps, L"Depth Factor", L"", 1., 0.0, 100.0, dft, dft, param);
	prop.AddParameter(L"RayTraceMaxSpecularDepth", CValue::siInt2, siPersistable, L"Max Specular Depth", L"", 2., 1., 100., dft, dft, param);
	prop.AddParameter(L"RayTraceMaxDiffuseDepth", CValue::siInt2, siPersistable, L"Max Diffuse Depth", L"", 1., 1., 100., dft, dft, param);
	prop.AddParameter(L"RayTraceDisplacements", CValue::siBool, siPersistable, L"Trace Displacements", L"", dft, param);
	prop.AddParameter(L"RayTraceSampleMotion", CValue::siBool, siPersistable, L"Sample Motion", L"", dft, param);
	prop.AddParameter(L"EnableAmbientOcclusion", CValue::siBool, siPersistable, L"Enable Ambient Occlusion", L"", dft, param);
	prop.AddParameter(L"OcclusionSamples", CValue::siFloat, caps, L"Ambient Occlusion Samples", L"", 16., 0.0, 1000.0, dft, dft, param);
	prop.AddParameter(L"OcclusionMaxdist", CValue::siFloat, caps, L"Ambient Occlusion Max Distance", L"", 1000000., param);
	prop.AddParameter(L"EnableIndirectIllumination", CValue::siBool, siPersistable, L"Enable Indirect Illumination", L"", dft, param);
	prop.AddParameter(L"IndirectSamples", CValue::siFloat, caps, L"Indirect Illumination Samples", L"", 16., 0.0, 1000.0, dft, dft, param);
	prop.AddParameter(L"RayTraceMaxError", CValue::siFloat, caps, L"Max Error", L"", -1., -1.0, 100.0, dft, dft, param);
	prop.AddParameter(L"RayTraceMaxPixDist", CValue::siFloat, caps, L"Max Pixel Distance", L"", -1., -1.0, 100.0, dft, dft, param);
	prop.AddParameter(L"IrradianceCache", CValue::siString, caps, L"Irradiance Cache", L"", dft, param);
	prop.AddParameter(L"IrradianceCacheMode", CValue::siString, siPersistable, L"Cache Mode", L"", dft, param);
	prop.AddParameter(L"PhotonShadingModel", CValue::siString, siPersistable, L"Shading Model", L"", dft, param);
	prop.AddParameter(L"PhotonCausticMap", CValue::siString, caps, L"Caustic Map", L"", dft, param);
	prop.AddParameter(L"PhotonGlobalMap", CValue::siString, caps, L"Global Map", L"", dft, param);
	prop.AddParameter(L"PhotonEstimator", CValue::siFloat, caps, L"Estimator", L"", 50., 0.0, 1000.0, dft, dft, param);

	// accel tab parameters
	prop.AddParameter(L"RenderSelectedObjects", CValue::siBool, siPersistable, L"Render Selected Objects", L"", dft, param);
	prop.AddParameter(L"RenderSelectedLights", CValue::siBool, siPersistable, L"Render Selected Lights", L"", dft, param);
	prop.AddParameter(L"ApproximatedMesh", CValue::siBool, siPersistable, L"Output Approximated Mesh", L"", dft, param);
	prop.AddParameter(L"PolygonClusters", CValue::siBool, siPersistable, L"Output Polygon Clusters", L"", dft, param);
	prop.AddParameter(L"InstanceGeometry", CValue::siBool, siPersistable, L"Create Archive", L"", dft, param);
	prop.AddParameter(L"ReuseInstanceGeometry", CValue::siBool, siPersistable, L"Reuse Archive", L"", dft, param);
	prop.AddParameter(L"CropWindowX1", CValue::siFloat, caps, L"X min, max", L"", 0.0, 0.0, 1.0, dft, dft, param);
	prop.AddParameter(L"CropWindowX2", CValue::siFloat, caps, L"", L"", 1.0, 0.0, 1.0, dft, dft, param);
	prop.AddParameter(L"CropWindowY1", CValue::siFloat, caps, L"Y min, max", L"", 0.0, 0.0, 1.0, dft, dft, param);
	prop.AddParameter(L"CropWindowY2", CValue::siFloat, caps, L"", L"", 1.0, 0.0, 1.0, dft, dft, param);

	// Pipe tab parameters
	prop.AddParameter(L"UseJobScript", CValue::siBool, siPersistable, L"Use Job Script", L"", dft, param);
	prop.AddParameter(L"JobScriptType", CValue::siString, siPersistable, L"Job Script Type", L"", dft, param);
	prop.AddParameter(L"MinProcs", CValue::siInt4, siPersistable, L"Min/Max Processors", L"", 1., 1., 100., dft, dft, param);
	prop.AddParameter(L"MaxProcs", CValue::siInt4, siPersistable, L"", L"", 3., 1., 100., dft, dft, param);
	prop.AddParameter(L"AlfredTags", CValue::siString, caps, L"Alfred Tags", L"", dft, param);
	prop.AddParameter(L"AlfredServices", CValue::siString, caps, L"Alfred Services", L"", dft, param);
	prop.AddParameter(L"Imager", CValue::siString, caps, L"Imager", L"", dft, param);
	prop.AddParameter(L"RemoteCmd", CValue::siBool, siPersistable, L"Remote Command", L"", dft, param);
	prop.AddParameter(L"PreJobCommand", CValue::siString, caps, L"Pre Job Command", L"", dft, param);
	prop.AddParameter(L"PreFrameCommand", CValue::siString, caps, L"Pre Frame Command", L"", dft, param);
	prop.AddParameter(L"PostFrameCommand", CValue::siString, caps, L"Post Frame Command", L"", dft, param);
	prop.AddParameter(L"PostJobCommand", CValue::siString, caps, L"Post Job Command", L"", dft, param);
	prop.AddParameter(L"CleanJobScript", CValue::siBool, siPersistable, L"Clean Job Script", L"", dft, param);
	prop.AddParameter(L"PreJobScript", CValue::siString, caps, L"Pre Job Script", L"", dft, param);
	prop.AddParameter(L"PreFrameScript", CValue::siString, caps, L"Pre Frame Script", L"", dft, param);
	prop.AddParameter(L"PostFrameScript", CValue::siString, caps, L"Post Frame Script", L"", dft, param);
	prop.AddParameter(L"PostJobScript", CValue::siString, caps, L"Post Job Script", L"", dft, param);
	prop.AddParameter(L"ScriptLanguage", CValue::siString, siPersistable, L"Script Language", L"", dft, param);
	prop.AddParameter(L"LaunchRender", CValue::siBool, siPersistable, L"Launch Render", L"", dft, param);
	//prop.AddParameter(L"BypassRib", CValue::siBool, siPersistable, L"Bypass RIB Generation", L"", dft, param);
	prop.AddParameter(L"Renderer", CValue::siString, caps, L"Select Renderer", L"", dft, param);
	prop.AddParameter(L"RenderCommand", CValue::siString, caps, L"Render Command", L"", dft, param);
	prop.AddParameter(L"CustomCommand", CValue::siString, caps, L"Custom Command", L"", dft, param);
	prop.AddParameter(L"NumProcs", CValue::siInt4, siPersistable, L"Number of Processors", L"", 1., 0., 8., dft, dft, param);
	prop.AddParameter(L"ShowProgress", CValue::siBool, siPersistable, L"Show Progress", L"", dft, param);
	prop.AddParameter(L"RenderStatistics", CValue::siInt4, siPersistable, L"Render Statistics", L"", 0., 0., 3., dft, dft, param);
	prop.AddParameter(L"StatisticsFileName", CValue::siString, caps, L"Statistics File Name", L"", dft, param);
	prop.AddParameter(L"StatisticsXMLFileName", CValue::siString, caps, L"Statistics XML File Name", L"", dft, param);
	prop.AddParameter(L"RibOutput", CValue::siString, siPersistable, L"RIB Output", L"", dft, param);
	prop.AddParameter(L"JobSummary", CValue::siBool, siPersistable, L"Output Job Summary", L"", dft, param);
	prop.AddParameter(L"GzipRib", CValue::siBool, siPersistable, L"Compress RIB", L"", dft, param);
	prop.AddParameter(L"RenderFrames", CValue::siBool, siPersistable, L"Render Frames", L"", dft, param);
	prop.AddParameter(L"RenderMode", CValue::siString, siPersistable, L"RIB Generation Mode", L"", dft, param);
	prop.AddParameter(L"StartFrame", CValue::siInt4, siPersistable, L"Start Frame", L"", 1., 1., 10000., dft, dft, param);
	prop.AddParameter(L"EndFrame", CValue::siInt4, siPersistable, L"End Frame", L"", 100., 1., 10000., dft, dft, param);
	prop.AddParameter(L"StepSize", CValue::siInt4, siPersistable, L"Step Size", L"", 1., 1., 10000., dft, dft, param);
	prop.AddParameter(L"SkipRenderedFrames", CValue::siBool, siPersistable, L"Skip Rendered Frames", L"", dft, param);
	prop.AddParameter(L"Cleanup", CValue::siInt4, siPersistable, L"Cleanup", L"", 0l, 0l, 277l, dft, dft, param);
	prop.AddParameter(L"NamedHandles", CValue::siBool, siPersistable, L"Use Light Name", L"", dft, param);
	prop.AddParameter(L"HandlePrefix", CValue::siString, caps, L"Handle Prefix", L"", dft, param);
	
	// FX tab parameters
	prop.AddParameter(L"RenderTreeSupport", CValue::siBool, siPersistable, L"Enable", L"", dft, param);
	
	// Global texture settings
	prop.AddParameter(L"TexResize", CValue::siString, caps, L"Resize", L"", dft, param);
	prop.AddParameter(L"TexSMode", CValue::siString, caps, L"S Mode", L"", dft, param);
	prop.AddParameter(L"TexTMode", CValue::siString, caps, L"T Mode", L"", dft, param);
	prop.AddParameter(L"TexPattern", CValue::siString, caps, L"Pattern", L"", dft, param);
	prop.AddParameter(L"TexFormat", CValue::siString, caps, L"Format", L"", dft, param);
	prop.AddParameter(L"TexBytes", CValue::siString, caps, L"Bytes", L"", dft, param);
	prop.AddParameter(L"TexFilter", CValue::siString, caps, L"Filter", L"", dft, param);
	prop.AddParameter(L"TexFilterWidthS", CValue::siInt4, siPersistable, L"Filter Width S T", L"", 3., 1., 100., dft, dft, param);
	prop.AddParameter(L"TexFilterWidthT", CValue::siInt4, siPersistable, L"", L"", 3., 1., 100., dft, dft, param);

	// Paths tab parameters
	prop.AddParameter(L"RibPath", CValue::siString, caps, L"RIB Files", L"", dft, param);
	prop.AddParameter(L"PixPath", CValue::siString, caps, L"Rendered Pictures", L"", dft, param);
	prop.AddParameter(L"TexPath", CValue::siString, caps, L"Texture Maps", L"", dft, param);
	prop.AddParameter(L"ShdPath", CValue::siString, caps, L"Shadow Maps", L"", dft, param);
	prop.AddParameter(L"ShaderPath", CValue::siString, caps, L"Shaders", L"", dft, param);
	prop.AddParameter(L"JobPath", CValue::siString, caps, L"Job Scripts", L"", dft, param);
	prop.AddParameter(L"SloPath", CValue::siString, caps, L"Shaders", L"", dft, param);
	prop.AddParameter(L"ImgPath", CValue::siString, caps, L"Source Images", L"", dft, param);
	prop.AddParameter(L"DsoPath", CValue::siString, caps, L"Plug-ins", L"", dft, param);

	Parameter gridParam2 = prop.AddGridParameter(L"DirMap");
	GridData gridData2 = gridParam2.GetValue();
	gridData2.PutColumnCount(3);
	gridData2.PutRowCount(16);
	gridData2.PutColumnLabel(0, L"Zone");
	gridData2.PutColumnType(0, siColumnCombo);
	CValueArray comboItems1(4);
	comboItems1[0] = L"UNC"; comboItems1[1] = 0l;
	comboItems1[2] = L"NFS"; comboItems1[3] = 1l;
	gridData2.PutColumnComboItems(0, comboItems1);
	gridData2.PutColumnLabel(1, L"From");
	gridData2.PutColumnLabel(2, L"To");
	for (long i = 0; i < 16; i++) gridData2.PutRowLabel(i, CValue(i+1).GetAsText());

	// set defaults
	prop.PutParameterValue(L"ExportCamera", CValue(L"Camera"));
	prop.PutParameterValue(L"RibName", CValue(L"untitled"));
	prop.PutParameterValue(L"ImageName", CValue(L"untitled"));
	prop.PutParameterValue(L"DisplayDriver", CValue(L"framebuffer"));
	prop.PutParameterValue(L"DisplayMode", CValue(L"rgba"));
	prop.PutParameterValue(L"Quantization", CValue(L"8bit"));
	prop.PutParameterValue(L"AOVType", CValue(L"tiff"));
	prop.PutParameterValue(L"AOVPixelFilter", CValue(2l));
	prop.PutParameterValue(L"AOVQuantization", CValue(L"8bit"));
	prop.PutParameterValue(L"ShutterTiming", CValue(L"OpenOnFrame"));
	prop.PutParameterValue(L"FrontplaneGeom", false);
	prop.PutParameterValue(L"BackplaneGeom", false);
	prop.PutParameterValue(L"PixelFilter", CValue(2l));
	prop.PutParameterValue(L"DicingStrategy", CValue(L"planarprojection"));
	prop.PutParameterValue(L"DicingCamera", CValue(L"worldcamera"));
	prop.PutParameterValue(L"RasterOriented", true);
	prop.PutParameterValue(L"UseJobScript", false);
	prop.PutParameterValue(L"JobScriptType", CValue(L"Alfred"));
	prop.PutParameterValue(L"AlfredTags", CValue(L"prman"));
	prop.PutParameterValue(L"AlfredServices", CValue(L"pixarRender"));
	prop.PutParameterValue(L"Imager", CValue(L"sho"));
	prop.PutParameterValue(L"RemoteCmd", false);
	prop.PutParameterValue(L"CleanJobScript", true);
	prop.PutParameterValue(L"PreJobScript", CValue(L""));
	prop.PutParameterValue(L"PreFrameScript", CValue(L""));
	prop.PutParameterValue(L"PostFrameScript", CValue(L""));
	prop.PutParameterValue(L"PostJobScript", CValue(L""));
	prop.PutParameterValue(L"ScriptLanguage", CValue(L"VBScript"));
	prop.PutParameterValue(L"LaunchRender", true);
	//prop.PutParameterValue(L"BypassRib", false);
	prop.PutParameterValue(L"Renderer", CValue(L"PRMan"));
	prop.PutParameterValue(L"RenderCommand", CValue(L"prman"));
	prop.PutParameterValue(L"NumProcs", CValue(1l));
	prop.PutParameterValue(L"ShowProgress", false);
	prop.PutParameterValue(L"RenderStatistics", CValue((long)0));
	prop.PutParameterValue(L"StatisticsFileName", CValue(L""));
	prop.PutParameterValue(L"StatisticsXMLFileName", CValue(L""));
	prop.PutParameterValue(L"JobSummary", false);
	prop.PutParameterValue(L"TexResize", CValue(L"up-"));
	prop.PutParameterValue(L"TexSMode", CValue(L"periodic"));
	prop.PutParameterValue(L"TexTMode", CValue(L"periodic"));
	prop.PutParameterValue(L"TexPattern", CValue(L"diagonal"));
	prop.PutParameterValue(L"TexFormat", CValue(L"pixar"));
	prop.PutParameterValue(L"TexBytes", CValue(L""));
	prop.PutParameterValue(L"TexFilter", CValue(L"catmull-rom"));
	prop.PutParameterValue(L"RenderSelectedObjects", false);
	prop.PutParameterValue(L"RenderSelectedLights", false);
	prop.PutParameterValue(L"ApproximatedMesh", false);
	prop.PutParameterValue(L"PolygonClusters", false);
	prop.PutParameterValue(L"InstanceGeometry", false);
	prop.PutParameterValue(L"ReuseInstanceGeometry", false);
	prop.PutParameterValue(L"RibOutput", CValue(L"ascii"));
	prop.PutParameterValue(L"RenderMode", CValue(L"immediate"));
	prop.PutParameterValue(L"RenderTreeSupport", false);
	prop.PutParameterValue(L"RibPath", CValue(L""));
	prop.PutParameterValue(L"PixPath", CValue(L""));
	prop.PutParameterValue(L"TexPath", CValue(L""));
	prop.PutParameterValue(L"ShdPath", CValue(L""));
	prop.PutParameterValue(L"ShaderPath", CValue(L""));
	prop.PutParameterValue(L"JobPath", CValue(L""));
	prop.PutParameterValue(L"SloPath", CValue(L""));
	prop.PutParameterValue(L"ImgPath", CValue(L""));
	prop.PutParameterValue(L"DsoPath", CValue(L""));
	prop.PutParameterValue(L"IrradianceCacheMode", CValue(L"ignore"));
	prop.PutParameterValue(L"PhotonShadingModel", CValue(L""));
	prop.PutParameterValue(L"NamedHandles", false);
	prop.PutParameterValue(L"HandlePrefix", CValue(L""));

	// Retrieve the XSIManRenderGlobals structure	
	Model root = app.GetActiveSceneRoot();
	CRef propRef = root.GetProperties().GetItem(L"XSIManOptions");
	if (!propRef.IsValid()) {
		Property XSIManOptions(propRef);
		Parameter exportCameraParam = XSIManOptions.GetParameters().GetItem(L"ExportCamera");
		XSIManOptions.PutParameterValue(L"ExportCamera", exportCameraParam.GetValue());
		Parameter renderCommandParam = XSIManOptions.GetParameters().GetItem(L"RenderCommand");
		XSIManOptions.PutParameterValue(L"RenderCommand", renderCommandParam.GetValue());
		Parameter ribNameParam = XSIManOptions.GetParameters().GetItem(L"RibName");
		XSIManOptions.PutParameterValue(L"RibName", ribNameParam.GetValue());
		Parameter imageNameParam = XSIManOptions.GetParameters().GetItem(L"ImageName");
		XSIManOptions.PutParameterValue(L"ImageName", imageNameParam.GetValue());
	}

	return CStatus::OK;	
}

#ifdef unix
extern "C"
#endif
CStatus XSIManOptions_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	oLayout.AddTab(L"Display");

		item = oLayout.AddItem(L"ExportCamera", L"Camera", siControlEdit);
		item.PutLabelPercentage(90);

		item = oLayout.AddItem(L"RibName", L"RIB Name", siControlEdit);
		item.PutLabelPercentage(90);

		item = oLayout.AddItem(L"FramePadding", L"");
		item.PutAttribute(siUIThumbWheel, true); 
		item.PutAttribute(siUINoSlider, true);

		oLayout.AddRow();
			item = oLayout.AddItem(L"ResolutionX", L"");
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutWidthPercentage(83);
			item = oLayout.AddItem(L"ResolutionY", L"");
			item.PutAttribute(siUINoLabel, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);
		oLayout.EndRow();

		item = oLayout.AddItem(L"PixelAspectRatio", L"");
		item.PutAttribute(siUIThumbWheel, true); 
		item.PutAttribute(siUINoSlider, true);
		item.PutAttribute(siUIDecimals, L"2");

		CValueArray comboItems1(28);
		comboItems1[0] = L"it"; comboItems1[1] = L"it";
		comboItems1[2] = L"combiner"; comboItems1[3] = L"combiner";
		comboItems1[4] = L"framebuffer"; comboItems1[5] = L"framebuffer";
		comboItems1[6] = L"file"; comboItems1[7] = L"file";
		comboItems1[8] = L"tiff"; comboItems1[9] = L"tiff";
		comboItems1[10] = L"cineon"; comboItems1[11] = L"cineon";
		comboItems1[12] = L"alias"; comboItems1[13] = L"alias";
		comboItems1[14] = L"mayaiff"; comboItems1[15] = L"mayaiff";
		comboItems1[16] = L"targa"; comboItems1[17] = L"targa";
		comboItems1[18] = L"sgif"; comboItems1[19] = L"sgi";
		comboItems1[20] = L"picio"; comboItems1[21] = L"picio";
		comboItems1[22] = L"softimage"; comboItems1[23] = L"softimage";
		comboItems1[24] = L"shadowmap"; comboItems1[25] = L"shadowmap";
		comboItems1[26] = L"zfile"; comboItems1[27] = L"zfile";

		item = oLayout.AddEnumControl(L"DisplayDriver", comboItems1, L"", siControlCombo);
		item.PutLabelPercentage(90);

		CValueArray comboItems2(8);
		comboItems2[0] = L"rgb"; comboItems2[1] = L"rgb";
		comboItems2[2] = L"rgba"; comboItems2[3] = L"rgba";
		comboItems2[4] = L"rgbaz"; comboItems2[5] = L"rgbaz";
		comboItems2[6] = L"z"; comboItems2[7] = L"z";
		item = oLayout.AddEnumControl(L"DisplayMode", comboItems2, L"", siControlCombo);
		item.PutLabelPercentage(90);

		oLayout.AddRow();
			CValueArray comboItems3(6);
			comboItems3[0] = L"8bit"; comboItems3[1] = L"8bit";
			comboItems3[2] = L"16bit"; comboItems3[3] = L"16bit";
			comboItems3[4] = L"float"; comboItems3[5] = L"float";
			item = oLayout.AddEnumControl(L"Quantization", comboItems3, L"", siControlCombo);
			item = oLayout.AddItem(L"QuantizeOne", L"");
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoLabel, true);
			item = oLayout.AddItem(L"QuantizeMin", L"");
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoLabel, true);
			item = oLayout.AddItem(L"QuantizeMax", L"");
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoLabel, true);
			item = oLayout.AddItem(L"QuantizeDither", L"");
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoLabel, true);
		oLayout.EndRow();

		oLayout.AddRow();
			item = oLayout.AddItem(L"ExposureGain", L"");
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);
			item.PutWidthPercentage(83);
			item = oLayout.AddItem(L"ExposureGamma", L"");
			item.PutAttribute(siUINoLabel, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);
		oLayout.EndRow();
		
		item = oLayout.AddItem(L"PrimaryDisplay", L"");

	oLayout.AddTab(L"AOVs");

		oLayout.AddRow() ;
			item = oLayout.AddButton(L"AddAOV", L"Add");
			item.PutWidthPercentage(1l);
			item = oLayout.AddButton(L"DeleteAOV", L"Delete");
			item.PutWidthPercentage(1l);
		oLayout.EndRow();
		
		item = oLayout.AddItem(L"AOVList", L"List", siControlGrid);
		item.PutAttribute(siUIGridColumnWidths, L"25:100:100:100:100:100:100:100:100:100:100");
		item.PutAttribute(siUICY, 100l);
		item.PutAttribute(siUIGridLockColumnHeader, true);
		item.PutAttribute(siUINoLabel, true);
		item.PutAttribute(siUIGridReadOnlyColumns, true);
		
		item = oLayout.AddItem(L"AOVName", L"Name", siControlEdit);
		item.PutLabelPercentage(90);

		CValueArray comboItems8(6);
		comboItems8[0] = L"tiff"; comboItems8[1] = L"tiff";
		comboItems8[2] = L"alias"; comboItems8[3] = L"alias";
		comboItems8[4] = L"file"; comboItems8[5] = L"file";

		item = oLayout.AddEnumControl(L"AOVType", comboItems8, L"", siControlCombo);
		item.PutLabelPercentage(90);

		oLayout.AddRow();

			item = oLayout.AddItem(L"AOVMode", L"Mode", siControlEdit);

			CValueArray comboItems9(36);
			comboItems9[0] = L"varying point P"; comboItems9[1] = L"varying point P";
			comboItems9[2] = L"varying normal N"; comboItems9[3] = L"varying normal N";
			comboItems9[4] = L"varying normal Ng"; comboItems9[5] = L"varying normal Ng";
			comboItems9[6] = L"varying vector E"; comboItems9[7] = L"varying vector E";
			comboItems9[8] = L"varying vector dPdu"; comboItems9[9] = L"varying vector dPdu";
			comboItems9[10] = L"varying vector dPdv"; comboItems9[11] = L"varying vector dPdv";
			comboItems9[12] = L"varying float s"; comboItems9[13] = L"varying float s";
			comboItems9[14] = L"varying float t"; comboItems9[15] = L"varying float t";
			comboItems9[16] = L"varying float u"; comboItems9[17] = L"varying float u";
			comboItems9[18] = L"varying float v"; comboItems9[19] = L"varying float v"; 
			comboItems9[20] = L"varying float du"; comboItems9[21] = L"varying float du";
			comboItems9[22] = L"varying float dv"; comboItems9[23] = L"varying float dv";
			comboItems9[24] = L"varying vector dPdtime"; comboItems9[25] = L"varying vector dPdtime"; 
			comboItems9[26] = L"varying color Cs"; comboItems9[27] = L"varying color Cs";
			comboItems9[28] = L"varying color Os"; comboItems9[29] = L"varying color Os"; 
			comboItems9[30] = L"varying color Ci"; comboItems9[31] = L"varying color Ci";
			comboItems9[32] = L"varying color Oi"; comboItems9[33] = L"varying color Oi";
			comboItems9[34] = L"uniform float __CPUtime"; comboItems9[35] = L"uniform float __CPUtime";

			item = oLayout.AddEnumControl(L"AOVModeList", comboItems9, L"", siControlCombo);
			item.PutAttribute(siUINoLabel, true);

		oLayout.EndRow();

		CValueArray comboItems5(18);
		comboItems5[0] = L"Box"; comboItems5[1] = 0l;
		comboItems5[2] = L"Triangle"; comboItems5[3] = 1l;
		comboItems5[4] = L"CatmullRom"; comboItems5[5] = 2l;
		comboItems5[6] = L"Gauss"; comboItems5[7] = 3l;
		comboItems5[8] = L"Sinc"; comboItems5[9] = 4l;
		comboItems5[10] = L"BlackmanHarris"; comboItems5[11] = 5l;
		comboItems5[12] = L"Mitchell"; comboItems5[13] = 6l;
		comboItems5[14] = L"SepCatmullRom"; comboItems5[15] = 7l;
		comboItems5[16] = L"Bessel"; comboItems5[17] = 8l;
		item = oLayout.AddEnumControl(L"AOVPixelFilter", comboItems5, L"", siControlCombo);
		item.PutLabelPercentage(90);

		oLayout.AddRow();
			item = oLayout.AddItem(L"AOVFilterSizeX", L"");
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);
			item.PutWidthPercentage(83);
			item = oLayout.AddItem(L"AOVFilterSizeY", L"");
			item.PutAttribute(siUINoLabel, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);
		oLayout.EndRow();
		
		oLayout.AddRow();
			item = oLayout.AddEnumControl(L"AOVQuantization", comboItems3, L"", siControlCombo) ;
			item = oLayout.AddItem(L"AOVQuantizeZero", L"");
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoLabel, true);
			item = oLayout.AddItem(L"AOVQuantizeOne", L"");
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoLabel, true);
			item = oLayout.AddItem(L"AOVQuantizeMin", L"");
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoLabel, true);
			item = oLayout.AddItem(L"AOVQuantizeMax", L"");
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoLabel, true);
			item = oLayout.AddItem(L"AOVQuantizeDither", L"");
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoLabel, true);
		oLayout.EndRow();

	oLayout.AddTab(L"Camera");
		
		item = oLayout.AddItem(L"ImageName", L"Image Name", siControlEdit);
		item.PutLabelPercentage(90);

		item = oLayout.AddItem(L"fStop", L"");
		item.PutAttribute(siUIThumbWheel, true);
		item.PutAttribute(siUINoSlider, true);

		item = oLayout.AddItem(L"FocalDistance", L"");
		item.PutAttribute(siUIThumbWheel, true);
		item.PutAttribute(siUINoSlider, true);

		item = oLayout.AddItem(L"MotionBlur", L"");

		item = oLayout.AddItem(L"DeformBlur", L"");

		item = oLayout.AddItem(L"CameraBlur", L"");

		item = oLayout.AddItem(L"ShutterAngle", L"");
		item.PutAttribute(siUIThumbWheel, true); 
		item.PutAttribute(siUINoSlider, true);
		item.PutAttribute(siUIDecimals, L"2");

		CValueArray comboItems4(6);
		comboItems4[0] = L"Open on Frame"; comboItems4[1] = L"OpenOnFrame";
		comboItems4[2] = L"Close on Frame"; comboItems4[3] = L"CloseOnFrame";
		comboItems4[4] = L"Center on Frame"; comboItems4[5] = L"CenterOnFrame";
		item = oLayout.AddEnumControl(L"ShutterTiming", comboItems4, L"", siControlCombo);
		item = oLayout.AddItem(L"FrontplaneGeom", L"");
		item = oLayout.AddItem(L"BackplaneGeom", L"");
		item.PutLabelPercentage(90);

	oLayout.AddTab(L"Reyes");

		item = oLayout.AddItem(L"ShadingRate", L"");
		item.PutAttribute(siUIThumbWheel, true); 
		item.PutAttribute(siUINoSlider, true);
		item.PutAttribute(siUIDecimals, L"2");

		oLayout.AddRow();
			item = oLayout.AddItem(L"PixelSamplesX", L"");
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutWidthPercentage(83);
			item = oLayout.AddItem(L"PixelSamplesY", L"");
			item.PutAttribute(siUINoLabel, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);
		oLayout.EndRow();

		item = oLayout.AddEnumControl(L"PixelFilter", comboItems5, L"", siControlCombo) ;
		item.PutLabelPercentage(90);

		oLayout.AddRow();
			item = oLayout.AddItem(L"FilterSizeX", L"");
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);
			item.PutWidthPercentage(83);
			item = oLayout.AddItem(L"FilterSizeY", L"");
			item.PutAttribute(siUINoLabel, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);
		oLayout.EndRow();

		item = oLayout.AddItem(L"Eyesplits", L"");
		item.PutAttribute(siUIThumbWheel, true);
		item.PutAttribute(siUINoSlider, true);

		oLayout.AddRow();
			item = oLayout.AddItem(L"BucketSizeX", L"");
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);
			item.PutWidthPercentage(83);
			item = oLayout.AddItem(L"BucketSizeY", L"");
			item.PutAttribute(siUINoLabel, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);
		oLayout.EndRow();

		item = oLayout.AddItem(L"GridSize", L"");
		item.PutAttribute(siUIThumbWheel, true);
		item.PutAttribute(siUINoSlider, true);

		item = oLayout.AddItem(L"TextureMemory", L"");
		item.PutAttribute(siUIThumbWheel, true);
		item.PutAttribute(siUINoSlider, true);

		item = oLayout.AddItem(L"BinaryDicing", L"");

		item = oLayout.AddItem(L"ShadingInterpolation", L"");

		item = oLayout.AddItem(L"ShadowBias", L"");
		item.PutAttribute(siUIThumbWheel, true);
		item.PutAttribute(siUINoSlider, true);

		item = oLayout.AddItem(L"MotionFactor", L"");
		item.PutAttribute(siUIThumbWheel, true);
		item.PutAttribute(siUINoSlider, true);
					
		CValueArray comboItems28(4);
		comboItems28[0] = L"Planar Projection"; comboItems28[1] = L"planarprojection";
		comboItems28[2] = L"Spherical Projection"; comboItems28[3] = L"sphericalprojection";
		item = oLayout.AddEnumControl(L"DicingStrategy", comboItems28, L"", siControlCombo) ;
		item.PutLabelPercentage(90);
				
		CValueArray comboItems29(4);
		comboItems29[0] = L"World Camera"; comboItems29[1] = L"worldcamera";
		comboItems29[2] = L"Frame Camera"; comboItems29[3] = L"framecamera";
		item = oLayout.AddEnumControl(L"DicingCamera", comboItems29, L"", siControlCombo) ;
		item.PutLabelPercentage(90);

		item = oLayout.AddItem(L"RasterOriented", L"");

		item = oLayout.AddColor(L"OpacityThresholdR", L"Opacity Threshold", false);

		item = oLayout.AddColor(L"OpacityCullingR", L"Opacity Culling", false);;

	oLayout.AddTab(L"Raytrace");

		oLayout.AddGroup(L"General");

			item = oLayout.AddItem(L"EnableRayTracing", L"");

			item = oLayout.AddItem(L"MaxRayDepth", L"");
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);

			item = oLayout.AddItem(L"RayTraceBias", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"2");

			item = oLayout.AddItem(L"RayTraceBreadthFactor", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"2");
				
			item = oLayout.AddItem(L"RayTraceDepthFactor", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"2");

			item = oLayout.AddItem(L"RayTraceMaxSpecularDepth", L"");
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);

			item = oLayout.AddItem(L"RayTraceMaxDiffuseDepth", L"");
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);

			item = oLayout.AddItem(L"RayTraceDisplacements", L"");

			item = oLayout.AddItem(L"RayTraceSampleMotion", L"");

		oLayout.EndGroup();

		oLayout.AddGroup(L"Irradiance Cache");

			item = oLayout.AddItem(L"RayTraceMaxError", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"2");
				
			item = oLayout.AddItem(L"RayTraceMaxPixDist", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"2");

			item = oLayout.AddItem(L"IrradianceCache", L"Irradiance File", siControlFilePath);
			item.PutAttribute(siUIOpenFile, true);

			CValueArray comboItems16(6);
			comboItems16[0] = L"ignore"; comboItems16[1] = L"ignore";
			comboItems16[2] = L"read"; comboItems16[3] = L"read";
			comboItems16[4] = L"write"; comboItems16[5] = L"write";
			item = oLayout.AddEnumControl(L"IrradianceCacheMode", comboItems16, L"", siControlCombo);
			item.PutLabelPercentage(90);

		oLayout.EndGroup();

		oLayout.AddGroup(L"Photon Mapping");

			CValueArray comboItems17(12);
			comboItems17[0] = L""; comboItems17[1] = L"";
			comboItems17[2] = L"glass"; comboItems17[3] = L"glass";
			comboItems17[4] = L"water"; comboItems17[5] = L"water";
			comboItems17[6] = L"chrome"; comboItems17[7] = L"chrome";
			comboItems17[8] = L"matte"; comboItems17[9] = L"matte";
			comboItems17[10] = L"transparent"; comboItems17[11] = L"transparent";
			item = oLayout.AddEnumControl(L"PhotonShadingModel", comboItems17, L"", siControlCombo);
			item.PutLabelPercentage(90);

			item = oLayout.AddItem(L"PhotonCausticMap", L"Caustic Map", siControlFilePath);
			item.PutAttribute(siUIOpenFile, true);

			item = oLayout.AddItem(L"PhotonGlobalMap", L"Global Map", siControlFilePath);
			item.PutAttribute(siUIOpenFile, true);

			item = oLayout.AddItem(L"PhotonEstimator", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"0");

		oLayout.EndGroup();

	oLayout.AddTab(L"Accel");

		item = oLayout.AddItem(L"RenderSelectedObjects");
		
		item = oLayout.AddItem(L"RenderSelectedLights");
			
		item = oLayout.AddItem(L"ApproximatedMesh");

		item = oLayout.AddItem(L"PolygonClusters");

		oLayout.AddGroup(L"Instance Geometry");

			item = oLayout.AddItem(L"InstanceGeometry");

			item = oLayout.AddItem(L"ReuseInstanceGeometry");

		oLayout.EndGroup();
				
		oLayout.AddGroup(L"Crop Window");
		
			oLayout.AddRow();
				item = oLayout.AddItem(L"CropWindowX1");
				item.PutAttribute(siUINoSlider, true);
				item.PutAttribute(siUIThumbWheel, true);
				item = oLayout.AddItem(L"CropWindowX2");
				item.PutAttribute(siUINoSlider, true);
				item.PutAttribute(siUIThumbWheel, true);
				item.PutAttribute(siUINoLabel, true);
			oLayout.EndRow();

			oLayout.AddRow();
				item = oLayout.AddItem(L"CropWindowY1");
				item.PutAttribute(siUINoSlider, true);
				item.PutAttribute(siUIThumbWheel, true);
				item = oLayout.AddItem(L"CropWindowY2");
				item.PutAttribute(siUINoSlider, true);
				item.PutAttribute(siUIThumbWheel, true);
				item.PutAttribute(siUINoLabel, true);
			oLayout.EndRow();

		oLayout.EndGroup();

	oLayout.AddTab(L"Pipe");

		oLayout.AddGroup(L"Job Script");

			item = oLayout.AddItem(L"UseJobScript", L"");

			CValueArray comboItems10(4);
			comboItems10[0] = L"none"; comboItems10[1] = L"none";
			comboItems10[2] = L"Alfred"; comboItems10[3] = L"Alfred";
			item = oLayout.AddEnumControl(L"JobScriptType", comboItems10, L"", siControlCombo) ;
			item.PutLabelPercentage(90);

			oLayout.AddRow();
				item = oLayout.AddItem(L"MinProcs", L"");
				item.PutAttribute(siUIThumbWheel, true);
				item.PutAttribute(siUINoSlider, true);

				item = oLayout.AddItem(L"MaxProcs", L"");
				item.PutAttribute(siUIThumbWheel, true);
				item.PutAttribute(siUINoSlider, true);
				item.PutAttribute(siUINoLabel, true);
			oLayout.EndRow();

			item = oLayout.AddItem(L"AlfredTags", L"Alfred Tags", siControlEdit);

			item = oLayout.AddItem(L"AlfredServices", L"Alfred Services", siControlEdit);

			item = oLayout.AddItem(L"Imager", L"Imager", siControlEdit);
			
			item = oLayout.AddItem(L"RemoteCmd", L"");

			item = oLayout.AddItem(L"PreJobCommand", L"Pre Job Command", siControlEdit);
			item = oLayout.AddItem(L"PreFrameCommand", L"Pre Frame Command", siControlEdit);
			item = oLayout.AddItem(L"PostFrameCommand", L"Post Frame Command", siControlEdit);
			item = oLayout.AddItem(L"PostJobCommand", L"Post Job Command", siControlEdit);
			
			item = oLayout.AddItem(L"CleanJobScript", L"");

		oLayout.EndGroup();


		oLayout.AddGroup(L"Render Job");

			item = oLayout.AddItem(L"PreJobScript", L"Pre Job Script", siControlEdit);
			item = oLayout.AddItem(L"PreFrameScript", L"Pre Frame Script", siControlEdit);
			item = oLayout.AddItem(L"PostFrameScript", L"Post Frame Script", siControlEdit);
			item = oLayout.AddItem(L"PostJobScript", L"Post Job Script", siControlEdit);
						
			CValueArray comboItems27(4);
			comboItems27[0] = L"VBScript"; comboItems27[1] = L"VBScript";
			comboItems27[2] = L"JScript"; comboItems27[3] = L"JScript";
			item = oLayout.AddEnumControl(L"ScriptLanguage", comboItems27, L"", siControlCombo) ;
			item.PutLabelPercentage(90);

			CValueArray comboItems15(6);
			comboItems15[0] = L"PRMan"; comboItems15[1] = L"PRMan";
			comboItems15[2] = L"3Delight"; comboItems15[3] = L"3Delight";
			comboItems15[4] = L"Air"; comboItems15[5] = L"Air";
			//comboItems15[6] = L"RenderDotC"; comboItems15[7] = L"RenderDotC";
			item = oLayout.AddEnumControl(L"Renderer", comboItems15, L"", siControlCombo) ;
			item.PutLabelPercentage(90);

			item = oLayout.AddItem(L"LaunchRender", L"");
			
			//item = oLayout.AddItem(L"BypassRib", L"");

			CValueArray comboItems11(10);
			comboItems11[0] = L"none"; comboItems11[1] = L"none";
			comboItems11[2] = L"prman"; comboItems11[3] = L"prman";
			comboItems11[4] = L"render"; comboItems11[5] = L"render";
			comboItems11[6] = L"netrender"; comboItems11[7] = L"netrender";
			comboItems11[8] = L"custom"; comboItems11[9] = L"custom";
			item = oLayout.AddEnumControl(L"RenderCommand", comboItems11, L"", siControlCombo) ;
			item.PutLabelPercentage(90);

			item = oLayout.AddItem(L"CustomCommand", L"Custom Command", siControlEdit);

			item = oLayout.AddItem(L"NumProcs", L"");
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);
			
			item = oLayout.AddItem(L"ShowProgress");
			
			item = oLayout.AddItem(L"RenderStatistics");
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);
			
			item = oLayout.AddItem(L"StatisticsFileName", L"Statistics File Name", siControlEdit);
			item = oLayout.AddItem(L"StatisticsXMLFileName", L"Statistics XML File Name", siControlEdit);
			
			item = oLayout.AddItem(L"JobSummary");

		oLayout.EndGroup();

		oLayout.AddGroup(L"RIB Output");
			CValueArray comboItems6(4);
			comboItems6[0] = L"ascii"; comboItems6[1] = L"ascii";
			comboItems6[2] = L"binary"; comboItems6[3] = L"binary";
			item = oLayout.AddEnumControl(L"RibOutput", comboItems6, L"", siControlCombo) ;
			item.PutLabelPercentage(90);

			item = oLayout.AddItem(L"GzipRib", L"");

		oLayout.EndGroup();

		oLayout.AddGroup(L"Frame Range");
			item = oLayout.AddItem(L"RenderFrames", L"");

			CValueArray comboItems7(4);
			comboItems7[0] = L"immediate"; comboItems7[1] = L"immediate";
			comboItems7[2] = L"deferred"; comboItems7[3] = L"deferred";
			item = oLayout.AddEnumControl(L"RenderMode", comboItems7, L"", siControlCombo);
			item.PutLabelPercentage(90);

			item = oLayout.AddItem(L"StartFrame", L"");
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);

			item = oLayout.AddItem(L"EndFrame", L"");
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);

			item = oLayout.AddItem(L"StepSize", L"");
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);
			
			item = oLayout.AddItem(L"SkipRenderedFrames", L"");

		oLayout.EndGroup();

		oLayout.AddGroup(L"Options");
		
			oLayout.AddGroup(L"Light Handles");
				item = oLayout.AddItem(L"NamedHandles");
				item = oLayout.AddItem(L"HandlePrefix", L"Handle Prefix", siControlEdit);
			oLayout.EndGroup();

		oLayout.EndGroup();

		oLayout.AddGroup(L"Cleanup");

			CValueArray bitmaskItems(6) ;
			bitmaskItems[0] = L"RIBs"; bitmaskItems[1] = 1l ;
			bitmaskItems[2] = L"Textures"; bitmaskItems[3] = 4l ;
			bitmaskItems[4] = L"Maps"; bitmaskItems[5] = 16l ;
			item = oLayout.AddEnumControl(L"Cleanup", bitmaskItems, L"", siControlCheck);
			item.PutLabelPercentage(90);
			item.PutAttribute(siUINoLabel, true);

		oLayout.EndGroup();

	oLayout.AddTab(L"FX");
	
		oLayout.AddGroup(L"Render Tree Translation");

			item = oLayout.AddItem(L"RenderTreeSupport");
			
		oLayout.EndGroup();
			
		oLayout.AddGroup(L"Global Illumination");
		
			item = oLayout.AddItem(L"EnableAmbientOcclusion", L"");

			item = oLayout.AddItem(L"OcclusionSamples", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"0");

			item = oLayout.AddItem(L"OcclusionMaxdist", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"2");

			item = oLayout.AddItem(L"EnableIndirectIllumination", L"");
						
			item = oLayout.AddItem(L"IndirectSamples", L"");
			item.PutAttribute(siUIThumbWheel, true); 
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIDecimals, L"0");

		oLayout.EndGroup();
	
	oLayout.AddTab(L"Paths");

		oLayout.AddGroup(L"Output Directories");

			item = oLayout.AddItem(L"RibPath", L"RIB Files", siControlFolder);
			item = oLayout.AddItem(L"PixPath", L"Rendered Pictures", siControlFolder);
			item = oLayout.AddItem(L"TexPath", L"Texture Maps", siControlFolder);
			item = oLayout.AddItem(L"ShdPath", L"Shadow Maps", siControlFolder);
			item = oLayout.AddItem(L"ShaderPath", L"Shaders", siControlFolder);
			item = oLayout.AddItem(L"JobPath", L"Job Scripts", siControlFolder);

		oLayout.EndGroup();

		oLayout.AddGroup(L"Resource Directories");

			item = oLayout.AddItem(L"SloPath", L"Shaders", siControlFolder);
			item = oLayout.AddItem(L"ImgPath", L"Source Images", siControlFolder);
			item = oLayout.AddItem(L"DsoPath", L"Plug-ins", siControlFolder);

		oLayout.EndGroup();

		oLayout.AddGroup(L"Directory Maps");

			item = oLayout.AddItem(L"DirMap", L"", siControlGrid);
			item.PutAttribute(siUIGridColumnWidths, L"25:50:150:150");
			item.PutAttribute(siUICY, 100l);
			item.PutAttribute(siUINoLabel, true);
			item.PutAttribute(siUIGridLockColumnHeader, true);

		oLayout.EndGroup();
		
	oLayout.AddTab(L"Textures");

		CValueArray comboItems19(12);
		comboItems19[0] = L"up"; comboItems19[1] = L"up";
		comboItems19[2] = L"down"; comboItems19[3] = L"down";
		comboItems19[4] = L"round"; comboItems19[5] = L"round";
		comboItems19[6] = L"up-"; comboItems19[7] = L"up-";
		comboItems19[8] = L"down-"; comboItems19[9] = L"down-";
		comboItems19[10] = L"round-"; comboItems19[11] = L"round-";
		item = oLayout.AddEnumControl(L"TexResize", comboItems19, L"", siControlCombo);
		item.PutLabelPercentage(90);

		CValueArray comboItems20(6);
		comboItems20[0] = L"black"; comboItems20[1] = L"black";
		comboItems20[2] = L"clamp"; comboItems20[3] = L"clamp";
		comboItems20[4] = L"periodic"; comboItems20[5] = L"periodic";
		item = oLayout.AddEnumControl(L"TexSMode", comboItems20, L"", siControlCombo);
		item.PutLabelPercentage(90);

		CValueArray comboItems21(6);
		comboItems21[0] = L"black"; comboItems21[1] = L"black";
		comboItems21[2] = L"clamp"; comboItems21[3] = L"clamp";
		comboItems21[4] = L"periodic"; comboItems21[5] = L"periodic";
		item = oLayout.AddEnumControl(L"TexTMode", comboItems21, L"", siControlCombo);
		item.PutLabelPercentage(90);
			
		CValueArray comboItems22(6);
		comboItems22[0] = L"single"; comboItems22[1] = L"single";
		comboItems22[2] = L"diagonal"; comboItems22[3] = L"diagonal";
		comboItems22[4] = L"all"; comboItems22[5] = L"all";
		item = oLayout.AddEnumControl(L"TexPattern", comboItems22, L"", siControlCombo) ;
		item.PutLabelPercentage(90);
			
		CValueArray comboItems23(4);
		comboItems23[0] = L"pixar"; comboItems23[1] = L"pixar";
		comboItems23[2] = L"tiff"; comboItems23[3] = L"tiff";
		item = oLayout.AddEnumControl(L"TexFormat", comboItems23, L"", siControlCombo);
		item.PutLabelPercentage(90);
			
		CValueArray comboItems25(6);
		comboItems25[0] = L""; comboItems25[1] = L"";
		comboItems25[2] = L"short"; comboItems25[3] = L"short";
		comboItems25[4] = L"float"; comboItems25[5] = L"float";
		item = oLayout.AddEnumControl(L"TexBytes", comboItems25, L"", siControlCombo);
		item.PutLabelPercentage(90);

		CValueArray comboItems24(12);
		comboItems24[0] = L"box"; comboItems24[1] = L"box";
		comboItems24[2] = L"triangle"; comboItems24[3] = L"triangle";
		comboItems24[4] = L"gaussian"; comboItems24[5] = L"gaussian";
		comboItems24[6] = L"catmull-rom"; comboItems24[7] = L"catmull-rom";
		comboItems24[8] = L"sinc"; comboItems24[9] = L"sinc";
		comboItems24[10] = L"bessel"; comboItems24[11] = L"bessel";
		item = oLayout.AddEnumControl(L"TexFilter", comboItems24, L"", siControlCombo);
		item.PutLabelPercentage(90);

		oLayout.AddRow();
			item = oLayout.AddItem(L"TexFilterWidthS", L"");
			item.PutAttribute(siUINoSlider, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutWidthPercentage(83);
			item = oLayout.AddItem(L"TexFilterWidthT", L"");
			item.PutAttribute(siUINoLabel, true);
			item.PutAttribute(siUIThumbWheel, true);
			item.PutAttribute(siUINoSlider, true);
		oLayout.EndRow();	

	return CStatus::OK;	
}

#ifdef unix
extern "C"
#endif
CStatus XSIManOptions_PPGEvent(const CRef& io_Ctx)
{
	Application app;

	PPGEventContext ctx(io_Ctx);

	PPGEventContext::PPGEvent eventID = ctx.GetEventID();

	CustomProperty prop = ctx.GetSource();	

	if (eventID == PPGEventContext::siOnInit) {
		Selection sel(app.GetSelection());
	}
	else if (eventID == PPGEventContext::siButtonClicked) {
		CValue buttonPressed = ctx.GetAttribute(L"Button");	

		if (buttonPressed.GetAsText() == L"AddAOV") {
			Parameter aovname = prop.GetParameter(L"AOVName");
			Parameter aovtype = prop.GetParameter(L"AOVType");
			Parameter aovmode = prop.GetParameter(L"AOVMode");
			Parameter aovfilter = prop.GetParameter(L"AOVPixelFilter");
			Parameter aovsizex = prop.GetParameter(L"AOVFilterSizeX");
			Parameter aovsizey = prop.GetParameter(L"AOVFilterSizeY");
			Parameter aovquantzero = prop.GetParameter(L"AOVQuantizeZero");
			Parameter aovquantone = prop.GetParameter(L"AOVQuantizeOne");
			Parameter aovquantmin = prop.GetParameter(L"AOVQuantizeMin");
			Parameter aovquantmax = prop.GetParameter(L"AOVQuantizeMax");
			Parameter aovquantdither = prop.GetParameter(L"AOVQuantizeDither");

			if (aovname.GetValue().GetAsText() != L"") {
				// make sure aovname has no spaces!
				wchar_t aovName[100];
				swscanf(aovname.GetValue().GetAsText().GetWideString(), L"%s", aovName);

				// get contents of AOVList
				Parameter aovList = prop.GetParameter(L"AOVList");
				GridData gridData = aovList.GetValue();
				GridWidget gridWidget = gridData.GetGridWidget();
				long rowCount = gridData.GetRowCount();

				long rowSelection = -1;
				for (long i = 0; i < rowCount; i++) {
					if (gridWidget.IsRowSelected(i)) {
						rowSelection = i;
						break;
					}
				}

				// if no row is selected find first empty row
				if (rowSelection == -1) {
					for (long i = 0; i < rowCount; i++) {
						CString aovname = gridData.GetCell(0, i);
						if (aovname == L"") {
							rowSelection = i;
							break;
						}
					}
				}

				gridData.PutCell(0, rowSelection, CString(aovName));
				gridData.PutCell(1, rowSelection, aovtype.GetValue().GetAsText());
				gridData.PutCell(2, rowSelection, aovmode.GetValue().GetAsText());
				gridData.PutCell(3, rowSelection, aovfilter.GetValue().GetAsText());
				gridData.PutCell(4, rowSelection, aovsizex.GetValue().GetAsText());
				gridData.PutCell(5, rowSelection, aovsizey.GetValue().GetAsText());
				gridData.PutCell(6, rowSelection, aovquantzero.GetValue().GetAsText());
				gridData.PutCell(7, rowSelection, aovquantone.GetValue().GetAsText());
				gridData.PutCell(8, rowSelection, aovquantmin.GetValue().GetAsText());
				gridData.PutCell(9, rowSelection, aovquantmax.GetValue().GetAsText());
				gridData.PutCell(10, rowSelection, aovquantdither.GetValue().GetAsText());
	
				//Redraw the PPG 
				ctx.PutAttribute(L"Refresh",true);
			}
		}
		else if (buttonPressed.GetAsText() == L"DeleteAOV") {
			// get contents of AOVList
			Parameter aovList = prop.GetParameter(L"AOVList");
			GridData gridData = aovList.GetValue();
			GridWidget gridWidget = gridData.GetGridWidget();
			long rowCount = gridData.GetRowCount();

			long rowSelection = -1;
			for (long i = 0; i < rowCount; i++) {
				if (gridWidget.IsRowSelected(i)) {
					rowSelection = i;
					break;
				}
			}

			if (rowSelection != -1) {
				gridData.PutCell(0, rowSelection, L"");
				gridData.PutCell(1, rowSelection, L"");
				gridData.PutCell(2, rowSelection, L"");
				gridData.PutCell(3, rowSelection, L"");
				gridData.PutCell(4, rowSelection, L"");
				gridData.PutCell(5, rowSelection, L"");
				gridData.PutCell(6, rowSelection, L"");
				gridData.PutCell(7, rowSelection, L"");
				gridData.PutCell(8, rowSelection, L"");
				gridData.PutCell(9, rowSelection, L"");
			}

			//Redraw the PPG 
			ctx.PutAttribute(L"Refresh",true);
		}
	}
	else if (eventID == PPGEventContext::siParameterChange) {
		Parameter changed = ctx.GetSource();	
		CustomProperty prop = changed.GetParent();	
		CString paramName = changed.GetScriptName(); 

		if (paramName == L"Quantization") {
			CValue quantValue = changed.GetValue();
			if (quantValue.GetAsText() == L"8bit") {
				prop.PutParameterValue(L"QuantizeOne", 255.);
				prop.PutParameterValue(L"QuantizeMin", 0.);
				prop.PutParameterValue(L"QuantizeMax", 255.);
			}
			else if (quantValue.GetAsText() == L"16bit") {
				prop.PutParameterValue(L"QuantizeOne", 65535.);
				prop.PutParameterValue(L"QuantizeMin", 0.);
				prop.PutParameterValue(L"QuantizeMax", 65535.);
			}
			else if (quantValue.GetAsText() == L"float") {
				prop.PutParameterValue(L"QuantizeOne", 0.);
				prop.PutParameterValue(L"QuantizeMin", 0.);
				prop.PutParameterValue(L"QuantizeMax", 0.);
			}
		}

		if (paramName == L"AOVQuantization") {
			CValue quantValue = changed.GetValue();
			if (quantValue.GetAsText() == L"8bit") {
				prop.PutParameterValue(L"AOVQuantizeZero", 0.);
				prop.PutParameterValue(L"AOVQuantizeOne", 255.);
				prop.PutParameterValue(L"AOVQuantizeMin", 0.);
				prop.PutParameterValue(L"AOVQuantizeMax", 255.);
			}
			else if (quantValue.GetAsText() == L"16bit") {
				prop.PutParameterValue(L"AOVQuantizeZero", 0.);
				prop.PutParameterValue(L"AOVQuantizeOne", 65535.);
				prop.PutParameterValue(L"AOVQuantizeMin", 0.);
				prop.PutParameterValue(L"AOVQuantizeMax", 65535.);
			}
			else if (quantValue.GetAsText() == L"float") {
				prop.PutParameterValue(L"AOVQuantizeZero", 0.);
				prop.PutParameterValue(L"AOVQuantizeOne", 0.);
				prop.PutParameterValue(L"AOVQuantizeMin", 0.);
				prop.PutParameterValue(L"AOVQuantizeMax", 0.);
			}
		}

		if (paramName == L"AOVModeList") {
			CValue modeValue = changed.GetValue();
			prop.PutParameterValue(L"AOVMode", modeValue);
		}

		if (paramName == L"RenderCommand") {
			CValue paramValue = changed.GetValue();
			if (paramValue.GetAsText() == L"netrender") {
				prop.PutParameterValue(L"AlfredServices", CValue(L"pixarNRM"));
			}
			else  {
				prop.PutParameterValue(L"AlfredServices", CValue(L"pixarRender"));
			}
		}

		if (paramName == L"Renderer") {
			CValue paramValue = changed.GetValue();
			if (paramValue.GetAsText() == L"PRMan") {
				prop.PutParameterValue(L"RenderCommand", CValue(L"prman"));
				prop.PutParameterValue(L"Imager", CValue(L"sho"));
			}
			else if (paramValue.GetAsText() == L"3Delight") {
				prop.PutParameterValue(L"RenderCommand", CValue(L"custom"));
				prop.PutParameterValue(L"CustomCommand", CValue(L"renderdl"));
				prop.PutParameterValue(L"Imager", CValue(L"i-display"));
			}
			else if (paramValue.GetAsText() == L"Air") {
				prop.PutParameterValue(L"RenderCommand", CValue(L"custom"));
				prop.PutParameterValue(L"CustomCommand", CValue(L"air"));
				prop.PutParameterValue(L"Imager", CValue(L"airshow"));
			}
			//else if (paramValue.GetAsText() == L"RenderDotC") {
				//prop.PutParameterValue(L"RenderCommand", CValue(L"custom"));
				//prop.PutParameterValue(L"CustomCommand", CValue(L"renderdc"));
				//prop.PutParameterValue(L"Imager", CValue(L"sho"));
			//}
		}
	}


	return CStatus::OK;	
}

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

CString GetUserSelectedObject()
{
	Application app;
	Model root(app.GetActiveSceneRoot());
	CStringArray emptyArray;
	CRefArray cRefArray = root.FindChildren(L"", L"", emptyArray, true);

	CStringArray nameArray(cRefArray.GetCount());
	for (long i=0; i < cRefArray.GetCount(); i++) {
		nameArray[i] = SIObject(cRefArray[i]).GetName();
	}
	//todo qsort the nameArray

	// Using the COMAPIHandler for creating a "XSIDial.XSIDialog"
	CComAPIHandler xsidialog;
	xsidialog.CreateInstance(L"XSIDial.XSIDialog");
	CValue index;
	CValueArray args(cRefArray.GetCount());
	for (long y=0; y < cRefArray.GetCount(); y++)
		args[y]=nameArray[y];

	xsidialog.Call(L"Combo", index, L"Select Item", args);

	long ind = (long)index;
	return args[ind];
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


CValue InspectObj(const CString& in_inputobjs, const CString& in_keywords, const CString& in_title, const CValue& /*number*/ in_mode, bool in_throw)
{
	Application app;
	CValueArray args(5);
	CValue retval;
	long i(0);

	args[i++]= in_inputobjs;
	args[i++]= in_keywords;
	args[i++]= in_title;
	args[i++]= in_mode;
	args[i++]= in_throw;

	CStatus st = app.ExecuteCommand(L"InspectObj", args, retval);

	return retval;
}

void DeleteObj(const CValue& in_inputobj)
{
	Application app;
	CValueArray args(1);
	CValue retval;
	long i(0);

	args[i++]= in_inputobj;

	CStatus st = app.ExecuteCommand(L"DeleteObj", args, retval);

	return;
}

static void nlmacaddr(unsigned char addr[/*6*/])
{
#ifdef WIN32
	getmacaddr_win(addr);
#endif
#ifdef __linux__
	getmacaddr_lin(addr);
#endif
	if (nltrace > 0) {
		printf("mac addr = ");
		for( int i = 0; i < 6; i++ ) printf("%02x", addr[i]);
		printf("\n");
	}
#ifdef TESTING_ONLY
	// force my other computers address, to test
	addr[0] = 0x00;
	addr[1] = 0x0a;
	addr[2] = 0x95;
	addr[3] = 0x66;
	addr[4] = 0xaa;
	addr[5] = 0x7e;
	if (nltrace > 0) {
		for( int i = 0; i < 6; i++ ) printf("%02x", addr[i]);
		printf("\n");
	}
#endif
} 

static void nlencrypt(const char *str, const unsigned char macaddr[],
		      unsigned char encrypted[])
{
	md5_state_t state;
	md5_init(&state);

	int len = (int)strlen(str);
	for( int i = 0; i < len; i++ ) {
		md5_append(&state, (md5_byte_t *)&str[i], sizeof(char));    
	}

	for( int i = 0; i < 6; i++ ) {
		md5_append(&state, (md5_byte_t *)&macaddr[i], sizeof(macaddr[i]));
	}
	md5_finish(&state, (md5_byte_t *)encrypted);
}

static bool nlcheck(const char *str, unsigned int date, const char *hash)
{
	Application app;
	unsigned char addr[6];
	unsigned char encrypted[16];
	nlmacaddr(addr);

	nlencrypt(str, addr, encrypted);

	//if (nltrace) cout << "hash =       " <<  hash << std::endl;
	//if (nltrace) cout << "key =        ";

	bool match = true;
	for( int i = 0; i < 16; i++ ) {
		unsigned int e = encrypted[i];

		assert((e >= 0) && (e < 256));
		unsigned int e1 = e & 0xf;
		unsigned int e2 = (e>>4) & 0xf;
		assert((e1 >= 0) && (e1 < 16));
		assert((e2 >= 0) && (e2 < 16));

		char c2 = hexmap[e2];
		char c1 = hexmap[e1];

		if (c2 != hash[2*i]) match = false;
		if (c1 != hash[2*i+1]) match = false;

		if (nltrace) {
			//if (nltrace > 0) cout << c2 << c1 ;
		}
		else {
			if (!match) break;
		}

	}
	//if (nltrace > 0) cout << std::endl;
	if (!match) {
		//cerr << "bad license key\n";	// TODO: hide this string
		return 0;
	}


	// if we got here license key is ok, now check the date
	if (date != 0) {
		int t = zlib_timeops::expire(date);

		if (t < 0) {
			app.LogMessage(L"license has expired");	
			return 0;
		}

		//if (nltrace)
			//cout << "expiration time = " << zlib_timeops::ctime(date) << std::endl;

		if (t < (60*60*24*14)) 
			app.LogMessage(L"Warning: license will expire on " + CValue(zlib_timeops::ctime(date)).GetAsText());
	} 


  return 1;
} 

bool nlcheck(const char *licfile, const char *progname)
{
	Application app;
	wchar_t* licFile;
	wchar_t* progName;
	XSIA2W(&licFile, licfile);
	XSIA2W(&progName, progname);

	std::ifstream fin(licfile);
	if (!fin) {
		//app.LogMessage(L"Could not open " + CString(licFile));
		return false;
	}
  
	std::string token;
	while( fin >>  token ) {
		if (token == "key") {
			fin >> token;
			if (token == progname) {
				std::string datestr;
				std::string hashstr;
				fin >> datestr;
				fin >> hashstr;
				//if (nltrace > 1)
					//std::cout << "date="<<datestr<<"  hash="<<hashstr<<std::endl;
				unsigned int date = atoi(datestr.c_str());

				std::string str = std::string(MAGIC) + progname + datestr;
				fin.close();

				return nlcheck(str.c_str(), date, (const char *)hashstr.c_str());
			}
		}
		if (token == "/*") {
		do {
			fin >> token;
		} while( token != "*/" );
		}
		//if (nltrace > 1) std::cout << token << " ";
	}

	fin.close();
	//if (nltrace > 1) std::cout << std::endl;

	app.LogMessage(L"Did not find license for " + CString(progName));

	return false;	
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIAirOptionsAttach(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManAirOptions");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding Air options property to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManAirOptions");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIAirOptionsDetach(XSI::CRef& in_ref)
{
	Application app;
	CStatus st(CStatus::OK);
	CValue retval;

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManAirOptions");
		if (propRef.IsValid()) {
			DeleteObj(propRef);
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManAirOptions_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"truedisplacement", CValue::siBool, siAnimatable, L"True Displacement", L"", false, param);
	prop.AddParameter(L"bevelcurves", CValue::siBool, siAnimatable, L"Bevel Curves", L"", false, param);
	prop.AddParameter(L"flatness", CValue::siDouble, siAnimatable, L"Flatness", L"flatness", 0.5, 0.0, 10., 0., 10., param);
	
	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManAirOptions_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	item = oLayout.AddItem(L"truedisplacement", L"");

	item = oLayout.AddItem(L"bevelcurves", L"");

	item = oLayout.AddItem(L"flatness", L"");
	
	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManAttributes.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIMiscOptionsAttach(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManMiscOptions");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding miscellaneous options property to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManMiscOptions");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIMiscOptionsDetach(XSI::CRef& in_ref)
{
	Application app;
	CStatus st(CStatus::OK);
	CValue retval;

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManMiscOptions");
		if (propRef.IsValid()) {
			DeleteObj(propRef);
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManMiscOptions_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"surfacecolor", CValue::siBool, siPersistable, L"", L"", false, param);
	prop.AddParameter(L"cred", CValue::siFloat, siPersistable, L"", L"", 1., 0.0, 1.0, 0.0, 1.0, param);
	prop.AddParameter(L"cgreen", CValue::siFloat, siPersistable, L"", L"", 1., 0.0, 1.0, 0.0, 1.0, param);
	prop.AddParameter(L"cblue", CValue::siFloat, siPersistable, L"", L"", 1., 0.0, 1.0, 0.0, 1.0, param);
	prop.AddParameter(L"surfaceopacity", CValue::siBool, siPersistable, L"", L"", false, param);
	prop.AddParameter(L"ored", CValue::siFloat, siPersistable, L"", L"", 1., 0.0, 1.0, 0.0, 1.0, param);
	prop.AddParameter(L"ogreen", CValue::siFloat, siPersistable, L"", L"", 1., 0.0, 1.0, 0.0, 1.0, param);
	prop.AddParameter(L"oblue", CValue::siFloat, siPersistable, L"", L"", 1., 0.0, 1.0, 0.0, 1.0, param);
	prop.AddParameter(L"displacementbound_flag", CValue::siBool, siPersistable, L"", L"", false, param);
	prop.AddParameter(L"displacementbound", CValue::siFloat, siPersistable, L"Displacement Bound", L"", 0., 0.0, 100.0, dft, dft, param);
	prop.AddParameter(L"shadingrate_flag", CValue::siBool, siPersistable, L"", L"", false, param);
	prop.AddParameter(L"shadingrate", CValue::siFloat, caps, L"Shading Rate", L"", 5.0, 0.0, 100.0, dft, dft, param);
	prop.AddParameter(L"eyesplits_flag", CValue::siBool, siPersistable, L"", L"", false, param);
	prop.AddParameter(L"eyesplits", CValue::siInt4, caps, L"Eyesplits", L"", 10., 0., 10., dft, dft, param);
	prop.AddParameter(L"nonbackfacecull", CValue::siBool, siPersistable, L"Non-backface Culling", L"", false, param);
	prop.AddParameter(L"nonhiddencull", CValue::siBool, siPersistable, L"Non-hidden Culling", L"", false, param);
	prop.AddParameter(L"nonrasterorient", CValue::siBool, siPersistable, L"Non-raster Oriented Dicing", L"", false, param);
	
	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManMiscOptions_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();
	
	oLayout.AddGroup(L"Cs");
		oLayout.AddRow();
			item = oLayout.AddItem(L"surfacecolor", L"");
			item.PutWidthPercentage(10);
			item.PutAttribute(siUINoLabel, true);
			item = oLayout.AddColor(L"cred", L"Cs", false);
			item.PutAttribute(siUINoLabel, true);
		oLayout.EndRow();
	oLayout.EndGroup();

	oLayout.AddGroup(L"Os");
		oLayout.AddRow();
			item = oLayout.AddItem(L"surfaceopacity", L"");
			item.PutWidthPercentage(10);
			item.PutAttribute(siUINoLabel, true);
			item = oLayout.AddColor(L"ored", L"Os", false);
			item.PutAttribute(siUINoLabel, true);
		oLayout.EndRow();
	oLayout.EndGroup();

	oLayout.AddRow();
		item = oLayout.AddItem(L"displacementbound_flag", L"");
		item.PutAttribute(siUINoLabel, true);
		item.PutWidthPercentage(10);
		item = oLayout.AddItem(L"displacementbound", L"");
	oLayout.EndRow();
	
	oLayout.AddRow();
		item = oLayout.AddItem(L"shadingrate_flag", L"");
		item.PutAttribute(siUINoLabel, true);
		item.PutWidthPercentage(10);
		item = oLayout.AddItem(L"shadingrate", L"");
	oLayout.EndRow();
	
	oLayout.AddRow();
		item = oLayout.AddItem(L"eyesplits_flag", L"");
		item.PutAttribute(siUINoLabel, true);
		item.PutWidthPercentage(10);
		item = oLayout.AddItem(L"eyesplits", L"");
	oLayout.EndRow();

	item = oLayout.AddItem(L"nonbackfacecull", L"");
	item = oLayout.AddItem(L"nonhiddencull", L"");
	item = oLayout.AddItem(L"nonrasterorient", L"");
	
	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManAttributes.htm").GetAsText());

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIDelightOptionsAttach(XSI::CRef& in_ref)
{
	Application app;
	Parameter param;
	CString objName;
	CStatus st(CStatus::OK);

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManDelightOptions");
		if (!propRef.IsValid()) {
			objName = SIObject(sel[i]).GetName();
			app.LogMessage(L"Adding 3Delight options property to " + objName);
			Property prop = SceneItem(sel[i]).AddProperty(L"XSIManDelightOptions");
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
XSI::CStatus OnXSIDelightOptionsDetach(XSI::CRef& in_ref)
{
	Application app;
	CStatus st(CStatus::OK);
	CValue retval;

	Selection sel(app.GetSelection());

	for (long i = 0; i < sel.GetCount(); i++) {
		CRef propRef = SceneItem(sel[i]).GetProperties().GetItem(L"XSIManDelightOptions");
		if (propRef.IsValid()) {
			DeleteObj(propRef);
		}
	}

	return st;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManDelightOptions_Define(const CRef & in_Ctx)
{
	CustomProperty prop = Context(in_Ctx).GetSource();
	Parameter param;

	// Default capabilities for most of these parameters
	int caps = siPersistable;
	CValue dft;	// Used for arguments we don't want to set

	prop.AddParameter(L"subsurface", CValue::siBool, siAnimatable, L"Subsurface Scattering", L"", false, param);
	prop.AddParameter(L"ssGroup", CValue::siString, caps, L"Subsurface Group", L"", dft, param);
	prop.PutParameterValue(L"ssGroup", CValue(L"marble"));
	prop.AddParameter(L"sssred", CValue::siFloat, siPersistable, L"", L"", 2.19, 0.0, 100.0, 0.0, 100.0, param);
	prop.AddParameter(L"sssgreen", CValue::siFloat, siPersistable, L"", L"", 2.62, 0.0, 100.0, 0.0, 100.0, param);
	prop.AddParameter(L"sssblue", CValue::siFloat, siPersistable, L"", L"", 3.0, 0.0, 100.0, 0.0, 100.0, param);
	prop.AddParameter(L"ssared", CValue::siFloat, siPersistable, L"", L"", .0021, 0.0, 10.0, 0.0, 100.0, param);
	prop.AddParameter(L"ssagreen", CValue::siFloat, siPersistable, L"", L"", .0041, 0.0, 10.0, 0.0, 100.0, param);
	prop.AddParameter(L"ssablue", CValue::siFloat, siPersistable, L"", L"", .0071, 0.0, 10.0, 0.0, 100.0, param);
	prop.AddParameter(L"ssRefractionIndex", CValue::siFloat, siPersistable, L"Refraction Index", L"", 1.5, 0.0, 10.0, 0.0, 10.0, param);
	prop.AddParameter(L"ssScale", CValue::siFloat, siPersistable, L"Scale", L"", 0.1, 0.0, 1000.0, 0.0, 1000.0, param);
	prop.AddParameter(L"ssShadingRate", CValue::siFloat, siPersistable, L"Shading Rate", L"", 1., 0.0, 100.0, 0.0, 100.0, param);

	return CStatus::OK;
}

#ifdef unix
extern "C"
#endif
CStatus XSIManDelightOptions_DefineLayout(const CRef & in_Ctx)
{
	PPGLayout oLayout = Context(in_Ctx).GetSource();
	PPGItem item;

	oLayout.Clear();

	oLayout.AddGroup(L"Subsurface Scattering");
	
		item = oLayout.AddItem(L"subsurface", L"");
			
		item = oLayout.AddItem(L"ssGroup", L"");

		oLayout.AddGroup(L"Scattering Color");
			oLayout.AddRow();
				item = oLayout.AddColor(L"sssred", L"Scattering Color", false);
			oLayout.EndRow();
		oLayout.EndGroup();
		
		oLayout.AddGroup(L"Absorption Color");
			oLayout.AddRow();
				item = oLayout.AddColor(L"ssared", L"Absorption Color", false);
			oLayout.EndRow();
		oLayout.EndGroup();

		oLayout.AddRow();
		item.PutWidthPercentage(10);
		item = oLayout.AddItem(L"ssRefractionIndex", L"");
		oLayout.EndRow();
				
		oLayout.AddRow();
		item.PutWidthPercentage(10);
		item = oLayout.AddItem(L"ssScale", L"");
		oLayout.EndRow();
			
		oLayout.AddRow();
		item.PutWidthPercentage(10);
		item = oLayout.AddItem(L"ssShadingrate", L"");
		oLayout.EndRow();

	oLayout.EndRow();
	
	oLayout.EndGroup();
	
	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	oLayout.PutAttribute(siUIHelpFile, CValue(xsimanhome).GetAsText() + CValue(L"/html/XSIManAttributes.htm").GetAsText());

	return CStatus::OK;
}
