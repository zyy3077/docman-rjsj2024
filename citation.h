#pragma once
#ifndef CITATION_H
#define CITATION_H

#include <string>

class Citation {
    std::string id;
public:
    Citation(std::string& id) : id{id} {}
};
class Book : public Citation {
    std::string isbn;
public:
    Book(std::string& id, std::string& isbn) : Citation(id), isbn{isbn} {}
};
class Webpage : public Citation {
    std::string url;
public:
    Webpage(std::string& id, std::string& url) : Citation(id), url{url} {}
};
class Article : public Citation {
    std::string title;
    std::string author;
    std::string journal;
    std::string year;
    int volume;
    int issue;
public:
    Article(std::string& id, std::string& title, std::string& author, std::string& journal, std::string& year, int& volume, int& issue) :
    Citation(id), title{title}, author{author}, journal{journal}, year{year}, volume{volume}, issue{issue} {}
};

#endif