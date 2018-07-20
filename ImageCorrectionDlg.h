
// ImageCorrectionDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "math.h"
//方差
inline double variance(double x, double averageValue)
{
	return (x - averageValue)*(x - averageValue);
}
//标准差
inline double StandardDeviation(double x, double averageValue)
{
	return sqrt((x - averageValue)*(x - averageValue));
}
#define uchar unsigned char
#define ushort unsigned short
// CImageCorrectionDlg 对话框
class CImageCorrectionDlg : public CDialogEx
{
// 构造
public:
	CImageCorrectionDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_IMAGECORRECTION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ImageTxt;
	afx_msg void OnBnClickedOk();
	CEdit m_xTxt;
	CEdit m_YTxt;
	double m_x, m_y;
private:
	bool DealOne(CString fileName, double dx, double dy);
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();

	void DealFolder();

	//拉伸
	int Stretch(const char* pszSrcFile, const char* pszDstFile, double percentMin, double percentMax);

	//线性拉伸 基于最大最小值
	template<typename T>
	int		LinearTension(const T* poSrcData, T* poDstData, int nSizex, T smin, T smax, T dmin, T dmax);
	//百分比截断拉伸
	template<typename T>
	int PercentLinearTension(const T* poSrcData, T* poDstData, int nSizex, T smin, T smax, T dmin, T dmax,double percentMin, double percentMax);

	//标准差拉伸
	//double StandardDeviation, 标准差
	//double avg, 平均值
	//int K 大于k倍标准差的直接映射到255
	template<typename T>
	int StandardDeviationTension(const T * poSrcData, T * poDstData, int nSizex,
		T smin, T smax, T dmin, T dmax,
		double StandardDeviation, double avg, int Kb);
	template<typename T>
	bool MaxMinValue(const T* pData, int length, T & max, T & min);


	// 输出目录
	CEdit m_OutFilePath;
	afx_msg void OnBnClickedButton2();
	// 拉伸类型
	CComboBox m_CbxStType;
	int m_StType;

	int PercentClipStretchAglImpU8(const uchar* poSrcData, uchar* poDstData, long long nAllPixels, int nSizex, unsigned long long ImageHist[], int histLength, double percentMin, double percentMax);
	int PercentClipStretchAglImpU16(const ushort* poSrcData, ushort* poDstData, int nAllPixels, int ImageHist[], int histLength, double percentMin, double percentMax);

};

template<typename T>
inline int CImageCorrectionDlg::PercentLinearTension(const T * poSrcData, T * poDstData, int nSizex, T smin, T smax, T dmin, T dmax, double percentMin, double percentMax)
{


	double s = smax - smin+ 0.5;
	int lowx = smin, lowy = smin + s*percentMin / 100;
	int highx = smax - s*percentMin / 100, highy = smax;
	//double k = (double)(dmax - dmin) / (smax - smin);
	//double b = (double)(smax*dmin - smin*dmax) / (smax - smin);
	double k = (double)(dmax - dmin) / (highx - lowy);
	double b = (double)(highx*dmin - lowy*dmax) / (highx - lowy);
#pragma omp parallel for
	for (int i = 0; i < nSizex; i++)
	{
		if (poSrcData[i] == 0)
		{
			poDstData[i] = 0;
			continue;
		}

		if ( lowx <= poSrcData[i] && poSrcData[i] <= lowy)
		{
			poDstData[i] = 0;
			continue;
		}
		if (highx <= poSrcData[i] && poSrcData[i] <=highy)
		{
			poDstData[i] = 255;
			continue;
		}

		double at = k*poSrcData[i] + b;
		int tmp = 0;
		if (at - (int)at > 0.5)
			tmp = at + 0.5;
		else
			tmp = at - 0.5;

		if (tmp <= 0)
		{
			if (poSrcData[i] == 0)
				poDstData[i] = 0;
			else
				poDstData[i] = 0;//原来不是0的如果直接拉伸0将变为nodata
		}
		else if (tmp >= 255)
			poDstData[i] = 255;
		else
			poDstData[i] = (T)tmp;
	}
	return 0;
}

//y = x*(dmax -dmin)/(smax - smin)+(smax*dmin-smin*dmax)/(smax - smin)
template<typename T>
inline int CImageCorrectionDlg::LinearTension(const T * poSrcData, T * poDstData, int nSizex, T smin, T smax, T dmin, T dmax)
{
	double k = (double)(dmax - dmin) / (smax - smin);
	double b = (double)(smax*dmin - smin*dmax) / (smax - smin);
#pragma omp parallel for
	for (int i = 0; i < nSizex; i++)
	{
		int tmp =  k*poSrcData[i] + b;
		
		if (tmp <= 0)
		{
			if(poSrcData[i] == 0)
				poDstData[i] = 0;
			else
				poDstData[i] = 1;//原来不是0的如果直接拉伸0将变为nodata
		}
		else if (tmp >= 255)
			poDstData[i] = 255;
		else
			poDstData[i] = (T)tmp;
	}
	return 0;
}


/// \brief 标准差拉伸
/// \param pInter  const T * poSrcData, 原始数据
/// \param T * poDstData, 目标数据块
/// \param int nSizex 数据长度
/// \param T smin, T smax  原始像素值域
/// \param  T dmin, T dmax  目标像素值域
/// \param double StandardDeviation, 标准差
/// \param double avg 平均值
/// \param int Kn  倍数
/// \return 
template<typename T>
inline int CImageCorrectionDlg::StandardDeviationTension(const T * poSrcData, T * poDstData, int nSizex,
	T smin, T smax, T dmin, T dmax,
	double StandardDeviation, double avg, int Kn)
{
	//double k = (double)(dmax - dmin) / (smax - smin);
	//double b = (double)(smax*dmin - smin*dmax) / (smax - smin);

	////0~K倍标准差 范围的值做映射其他值做极值
	//double kb = K*StandardDeviation;
	double ucMax = avg + Kn * StandardDeviation;
	double ucMin = avg - Kn * StandardDeviation;

	double k = (double)(dmax - dmin) / (ucMax - ucMin);
	double b = (double)(ucMax*dmin - ucMin*dmax) / (ucMax - ucMin);

	if (ucMin <= 0)
	{
		ucMin = 0;
	}
	#pragma omp parallel for
	for (int i = 0; i < nSizex; i++)
	{
		if (poSrcData[i] == 0)
		{
			poDstData[i] = 0;
			continue;
		}

		//当前点的标准差，
		//int sd = (T)poSrcData[i] - (T)avg;
		if (poSrcData[i] <= ucMin)
		{
			poDstData[i] = 0;
			continue;
		}
		else if (poSrcData[i] >= ucMax)
		{
			poDstData[i] = 255;
			continue;
		}
		//else if (poSrcData[i] >= ucMin && poSrcData[i] <= ucMax)
		//{
		//	poDstData[i] = double(poSrcData[i] - ucMin) / (double)(ucMax - ucMin) * 255;
		//}

		int tmp = k*poSrcData[i] + b;

		if (tmp <= 0)
		{
			if (poSrcData[i] == 0)
				poDstData[i] = 0;
			else
				poDstData[i] = 1;//原来不是0的如果直接拉伸0将变为nodata
		}
		else if (tmp >= 255)
			poDstData[i] = 255;
		else
			poDstData[i] = (T)tmp;
	}
	return 0;
}


//找第一个和最后一个出现的颜色值
template<typename T>
inline bool CImageCorrectionDlg::MaxMinValue(const T * pData, int length, T & max, T & min)
{
	 min = 0, max = 0;
	bool isInit = false;
	for (int i = 0; i < length; i++)
	{
		if (pData[i] == 0)
			continue;
		if (!isInit)
		{
			min = i;
			isInit = true;
		}
		max = i;
	}
	return false;
}


