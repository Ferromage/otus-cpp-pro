#pragma once

#include <memory>

enum class ShapeType {Point, Line, Rectangle};

class ShapeAbstract {
public:
    ShapeAbstract(int id, int x = 0, int y = 0) : id_(id), x_(x), y_(y) {}
    virtual ~ShapeAbstract() {}
    virtual const char* name() const = 0;
    virtual int id() const {
        return id_;
    }

protected:
    int id_;
    int x_;
    int y_;
};

std::shared_ptr<ShapeAbstract> makeShape(ShapeType type);