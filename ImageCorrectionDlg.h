
// ImageCorrectionDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "math.h"
//����
inline double variance(double x, double averageValue)
{
	return (x - averageValue)*(x - averageValue);
}
//��׼��
inline double StandardDeviation(double x, double averageValue)
{
	return sqrt((x - averageValue)*(x - averageValue));
}
#define uchar unsigned char
#define ushort unsigned short
// CImageCorrectionDlg �Ի���
class CImageCorrectionDlg : public CDialogEx
{
// ����
public:
	CImageCorrectionDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_IMAGECORRECTION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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

	//����
	int Stretch(const char* pszSrcFile, const char* pszDstFile, double percentMin, double percentMax);

	//�������� ���������Сֵ
	template<typename T>
	int		LinearTension(const T* poSrcData, T* poDstData, int nSizex, T smin, T smax, T dmin, T dmax);
	//�ٷֱȽض�����
	template<typename T>
	int PercentLinearTension(const T* poSrcData, T* poDstData, int nSizex, T smin, T smax, T dmin, T dmax,double percentMin, double percentMax);

	//��׼������
	//double StandardDeviation, ��׼��
	//double avg, ƽ��ֵ
	//int K ����k����׼���ֱ��ӳ�䵽255
	template<typename T>
	int StandardDeviationTension(const T * poSrcData, T * poDstData, int nSizex,
		T smin, T smax, T dmin, T dmax,
		double StandardDeviation, double avg, int Kb);
	template<typename T>
	bool MaxMinValue(const T* pData, int length, T & max, T & min);


	// ���Ŀ¼
	CEdit m_OutFilePath;
	afx_msg void OnBnClickedButton2();
	// ��������
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
				poDstData[i] = 0;//ԭ������0�����ֱ������0����Ϊnodata
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
				poDstData[i] = 1;//ԭ������0�����ֱ������0����Ϊnodata
		}
		else if (tmp >= 255)
			poDstData[i] = 255;
		else
			poDstData[i] = (T)tmp;
	}
	return 0;
}


/// \brief ��׼������
/// \param pInter  const T * poSrcData, ԭʼ����
/// \param T * poDstData, Ŀ�����ݿ�
/// \param int nSizex ���ݳ���
/// \param T smin, T smax  ԭʼ����ֵ��
/// \param  T dmin, T dmax  Ŀ������ֵ��
/// \param double StandardDeviation, ��׼��
/// \param double avg ƽ��ֵ
/// \param int Kn  ����
/// \return 
template<typename T>
inline int CImageCorrectionDlg::StandardDeviationTension(const T * poSrcData, T * poDstData, int nSizex,
	T smin, T smax, T dmin, T dmax,
	double StandardDeviation, double avg, int Kn)
{
	//double k = (double)(dmax - dmin) / (smax - smin);
	//double b = (double)(smax*dmin - smin*dmax) / (smax - smin);

	////0~K����׼�� ��Χ��ֵ��ӳ������ֵ����ֵ
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

		//��ǰ��ı�׼�
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
				poDstData[i] = 1;//ԭ������0�����ֱ������0����Ϊnodata
		}
		else if (tmp >= 255)
			poDstData[i] = 255;
		else
			poDstData[i] = (T)tmp;
	}
	return 0;
}


//�ҵ�һ�������һ�����ֵ���ɫֵ
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


