#pragma once

#include <vector>
#include <string>

struct Project
{
public:
	Project();

	struct FileCache
	{
		int               lastWriteTime;
		std::vector<char> save;
	};

	const std::string &getName() const { return _name; }

private:
	std::string _name;


	std::vector <FileCache> _rtCache;
};