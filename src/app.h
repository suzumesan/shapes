#pragma once

class Renderer;
struct SDL_Window;

class App
{
	Renderer* m_renderer;
	SDL_Window* m_window;

	void update();

public:
	App();
	~App();

	App(const App& app) = delete;
	App& operator=(const App& app) = delete;

	void start();
};