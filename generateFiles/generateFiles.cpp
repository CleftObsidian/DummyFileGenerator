#include "tinyxml2.h"
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <filesystem>
#include <iostream>
#include <string>

using namespace tinyxml2;
namespace fs = std::filesystem;

void GenerateFilesWithXML(void);
void DisplayError(LPCTSTR lpszFunction);

int main(void)
{
	GenerateFilesWithXML();

	return 0;
}

void GenerateFilesWithXML(void)
{
	char flag = '\0';
	std::cout << "Generate files with XML data file\n";
	std::cout << "Remove existing files before generating files. [Y/n]: ";
	std::cin >> flag;
	switch (flag)
	{
	case 'Y':
	case 'y':
		break;
	case 'N':
	case 'n':
		return;
	default:
		return;
	}

	fs::path infoPath = fs::absolute(fs::current_path().parent_path().append("output/filesInfo/FilesInformation.xml"));

	fs::path outPath = fs::absolute(fs::current_path().parent_path().append("output/generatedFiles"));
	std::cout << "Removing existing files..." << std::endl;
	fs::remove_all(outPath);
	std::cout << "Complete" << std::endl;
	fs::create_directories(outPath);

	tinyxml2::XMLDocument doc;
	doc.LoadFile(infoPath.string().c_str());

	XMLElement* root = doc.RootElement();
	XMLElement* fileInfo = root->FirstChildElement("FileInfo");
	for (XMLElement* ele = fileInfo; ele != nullptr; ele = ele->NextSiblingElement("FileInfo"))
	{
		size_t idx = ele->UnsignedAttribute("idx");

		XMLElement* fileNameEle = ele->FirstChildElement("FileName");
		const char* fileName = fileNameEle->GetText();

		XMLElement* fileSizeEle = fileNameEle->NextSiblingElement("FileSize");
		double fileSize = fileSizeEle->DoubleText();

		XMLElement* processingTimeEle = fileSizeEle->NextSiblingElement("ProcessingTime");
		double processingTime = processingTimeEle->DoubleText();

		XMLElement* dependenciesEle = processingTimeEle->NextSiblingElement("Dependencies");
		XMLElement* dependencyEle = dependenciesEle->FirstChildElement("Dependency");
		std::vector<size_t> dependencies;
		for (XMLElement* eleDep = dependencyEle; eleDep != nullptr; eleDep = eleDep->NextSiblingElement("Dependency"))
		{
			dependencies.push_back(eleDep->UnsignedText());
		}

		// print each file's information to console
		//{
		//	std::cout << "FileName: " << fileName << " FileSize: " << fileSize << " KB\n";
		//	std::cout << "Dependencies: ";
		//	for (size_t i = 0; i < dependencies.size(); ++i)
		//	{
		//		std::cout << dependencies[i] << ' ';
		//	}
		//	std::cout << std::endl;
		//}
		
		// make buffer to write with informations
		std::string infoString;
		infoString.append(std::to_string(processingTime));
		infoString.append("\n");
		for (size_t i = 0; i < dependencies.size(); ++i)
		{
			infoString.append(std::to_string(dependencies[i]));
			infoString.append(" ");
		}
		infoString.append("\n");
		size_t bufferSize = static_cast<size_t>(fileSize * 1024);
		char* buffer = new char[bufferSize];
		for (size_t i = 0; i < infoString.size(); ++i)
		{
			buffer[i] = infoString[i];
		}
		buffer[bufferSize - 1] = '\0';

		fs::path outFilePath = outPath;
		//if (false == fs::exists(outPath))
		//{
		//	fs::create_directories(outPath);
		//}
		outFilePath.append(fileName);
		HANDLE hFile;
		DWORD dwBytesToWrite = static_cast<DWORD>(bufferSize);
		DWORD dwBytesWritten = 0;
		BOOL bErrorFlag = FALSE;

		hFile = CreateFile(outFilePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			std::cout << "CreateFile error" << std::endl;
			DisplayError(TEXT("CreateFile"));
			return;
		}

		bErrorFlag = WriteFile(hFile, buffer, dwBytesToWrite, &dwBytesWritten, NULL);
		if (FALSE == bErrorFlag)
		{
			std::cout << "WriteFile error" << std::endl;
			return;
		}
		else
		{
			if (dwBytesWritten != dwBytesToWrite)
			{
				// This is an error because a synchronous write that results in
				// success (WriteFile returns TRUE) should write all data as
				// requested. This would not necessarily be the case for
				// asynchronous writes.
				std::cout << "Error: dwBytesWritten != dwBytesToWrite\n";
			}
			else
			{
				std::cout << "FileName: " << fileName << " wrote " << dwBytesWritten << " bytes successfully.\n\n";
			}
		}

		CloseHandle(hFile);
		delete[] buffer;
	}
}

void DisplayError(LPCTSTR lpszFunction)
// Routine Description:
// Retrieve and output the system error message for the last-error code
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL);

	lpDisplayBuf =
		(LPVOID)LocalAlloc(LMEM_ZEROINIT,
			(lstrlen((LPCTSTR)lpMsgBuf)
				+ lstrlen((LPCTSTR)lpszFunction)
				+ 40) // account for format string
			* sizeof(TCHAR));

	if (FAILED(StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error code %d as follows:\n%s"),
		lpszFunction,
		dw,
		lpMsgBuf)))
	{
		printf("FATAL ERROR: Unable to output error code.\n");
	}

	_tprintf(TEXT("ERROR: %s\n"), (LPCTSTR)lpDisplayBuf);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}