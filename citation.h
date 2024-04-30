#pragma once
#ifndef CITATION_H
#define CITATION_H

#include <string>

class Citation {
public:
    std::string id;
    std::string title;

    Citation(std::string& id) : id{id} {}
    Citation(std::string& id, std::string& title) : id{id}, title{title} {} 
};
class Book : public Citation {
public:
    std::string author;
    std::string isbn;
    std::string publisher;
    std::string year;

    Book(std::string& id, std::string& isbn) : Citation(id), isbn{isbn} {}
};
class Webpage : public Citation {
public:
    std::string url;
    Webpage(std::string& id, std::string& url) : Citation(id), url{url} {}
};
class Article : public Citation {
public:
    std::string author;
    std::string journal;
    std::string year;
    int volume;
    int issue;

    Article(std::string& id, std::string& title, std::string& author, std::string& journal, std::string& year, int& volume, int& issue) :
    Citation(id, title), author{author}, journal{journal}, year{year}, volume{volume}, issue{issue} {}
};

#endif