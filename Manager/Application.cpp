#include "Application.hpp"

#include <iostream>

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
	_displaySummary(false)
{}

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
					_sources.push_back(source);
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
					_excludedFolders.push_back(source);
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
					_excludedSources.push_back(source);
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
	return true;
}

Application::~Application()
{}

void Application::run()
{
	if (_guiEnabled)
	{
		//_initGui();
	}
}