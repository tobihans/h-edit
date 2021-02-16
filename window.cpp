#include "window.h"
#include "editor.h"

Window::Window(QWidget *parent) : QMainWindow(parent)
{

}

void Window::createNewWindow()
{
    auto *window = new Editor;
    windows.append(window);
    connect(window, &Editor::newWindowRequest, this, &Window::createNewWindow);
    window->showMaximized();
}
