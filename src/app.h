#pragma once

class D3DContext;
struct SDL_Window;

class App
{
	D3DContext* m_context;
	SDL_Window* m_window;

	void update();

public:
	App();
	~App();

	App(const App& app) = delete;
	App& operator=(const App& app) = delete;

	void start();
};