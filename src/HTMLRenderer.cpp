#include "HTMLRenderer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <curl/curl.h>

const int WINDOW_WIDTH = 1010;
const int WINDOW_HEIGHT = 735;

HTMLRenderer::HTMLRenderer() : window(nullptr), renderer(nullptr), font(nullptr), last_modified(0), isInputActive(false), scrollOffset(0)
{
    font_sizes["h1"] = 32;
    font_sizes["h2"] = 28;
    font_sizes["h3"] = 24;
    font_sizes["h4"] = 20;
    font_sizes["h5"] = 18;
    font_sizes["h6"] = 16;
    font_sizes["p"] = 16;

    inputBox = {10, WINDOW_HEIGHT - 40, 300, 30};
    submitButton = {320, WINDOW_HEIGHT - 40, 100, 30};
}

bool HTMLRenderer::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() < 0)
    {
        std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Silly Browser", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    font = TTF_OpenFont("arial.ttf", 16);
    if (!font)
    {
        std::cerr << "Font loading failed: " << TTF_GetError() << std::endl;
        return false;
    }

    return true;
}

void HTMLRenderer::renderText(const std::string &text, int x, int y, int font_size, SDL_Color color)
{
    TTF_SetFontSize(font, font_size);
    SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture)
        {
            SDL_Rect rect = {x, y, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

int HTMLRenderer::getTextWidth(const std::string &text, int font_size)
{
    TTF_Font *temp_font = TTF_OpenFont("arial.ttf", font_size);
    int text_width = 0;
    if (temp_font)
    {
        TTF_SizeText(temp_font, text.c_str(), &text_width, nullptr);
        TTF_CloseFont(temp_font);
    }
    return text_width;
}

void HTMLRenderer::render()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    if (elements.empty())
    {
        SDL_Color black = {0, 0, 0, 255};
        renderText("No elements loaded", 10, 10 - scrollOffset, 16, black);
    }
    else
    {
        for (const auto &element : elements)
        {
            int font_size = font_sizes[element.tag];
            renderText(element.content, element.x, element.y - scrollOffset, font_size, element.color);
        }
    }

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &inputBox);
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color blue = {0, 0, 255, 255};
    renderText(inputText, inputBox.x + 5, inputBox.y + 5, 16, black);

    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderFillRect(renderer, &submitButton);
    renderText("Submit", submitButton.x + 5, submitButton.y + 5, 16, blue);

    SDL_RenderPresent(renderer);
}

void HTMLRenderer::handleEvent(SDL_Event &event)
{
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        int x, y;
        SDL_GetMouseState(&x, &y);
        if (pointInRect(x, y, inputBox))
        {
            isInputActive = true;
        }
        else if (pointInRect(x, y, submitButton))
        {
            loadFromURL(inputText);
        }
        else
        {
            isInputActive = false;
        }
    }
    else if (event.type == SDL_TEXTINPUT && isInputActive)
    {
        inputText += event.text.text;
    }
    else if (event.type == SDL_KEYDOWN && isInputActive)
    {
        if (event.key.keysym.sym == SDLK_BACKSPACE && !inputText.empty())
        {
            inputText.pop_back();
        }
    }
    else if (event.type == SDL_MOUSEWHEEL)
    {
        scrollOffset -= event.wheel.y * 20;
        if (scrollOffset < 0)
            scrollOffset = 0;
    }
}

size_t HTMLRenderer::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

bool HTMLRenderer::loadFromURL(const std::string &url)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "Failed to fetch URL: " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        elements.clear();
        int y = 10;

        std::regex re("<(p|h[1-6]|ul|li|a)\\s*(?:x=[\"']?(\\d+)[\"']?\\s*y=[\"']?(\\d+)[\"']?)?\\s*>(.*?)</\\1>", std::regex::icase);

        std::smatch match;

        std::string::const_iterator searchStart(readBuffer.cbegin());
        while (std::regex_search(searchStart, readBuffer.cend(), match, re))
        {
            std::string tag = match[1].str();
            std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
            int x = match[2].matched ? std::stoi(match[2].str()) : 10;
            int contentY = match[3].matched ? std::stoi(match[3].str()) : y;
            std::string text = match[4].str();

            SDL_Color color = {0, 0, 0, 255};

            if (tag == "a")
            {
                SDL_Color color = {0, 0, 255, 255};
            }

            auto [newX, newY, height] = processTagAndContent(tag, text, contentY);
            elements.push_back({tag, text, x, contentY, newX, height, color});

            y += height + 10;
            searchStart = match.suffix().first;
        }

        return true;
    }

    return false;
}

std::tuple<int, int, int> HTMLRenderer::processTagAndContent(const std::string &tag, const std::string &content, int currentYPosition)
{
    int font_size = font_sizes[tag];
    int text_width = getTextWidth(content, font_size);
    int text_height = TTF_FontHeight(font);

    return {20 + text_width, currentYPosition + text_height, text_height};
}

bool HTMLRenderer::pointInRect(int x, int y, const SDL_Rect &rect)
{
    return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

void HTMLRenderer::run()
{
    bool quit = false;
    SDL_Event event;

    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            handleEvent(event);
        }

        render();
    }
}

void HTMLRenderer::cleanup()
{
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}
