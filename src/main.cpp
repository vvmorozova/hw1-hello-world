#include "model.h"
#include "view.h"
#include "controller.h"

#include <memory>
#include <iostream>
#include <string>

void View::UI::onMouseDown(int x, int y) {
	m_controller->handleMouseDown(x, y);
};

void View::UI::onMouseUp(int x, int y) {
	m_controller->handleMouseUp(x, y);
};

void View::UI::onMouseMove(int x, int y) {
	m_controller->handleMouseMove(x, y);
};


void View::UI::onNewDoc() {
	m_controller->handleNewDoc();
};

void View::UI::onImportDoc(const std::string& filename) {
	m_controller->handleImportDoc(filename);
};

void View::UI::onExportDoc(const std::string& filename) {
	m_controller->handleExportDoc(filename);
};


void View::UI::setController(std::shared_ptr<class Controller::Logic> controller)
{
	m_controller = controller;
}

int main(int, char **) {
	auto s = Controller::createStorage();
	auto ui = Controller::createUI();

	auto l = std::make_shared<Controller::Logic>(s, ui);
	l->run();
	return 0;
} 


