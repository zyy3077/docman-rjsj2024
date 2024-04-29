#pragma once
#ifndef CITATION_H
#define CITATION_H

#include <string>

class Citation {
    std::string text;
public:
    Citation(std::string& line) : text{line} {}
};

#endif