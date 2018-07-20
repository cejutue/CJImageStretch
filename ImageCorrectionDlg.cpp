
// ImageCorrectionDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	CImageCorrectionDlg* pdlgParent;
// ʵ��
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


// CImageCorrectionDlg �Ի���




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


// CImageCorrectionDlg ��Ϣ�������

BOOL CImageCorrectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	m_ImageTxt.SetWindowTextW(L"E:\\TestData1\\hy\\421081002007.tif");
	// TOO: �ڴ���Ӷ���ĳ�ʼ������
	m_xTxt.SetWindowTextW(L"5");
	m_YTxt.SetWindowTextW(L"5");

	m_CbxStType.AddString(L"��ֵ��������");
	m_CbxStType.AddString(L"�ٷֱȲü�����");
	m_CbxStType.AddString(L"��׼������");
	m_CbxStType.SetCurSel(0);
	
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CImageCorrectionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CImageCorrectionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//�ٷֱȼ�������
int CImageCorrectionDlg::Stretch(const char* pszSrcFile, const char* pszDstFile, double percentMin, double percentMax)
{
	GDALDataset* poSrcDataset = (GDALDataset *)GDALOpen(pszSrcFile, GA_Update);
	if (poSrcDataset == NULL)
	{
		::MessageBox(this->m_hWnd, _T("���ļ�ʧ��"), _T("����"),
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
	//�������в���
	for (int iBand = 1; iBand <= nBandCount; iBand++)
	{
		//��õ�ǰ����
		GDALRasterBand* pSrcBand = poSrcDataset->GetRasterBand(iBand);
		GDALDataType dataType = pSrcBand->GetRasterDataType();
		GDALRasterBand* pDetBand = poDstDataset->GetRasterBand(iBand);
		//���ͳ����Ϣ
		//cout << "���ͳ����Ϣ" << endl;
		pSrcBand->ComputeStatistics(TRUE, &pdfMin, &pdfMax, &pdfMean, &pdfStdDev, NULL, NULL);// pfnProgress, pProgress);

		switch (dataType)
		{
		case GDT_Byte:	  //��Ӧͼ���Byte����,8bit.
		{
			uchar* pSrcBuf = new uchar[nXSize];
			uchar* pDstBuf = new uchar[nXSize];
			int nBuckets = 256;
			GUIntBig* pBandHistogram = new GUIntBig[nBuckets];
			memset(pBandHistogram, 0,sizeof(GUIntBig)*nBuckets);
			//cout << "����ͳ��ֱ��ͼ..." << endl;
			pSrcBand->GetHistogram(-0.5, 255.5, nBuckets, pBandHistogram, FALSE, TRUE, NULL, NULL);// pfnProgress, pProgress);
			GUIntBig min = 0, max = 0;
			MaxMinValue<GUIntBig>(pBandHistogram, nBuckets, max, min);
			
			//#pragma omp parallel for	RasterIO����ʹ�ò��У������������ݴ���
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
			//�ͷŻ�����
			delete pBandHistogram;
			delete pSrcBuf;
			delete pDstBuf;
			break;
		}
		case GDT_UInt16:	//��Ӧͼ���UInt���ͣ�16bit.
		{
			ushort* pSrcBuf = new ushort[nXSize];
			ushort* pDstBuf = new ushort[nXSize];
			int nBuckets = 65536;
			GUIntBig* imageHist = new GUIntBig[nBuckets];
			//cout << "����ͳ��ֱ��ͼ..." << endl;
			//FALSE:��ͳ�Ʒ�Χ֮������أ�TRUE��ȷ�����������ֵ
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
			//�ͷŻ�����
			delete pSrcBuf;
			delete pDstBuf;
		}
		break;
		default:
			return 0; // ��ʶ�����������
		}
	}
	// �ͷ��ڴ�ռ�
	GDALClose(poDstDataset);
	GDALClose(poSrcDataset);
	return -1;
}



void CImageCorrectionDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//CDialogEx::OnOK();
	CString FIlePath, FoutPath,ax, ay;
	m_ImageTxt.GetWindowTextW(FIlePath);
	m_OutFilePath.GetWindowTextW(FoutPath);
	// TOO: �ڴ���Ӷ���ĳ�ʼ������
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
		::MessageBox(this->m_hWnd, _T("���"), _T("��ʾ"),
			MB_ICONINFORMATION |MB_OK);
	}
	else
	{
		::MessageBox(this->m_hWnd, _T("ʧ��,��ȷ�������ļ��Ƿ���ȷ"), _T("����"),
			MB_ICONEXCLAMATION);
	}

	//����ΪӰ��ƫ���޸�
	//GDALDataset* hDataset = (GDALDataset *)GDALOpen(imgpath.c_str(), GA_Update);
	//if (hDataset == NULL)
	//{
	//	//MessageBox(L"��ʧ��");
	//	::MessageBox(this->m_hWnd, _T("���ļ�ʧ��"), _T("����"),
	//		MB_ICONERROR);
	//	return;
	//}
	//
	////���ش�С
	//int sizex = GDALGetRasterXSize(hDataset);
	//int sizeY = GDALGetRasterYSize(hDataset);
	////�ռ�ο�
	//const char* spatialString = GDALGetProjectionRef(hDataset);
	////������Ϣ
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
	//		::MessageBox(this->m_hWnd, _T("�޸����"), _T("��ʾ"),
	//			MB_ICONINFORMATION |MB_OK);
	//	}
	//	else
	//	{
	//		::MessageBox(this->m_hWnd, _T("�޸�ʧ��,��ȷ�������ļ��Ƿ���ȷ"), _T("����"),
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
	//���ش�С
	int sizex = GDALGetRasterXSize(hDataset);
	int sizeY = GDALGetRasterYSize(hDataset);
	//�ռ�ο�
	const char* spatialString = GDALGetProjectionRef(hDataset);
	//������Ϣ
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}


void CImageCorrectionDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	TCHAR servPath[MAX_PATH];//��������ļ���·��
	BROWSEINFO bi;
	LPITEMIDLIST pidl;
	bi.hwndOwner = this->m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = servPath;
	bi.lpszTitle = _T("ѡ���ļ���·��");
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = NULL;
	bi.iImage = NULL;
	if ((pidl = SHBrowseForFolder(&bi)) != NULL)
	{
		if (SUCCEEDED(SHGetPathFromIDList(pidl, servPath))) //�õ��ļ��е�ȫ·������Ҫ�Ļ���ֻ�ñ��ļ�����
		{
			txtFolder = servPath;//SetDlgItemText(IDC_EDIT_SERV_PATH, servPath);
		}
		else
		{
			MessageBox(L"��ȡ�ļ���ʧ��");
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
	MessageBox(L"�޸����",L"��ʾ", MB_OK);
}


void CAboutDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	//ȷ��ͼ�����ݻҶ�ֵ��
	int grayScale = 255;

	int countMin = 0;
	int countMax = 0;
	int dbMin = 0, dbMax = 255;
	percentMin = long long(percentMin * nAllPixels);
	percentMax = long long(percentMax * nAllPixels);

	//�ҳ� percentMin�����صĻҶ�ֵ
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

	//�ҳ�percnetMax�����صĻҶ�ֵ
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
	//ȷ��ͼ�����ݻҶ�ֵ��
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

	//�ҳ� percentMin�����صĻҶ�ֵ
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

	//�ҳ�percnetMax�����صĻҶ�ֵ
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
