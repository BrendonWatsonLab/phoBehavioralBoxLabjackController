#pragma once
#include <iostream>
#include <Windows.h>
#include <atlstr.h>

class OutputWriter
{
public:
	bool isValid = false;

	OutputWriter();
	~OutputWriter();

private:
	HANDLE hFile;
	OVERLAPPED oFile;
	CString tempText;

	VOID WINAPI FileWrittenCallback(DWORD dwErrorCode, DWORD dwBytesTransferred, LPOVERLAPPED lpOverlapped);

};

