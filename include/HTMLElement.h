#ifndef HTMLELEMENT_H
#define HTMLELEMENT_H

#include <string>

struct HTMLElement
{
    std::string tag;
    std::string content;
    int x, y, width, height;
};

#endif
