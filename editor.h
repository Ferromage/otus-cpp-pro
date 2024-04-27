#pragma once

#include "view.h"
#include "document.h"
#include "shape.h"
#include <map>

class Editor {
public:
    Editor(std::shared_ptr<ViewAbstract> view);

    void createNewDocument(const char* path);
    void importDocument(const char* fileIn);
    void exportDocument(const char* fileOut);
    int addShape(std::shared_ptr<ShapeAbstract> shape);
    void removeShape(int shapeId);

private:
    std::shared_ptr<ViewAbstract> view_;
    std::shared_ptr<DocumentAbstract> document_;
    std::map<int, std::shared_ptr<ShapeAbstract>> shapes_;
};