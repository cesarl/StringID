#pragma once

#include <stdint.h>
#include <windows.h>

struct FileFilter
{
	std::vector<std::string> _excludedPath;
	std::vector<std::string> _excludedDir;
	std::vector<std::string> _extensions;
	uint32_t                 _minimumWriteTime = 0;
};

struct FileInfo
{
	std::string _name;
	uint32_t    _attributes;
	uint32_t    _lastWriteTime;
	uint32_t    _size;
};

static void SearchFiles(
	std::string &path,
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
		info._name = path;
		info._attributes = data.dwFileAttributes;
		info._lastWriteTime = (uint64_t)data.ftLastWriteTime.dwLowDateTime | ((uint64_t)data.ftLastWriteTime.dwHighDateTime << 32);
		info._size = (uint64_t)data.nFileSizeLow | ((uint64_t)data.nFileSizeHigh << 32);

		//if (filter && filter->pass(info))
		//{
		results.push_back(std::move(info));
		//}
	} while (FindNextFile(find, &data) != 0);

	FindClose(find);
}