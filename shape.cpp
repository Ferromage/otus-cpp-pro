#include "shape.h"

class Point : public ShapeAbstract {
public:
    Point(int id, int centerX = 0, int centerY = 0) : ShapeAbstract(id, centerX, centerY) {

    }

    const char* name() const override {
        return "point";
    }
};

class Line : public ShapeAbstract {
public:
    Line(int id, int centerX = 0, int centerY = 0, int endX = 1, int endY = 1) : ShapeAbstract(id, centerX, centerY), endX_(endX), endY_(endY) {

    }

    const char* name() const override {
        return "line";
    }

private:
    int endX_;
    int endY_;
};

class Rectangle : public ShapeAbstract {
public:
    Rectangle(int id, int centerX = 0, int centerY = 0, int endX = 1, int endY = 1) : ShapeAbstract(id, centerX, centerY), endX_(endX), endY_(endY) {

    }

    const char* name() const override {
        return "rectangle";
    }

private:
    int endX_;
    int endY_;
};


std::shared_ptr<ShapeAbstract> makeShape(ShapeType type) {
    static int id;

    if (type == ShapeType::Point) {
        return std::make_shared<Point>(id++);
    } else if (type == ShapeType::Line) {
        return std::make_shared<Line>(id++);
    } else if (type == ShapeType::Rectangle) {
        return std::make_shared<Rectangle>(id++);
    }

    return nullptr;
}