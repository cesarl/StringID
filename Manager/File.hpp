#pragma once

#include <algorithm>
#include <stdint.h>
#include <windows.h>

static const char *GetCurrentDirectory(void)
{
	static char buf[1024];
	static wchar_t wbuf[1024];
	_wgetcwd(wbuf, sizeof(wbuf) / sizeof(wchar_t));
	wbuf[sizeof(wbuf) / sizeof(wchar_t) - 1] = '\0';
	WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, buf, sizeof(buf), nullptr, nullptr);
	if (buf[0]) buf[0] = toupper(buf[0]);
	buf[sizeof(buf) - 1] = '\0';
	for (char *s = buf; *s != '\0'; s++)
	{
		if (*s == '\\')
		{
			*s = '/';
		}
	}
	std::strcat(buf, "/");
	return buf;
}

static std::string CleanPath(const std::string &path)
{
	auto r = path;
	std::replace(r.begin(), r.end(), '\\', '/');
	return r;
}

static std::string GetExtension(const std::string &path)
{
	std::string::size_type		pos;
	pos = path.find_last_of(".");
	if (pos != std::string::npos)
		return path.substr(pos + 1, std::string::npos);
	else
		return "";
}

static std::string GetName(const std::string &path)
{
	auto copy = path;
	std::string::size_type		pos;
	pos = copy.find_last_of("/");
	if (pos != std::string::npos)
		return copy.substr(pos + 1, std::string::npos);
	else
		return path;
}

static std::string DirectPath(const std::string &path)
{
	auto copy = path;
	std::string::size_type pos;
	pos = copy.find_last_of("/../");
	while (pos != std::string::npos)
	{

		pos = copy.find_last_of("/../");
	}
	if (pos != std::string::npos)
		return copy.substr(pos + 1, std::string::npos);

}

static std::string MakePathAbsolute(const std::string &path)
{
	std::string value = path, out(MAX_PATH, '\0');
	DWORD length = GetFullPathName(value.c_str(), MAX_PATH, &out[0], NULL);
	if (length == 0)
		return "";
	return out.substr(0, length);
}

struct FileInfo
{
	std::string absPath;
	std::string relPath;
	uint32_t    attributes;
	uint32_t    lastWriteTime;
	uint32_t    size;
};

struct FileFilter
{
	std::vector<std::string> _excludedPath;
	std::vector<std::string> _excludedDir;
	std::vector<std::string> _extensions;
	uint32_t                 _minimumWriteTime = 0;

	bool filePass(const FileInfo &info) const
	{
		if (info.lastWriteTime <= _minimumWriteTime)
			return false;
		if (info.absPath.size() == 0)
			return false;
		auto extension = GetExtension(info.absPath);
		if (extension.size() == 0 && _extensions.size())
			return false;
		if (extension.size() && _extensions.size())
		{
			bool extensionFound = false;
			for (auto &e : _extensions)
			{
				if (e == extension)
				{
					extensionFound = true;
					break;
				}
			}
			if (!extensionFound)
				return false;
		}
		return true;
	}
};

static bool GetFileInfo(const std::string &path, FileInfo &info)
{
	HANDLE hFile;

	hFile = CreateFile(path.data(), // file to open
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
		NULL);                 // no attr. template

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	BY_HANDLE_FILE_INFORMATION i;

	if (!GetFileInformationByHandle(hFile, &i))
	{
		CloseHandle(hFile);
		return false;
	}
	CloseHandle(hFile);

	info.absPath = CleanPath(path);
	info.attributes = i.dwFileAttributes;
	info.lastWriteTime = (uint64_t)i.ftLastWriteTime.dwLowDateTime | ((uint64_t)i.ftLastWriteTime.dwHighDateTime << 32);
	info.size = (uint64_t)i.nFileSizeLow | ((uint64_t)i.nFileSizeHigh << 32);

	return true;
}

static void SearchFiles(
	std::string path,
	const FileFilter *filter,
	std::vector<FileInfo> &results)
{
	const size_t baseLength = path.length();

	path += '*';

	/*
	Folders
	*/
	WIN32_FIND_DATA data;

	HANDLE find = FindFirstFileEx(
		path.data()
		, FindExInfoBasic
		, &data
		, FindExSearchLimitToDirectories
		, nullptr
		, 0);

	if (find == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do
	{
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (data.cFileName[0] == '.' &&
				((data.cFileName[1] == '.') || !data.cFileName[1]))
			{
				continue;
			}

			path.resize(baseLength);
			path += data.cFileName;
			path += '/';
			SearchFiles(path, filter, results);
		}
	} while (FindNextFile(find, &data) != 0);
	FindClose(find);

	/*
	Files
	*/
	path.resize(baseLength);
	path += '*';
	find = FindFirstFileEx(
		path.c_str()
		, FindExInfoBasic
		, &data
		, FindExSearchNameMatch
		, nullptr
		, 0);

	if (find == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do
	{
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}

		path.resize(baseLength);
		path += data.cFileName;

		FileInfo info;
		info.absPath = CleanPath(path);
		info.attributes = data.dwFileAttributes;
		info.lastWriteTime = (uint64_t)data.ftLastWriteTime.dwLowDateTime | ((uint64_t)data.ftLastWriteTime.dwHighDateTime << 32);
		info.size = (uint64_t)data.nFileSizeLow | ((uint64_t)data.nFileSizeHigh << 32);

		if (filter && filter->filePass(info))
		{
			results.push_back(std::move(info));
		}
	} while (FindNextFile(find, &data) != 0);

	FindClose(find);
}