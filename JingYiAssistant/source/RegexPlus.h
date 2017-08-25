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
	// ʹ��������ʽƥ���ַ���
	bool CreateMatch(string sRegex, string sMatchedString);
	// ��ȡƥ������
	size_t GetMatchedCount();
	// ��ȡƥ���ı�
	string GetMatchedString(size_t nMatchedIndex);
	// ��ȡ��ƥ������
	size_t GetChildMatchedCount();
	// ��ȡ��ƥ��ʽ�ı�
	string GetChildMatchedString(size_t nMatchedIndex, size_t nChildMatchedIndex);
	// ʹ��������ʽ�滻�ַ���,�����滻������ַ���
	static string MatchReplace(string sWantReplaceString, string pattern, string sReplaceString);
};

