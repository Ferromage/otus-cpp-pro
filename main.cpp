#include "editor.h"
#include "view.h"
#include "shape.h"

int main() {
    auto view = createView();
    Editor editor(view);

    editor.createNewDocument("new_document.txt");
    editor.importDocument("file1.txt");
    editor.exportDocument("file2.txt");
    editor.addShape(makeShape((ShapeType::Point)));
    editor.addShape(makeShape((ShapeType::Line)));
    editor.removeShape(1);
}