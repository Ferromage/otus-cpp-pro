#include "document.h"
#include <iostream>

class DocumentImpl : public DocumentAbstract {
public:
    DocumentImpl() {
        
    }
     
    ~DocumentImpl() {
        close();
    }
    
    void open([[maybe_unused]] const char* path) override {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    void close() override {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    void read([[maybe_unused]] const char* pathIn) override {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    void write([[maybe_unused]] const char* pathOut) override {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
};

std::shared_ptr<DocumentAbstract> createDocument(const char* path) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    auto doc = std::make_shared<DocumentImpl>();
    doc->open(path);
    return doc;
}

std::shared_ptr<DocumentAbstract> importDocumentFromFile(const char* pathIn) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    auto doc = std::make_shared<DocumentImpl>();
    doc->read(pathIn);
    return doc;
}

void exportDocumentToFile(const char* pathOut, std::shared_ptr<DocumentAbstract> doc) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    doc->write(pathOut);
}