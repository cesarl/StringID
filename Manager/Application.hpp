#pragma once

#include <string>
#include <vector>
#include <thread>
#include <atomic>

#include "Project.hpp"
#include "File.hpp"

class Application
{
public:
	Application();
	~Application();

	bool init(int argc, char *argv[]);
	void run();
private:
	struct Save
	{
		uint32_t    from = 0;
		uint32_t    to = 0;
		std::string path = "";
		std::string dest = "";
		bool modified = false;
	};

	void searchAndReplaceInFile(const FileInfo &filepath, Save *save);
	void treatFile(std::size_t index);
	bool waitAndTreatFile();
	void saveSaveBigFile();

	std::string               _projectName;
	bool                      _guiEnabled;
	std::vector<std::string>  _sources;
	std::string               _destination;
	std::vector<std::string>  _excludedFolders;
	std::vector<std::string>  _excludedSources;
	std::vector<std::string>  _extensions;
	bool                      _clean;
	bool                      _verbose;
	bool                      _saveforundo;
	bool                      _undo;
	bool                      _displaySummary;
	bool                      _inPlace;

	std::string               _currentDirectory;

	std::vector<std::thread> _threads;
	std::atomic_size_t       _fileCounter;
	std::atomic_size_t       _treatedFileCounter;
	bool                     _exit;

	Project                  _project;

	std::vector<Save>       _rtSaves;
	std::vector<FileInfo>   _rtInfos;

	Application(const Application &) = delete;
	Application(Application &&)      = delete;
	Application &operator=(const Application &) = delete;
	Application &operator=(Application &&)      = delete;
};