#include "view.h"
#include <iostream>

class StandardView : public ViewAbstract {
public:
    StandardView() {

    }
    
    void showDocument([[maybe_unused]] const DocumentAbstract& document) override {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    void exportDocument([[maybe_unused]] const DocumentAbstract& document) override {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    void addShape([[maybe_unused]] const ShapeAbstract& shape) override {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    void removeShape([[maybe_unused]] const ShapeAbstract& shape) override {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
};

std::shared_ptr<ViewAbstract> createView() {
    return std::make_shared<StandardView>();
}