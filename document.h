#pragma once

#include <memory>

class DocumentAbstract {
public:
    virtual ~DocumentAbstract() {}
    virtual void open(const char* path) = 0;
    virtual void close() = 0;
    virtual void read(const char* pathIn) = 0;
    virtual void write(const char* pathOut) = 0;
};

std::shared_ptr<DocumentAbstract> createDocument(const char* path);
std::shared_ptr<DocumentAbstract> importDocumentFromFile(const char* pathIn);
void exportDocumentToFile(const char* pathOut, std::shared_ptr<DocumentAbstract> doc);