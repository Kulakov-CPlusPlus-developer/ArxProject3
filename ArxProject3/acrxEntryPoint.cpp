// (C) Copyright 2002-2012 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"

#include "rxobject.h"
#include <thread>
#include <sstream>
#include <iostream>
#include "adui.h"
#include "acdb.h"
#include <mutex>
#include <chrono>
#include <aced.h>
#include "acedCmdNF.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("ADSK")

class CArxProject3App;



CArxProject3App* g_pApp = nullptr;


class CArxProject3App : public AcRxArxApp {

public:
	CArxProject3App() {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here


		InitApp();

		//if (collission) {
			//showMyWindow();
		//}		

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents() {
	}

	void CArxProject3App::InitApp() {
		g_pApp = this;

	}			

	Acad::ErrorStatus GetAllLines() {

		ads_name ssname;
		struct resbuf rb;

		rb.restype = 0;
		rb.resval.rstring = L"LINE";
		rb.rbnext = NULL;

		acedSSGet(L"X", NULL, NULL, &rb, ssname);

		int numObj;
		acedSSLength(ssname, &numObj);

		AcDbObjectIdArray lineIds_(numObj);
		Acad::ErrorStatus es;

		for (int i = 0; i < numObj; i++) {
			ads_name objName;
			acedSSName(ssname, i, objName);

			es = acdbGetObjectId(lineIds_[i], objName);

			if (es == Acad::eOk) {
				continue;
			}


			if (es != Acad::eOk) {
				ads_name objName;
				acedSSName(ssname, i, objName);

				ads_point pt;
				acedGetPoint(NULL, L"\nВыберите точку:", pt);
			}
		}
		acedSSFree(ssname);		

		int b = lineIds_.physicalLength();
		
		for (int i = 0; i < lineIds_.physicalLength(); i++) {
			lineIds.append(lineIds_[i]);
		}		
		return Acad::eOk;
	}

	void SearchForOverlays() {	

		bool collission = false;

		for (int i = 0; i < lineIds.logicalLength(); i++) {
			for (int j = i+1; j < lineIds.logicalLength(); j++) {
				AcDbLine* pLine1 = nullptr;
				AcDbLine* pLine2 = nullptr;
				Acad::ErrorStatus es1 = acdbOpenObject(pLine1, lineIds[i], AcDb::kForRead);
				Acad::ErrorStatus es2 = acdbOpenObject(pLine2, lineIds[j], AcDb::kForRead);
				if (es1 == Acad::eOk && es2 == Acad::eOk) {

					AcGeVector3d dir1 = pLine1->endPoint() - pLine1->startPoint();
					AcGeVector3d dir2 = pLine2->endPoint() - pLine2->startPoint();

					dir1.normalize();
					dir2.normalize();

					

					if (fabs(dir1.dotProduct(dir2)) >= 0.9999) {

						collission = true;

						const ACHAR* myString = _T("There is a collision!");
						AcCmColor* MyColor = new AcCmColor;
						MyColor->setRGB(255, 0, 0);

						AcDbText* pText = new AcDbText();
						pText->setPosition(pLine1->startPoint());
						pText->setTextString(myString);
						pText->setHeight(3.5);
						pText->setColor(*MyColor);	

						AcDbBlockTable* pBlockTable;
						if (acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlockTable, AcDb::kForRead) == Acad::eOk) {
							AcDbBlockTableRecord* pBlockTableRecord;
							if (pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite) == Acad::eOk) {
								AcDbObjectId textId;
								pBlockTableRecord->appendAcDbEntity(textId, pText);
								pBlockTableRecord->close();
							}
							pBlockTable->close();
						}

						pText->close();						
					}

					pLine1->close();
					pLine2->close();
				}
			}
		}
		if (collission) {
			CAdUiDialog* MyWindow = new CAdUiDialog(102, nullptr, nullptr);
			MyWindow->Create(102);
			MyWindow->ShowWindow(SW_SHOW);
			collission = false;
		}		
	}

	static void ADSKMyPlagin_StartTest() {
		if (g_pApp) {
			g_pApp->GetAllLines();
			g_pApp->SearchForOverlays();
		}
	}


private:	
	AcDbObjectIdArray lineIds;	
};

IMPLEMENT_ARX_ENTRYPOINT(CArxProject3App)

ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject3App, ADSKMyPlagin, _StartTest, _StartTest, ACRX_CMD_TRANSPARENT, NULL)


