#pragma once

class Application
{
public:
	Application(int argc, const char **argv);
	void run();
	~Application();



private:
	Application(const Application &) = delete;
	Application(Application &&)      = delete;
	Application &operator=(const Application &) = delete;
	Application &operator=(Application &&)      = delete;
};