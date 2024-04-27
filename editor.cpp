#include "editor.h"
#include <iostream>

Editor::Editor(std::shared_ptr<ViewAbstract> view) : view_(view) {

}

void Editor::createNewDocument(const char* path) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    document_ = createDocument(path);
    if (view_) {
        view_->showDocument(*document_);
    }
}

void Editor::importDocument(const char* fileIn) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    document_ = importDocumentFromFile(fileIn);
    if (view_) {
        view_->showDocument(*document_);
    }
}

void Editor::exportDocument(const char* fileOut) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    exportDocumentToFile(fileOut, document_);
    if (view_) {
        view_->exportDocument(*document_);
    }
}

int Editor::addShape(std::shared_ptr<ShapeAbstract> shape) {
    std::cout << __PRETTY_FUNCTION__ << "; shape name: " << shape->name() << ", id: " << shape->id() << std::endl;
    shapes_.emplace(shape->id(), shape);
    if (view_) {
        view_->addShape(*shape);
    }
    return shape->id();
}

void Editor::removeShape(int shapeId) {
    if (auto it = shapes_.find(shapeId); it != shapes_.end()) {
        std::cout << __PRETTY_FUNCTION__ << "; shape name: " << it->second->name() << ", id: " << it->second->id() << std::endl;
        if (view_) {
            view_->removeShape(*it->second);
        }
        shapes_.erase(it);
    }
}