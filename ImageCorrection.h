
// ImageCorrection.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CImageCorrectionApp:
// �йش����ʵ�֣������ ImageCorrection.cpp
//

class CImageCorrectionApp : public CWinApp
{
public:
	CImageCorrectionApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CImageCorrectionApp theApp;