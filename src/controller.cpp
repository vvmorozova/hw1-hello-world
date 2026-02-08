#include "controller.h"
#include "view.h"

bool Controller::Logic::run()
{
	if (!m_pUI) {
		return false;
	}

	m_pUI->setController(shared_from_this());

	return true;
};

std::shared_ptr<Model::Storage> Controller::createStorage() {
	return std::make_shared<Model::Storage>();
}

std::shared_ptr<View::UI> Controller::createUI() {
	return std::make_shared<View::UI>();
}

void Controller::Logic::handleMouseDown(int x, int y)
{
	std::cout << x << " " << y << std::endl;
}

void Controller::Logic::handleMouseUp(int x, int y)
{
	std::cout << x << " " << y << std::endl;
}

void Controller::Logic::handleMouseMove(int x, int y)
{
	std::cout << x << " " << y << std::endl;
}


void Controller::Logic::handleNewDoc() {}
void Controller::Logic::handleImportDoc(const std::string& filename)
{
	std::cout << filename << std::endl;
}

void Controller::Logic::handleExportDoc(const std::string& filename)
{
	std::cout << filename << std::endl;
}

void Controller::Logic::execDrawCmd(std::shared_ptr<Controller::DrawCommand> pCmd)
{
	pCmd->execute(m_pS);
}
