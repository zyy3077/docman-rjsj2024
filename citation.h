#pragma once
#ifndef CITATION_H
#define CITATION_H

#include <string>

class Citation {
public:
    enum Type {BOOK, WEBPAGE, ARTICLE};
    Type type;
    std::string id;
    std::string title;

    Citation(std::string id) : id{id} {}
    Citation(std::string id, std::string title, Type type) : id{id}, title{title}, type{type} {} 
};
class Book : public Citation {
public:
    std::string author;
    std::string isbn;
    std::string publisher;
    std::string year;
    Book(std::string id, std::string title, std::string author, std::string publisher, std::string year) : Citation(id, title, BOOK), author{author}, publisher{publisher}, year{year} {}
};
class Webpage : public Citation {
public:
    std::string url;
    Webpage(std::string id, std::string url, std::string title) : Citation(id, title, WEBPAGE), url{url} {}
};
class Article : public Citation {
public:
    std::string author;
    std::string journal;
    int year;
    int volume;
    int issue;

    Article(std::string id, std::string title, std::string author, std::string journal, int year, int volume, int issue) :
    Citation(id, title, ARTICLE), author{author}, journal{journal}, year{year}, volume{volume}, issue{issue} {}
};

#endif