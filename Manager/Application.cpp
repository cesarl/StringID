#include "Application.hpp"

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
"-showcmds                 Displays the full command lines passed\n"
"-verbose                  Show detailed diagnostic information for debugging\n"
"-saveforundo              Will keep a save of modified files in project's file\n"
"-undo                     Used in association with -project will undo last generation\n"
"-summary                  Display a detailed summary\n";

Application::Application(int argc, const char **argv)
{

}

Application::~Application()
{}

void Application::run()
{
}