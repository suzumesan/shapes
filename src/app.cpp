#include "app.h"

#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#include "renderer.h"

App::App() :
	m_renderer(nullptr),
	m_window(nullptr)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
}

App::~App()
{
	if (m_renderer)
	{
		delete m_renderer;
		m_renderer = nullptr;
	}

	if (m_window)
	{
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}

	SDL_Quit();
}

void App::start()
{
	m_window = SDL_CreateWindow("shapes", 100, 100, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(m_window, &wmInfo);

	m_renderer = new Renderer;
	m_renderer->create(wmInfo.info.win.window);

	update();
}

void App::update()
{
	bool quit = false;
	while (!quit)
	{
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			}
		}
	}
}