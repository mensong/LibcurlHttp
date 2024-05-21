#pragma once
#include <iostream>
#include <string>
#include <set>

class UrlCoding
{
public:
	static void UTF8ToGB2312(std::string& out, const std::string& sText);//utf_8תΪgb2312
	static void GB2312ToUTF8(std::string& out, const std::string& sText);//gb2312 תutf_8

	static std::string UrlGB2312Encode(const std::string& str, const std::set<char>* escape = NULL);//urlgb2312����
	static std::string UrlGB2312Decode(const std::string& str);								//urlgb2312����
	static std::string UrlUTF8Encode(const std::string& str, const std::set<char>* escape = NULL);//urlutf8 ����
	static std::string UrlUTF8Decode(const std::string& str);								//urlutf8����	

	static char CharToInt(char ch);
	static char StrToBin(const char *str);
};