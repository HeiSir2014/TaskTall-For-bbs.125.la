#include "RegexPlus.h"
#include <sstream>
#include <iostream> 

CRegexPlus::CRegexPlus()
{
	m_MatchedColumnCount = 0;
}


CRegexPlus::~CRegexPlus()
{
	m_MatchedResult.clear();
}


// ʹ��������ʽƥ���ַ���
bool CRegexPlus::CreateMatch(string sRegex, string sMatchedString)
{
	if (sRegex.empty() || sMatchedString.empty())
	{
		return false;
	}
	
	m_MatchedColumnCount = 0;
	m_MatchedResult.clear();

	m_Regex = sRegex;

	for (sregex_iterator p(sMatchedString.cbegin(), sMatchedString.cend(), m_Regex), q; p != q; ++p)
	{
		vector<string> sMatchResult;
		sMatchResult.push_back(p->str());

		for (size_t child = 1; child < p->size(); child++)
		{
			string sIndex;
			stringstream stream;
			stream << "$" << child;
			stream >> sIndex;
			sMatchResult.push_back(p->format(sIndex));
		}
		m_MatchedResult.push_back(sMatchResult);
	}
	if (m_MatchedResult.empty())
	{
		return false;
	}

	m_MatchedColumnCount = m_MatchedResult[0].size() - 1 ; // ��ƥ���ı���ȥ1 ������ƥ���ı��ĸ���

	return true;
}


// ��ȡƥ������
size_t CRegexPlus::GetMatchedCount()
{
	return m_MatchedResult.size();
}


// ��ȡƥ���ı�
string CRegexPlus::GetMatchedString(size_t nMatchedIndex)
{
	if (nMatchedIndex >= m_MatchedResult.size() || m_MatchedResult[nMatchedIndex].empty())
	{
		return string();
	}
	return m_MatchedResult[nMatchedIndex][0];
}


// ��ȡ��ƥ������
size_t CRegexPlus::GetChildMatchedCount()
{
	return m_MatchedColumnCount;
}


// ��ȡ�ӱ��ʽ�ı� nMatchedIndex��nChildMatchedIndex �� 0 ��ʼ
string CRegexPlus::GetChildMatchedString(size_t nMatchedIndex, size_t nChildMatchedIndex)
{
	nChildMatchedIndex++;
	if (nMatchedIndex >= m_MatchedResult.size() || nChildMatchedIndex >= m_MatchedResult[nMatchedIndex].size())
	{
		return string();
	}
	return m_MatchedResult[nMatchedIndex][nChildMatchedIndex];
}


// ʹ��������ʽ�滻�ַ���,�����滻������ַ���
string CRegexPlus::MatchReplace(string sWantReplaceString, string pattern, string sReplaceString)
{
	if (sWantReplaceString.empty() ||  pattern.empty())
	{
		return sWantReplaceString;
	}
	regex regex_tmp(pattern);

	regex_tmp;

	return regex_replace(sWantReplaceString, regex_tmp, sReplaceString);
}
