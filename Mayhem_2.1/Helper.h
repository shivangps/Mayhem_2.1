#pragma once
#include "CommonHeaders.h"
#include <comdef.h>

// This header file contains the convinient helper functions.

static std::wstring StringToWide(std::string str)
{
	std::wstring wide_str(str.begin(), str.end());
	return wide_str;
}

static void ErrorLog(std::string message)
{
	std::string error_message = "Error: " + message;
	MessageBoxA(NULL, error_message.c_str(), "Error: ", MB_ICONERROR);
}

static void ErrorLog(HRESULT hr, std::string message)
{
	_com_error error(hr);
	std::wstring error_message = L"Error: " + StringToWide(message) + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, error_message.c_str(), L"Error: ", MB_ICONERROR);
}

static void CheckError(HRESULT hr, std::string message)
{
	if (FAILED(hr))
	{
		ErrorLog(hr, message);
		exit(-1);
	}
}

// Function to remove the directory as well as the file extension from the file name.
static std::string GetFileNameFromDirectory(std::string fileDirectory)
{
	// Removing directory.
	const size_t last_slash_idx = fileDirectory.find_last_of("/");
	if (std::string::npos != last_slash_idx)
	{
		fileDirectory.erase(0, last_slash_idx + 1);
	}

	// Removing extension.
	const size_t period_idx = fileDirectory.rfind('.');
	if (std::string::npos != period_idx)
	{
		fileDirectory.erase(period_idx);
	}

	return fileDirectory;
}

// Function to remove just the file directory and get the file name.
static std::string GetFileNameWithExtFromDirectory(std::string fileDirectory)
{
	// Removing directory.
	const size_t last_slash_idx = fileDirectory.find_last_of("/");
	if (std::string::npos != last_slash_idx)
	{
		fileDirectory.erase(0, last_slash_idx + 1);
	}

	return fileDirectory;
}