#pragma once

#include "controller.h"

#include <memory>
#include <string>

namespace Controller  {

	class Logic;
}

namespace View {
	class UI {
	private:
		std::shared_ptr<class Controller::Logic> m_controller;

	public:
		void onMouseDown(int x, int y);
		void onMouseUp(int x, int y);
		void onMouseMove(int x, int y);

		void onNewDoc();
		void onImportDoc(const std::string& filename);
		void onExportDoc(const std::string& filename);

		void setController(std::shared_ptr<class Controller::Logic> controller);
	};	

}
