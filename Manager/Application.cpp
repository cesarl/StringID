#include "Application.hpp"

#include "StringIDImpl.hpp"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>

#include <regex>

#include "File.hpp"

#include <stdio.h>
#include <windows.h>
#include <algorithm>

static const char* g_help =
"-help                     Display this help and do nothing\n"
"-project                  Name of the project (default=\"StringIDProj\"\n"
"-gui                      Launch GUI application\n"
"-sources                  List folders to search (=\"C:/Folder1;../Folder2\")\n"
"-destination              Destination root folder where to copy results.\n"
"                          If empty, modifications will be in place\n"
"-excludedFolders          List folders to exclude (=\"C:/Folder1;../Folder2\")\n"
"-excludedSources          List sources to exclude (=\"path1;path2\")\n"
"-extensions               List extensions to parse (default=\".cpp;.h;.hh;.inl\")\n"
"-clean                    Will not use cache infos, will re-parse all files\n"
"-verbose                  Show detailed diagnostic information for debugging\n"
"-saveForUndo              Will keep a save of modified files in project's file\n"
"-undo                     Used in association with -project will undo last generation\n"
"-summary                  Display a detailed summary\n";

Application::Application()
	:
	_projectName("StringIDProj"),
	_guiEnabled(false),
	_destination(""),
	_clean(false),
	_verbose(false),
	_saveforundo(false),
	_undo(false),
	_displaySummary(false),
	_inPlace(false)
{
}

Application::~Application()
{
	_exit = true;
	_condition.notify_all();
	for (auto it = _threads.begin(); it != _threads.end(); ++it)
	{
		it->join();
	}
}

bool Application::init(int argc, char *argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];

		if (arg.empty())
			continue;

		if (arg[0] == '-')
		{
			if (arg == "-help")
			{
				std::cout << g_help << std::endl;
				continue;
			}
			if (arg == "-project")
			{
				int projectPathIndex = i + 1;
				if (projectPathIndex >= argc)
				{
					//ERROR
					std::cerr << "Project path missing\n";
					return false;
				}
				_projectName = argv[projectPathIndex];
				++i;
				continue;
			}
			if (arg == "-gui")
			{
				_guiEnabled = true;
				continue;
			}
			if (arg == "-sources")
			{
				int sourcesIndex = i + 1;
				if (sourcesIndex >= argc)
				{
					std::cerr << "Sources missing\n";
					return false;
					//ERROR
				}
				
				std::string source = argv[sourcesIndex];
				while (source.empty() == false && source[0] != '-')
				{
					_sources.push_back(CleanPath(MakePathAbsolute(source)) + "/");
					++sourcesIndex;
					source = "";
					if (sourcesIndex < argc)
					{
						source = argv[sourcesIndex];
					}
				}
				if (_sources.empty())
				{
					std::cerr << "Sources missing\n";
					return false;
					//ERROR
				}
				i = sourcesIndex - 1;
				continue;
			}
			if (arg == "-destination")
			{
				int destIndex = i + 1;
				if (destIndex >= argc)
				{
					std::cerr << "Destination missing\n";
					return false;
					//ERROR
				}
				arg = argv[destIndex];
				if (arg.empty() || arg[0] == '-')
				{
					std::cerr << "Destination missing\n";
					return false;
					//ERROR
				}
				_destination = CleanPath(MakePathAbsolute(arg));
				++i;
				continue;
			}
			if (arg == "-excludedFolders")
			{
				int excludedFoldersIndex = i + 1;
				if (excludedFoldersIndex >= argc)
				{
					std::cerr << "Excluded folders missing\n";
					return false;
					//ERROR
				}

				std::string source = argv[excludedFoldersIndex];
				while (source.empty() == false && source[0] != '-')
				{
					_excludedFolders.push_back(CleanPath(MakePathAbsolute(source)));
					++excludedFoldersIndex;
					source = "";
					if (excludedFoldersIndex < argc)
					{
						source = argv[excludedFoldersIndex];
					}
				}
				if (_excludedFolders.empty())
				{
					std::cerr << "Excluded folders missing\n";
					return false;
					//ERROR
				}
				i = excludedFoldersIndex - 1;
				continue;
			}
			if (arg == "-excludedSources")
			{
				int excludedSourcesIndex = i + 1;
				if (excludedSourcesIndex >= argc)
				{
					std::cerr << "Excluded sources missing\n";
					return false;
					//ERROR
				}

				std::string source = argv[excludedSourcesIndex];
				while (source.empty() == false && source[0] != '-')
				{
					_excludedSources.push_back(CleanPath(MakePathAbsolute(source)));
					++excludedSourcesIndex;
					source = "";
					if (excludedSourcesIndex < argc)
					{
						source = argv[excludedSourcesIndex];
					}
				}
				if (_excludedSources.empty())
				{
					std::cerr << "Excluded sources missing\n";
					return false;
					//ERROR
				}
				i = excludedSourcesIndex - 1;
				continue;
			}
			if (arg == "-extensions")
			{
				int extIndex = i + 1;
				if (extIndex >= argc)
				{
					std::cerr << "Extensions missing\n";
					return false;
					//ERROR
				}

				std::string source = argv[extIndex];
				while (source.empty() == false && source[0] != '-')
				{
					_extensions.push_back(source);
					++extIndex;
					source = "";
					if (extIndex < argc)
					{
						source = argv[extIndex];
					}
				}
				if (_extensions.empty())
				{
					std::cerr << "Extensions missing\n";
					return false;
					//ERROR
				}
				i = extIndex - 1;
				continue;
			}
			if (arg == "-clean")
			{
				_clean = true;
				continue;
			}
			if (arg == "-verbose")
			{
				_verbose = true;
				continue;
			}
			if (arg == "-saveForUndo")
			{
				_saveforundo = true;
				continue;
			}
			if (arg == "-undo")
			{
				_undo = true;
				continue;
			}
			if (arg == "-summary")
			{
				_displaySummary = true;
				continue;
			}
		}
	}

	if (_extensions.empty())
	{
		_extensions.push_back("h");
		_extensions.push_back("cpp");
		_extensions.push_back("hpp");
		_extensions.push_back("inl");
	}
	if (_sources.empty())
	{
		std::cerr << "Sources missing\n";
		return false;
		//ERROR
	}

	if (_destination.empty())
	{
		_inPlace = true;
	}

	_currentDirectory = GetCurrentDirectory();

	_exit = false;
	unsigned int maxThread = std::thread::hardware_concurrency();
	if (maxThread > 1)
		maxThread -= 1;
	for (unsigned int i = 0; i < maxThread; ++i)
	{
		_threads.emplace_back([this]() {
			
			while (waitAndTreatFile())
			{ }
		});
	}
	return true;
}

template< typename T >
std::string IntToHex(T i)
{
	std::stringstream stream;
	stream << "0x"
		<< std::setfill('0') << std::setw(sizeof(T) * 2)
		<< std::hex << i;
	return stream.str();
}

bool Application::waitAndTreatFile()
{
	FileInfo file;
	{
		std::unique_lock<std::mutex> lock(this->_mutex);
		this->_condition.wait(lock, [this](){return this->_queue.size() || this->_exit; });
		if (_exit && _queue.empty())
			return false;
		file = this->_queue.front();
		this->_queue.pop();
	}
	this->treatFile(file);
	_fileCounter.fetch_sub(1);
	return true;
}

void Application::treatFile(const FileInfo &fileInfo)
{
	std::ifstream file(fileInfo.absPath.c_str());

	std::ofstream output(std::string(_destination + fileInfo.absPath + ".sid").c_str());

	std::string line;
	std::size_t counter = 0;
	
	/*
	1 : crap StringID("
	2 : str
	*/
	std::regex regStringOnly   ("(.*?\\bStringID\\s*[(]{1}\\s*[\"]{1})(.+?)[\"]{1}\\s*[)]{1}");
	/*
	1 : Crap StringID
	2 : str
	3 : , 
	4 : 0x123
	*/
	std::regex regStringAndHash("(.*?\\bStringID\\s*[(]{1}\\s*\")(.+?)(\"\\s*,\\s*)(0x[\\d|a-f]+|[\\d|a-f]+)\\s*[)]{1}");
	std::match_results<std::string::const_iterator> match;
	
	while (std::getline(file, line))
	{
		std::istringstream iss(line);

		line += "\n";

		if (line.find("StringID") != std::string::npos)
		{
			std::string lineCopy = line;
			line.clear();

			auto flags = std::regex_constants::match_default | std::regex_constants::format_first_only | std::regex_constants::format_no_copy;
			bool pass = false;

			// We search for already hashed strings like StringID("Literal", 0x123);
			// We check if the hash is up to date, if not we re-generate it
			// We also register the hash if not already in db cache to check for collisions
			while (std::regex_search(lineCopy, match, regStringAndHash, flags))
			{
				auto &str = match[2].str();
				auto &h = match[4].str();
				StringIDType id = strtoll(h.c_str(), nullptr, 16);
				StringID sid = StringID(str);

				if (sid.getId() != id)
				{
					std::string replacer = "$1$2\", ";
					replacer += IntToHex(sid.getId());
					replacer += ")";
					line += std::regex_replace(lineCopy, regStringAndHash, replacer, flags);
					pass = true;
				}
				else
				{
					line = lineCopy;
				}
				lineCopy = match.suffix().str();
			}
			if (pass)
			{
				line += lineCopy;
			}

			if (!line.empty())
				lineCopy = line;
			line.clear();
			pass = false;

			// We search for StringID("Literal");
			// And generate ID for it
			//while (std::regex_search(lineCopy, match, regStringOnly, flags))
			{
				std::string replacer = "$1$2\", ";
				auto &str = match[2].str();
				replacer += IntToHex(StringID(str).getId());
				replacer += ")";
				line += std::regex_replace(lineCopy, regStringOnly, replacer, flags);
				lineCopy = match.suffix().str();
				pass = true;
			}
			if (pass)
			{
				line += lineCopy;
			}

			if (line.empty())
			{
				line = lineCopy;
			}
		}
		output << line;
	}

}

void Application::run()
{

	if (_guiEnabled)
	{
		//_initGui();
	}

	std::vector<FileInfo> infos;
	FileFilter filter;
	filter._extensions = _extensions;
	filter._excludedPath = _excludedSources;
	filter._excludedDir = _excludedFolders;
	filter._minimumWriteTime = 0;
	for (auto &source : _sources)
	{
		SearchFiles(/*"D:\Epic Games/"*/ source /*"../Tests/"*/, &filter, infos);
		
		// we set relative path
		for (auto &res : infos)
		{
			res.relPath = res.absPath;
			res.relPath.erase(0, source.size());
		}
	}
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_fileCounter = 0;
		for (auto &s : infos)
		{
			_queue.emplace(s);
			_condition.notify_all();
			++_fileCounter;
		}
	}
	while (_fileCounter != 0)
	{
		FileInfo file;
		{
			std::lock_guard<std::mutex> lock(_mutex);
			if (_queue.empty())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				continue;
			}
			file = this->_queue.front();
			this->_queue.pop();
		}
		this->treatFile(file);
		_fileCounter.fetch_sub(1);
	}
}