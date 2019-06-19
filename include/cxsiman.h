//*****************************************************************************
/*!	\file CXSIMan.h
 *	\brief CXSIMan object class.
 *
 * (c) Copyright 1996 - 2003 Softimage/Avid Corporation
 *
 */
//*****************************************************************************

#ifndef __CXSIMan_H__
#define __CXSIMan_H__

#include <xsi_status.h>
#include <xsi_model.h>
#include <xsi_geometry.h>
#include <xsi_polygonmesh.h>
#include <xsi_nurbscurvelist.h>
#include <xsi_nurbssurfacemesh.h>
#include <xsi_application.h>
#include <xsi_material.h>
#include <xsi_projectitem.h>
#include <xsi_parameter.h>
#include <xsi_siobject.h>
#include <xsi_shader.h>
#include <SemanticLayer.h>// Part of the FTK.  Make sure you have the XSIFTK_ROOT env variable set.

#include <ri.h>


//*****************************************************************************
/*! \class CXSIMan CXSIMan.h
 */
//*****************************************************************************
class CXSIMan
{
public:
	/*! Default constructor. */
	CXSIMan();

	XSI::CStatus ExportFile(const XSI::CString& in_FileName,const XSI::CString& in_ObjectToExport)const;
protected:	

	CSLModel* ExportCamera(CSLModel*,XSI::Camera&)const;

	CSLModel* ExportGeometry(CSLModel*,XSI::X3DObject&)const;

	CSLModel* ExportLight(CSLModel*,XSI::Light&)const;

	XSI::CStatus ExportNull(CSLModel*,XSI::Null&)const;

	XSI::CStatus ExportNurbsCurveList(CSLModel*,XSI::NurbsCurveList&)const;

	XSI::CStatus ExportNurbsSurfaceMesh(CSLModel*,XSI::NurbsSurfaceMesh&)const;

	XSI::CStatus ExportPolygonMesh(CSLModel*,XSI::PolygonMesh&)const;

	XSI::CStatus ExportShaderRecursively(CSLXSIMaterial*, XSI::Shader&, XSI::CRefArray&)const;

	XSI::CStatus ExportX3DObject(CSLModel*,XSI::X3DObject&)const;

	XSI::CStatus FillModel(CSLModel*, XSI::X3DObject&)const;

	XSI::CStatus FillParameter(CSLVariantParameter*, XSI::Parameter&, const XSI::CString&)const;

	XSI::CStatus FillShader(CSLXSIShader*, XSI::Shader&)const;

	XSI::CStatus FillTransform(CSLTransform*, XSI::MATH::CTransformation&)const;

	XSI::CStatus FillXSIMaterial(CSLXSIMaterial*,const XSI::Material& )const;

	XSI::CStatus SetName(CSLTemplate*, const XSI::CString& )const;
private:

	XSI::Application m_app;
	XSI::Model       m_sceneRoot;
	
};

#endif // __CXSIMan_H__
