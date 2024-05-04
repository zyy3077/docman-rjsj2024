#include <fstream>
#include <iostream>
#include <vector>

#include "utils.hpp"
#include "citation.h"
#include <cpp-httplib/httplib.h>
#include <nlohmann/json.hpp>

httplib::Client client{ API_ENDPOINT };
std::vector<std::string> citationID{};

std::vector<Citation*> loadCitations(const std::string& filename) {
    // FIXME: load citations from file
    //初始化所有json中的book web article，包含标题 id 作者等所有信息
    std::vector<Citation*> citations;

    //nlohmann::json data = nlohmann::json::parse(file);
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        std::exit(1);
    }

    nlohmann::json data = nlohmann::json::parse(file);
    // try {
    //     // Check if file is empty
    //     if (file.peek() == std::ifstream::traits_type::eof()) {
    //         std::cerr << "Empty file: " << filename << std::endl;
    //         std::exit(1);
    //     }

    //     data = nlohmann::json::parse(file);
    //     // Check if data is an object
    //     if (!data.is_object() || data.find("citations") == data.end()) {
    //         std::cerr << "Invalid JSON object in file: " << filename << std::endl;
    //         std::exit(1);
    //     }
    //     // use data...
    // } catch (nlohmann::json::parse_error& e) {
    //     std::cerr << "Failed to parse JSON from file: " << e.what() << std::endl;
    //     std::exit(1);
    // }

    for(auto c : data["citations"]) {
        if(!c["type"].is_string() || !c["id"].is_string()){
            std::exit(1);
        }
        std::string type = c["type"];
        std::string id = c["id"];
        citationID.push_back(id);
        if(type == "book"){
            if(!c["isbn"].is_string()){
                std::cerr << "isbn is typewrong" << std::endl;
                std::exit(1);
            }
            std::string isbn = c["isbn"];
            auto result = client.Get("/isbn/" + encodeUriComponent(isbn));
            if(result->body.empty()){
                std::cerr << "empty jsonstr\n";
                std::exit(1);
            }
            auto content = nlohmann::json::parse(result->body);
            if(!content["title"].is_string() || !content["author"].is_string() || !content["publisher"].is_string() || !content["year"].is_string()){
                std::cerr << "Value for book is typewrong." << std::endl;
                std::exit(1);
            }
            citations.push_back(new Book(id, content["title"], content["author"], content["publisher"], content["year"]));
        }
        else if(type == "webpage"){
            if(!c["url"].is_string()){
                std::cerr << "url is typewrong" << std::endl;
                std::exit(1);
            }
            std::string url = c["url"];
            auto result = client.Get("/title/" + encodeUriComponent(url));
            if(result->body.empty()){
                std::cerr << "empty jsonstr\n";
                std::exit(1);
            }
            auto content = nlohmann::json::parse(result->body);
            if(!content["title"].is_string()){
                std::cerr << "Value for web is typewrong," << std::endl;
                std::exit(1);
            }
            citations.push_back(new Webpage(id, url, content["title"]));
        }
        else if(type == "article"){
            if(!c["title"].is_string() || !c["author"].is_string() || !c["journal"].is_string() || !c["year"].is_number_integer() || !c["volume"].is_number_integer() || !c["issue"].is_number_integer()){
                std::cerr << "article value is typewrong" << std::endl;
                std::exit(1);
            }
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
    if (!file.is_open()) {
        std::cerr << "Failed to open input file: " << filename << std::endl;
        std::exit(1);
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (!content.empty() && content.back() == '\n') {
        content.pop_back();
    }
    return content;
}

std::string readFromStdin() {
    std::string content((std::istreambuf_iterator<char>(std::cin)), std::istreambuf_iterator<char>());
    return content;
}

bool checkBrackets(std::string& input){
    std::vector<char> check{};
    for(char c : input){
        if(check.size() > 1) return false;
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
    std::set<std::string> inputID{};
    bool record = false;
    std::string temp;
    for (char c : input) {
        if (c == '[') {
            record = true;
            temp = "";
        } else if (c == ']') {
            record = false;
            if(find(citationID.begin(), citationID.end(), temp) == citationID.end()){
                std::cerr << "input id not in the citation json" << std::endl;
                std::exit(1);
            }
            inputID.insert(temp);
        } else if (record) {
            temp += c;
        }
    }
    for(auto c : citations){
        if(std::find(inputID.begin(), inputID.end(), c->id) != inputID.end()){
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
    // for (int i = 1; i < argc; i++) {
    //     std::string arg(argv[i]);
    //     if (arg == "-c" && i + 1 < argc) {
    //         citationPath = argv[++i];
    //     } else if (arg == "-o" && i + 1 < argc) {
    //         outputPath = argv[++i];
    //     } else {
    //         inputPath = arg;
    //     }
    // }
    if(argc < 4){
        std::cerr << "more arguments needed" << std::endl;
        std::exit(1);
    }
    if(argc > 6){
        std::cerr << "too much arguments" << std::endl;
        std::exit(1);
    }
    if(std::string(argv[1]) != "-c"){
        std::cerr << "first option should be -c but is " << argv[1] << std::endl;
        std::exit(1);
    }
    citationPath = argv[2];
    if(std::string(argv[3]) == "-o"){
        outputPath = argv[4];
        inputPath = argv[5];
    }
    else{
        inputPath = argv[3];
    }
    if(inputPath == outputPath){
        std::cerr << "inputpath should be different from outputpath" << std::endl;
        std::exit(1);
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
        if (!output->good()) {
        std::cerr << "Failed to open output file: " << outputPath << std::endl;
        std::exit(1);
    }
    } else {
        output = &std::cout;
    }

    *output << input << '\n';  // print the paragraph first
    *output << "\nReferences:\n";
    
    for (auto c : printedCitations) {
        // FIXME: print citation
        *output << '[' << c->id << "] ";
        if(c->type == Citation::BOOK){
            Book* b = static_cast<Book*>(c);
            *output << "book: " << b->author << ", "<< b->title << ", "<< b->publisher << ", " << b->year << '\n';
        }
        else if(c->type == Citation::WEBPAGE){
            Webpage* b = static_cast<Webpage*>(c);
            *output << "webpage: " << b->title << ". Available at " << b->url <<'\n';
        }
        else if(c->type == Citation::ARTICLE){
            Article* b = static_cast<Article*>(c);
            *output << "article: " << b->author << ", "<< b->title << ", "<< b->journal << ", "<< b->year << ", " << b->volume << ", " << b->issue << '\n';
        }
    }

    for (auto c : citations) {
        delete c;
    }
}
