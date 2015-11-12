#pragma once

class Application
{
public:
	Application(int argc, char *argv[]);
	~Application();

	void run();
private:
	Application(const Application &) = delete;
	Application(Application &&)      = delete;
	Application &operator=(const Application &) = delete;
	Application &operator=(Application &&)      = delete;
};