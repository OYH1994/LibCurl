#include <stdio.h>
#include<iostream>
#include<string>
#define CURL_STATICLIB   //��̬����
#include "curl.h"

#pragma comment(lib, "libcurl.lib")

using namespace std;

typedef unsigned int uint;
typedef	PVOID pvoid;


std::string f_utf8togbk(const char *szUtf8)
{
	std::string result;
	wchar_t *strSrc;
	char *szRes;

	//�����ʱ�����Ĵ�С
	int i = ::MultiByteToWideChar(CP_UTF8, 0, szUtf8, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	::MultiByteToWideChar(CP_UTF8, 0, szUtf8, -1, strSrc, i);

	//�����ʱ�����Ĵ�С
	i = ::WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new char[i + 1];
	int j = ::WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

	result = szRes;
	delete[]strSrc;
	delete[]szRes;

	return result;
}

size_t f_onwritedata(void *buffer, size_t size, size_t nmemb, void *lpVoid)
{
	string *str = dynamic_cast<string *>((string *)lpVoid);
	if (NULL == str || NULL == buffer)
	{
		return -1;
	}
	char *pData = (char*)buffer;
	str->append(pData,size *nmemb);
	return nmemb;
}

bool f_get(const string &strUrl,string & strRespose)
{
	CURLcode res;
	CURL * curl = curl_easy_init();
	if (NULL == curl)
	{
		return false;
	}

	struct curl_slist *plst_header = NULL;
	plst_header = curl_slist_append(plst_header,"charset=UTF-8");
	plst_header = curl_slist_append(plst_header,"Connection:keep-alive");
	curl_easy_setopt(curl,CURLOPT_HTTPHEADER,plst_header);			     //����Э��ͷ		
	curl_easy_setopt(curl,CURLOPT_URL,strUrl.c_str());					 //���÷��ʵ�URL
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,f_onwritedata);			 //д���ݵķ���	
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,(void*)&strRespose);		 //CURLOPT_WRITEFUNCTION�е�������Դ ���ڽ�����������ļ���
	curl_easy_setopt(curl,CURLOPT_NOSIGNAL,1);							 //�������������ź�
	curl_easy_setopt(curl,CURLOPT_CONNECTTIMEOUT,7);					 //�������ӳ�ʱ ��λ��	
	curl_easy_setopt(curl,CURLOPT_TIMEOUT,7);						     //�������ӵȴ�ʱ�� ��λ��
	res = curl_easy_perform(curl);										 //ִ��
	if (CURLE_OK != res)
	{
		
	}
	return CURLE_OK == res ? true : false;

	return 1;
}

bool f_post(const string &strUrl,const string &strPost,string &strResponse)
{
	CURLcode res;
	CURL *curl = curl_easy_init();
	if (NULL == curl)
	{
		return false;
	}

	curl_easy_setopt(curl,CURLOPT_URL,strUrl.c_str());
	curl_easy_setopt(curl,CURLOPT_POST,1);
	curl_easy_setopt(curl,CURLOPT_POSTFIELDS,strPost.c_str());
	curl_easy_setopt(curl,CURLOPT_READFUNCTION,NULL);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,f_onwritedata);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,(void*)&strResponse);
	curl_easy_setopt(curl,CURLOPT_NOSIGNAL,1);
	curl_easy_setopt(curl,CURLOPT_CONNECTTIMEOUT,7);
	curl_easy_setopt(curl,CURLOPT_TIMEOUT,7);
	res = curl_easy_perform(curl);
	if (CURLE_OK != res)
	{
		
	}
	return CURLE_OK == res ? true : false;
	return 1;
}

uint f_wtfile(pvoid p_buf, uint n_buf, uint n_ret, pvoid p_ret)
{
	return fwrite(p_buf, n_buf, n_ret, (FILE*)p_ret);
}

LPCWSTR stringToLPCWSTR(string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}

bool f_download(const string & strUrl, const string & strLocalPath)
{
	int n_eid = CURLE_OK;
	bool b_ret = false;
	do
	{
		CURL* pcurl = curl_easy_init();
		if (!pcurl)
			break;

		FILE* pf;
		pf = fopen(strLocalPath.c_str(), "wb");
		if (pf == NULL)
		{
			perror(NULL);
			break;
		}

		curl_easy_setopt(pcurl, CURLOPT_SSL_VERIFYPEER, false); //�趨Ϊ����֤֤���HOST
		curl_easy_setopt(pcurl, CURLOPT_SSL_VERIFYHOST, false);

		curl_easy_setopt(pcurl, CURLOPT_URL, strUrl.c_str()); // ���ص�ַ
		curl_easy_setopt(pcurl, CURLOPT_CONNECTTIMEOUT, 5); // �������ӳ�ʱ����λ��
		curl_easy_setopt(pcurl, CURLOPT_WRITEDATA, pf);
		curl_easy_setopt(pcurl, CURLOPT_WRITEFUNCTION, f_wtfile);
		curl_easy_setopt(pcurl, CURLOPT_VERBOSE, 1L);
		n_eid = curl_easy_perform(pcurl);
		fclose(pf);

		if (CURLE_OK != n_eid)
		{
			curl_easy_cleanup(pcurl);
			::DeleteFile(stringToLPCWSTR(strLocalPath));
			break;
		}

		long n_code = 0;
		n_eid = curl_easy_getinfo(pcurl, CURLINFO_RESPONSE_CODE, &n_code);
		if (n_eid != CURLE_OK || n_code != 200)
		{
			curl_easy_cleanup(pcurl);
			DeleteFile(stringToLPCWSTR(strLocalPath));
			break;
		}

		curl_easy_cleanup(pcurl);
		b_ret = true;
	} while (0);
	return b_ret;
}

int main()
{
	/*string getUrl = "http://pk.uuuwin.com/ad/channel/sd";
	string getstrRespose = "";
	f_get(getUrl, getstrRespose);
	string strgetGBK = f_utf8togbk(getstrRespose.c_str());
	cout << strgetGBK << endl;*/

	/*string postUrl = "http://pk.uuuwin.com/static/ad/bar/299442";
	string postStrRespose = "";
	f_post(postUrl,"",postStrRespose);
	string strpostGBK = f_utf8togbk(postStrRespose.c_str());
	cout << strpostGBK << endl;*/

	string downloadUrl = "https://ws3.sinaimg.cn/large/005BYqpgly1g0nbuom4hhj303202sq2u.jpg";
	string strLocalPath = "E:\\C++\\libcurlTest\\libcurlTest\\Debug\\202.png";
	f_download(downloadUrl,strLocalPath);
	system("pause");
}
