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

#include <cereal/archives/json.hpp>

static const char* g_help =
"-help                     Display this help and do nothing\n"
"-project                  Project path (default=\"./StringIDProj\"\n"
"-gui                      Launch GUI application\n"
"-sources                  List folders to search (=\"C:/Folder1;../Folder2\")\n"
"-destination              Destination root folder where to copy results.\n"
"                          If empty, modifications will be in place\n"
"-excludedFolders          List folders to exclude (=\"C:/Folder1;../Folder2\")\n"
"-excludedSources          List sources to exclude (=\"path1;path2\")\n"
"-extensions               List extensions to parse (default=\".cpp;.h;.hh;.inl\")\n"
"-clean                    Will not use cache infos, will re-parse all files\n"
"-verbose                  Show detailed diagnostic information for debugging\n"
"-save                     Will keep a save of modified files in project's file\n"
"-undo                     Used in association with -project will undo last generation\n"
"-summary                  Display a detailed summary\n";

Application::Application()
	:
	_projectName("./StringIDProj"),
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
			if (arg == "-save")
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
	if (_sources.empty() && _undo == false)
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
	auto fileNumber = _rtInfos.size();
	while (_treatedFileCounter < fileNumber)
	{
		std::size_t it = _fileCounter.fetch_add(1);
		if (it >= fileNumber)
			continue;
		treatFile(it);
		_treatedFileCounter.fetch_add(1);
	}
	return false;
}

void Application::treatFile(std::size_t index)
{
	auto &file = _rtInfos[index];
	Save *save = nullptr;
	if (_saveforundo)
	{
		save = &_rtSaves[index];
		CopyFile(save->dest.c_str(), save->path.c_str(), FALSE);
	}
	searchAndReplaceInFile(file, save);
}

void Application::searchAndReplaceInFile(const FileInfo &fileInfo, Save *save)
{
	std::ifstream file(fileInfo.absPath.c_str());
	
	std::string output;
	output.reserve(fileInfo.size);

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

	bool disabledScope = false;
	bool modified = false;

	while (std::getline(file, line))
	{
		std::istringstream iss(line);

		line += "\n";

		bool scopeDisabledForThisLine = disabledScope;

		if (disabledScope == true)
		{
			if (line.find("STRINGID_MANAGER_ENABLED") != std::string::npos)
			{
				disabledScope = false;
			}
		}

		if (disabledScope == false)
		{
			if (line.find("STRINGID_MANAGER_DISABLED") != std::string::npos)
			{
				disabledScope = true;
			}
		}

		if (scopeDisabledForThisLine == false
			&& line.find("StringID") != std::string::npos)
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
					modified = true;
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
			while (std::regex_search(lineCopy, match, regStringOnly, flags))
			{
				std::string replacer = "$1$2\", ";
				auto &str = match[2].str();
				replacer += IntToHex(StringID(str).getId());
				replacer += ")";
				line += std::regex_replace(lineCopy, regStringOnly, replacer, flags);
				lineCopy = match.suffix().str();
				pass = true;
				modified = true;
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
		output += line;
	}
	if (save)
	{
		save->modified = modified;
	}
	file.close();
	std::ofstream outputFile(std::string(_destination + fileInfo.absPath).c_str());
	outputFile << output;

}

void Application::saveSaveBigFile()
{
	uint32_t from = 0;

	HANDLE hAppend;
	HANDLE hFile;
	std::string savePath = _projectName + ".SIDSave";
	DeleteFile(savePath.c_str());
	hAppend = CreateFile(savePath.c_str(),
		FILE_APPEND_DATA,         
		FILE_SHARE_READ,          
		NULL,                     
		OPEN_ALWAYS,              
		FILE_ATTRIBUTE_NORMAL,    
		NULL);

	if (hAppend == INVALID_HANDLE_VALUE)
	{
		// ERROR SAVING
	}

	BYTE   buff[4096];
	size_t sizeOfBuff = sizeof(buff);
	DWORD  dwBytesRead, dwBytesWritten, dwPos;

	for (auto &e : _rtSaves)
	{
		if (e.modified)
		{
			hFile = CreateFile(e.path.c_str(),
				GENERIC_READ,
				0,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				// ERROR OPENING
			}
			e.from = from;
			while (ReadFile(hFile, buff, sizeOfBuff, &dwBytesRead, NULL)
				&& dwBytesRead > 0)
			{
				dwPos = SetFilePointer(hAppend, 0, NULL, FILE_END);
				LockFile(hAppend, dwPos, 0, dwBytesRead, 0);
				WriteFile(hAppend, buff, dwBytesRead, &dwBytesWritten, NULL);
				UnlockFile(hAppend, dwPos, 0, dwBytesRead, 0);
				from += dwBytesWritten;
			}
			e.to = from;
			CloseHandle(hFile);
			DeleteFile(e.path.c_str());
		}
	}
	CloseHandle(hAppend);
	if (from == 0)
	{
		DeleteFile("save.sidsave");
	}
}

void Application::projectLoad()
{
	std::string projectPath = _projectName + ".SIDProj";
	std::ifstream file(projectPath.c_str(), std::ios::binary);
	if (file.is_open())
	{
		cereal::JSONInputArchive ar(file);
		ar(_project);
	}
}

void Application::projectSave()
{
	_project.filesLastWrite = GetCurrentTime();
	_project.save.clear();
	if (_saveforundo)
	{
		for (auto &s : _rtSaves)
		{
			if (s.modified)
			{
				_project.save.resize(_project.save.size() + 1);
				auto &save = _project.save.back();
				save.from = s.from;
				save.to = s.to;
				save.path = s.dest;
			}
		}
	}

	std::string projectPath = _projectName + ".SIDProj";
	std::ofstream file(projectPath.c_str(), std::ios::binary);
	if (file.is_open())
	{
		cereal::JSONOutputArchive ar(file);
		ar(_project);
	}
}

void Application::undoFile(const ProjectSave::PjcSave &save)
{
	std::string savePath = _projectName + ".SIDSave";
	std::ofstream dest(save.path.c_str());
	if (dest.is_open() == false)
	{
		//ERROR
		return;
	}
	std::string saveFilePath = _projectName + ".SIDSave";
	std::ifstream source(saveFilePath.c_str());
	if (source.is_open() == false)
	{
		//ERROR
		return;
	}
	source.seekg(save.from, source.beg);
	std::size_t length = save.to - save.from;
	STRINGID_ASSERT(length != 0);
	char *buffer = new char[length];
	source.read(buffer, length);
	dest.write(buffer, length);
	delete[] buffer;
}

void Application::run()
{

	if (_guiEnabled)
	{
		//_initGui();
	}

	projectLoad();

	if (_undo == true)
	{
		for (auto &e : _project.save)
		{
			undoFile(e);
		}
	}
	else
	{
		FileFilter filter;
		filter._extensions = _extensions;
		filter._excludedPath = _excludedSources;
		filter._excludedDir = _excludedFolders;
		filter._minimumWriteTime = 0;
		for (auto &source : _sources)
		{
			SearchFiles(source, &filter, _rtInfos);

			// we set relative path
			for (auto &res : _rtInfos)
			{
				res.relPath = res.absPath;
				res.relPath.erase(0, source.size());
			}
		}
		{
			_fileCounter = 0;
			_treatedFileCounter = 0;
			for (auto &s : _rtInfos)
			{
				if (_saveforundo)
				{
					Save save;
					save.dest = _destination + s.absPath;
					save.path = save.dest + ".sidtmpsave";
					_rtSaves.push_back(save);
				}
			}
		}
		unsigned int maxThread = std::thread::hardware_concurrency();
		if (maxThread > 1)
			maxThread -= 1;
		for (unsigned int i = 0; i < maxThread; ++i)
		{
			_threads.emplace_back([this]() {

				while (waitAndTreatFile())
				{
				}
			});
		}
		waitAndTreatFile();

		if (_saveforundo)
		{
			saveSaveBigFile();
		}

		projectSave();
	}
}