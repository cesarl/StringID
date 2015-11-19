#pragma once

#include <string>
#include <vector>
#include <thread>
#include <atomic>

#include "File.hpp"

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

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
		std::string path = "";
		std::string dest = "";
		bool modified = false;
	};

	struct ProjectSave
	{
		struct PjcSave
		{
			std::string path;
			std::string buffer;

			template<class Archive>
			void serialize(Archive & archive)
			{
				archive(CEREAL_NVP(path), CEREAL_NVP(buffer));
			}
		};
		std::vector<PjcSave> save;
		uint32_t filesLastWrite;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(save), CEREAL_NVP(filesLastWrite));
		}
	};

	void searchAndReplaceInFile(const FileInfo &filepath, Save *save);
	void treatFile(std::size_t index);
	bool waitAndTreatFile();
	void projectLoad();
	void projectSave();
	void undoFile(const ProjectSave::PjcSave &save);

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

	ProjectSave              _project;

	std::vector<Save>       _rtSaves;
	std::vector<FileInfo>   _rtInfos;

	Application(const Application &) = delete;
	Application(Application &&)      = delete;
	Application &operator=(const Application &) = delete;
	Application &operator=(Application &&)      = delete;
};