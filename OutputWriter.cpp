#include "src\OutputWriter.h"

VOID WINAPI FileWrittenCallback(DWORD dwErrorCode, DWORD dwBytesTransferred, LPOVERLAPPED lpOverlapped) {
	if (dwErrorCode != 0) {
		fprintf(stdout, "CompletionRoutine: Unable to write to file! Error: %u, AddrOverlapped: %p\n", dwErrorCode, lpOverlapped);
	}
	else {
		fprintf(stdout, "CompletionRoutine: Transferred: %u Bytes, AddrOverlapped: %p\n", dwBytesTransferred, lpOverlapped);
	}
}

OutputWriter::OutputWriter()
{
	this->hFile = CreateFile(_T("K:\\foo.txt"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (INVALID_HANDLE_VALUE == this->hFile) {
		fprintf(stdout, "Could not create file! Error %u\n", GetLastError());
		this->isValid = false;
		return;
	}

	this->oFile.Offset = 0xFFFFFFFF;
	this->oFile.OffsetHigh = 0xFFFFFFFF;

	/*this->tempText = _T("Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy\r\n");
	if (!WriteFileEx(this->hFile, this->tempText, this->tempText.GetLength() * 2, &this->oFile, (LPOVERLAPPED_COMPLETION_ROUTINE)this->FileWrittenCallback)) {
		fprintf(stdout, "Unable to write to file! Error %u\n", GetLastError());
	}*/
	this->isValid = true;
}


OutputWriter::~OutputWriter()
{
	CloseHandle(this->hFile);
}

