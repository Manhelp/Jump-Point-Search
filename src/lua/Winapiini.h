#ifndef __WINAPIINI_H__
#define __WINAPIINI_H__

#pragma once
#include "Define.h"
#include "MyWindows.h"
//#include "dump/DumpErrorBase.h"



class CWinApiIni
{
public:

	char m_szFileName[MAX_PATH];
	char m_szOpSecName[MAX_PATH];
public:

	__inline CWinApiIni()
	{
	    GetModuleFileName(NULL, m_szFileName, MAX_PATH);
		//GetModuleFileNameA(g_hinstance, m_szFileName, MAX_PATH);
		char* p = strrchr(m_szFileName,'.');
		if(p){*p='_';p++;}
		strcat(m_szFileName,".ini");
	}


	__inline CWinApiIni(LPSTR ptzFileName)
	{
		strcpy_s(m_szFileName,MAX_PATH-1, ptzFileName);
	}


	__inline void SetOpSection(LPSTR ptzSectionName)
	{
		strcpy_s(m_szOpSecName,MAX_PATH-1, ptzSectionName);
	}

	__inline void SetOpFile(LPSTR ptzFileName)
	{
		strcpy_s(m_szFileName,MAX_PATH-1, ptzFileName);
	}

	__inline UINT ReadInt(LPSTR ptzSectionName, LPSTR ptzKeyName, INT iDefault = 0)
	{
		char  szintbuf[256]={0};
		char szCh[] = {""};
		int n=GetPrivateProfileStringA(ptzSectionName, ptzKeyName,szCh,szintbuf,256 , m_szFileName);
		if(n<=0)
		{
			return 0;
		}
		else
		{
			return atoi(szintbuf);
		}
	}


	__inline bool WriteInt(LPSTR ptzSectionName, LPSTR ptzKeyName, INT iValue = 0)
	{
		CHAR tzString[30];
		sprintf(tzString, "%d", iValue);
		return WritePrivateProfileStringA(ptzSectionName, ptzKeyName, tzString, m_szFileName);
	}


	__inline uint ReadString(LPSTR ptzSectionName, LPSTR ptzKeyName, LPSTR ptzReturnedString, uint dwSize, LPSTR ptzDefault,bool bforceupdate=false)
	{
			return GetPrivateProfileStringA(ptzSectionName, ptzKeyName, ptzDefault, ptzReturnedString, dwSize, m_szFileName,bforceupdate);
	}


	__inline bool WriteString(LPSTR ptzSectionName, LPSTR ptzKeyName,
		LPSTR ptzString)
	{
		return WritePrivateProfileStringA(ptzSectionName, ptzKeyName, ptzString,m_szFileName);
	}


	__inline int64 ReadInt64(LPSTR ptzSectionName, LPSTR ptzKeyName, int64 iDefault)
	{
		CHAR tzString[64]={0};
		char szCh[] = {""};
		ReadString(ptzSectionName,ptzKeyName,tzString,sizeof(tzString)-1,szCh);
		return _strtoi64(tzString,NULL,10);
	}


	__inline bool WriteInt64(LPSTR ptzSectionName, LPSTR ptzKeyName, int64 iValue)
	{
		CHAR tzString[64]={0};
		sprintf_s(tzString,sizeof(tzString)-1,"%lld",iValue);
		return WriteString(ptzSectionName, ptzKeyName, tzString);
	}


	__inline bool ReadStruct(LPSTR ptzSectionName, LPSTR ptzKeyName, PVOID pvStruct, UINT uSize)
	{
		return 1;//GetPrivateProfileStructA(ptzSectionName, ptzKeyName, pvStruct, uSize, m_szFileName);
	}


	__inline bool WriteStruct(LPSTR ptzSectionName, LPSTR ptzKeyName, PVOID pvStruct, UINT uSize)
	{
		return 1;//WritePrivateProfileStructA(ptzSectionName, ptzKeyName, pvStruct, uSize, m_szFileName);
	}


	__inline uint ReadSection(LPSTR ptzSectionName, LPSTR ptzReturnBuffer, uint dwSize)
	{
		return 1;//GetPrivateProfileSectionA(ptzSectionName, ptzReturnBuffer, dwSize, m_szFileName);
	}


	__inline uint WriteSection(LPSTR ptzSectionName, LPSTR ptzString)
	{
		return 1;//WritePrivateProfileSectionA(ptzSectionName, ptzString, m_szFileName);
	}


	__inline uint ReadSectionNames(LPSTR ptzReturnBuffer, uint dwSize)
	{
		return 1;//GetPrivateProfileSectionNamesA(ptzReturnBuffer, dwSize, m_szFileName);
	}






	__inline UINT ReadInt(LPSTR ptzKeyName, INT iDefault = 0)
	{
		return ReadInt(m_szOpSecName,ptzKeyName,iDefault);
		//return 1;//GetPrivateProfileIntA(m_szOpSecName, ptzKeyName, iDefault, m_szFileName);
	}


	__inline bool WriteInt(LPSTR ptzKeyName, INT iValue = 0)
	{
		CHAR tzString[30];
		sprintf(tzString, "%d", iValue);
		return WritePrivateProfileStringA(m_szOpSecName, ptzKeyName, tzString, m_szFileName);
	}


	__inline uint ReadString(LPSTR ptzKeyName, LPSTR ptzReturnedString, uint dwSize, LPSTR ptzDefault)
	{
		return GetPrivateProfileStringA(m_szOpSecName, ptzKeyName, ptzDefault, ptzReturnedString, dwSize, m_szFileName);
	}


	__inline bool WriteString(LPSTR ptzKeyName, LPSTR ptzString)
	{
		return WritePrivateProfileStringA(m_szOpSecName, ptzKeyName, ptzString, m_szFileName);
	}


	__inline int64 ReadInt64(LPSTR ptzKeyName, int64 iDefault)
	{
		CHAR tzString[64]={0};
		char szCh[] = {""};
		ReadString(ptzKeyName,tzString,sizeof(tzString)-1,szCh);
		return _strtoi64(tzString,NULL,10);
	}


	__inline bool WriteInt64(LPSTR ptzKeyName, int64 iValue)
	{
		CHAR tzString[64]={0};
		sprintf_s(tzString,sizeof(tzString)-1,"%lld",iValue);
		return WriteString(ptzKeyName, tzString);
	}


	__inline bool ReadStruct(LPSTR ptzKeyName, PVOID pvStruct, UINT uSize)
	{
		return 1;//GetPrivateProfileStructA(m_szOpSecName, ptzKeyName, pvStruct, uSize, m_szFileName);
	}


	__inline bool WriteStruct(LPSTR ptzKeyName, PVOID pvStruct, UINT uSize)
	{
		return 1;//WritePrivateProfileStructA(m_szOpSecName, ptzKeyName, pvStruct, uSize, m_szFileName);
	}


	__inline uint ReadSection(LPSTR ptzReturnBuffer, uint dwSize)
	{
		return 1;//GetPrivateProfileSectionA(m_szOpSecName, ptzReturnBuffer, dwSize, m_szFileName);
	}


	__inline uint WriteSection(LPSTR ptzString)
	{
		return 1;//WritePrivateProfileSectionA(m_szOpSecName, ptzString, m_szFileName);
	}
};
#endif//end
