#include <fstream>
#include <iostream>
#include <vector>

#include "utils.hpp"
#include "citation.h"

#include <cpp-httplib/httplib.h>
#include <nlohmann/json.hpp>

httplib::Client client{ API_ENDPOINT };

std::vector<Citation*> loadCitations(const std::string& filename) {
    // FIXME: load citations from file
    //初始化所有json中的book web article，包含标题 id 作者等所有信息
    std::vector<Citation*> citations;
    std::ifstream file(filename);
    nlohmann::json data = nlohmann::json::parse(file);
    for(auto c : data["citations"]) {
        std::string type = c["type"];
        std::string id = c["id"];
        if(type == "book"){
            std::string isbn = c["isbn"];
            auto result = client.Get("/isbn/" + encodeUriComponent(isbn));
            auto content = nlohmann::json::parse(result->body);
            citations.push_back(new Book(id, content["title"], content["author"], content["publisher"], content["year"]));
        }
        else if(type == "webpage"){
            std::string url = c["url"];
            auto result = client.Get("/title/" + encodeUriComponent(url));
            auto content = nlohmann::json::parse(result->body);
            citations.push_back(new Webpage(id, url, content["title"]));
        }
        else if(type == "article"){
            std::string title = c["title"];
            std::string author = c["author"];
            std::string journal = c["journal"];
            int year = c["year"];
            int volume = c["volume"];
            int issue = c["issue"];
            citations.push_back(new Article(id, title, author, journal, year, volume, issue));
        }
    }
    return citations;
}

std::string readFromFile(const std::string& filename) {
    //读input文章
    std::ifstream file(filename);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

std::string readFromStdin() {
    std::string content((std::istreambuf_iterator<char>(std::cin)), std::istreambuf_iterator<char>());
    return content;
}

bool checkBrackets(std::string& input){
    std::vector<char> check{};
    for(char c : input){
        if(c == '['){
            check.push_back(c);
        }
        else if(c == ']'){
            if(check.empty()) return false;
            check.pop_back();
        }
    }
    return check.empty();
}

bool compare(Citation* a, Citation* b){
    return (a->id < b->id);
}
void getPrintedCitations(std::string& input, std::vector<Citation*>& printedCitations, std::vector<Citation*>& citations){
    std::set<std::string> id{};
    bool record = false;
    std::string temp;
    for (char c : input) {
        if (c == '[') {
            record = true;
            temp = "";
        } else if (c == ']') {
            record = false;
            id.insert(temp);
        } else if (record) {
            temp += c;
        }
    }
    for(auto c : citations){
        if(std::find(id.begin(), id.end(), c->id) != id.end()){
            printedCitations.push_back(c);
        }
    }
    std::sort(printedCitations.begin(), printedCitations.end(), compare);
}
int main(int argc, char** argv) {
    // "docman", "-c", "citations.json", "input.txt"

    // FIXME: read all input to the string, and process citations in the input text
    //auto input = readFromFile(argv[argc -1]);
    // ...
    std::string citationPath;
    std::string outputPath;
    std::string inputPath;
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg == "-c" && i + 1 < argc) {
            citationPath = argv[++i];
        } else if (arg == "-o" && i + 1 < argc) {
            outputPath = argv[++i];
        } else {
            inputPath = arg;
        }
    }

    auto citations = loadCitations(citationPath);
    std::vector<Citation*> printedCitations{};


    std::string input;
    if (inputPath == "-") {
        input = readFromStdin();
    } else {
        input = readFromFile(inputPath);
    }

    if(!checkBrackets(input)) std::exit(1);
    getPrintedCitations(input, printedCitations, citations);

    std::ostream* output;
    if (!outputPath.empty()) {
        output = new std::ofstream(outputPath);
    } else {
        output = &std::cout;
    }

    *output << input;  // print the paragraph first
    *output << "\nReferences:\n";
    
    for (auto c : printedCitations) {
        // FIXME: print citation
        *output << '[' << c->id << "] ";
        if(typeid(c) == typeid(Book*)){
            Book* b = static_cast<Book*>(c);
            *output << "book: " << b->author << ", "<< b->title << ", "<< b->publisher << ", " << b->year << '\n';
        }
        else if(typeid(c) == typeid(Webpage*)){
            Webpage* b = static_cast<Webpage*>(c);
            *output << "webpage: " << b->title << ". Available at " << b->url <<'\n';
        }
        else if(typeid(c) == typeid(Article*)){
            Article* b = static_cast<Article*>(c);
            *output << "article:  发表年份, 卷号, 期号" << b->author << ", "<< b->title << ", "<< b->journal << ", "<< b->year << ", " << b->volume << ", " << b->issue << '\n';
        }
    }

    for (auto c : citations) {
        delete c;
    }
}
