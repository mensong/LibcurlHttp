#pragma once
#include <iostream>
#include <string>
#include <set>

class UrlCoding
{
public:
	static void UTF8ToGB2312(std::string &pOut, const char *pText, int pLen);//utf_8转为gb2312
	static void GB2312ToUTF8(std::string& pOut, const char *pText, int pLen);//gb2312 转utf_8
	static void UTF8ToGB2312(std::string &pOut, const std::string& sText);//utf_8转为gb2312
	static void GB2312ToUTF8(std::string& pOut, const std::string& sText);//gb2312 转utf_8

	static std::string UrlGB2312Encode(const char * str, const std::set<char>* escape = NULL);//urlgb2312编码
	static std::string UrlGB2312Decode(const std::string& str);								//urlgb2312解码
	static std::string UrlUTF8Encode(const char * str, const std::set<char>* escape = NULL);//urlutf8 编码
	static std::string UrlUTF8Decode(const std::string& str);								//urlutf8解码	

	static void Gb2312ToUnicode(wchar_t* pOut,	const char *gbBuffer);
	static void UTF8ToUnicode(wchar_t* pOut,	const char *pText);
	static void UnicodeToUTF8(char* pOut,		const wchar_t* pText);
	static void UnicodeToGB2312(char* pOut,		const wchar_t uData);
	static char CharToInt(char ch);
	static char StrToBin(const char *str);
};