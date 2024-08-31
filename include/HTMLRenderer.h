#ifndef HTML_RENDERER_H
#define HTML_RENDERER_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include <map>
#include <tuple>

struct Element
{
    std::string tag;
    std::string content;
    int x, y;
    int width, height;
    SDL_Color color;
};

class HTMLRenderer
{
public:
    HTMLRenderer();
    bool init();
    void run();
    void cleanup();

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Rect inputBox;
    SDL_Rect submitButton;
    std::string inputText;
    int scrollOffset;
    std::map<std::string, int> font_sizes;
    std::vector<Element> elements;
    time_t last_modified;
    bool isInputActive;

    bool loadFromURL(const std::string &url);
    void render();
    void handleEvent(SDL_Event &event);
    void renderText(const std::string &text, int x, int y, int font_size, SDL_Color color);
    int getTextWidth(const std::string &text, int font_size);
    std::tuple<int, int, int> processTagAndContent(const std::string &tag, const std::string &content, int currentYPosition);
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
    bool pointInRect(int x, int y, const SDL_Rect &rect);
};

#endif // HTML_RENDERER_H
