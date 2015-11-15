#pragma once

#include <string>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "File.hpp"

class Application
{
public:
	Application();
	~Application();

	bool init(int argc, char *argv[]);
	void run();
private:
	void treatFile(const FileInfo &filepath);
	bool waitAndTreatFile();

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
	std::mutex               _mutex;
	std::condition_variable  _condition;
	std::queue<FileInfo>     _queue;
	std::atomic_size_t       _fileCounter;
	bool                     _exit;

	Application(const Application &) = delete;
	Application(Application &&)      = delete;
	Application &operator=(const Application &) = delete;
	Application &operator=(Application &&)      = delete;
};