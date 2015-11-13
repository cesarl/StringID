#pragma once

#include <string>
#include <vector>

class Application
{
public:
	Application();
	~Application();

	bool init(int argc, char *argv[]);
	void run();
private:
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

	std::string               _currentDirectory;


	Application(const Application &) = delete;
	Application(Application &&)      = delete;
	Application &operator=(const Application &) = delete;
	Application &operator=(Application &&)      = delete;
};