#include <Windows.h>
#include <stdio.h>
#include <strsafe.h>

BOOL ToCppCode(WCHAR* wszFile, WCHAR* wszName)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	hFile = CreateFileW(wszFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile : %d\r\n", GetLastError());
		return 0;
	}

	WCHAR wszOutPath[MAX_PATH] = { 0 };
	BYTE* pData = NULL;
	DWORD dwSize = 0;
	DWORD dwVal = 0;
	DWORD i = 0;
	DWORD dwErr = 0;
	

	dwSize = GetFileSize(hFile, NULL);
	pData = (BYTE*)LocalAlloc(LPTR, dwSize);
	dwErr = GetLastError();

	ReadFile(hFile, pData, dwSize, &dwVal, NULL);
	dwErr = GetLastError();
	if (dwSize != dwVal)
	{
		CloseHandle(hFile);
		return 0;
	}

	StringCchPrintfW(wszOutPath, MAX_PATH, L"%s.h", wszName);
	HANDLE hOut = INVALID_HANDLE_VALUE;
	hOut = CreateFileW(wszOutPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	SetFilePointer(hOut, 0, NULL, FILE_END);

	char szHeader[256] = { 0 };
	char szName[MAX_PATH] = { 0 };
	int nLength = 0;


	WideCharToMultiByte(CP_ACP, 0, wszName, -1, szName, MAX_PATH, NULL, NULL);
	sprintf_s(szHeader, "unsigned char g_sz%s[%d] = {\n", szName, dwSize);
	WriteFile(hOut, szHeader, lstrlenA(szHeader), &dwVal, NULL);

	char szText[64] = { 0 };
	for (i = 0; i < dwSize - 1; i++)
	{
		if ((i + 1) % 16 == 0)
		{
			sprintf_s(szText, "0x%02X, \n", pData[i]);
		}
		else
		{
			sprintf_s(szText, "0x%02X,", pData[i]);
		}

		WriteFile(hOut, szText, lstrlenA(szText), &dwVal, NULL);
	}

	memset(szHeader, 0, 256);
	sprintf_s(szHeader, "0x%02X};\r\n\r\n", pData[dwSize - 1]);
	WriteFile(hOut, szHeader, lstrlenA(szHeader), &dwVal, NULL);


	CloseHandle(hOut);
	CloseHandle(hFile);



	return 0;
}

int main(int argc, char ** argv)
{
	if (argc != 2)
	{
		printf("Usage : BinToCppCode.exe dllname\r\n");
		return 0;
	}

	WCHAR wszPath[MAX_PATH] = { 0 };
	WCHAR wszName[MAX_PATH] = { 0 };
	WCHAR wszTmp[MAX_PATH] = { 0 };
	WCHAR* pwszPos = NULL;

	MultiByteToWideChar(CP_ACP, 0, argv[1], -1, wszPath, MAX_PATH);
	StringCchCopyW(wszTmp, MAX_PATH, wszPath);
	pwszPos = wcsrchr(wszTmp, L'\\');
	if (pwszPos)
	{
		StringCchCopyW(wszName, MAX_PATH, pwszPos + 1);
	}
	else
	{
		StringCchCopyW(wszName, MAX_PATH, wszTmp);
	}

	pwszPos = NULL;
	pwszPos = wcschr(wszName, L'.');
	if (pwszPos)
	{
		*pwszPos = L'\0';
	}

	ToCppCode(wszPath, wszName);

	return 1;
}

