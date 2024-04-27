#pragma once

#include "document.h"
#include "shape.h"
#include <memory>

class ViewAbstract {
public:
    virtual ~ViewAbstract() {}
    virtual void showDocument(const DocumentAbstract& document) = 0;
    virtual void exportDocument(const DocumentAbstract& document) = 0;
    virtual void addShape(const ShapeAbstract& shape) = 0;
    virtual void removeShape(const ShapeAbstract& shape) = 0;
};

std::shared_ptr<ViewAbstract> createView();

