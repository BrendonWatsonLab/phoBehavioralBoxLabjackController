#include "WindowsHelpers.h"
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#ifndef UNICODE  
typedef std::string String;
#else
typedef std::wstring String;
#endif

WindowsHelpers::WindowsHelpers()
{
}

std::string WindowsHelpers::getHostName()
{
	std::string outputString;
	TCHAR buffer[256] = TEXT("");
	//"NetBIOS": "RDE20007" : 0
	//"DNS hostname": "RDE20007" : 1
	//"DNS domain": "umhs.med.umich.edu" : 2
	//"DNS fully-qualified": "RDE20007.umhs.med.umich.edu" : 3
	//"Physical NetBIOS": "RDE20007" : 4
	//"Physical DNS hostname": "RDE20007" : 5
	//"Physical DNS domain": "umhs.med.umich.edu" : 6
	//"Physical DNS fully-qualified": "RDE20007.umhs.med.umich.edu" : 7
	DWORD dwSize = sizeof(buffer);
	// Get "DNS hostname" (1)
	if (!GetComputerNameEx((COMPUTER_NAME_FORMAT)1, buffer, &dwSize))
	{
		_tprintf(TEXT("GetComputerNameEx failed (&#37;d)\n"), GetLastError());
		outputString = std::string();
	}
	else
	{
		// From https://stackoverflow.com/questions/6291458/how-to-convert-a-tchar-array-to-stdstring (Aemmel)
	#ifndef UNICODE
		outputString = buffer;
	#else
		std::wstring wStr = buffer;
		outputString = std::string(wStr.begin(), wStr.end());
	#endif
	}
	dwSize = sizeof(buffer);
	ZeroMemory(buffer, dwSize);
	return outputString;
}


//std::string WindowsHelpers::getExePath()
//{
//#ifdef _WIN32
//	char result[MAX_PATH];
//	return std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
//#else
//	char result[PATH_MAX];
//	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
//	return std::string(result, (count > 0) ? count : 0);
//#endif
//}
