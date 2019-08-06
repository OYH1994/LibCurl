#include <stdio.h>
#include<iostream>
#include<string>
#define CURL_STATICLIB   //静态链接
#include "curl.h"

#pragma comment(lib, "libcurl.lib")

using namespace std;
std::string f_utf8togbk(const char *szUtf8)
{
	std::string result;
	wchar_t *strSrc;
	char *szRes;

	//获得临时变量的大小
	int i = ::MultiByteToWideChar(CP_UTF8, 0, szUtf8, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	::MultiByteToWideChar(CP_UTF8, 0, szUtf8, -1, strSrc, i);

	//获得临时变量的大小
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
	curl_easy_setopt(curl,CURLOPT_HTTPHEADER,plst_header);			     //更改协议头		
	curl_easy_setopt(curl,CURLOPT_URL,strUrl.c_str());					 //设置访问的URL
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,f_onwritedata);			 //写数据的方法	
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,(void*)&strRespose);		 //CURLOPT_WRITEFUNCTION中的数据来源 用于将数据输出到文件里
	curl_easy_setopt(curl,CURLOPT_NOSIGNAL,1);							 //设置屏蔽其他信号
	curl_easy_setopt(curl,CURLOPT_CONNECTTIMEOUT,7);					 //设置连接超时 单位秒	
	curl_easy_setopt(curl,CURLOPT_TIMEOUT,7);						     //设置连接等待时间 单位秒
	res = curl_easy_perform(curl);										 //执行
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

int main()
{
	string getUrl = "http://pk.uuuwin.com/ad/channel/sd";
	string getstrRespose = "";
	f_get(getUrl, getstrRespose);
	string strgetGBK = f_utf8togbk(getstrRespose.c_str());
	cout << strgetGBK << endl;

	/*string postUrl = "http://pk.uuuwin.com/static/ad/bar/299442";
	string postStrRespose = "";
	f_post(postUrl,"",postStrRespose);
	string strpostGBK = f_utf8togbk(postStrRespose.c_str());
	cout << strpostGBK << endl;*/

	system("pause");
}
