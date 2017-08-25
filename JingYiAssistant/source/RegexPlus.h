#pragma once
#include <string>
#include <regex>
#include <vector>
using namespace std;

class CRegexPlus
{
private:
	vector<vector<string>> m_MatchedResult;
	size_t m_MatchedColumnCount;
	regex m_Regex;
public:
	CRegexPlus();
	~CRegexPlus();
	// 使用正则表达式匹配字符串
	bool CreateMatch(string sRegex, string sMatchedString);
	// 获取匹配数量
	size_t GetMatchedCount();
	// 获取匹配文本
	string GetMatchedString(size_t nMatchedIndex);
	// 获取子匹配数量
	size_t GetChildMatchedCount();
	// 获取子匹配式文本
	string GetChildMatchedString(size_t nMatchedIndex, size_t nChildMatchedIndex);
	// 使用正则表达式替换字符串,返回替换后的新字符串
	static string MatchReplace(string sWantReplaceString, string pattern, string sReplaceString);
};

