
// ImageCorrectionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageCorrection.h"
#include "ImageCorrectionDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "gdal.h" 
#include "gdal_priv.h" 
#include "ogr_spatialref.h" 
#include "gdal_alg.h"
#include "ogr_srs_api.h"
#include "cpl_string.h"
#include "cpl_conv.h"
#include "cpl_multiproc.h"
#pragma comment(lib,"gdal_i.lib") 
#pragma comment(lib,"proj.lib")
#pragma comment(lib,"Ws2_32.lib")


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	CImageCorrectionDlg* pdlgParent;
// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	void setDlgParant(CImageCorrectionDlg*p) { pdlgParent = p; }
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
	pdlgParent = NULL;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CImageCorrectionDlg 对话框




CImageCorrectionDlg::CImageCorrectionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImageCorrectionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageCorrectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_ImageTxt);
	DDX_Control(pDX, IDC_EDIT2, m_xTxt);
	DDX_Control(pDX, IDC_EDIT3, m_YTxt);
	DDX_Control(pDX, IDC_EDIT4, m_OutFilePath);
	DDX_Control(pDX, IDC_COMBO1, m_CbxStType);
}

BEGIN_MESSAGE_MAP(CImageCorrectionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CImageCorrectionDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CImageCorrectionDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CImageCorrectionDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CImageCorrectionDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CImageCorrectionDlg 消息处理程序

BOOL CImageCorrectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	m_ImageTxt.SetWindowTextW(L"E:\\TestData1\\hy\\421081002007.tif");
	// TOO: 在此添加额外的初始化代码
	m_xTxt.SetWindowTextW(L"5");
	m_YTxt.SetWindowTextW(L"5");

	m_CbxStType.AddString(L"最值线性拉伸");
	m_CbxStType.AddString(L"百分比裁剪拉伸");
	m_CbxStType.AddString(L"标准差拉伸");
	m_CbxStType.SetCurSel(0);
	
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CImageCorrectionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.setDlgParant(this);
		dlgAbout.DoModal();

	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CImageCorrectionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CImageCorrectionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//百分比剪辑拉伸
int CImageCorrectionDlg::Stretch(const char* pszSrcFile, const char* pszDstFile, double percentMin, double percentMax)
{
	GDALDataset* poSrcDataset = (GDALDataset *)GDALOpen(pszSrcFile, GA_Update);
	if (poSrcDataset == NULL)
	{
		::MessageBox(this->m_hWnd, _T("打开文件失败"), _T("错误"),
			MB_ICONERROR);
		return  0;
	}
	GDALDataset* poDstDataset = (GDALDataset *)GDALCreateCopy(poSrcDataset->GetDriver(), pszDstFile, poSrcDataset, FALSE,
		NULL, NULL, NULL);
	if (poDstDataset == NULL)
		return 0;

	int    nXSize = poSrcDataset->GetRasterXSize();
	int    nYSize = poSrcDataset->GetRasterYSize();
	int nBandCount = poSrcDataset->GetRasterCount();
	double pdfMin, pdfMax, pdfMean, pdfStdDev;
	//遍历所有波段
	for (int iBand = 1; iBand <= nBandCount; iBand++)
	{
		//获得当前波段
		GDALRasterBand* pSrcBand = poSrcDataset->GetRasterBand(iBand);
		GDALDataType dataType = pSrcBand->GetRasterDataType();
		GDALRasterBand* pDetBand = poDstDataset->GetRasterBand(iBand);
		//获得统计信息
		//cout << "获得统计信息" << endl;
		pSrcBand->ComputeStatistics(TRUE, &pdfMin, &pdfMax, &pdfMean, &pdfStdDev, NULL, NULL);// pfnProgress, pProgress);

		switch (dataType)
		{
		case GDT_Byte:	  //对应图像的Byte类型,8bit.
		{
			uchar* pSrcBuf = new uchar[nXSize];
			uchar* pDstBuf = new uchar[nXSize];
			int nBuckets = 256;
			GUIntBig* pBandHistogram = new GUIntBig[nBuckets];
			memset(pBandHistogram, 0,sizeof(GUIntBig)*nBuckets);
			//cout << "正在统计直方图..." << endl;
			pSrcBand->GetHistogram(-0.5, 255.5, nBuckets, pBandHistogram, FALSE, TRUE, NULL, NULL);// pfnProgress, pProgress);
			GUIntBig min = 0, max = 0;
			MaxMinValue<GUIntBig>(pBandHistogram, nBuckets, max, min);
			
			//#pragma omp parallel for	RasterIO不能使用并行，否则会出现数据错误。
			for (int h = 0; h < nYSize; h++)
			{
				CPLErr Err = pSrcBand->RasterIO(GF_Read, 0, h, nXSize, 1, pSrcBuf, nXSize, 1, GDT_Byte, 0, 0);
				if (m_StType == 0)
					LinearTension<uchar>(pSrcBuf, pDstBuf, nXSize, pdfMin, pdfMax, 0, 255);
				else if (m_StType == 1)
					PercentLinearTension<uchar>(pSrcBuf, pDstBuf, nXSize, pdfMin, pdfMax,0,255, percentMin, percentMax);
				else if (m_StType == 2)
					StandardDeviationTension<uchar>(pSrcBuf, pDstBuf, nXSize, pdfMin, pdfMax, 0, 255, pdfStdDev, pdfMean, percentMin);
				//PercentClipStretchAglImpU8(pSrcBuf, pDstBuf, nXSize*nYSize, nXSize, &pBandHistogram[0], nBuckets, percentMin, percentMax);
				Err = pDetBand->RasterIO(GF_Write, 0, h, nXSize, 1, pDstBuf, nXSize, 1, GDT_Byte, 0, 0);
			}
			//释放缓冲区
			delete pBandHistogram;
			delete pSrcBuf;
			delete pDstBuf;
			break;
		}
		case GDT_UInt16:	//对应图像的UInt类型，16bit.
		{
			ushort* pSrcBuf = new ushort[nXSize];
			ushort* pDstBuf = new ushort[nXSize];
			int nBuckets = 65536;
			GUIntBig* imageHist = new GUIntBig[nBuckets];
			//cout << "正在统计直方图..." << endl;
			//FALSE:不统计范围之外的像素，TRUE精确计算各个像素值
			pSrcBand->GetHistogram(-0.5, 65535.5, nBuckets, (GUIntBig*)imageHist, FALSE, FALSE, NULL, NULL);// pfnProgress, pProgress);
			GUIntBig min = 0, max = 0;
			MaxMinValue<GUIntBig>(imageHist, nBuckets, max, min);
			//#pragma omp parallel for
			for (int h = 0; h < nYSize; h++)
			{
				CPLErr Err = pSrcBand->RasterIO(GF_Read, 0, h, nXSize, 1, pSrcBuf, nXSize, 1, GDT_UInt16, 0, 0);
				if (m_StType == 0)
					LinearTension<ushort>(pSrcBuf, pDstBuf, nXSize, pdfMin, pdfMax, 0, 65535);
				else if (m_StType == 1)
					PercentLinearTension<ushort>(pSrcBuf, pDstBuf, nXSize, pdfMin, pdfMax, 0, 65535, percentMin, percentMax);
				else if (m_StType == 2)
					StandardDeviationTension<ushort>(pSrcBuf, pDstBuf, nXSize, pdfMin, pdfMax, 0, 65535, pdfStdDev, pdfMean, percentMin);
					//PercentClipStretchAglImpU16(pSrcBuf, pDstBuf, nXSize*nYSize, (int*)imageHist, nBuckets, percentMin, percentMax);
				Err = pDetBand->RasterIO(GF_Write, 0, h, nXSize, 1, pDstBuf, nXSize, 1, GDT_UInt16, 0, 0);
			}
			//释放缓冲区
			delete pSrcBuf;
			delete pDstBuf;
		}
		break;
		default:
			return 0; // 不识别的数据类型
		}
	}
	// 释放内存空间
	GDALClose(poDstDataset);
	GDALClose(poSrcDataset);
	return -1;
}



void CImageCorrectionDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	//CDialogEx::OnOK();
	CString FIlePath, FoutPath,ax, ay;
	m_ImageTxt.GetWindowTextW(FIlePath);
	m_OutFilePath.GetWindowTextW(FoutPath);
	// TOO: 在此添加额外的初始化代码
	m_xTxt.GetWindowTextW(ax);
	m_YTxt.GetWindowTextW(ay);
	double x = _ttof(ax);
	double y = _ttof(ay);
	m_x = x;
	m_y = y;
	std::string  imgpath = CW2A(FIlePath);
	std::string  imgpathOut = CW2A(FoutPath);
	m_StType = m_CbxStType.GetCurSel();
	int r = Stretch(imgpath.c_str(), imgpathOut.c_str(), x, y);
	if (r)
	{
		::MessageBox(this->m_hWnd, _T("完成"), _T("提示"),
			MB_ICONINFORMATION |MB_OK);
	}
	else
	{
		::MessageBox(this->m_hWnd, _T("失败,请确认输入文件是否正确"), _T("警告"),
			MB_ICONEXCLAMATION);
	}

	//以下为影像偏移修改
	//GDALDataset* hDataset = (GDALDataset *)GDALOpen(imgpath.c_str(), GA_Update);
	//if (hDataset == NULL)
	//{
	//	//MessageBox(L"打开失败");
	//	::MessageBox(this->m_hWnd, _T("打开文件失败"), _T("错误"),
	//		MB_ICONERROR);
	//	return;
	//}
	//
	////像素大小
	//int sizex = GDALGetRasterXSize(hDataset);
	//int sizeY = GDALGetRasterYSize(hDataset);
	////空间参考
	//const char* spatialString = GDALGetProjectionRef(hDataset);
	////坐标信息
	//double		adfGeoTransform[6];
	//if (GDALGetGeoTransform(hDataset, adfGeoTransform) == CE_None)
	//{
	//	if (adfGeoTransform[2] == 0.0 && adfGeoTransform[4] == 0.0)
	//	{
	//		printf("Origin = (%.15f,%.15f)\n",
	//			adfGeoTransform[0], adfGeoTransform[3]);

	//		printf("Pixel Size = (%.15f,%.15f)\n",
	//			adfGeoTransform[1], adfGeoTransform[5]);
	//		
	//	}
	//	else
	//		printf("GeoTransform =\n"
	//			"  %.16g, %.16g, %.16g\n"
	//			"  %.16g, %.16g, %.16g\n",
	//			adfGeoTransform[0],
	//			adfGeoTransform[1],
	//			adfGeoTransform[2],
	//			adfGeoTransform[3],
	//			adfGeoTransform[4],
	//			adfGeoTransform[5]);

	//	double srcCenterX = GDALGetRasterXSize(hDataset) / 2.0;
	//	double srcCenterY = GDALGetRasterYSize(hDataset) / 2.0;
	//	double csx = adfGeoTransform[0] + (srcCenterX - 1)* adfGeoTransform[1];
	//	double csy = adfGeoTransform[3] - (srcCenterY - 1)*adfGeoTransform[5];

	//	double dsx = csx + x;
	//	double dsy = csy + y;

	//	double dsOrx = dsx - (srcCenterX - 1)* adfGeoTransform[1];
	//	double dsOty = dsy + (srcCenterY - 1)* adfGeoTransform[5];

	//	adfGeoTransform[0] = dsOrx;
	//	adfGeoTransform[3] = dsOty;
	//	double		dsttrf[6];
	//	for (int i = 0; i < 6; i++)
	//		dsttrf[i] = adfGeoTransform[i];
	//	if (GDALSetGeoTransform(hDataset, dsttrf) == CE_None)
	//	{
	//		::MessageBox(this->m_hWnd, _T("修改完成"), _T("提示"),
	//			MB_ICONINFORMATION |MB_OK);
	//	}
	//	else
	//	{
	//		::MessageBox(this->m_hWnd, _T("修改失败,请确认输入文件是否正确"), _T("警告"),
	//			MB_ICONEXCLAMATION);
	//	}
	//}
	//

	//if(hDataset)
	// GDALClose(hDataset);
}

bool CImageCorrectionDlg::DealOne(CString fileName, double dx, double dy)
{
	std::string imgpath = CW2A(fileName);
	GDALDataset* hDataset = (GDALDataset *)GDALOpen(imgpath.c_str(), GA_Update);
	if (hDataset == NULL)
	{
		return false;
	}
	//像素大小
	int sizex = GDALGetRasterXSize(hDataset);
	int sizeY = GDALGetRasterYSize(hDataset);
	//空间参考
	const char* spatialString = GDALGetProjectionRef(hDataset);
	//坐标信息
	double		adfGeoTransform[6];
	if (GDALGetGeoTransform(hDataset, adfGeoTransform) == CE_None)
	{
		if (adfGeoTransform[2] == 0.0 && adfGeoTransform[4] == 0.0)
		{
			printf("Origin = (%.15f,%.15f)\n",
				adfGeoTransform[0], adfGeoTransform[3]);

			printf("Pixel Size = (%.15f,%.15f)\n",
				adfGeoTransform[1], adfGeoTransform[5]);

		}
		else
			printf("GeoTransform =\n"
				"  %.16g, %.16g, %.16g\n"
				"  %.16g, %.16g, %.16g\n",
				adfGeoTransform[0],
				adfGeoTransform[1],
				adfGeoTransform[2],
				adfGeoTransform[3],
				adfGeoTransform[4],
				adfGeoTransform[5]);

		double srcCenterX = GDALGetRasterXSize(hDataset) / 2.0;
		double srcCenterY = GDALGetRasterYSize(hDataset) / 2.0;
		double csx = adfGeoTransform[0] + (srcCenterX - 1)* adfGeoTransform[1];
		double csy = adfGeoTransform[3] - (srcCenterY - 1)*adfGeoTransform[5];

		double dsx = csx + dx;
		double dsy = csy + dy;

		double dsOrx = dsx - (srcCenterX - 1)* adfGeoTransform[1];
		double dsOty = dsy + (srcCenterY - 1)* adfGeoTransform[5];

		adfGeoTransform[0] = dsOrx;
		adfGeoTransform[3] = dsOty;
		double		dsttrf[6];
		for (int i = 0; i < 6; i++)
			dsttrf[i] = adfGeoTransform[i];
		CPLErr er = GDALSetGeoTransform(hDataset, dsttrf);
		if (er == CE_None)
		{
			if (hDataset)
				GDALClose(hDataset);
			return true;
		}
	}


	if (hDataset)
		GDALClose(hDataset);
	return false;
}

void CImageCorrectionDlg::OnBnClickedCancel()
{
	//DealFolder();
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CImageCorrectionDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strFile = _T("");

	CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("All Image  Files (*.*)|*.*"), NULL);

	if (dlgFile.DoModal())
	{
		strFile = dlgFile.GetPathName();
		m_ImageTxt.SetWindowTextW(strFile);
	}
}

void CImageCorrectionDlg::DealFolder()
{
	CString txtFolder;
	CString m_strFileOut = _T("");
	TCHAR servPath[MAX_PATH];//用来存放文件夹路径
	BROWSEINFO bi;
	LPITEMIDLIST pidl;
	bi.hwndOwner = this->m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = servPath;
	bi.lpszTitle = _T("选择文件夹路径");
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = NULL;
	bi.iImage = NULL;
	if ((pidl = SHBrowseForFolder(&bi)) != NULL)
	{
		if (SUCCEEDED(SHGetPathFromIDList(pidl, servPath))) //得到文件夹的全路径，不要的话，只得本文件夹名
		{
			txtFolder = servPath;//SetDlgItemText(IDC_EDIT_SERV_PATH, servPath);
		}
		else
		{
			MessageBox(L"获取文件夹失败");
			return;
		}
	}


	std::vector<CString> CWfiles;
	if (SetCurrentDirectory(txtFolder) == TRUE)
	{
		CFileFind  finder;
		BOOL bWorking = finder.FindFile();
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			if (finder.IsDirectory())
				continue;
			CString inPutFilename = finder.GetFileName();
			CWfiles.push_back(inPutFilename);
		}
	}

	CString ax, ay;
	m_xTxt.GetWindowTextW(ax);
	m_YTxt.GetWindowTextW(ay);
	double x = _ttof(ay);
	double y = _ttof(ay);
	for (int i = 0; i < CWfiles.size(); i++)
	{
		DealOne(CWfiles[i], x, y);
	}
	MessageBox(L"修改完成",L"提示", MB_OK);
}


void CAboutDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if(pdlgParent!= NULL)
		pdlgParent->DealFolder();
	CDialogEx::OnOK();
}


void CImageCorrectionDlg::OnBnClickedButton2()
{
	CString strFile = _T("");

	CFileDialog    dlgFile(FALSE, NULL, NULL, OFN_HIDEREADONLY, _T("All Image  Files (*.*)|*.*"), NULL);

	if (dlgFile.DoModal())
	{
		strFile = dlgFile.GetPathName();
		m_OutFilePath.SetWindowTextW(strFile);
	}
}

int CImageCorrectionDlg::PercentClipStretchAglImpU8(const uchar* poSrcData, uchar* poDstData, long long nAllPixels, int nSizex, unsigned long long  ImageHist[], int histLength, double percentMin, double percentMax)
{
	//确定图像数据灰度值。
	int grayScale = 255;

	int countMin = 0;
	int countMax = 0;
	int dbMin = 0, dbMax = 255;
	percentMin = long long(percentMin * nAllPixels);
	percentMax = long long(percentMax * nAllPixels);

	//找出 percentMin的像素的灰度值
	//#pragma omp parallel for
	for (int i = 0; i < histLength; i++)
	{
		countMin += ImageHist[i];
		if (countMin > percentMin)
		{
			dbMin = i;
			break;
		}
	}

	//找出percnetMax的像素的灰度值
	//#pragma omp parallel for
	for (int k = 0; k < histLength; k++)
	{
		countMax += ImageHist[k];
		if (countMax > percentMax)
		{
			dbMax = k;
			break;
		}
	}

	//#pragma omp parallel for
	for (int i = 0; i < nSizex; i++)
	{
		if (poSrcData[i] <= dbMin)
		{
			poDstData[i] = 0;
		}
		else if (poSrcData[i] >= dbMax)
		{
			poDstData[i] = grayScale;
		}
		else if (poSrcData[i] > dbMin && poSrcData[i] < dbMax)
		{
			poDstData[i] = double(poSrcData[i] - dbMin) / (double)(dbMax - dbMin) * grayScale;
		}
	}
	return 0;
}

int CImageCorrectionDlg::PercentClipStretchAglImpU16(const ushort* poSrcData, ushort* poDstData, int nAllPixels, int ImageHist[], int histLength, double percentMin, double percentMax)
{
	//确定图像数据灰度值。
	int grayScale = 65536;

	int countMin = 0;
	int countMax = 0;
	int dbMin, dbMax;
	int minPixelIndex;
	int maxPixelIndex;

	minPixelIndex = int(percentMin * nAllPixels);
	maxPixelIndex = int(percentMax * nAllPixels);

	int total = 0;
	for (int i = 0; i < histLength; i++)
	{
		total += ImageHist[i];
	}

	//找出 percentMin的像素的灰度值
	//#pragma omp parallel for
	for (int i = 0; i < histLength; i++)
	{
		countMin += ImageHist[i];
		if (countMin >= minPixelIndex)
		{
			dbMin = i;
			break;
		}
	}

	//找出percnetMax的像素的灰度值
	//#pragma omp parallel for
	for (int k = 0; k < histLength; k++)
	{
		countMax += ImageHist[k];
		if (countMax >= maxPixelIndex)
		{
			dbMax = k;
			break;
		}
	}

	//#pragma omp parallel for
	for (int i = 0; i < nAllPixels; i++)
	{
		if (poSrcData[i] <= dbMin)
		{
			poDstData[i] = 0;
		}
		else if (poSrcData[i] >= dbMax)
		{
			poDstData[i] = grayScale;
		}
		else if (poSrcData[i] > dbMin && poSrcData[i] < dbMax)
		{
			poDstData[i] = double(poSrcData[i] - dbMin) / (double)(dbMax - dbMin) * grayScale;
		}
	}
	return 0;
}
