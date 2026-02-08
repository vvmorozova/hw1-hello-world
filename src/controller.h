#pragma once

#include "model.h"
#include "view.h"

#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <string>
#include <memory>

namespace Controller {
	class DrawCommand {
	public:
		virtual ~DrawCommand() {};
		virtual bool execute(std::shared_ptr<Model::Storage> storage);
	};

	class DrawCircle : public DrawCommand{
	public:
		virtual bool execute(std::shared_ptr<Model::Storage> storage);
	};

	class DrawRectangle : public DrawCommand{
	public:
		virtual bool execute(std::shared_ptr<Model::Storage> storage);
	};

	class EraseCommand : public DrawCommand{
	public:
		virtual bool execute(std::shared_ptr<Model::Storage> storage);
	};

	class DrawLine : public DrawCommand{
	public:
		virtual bool execute(std::shared_ptr<Model::Storage> storage);
	};

	class RecolorCommand : public DrawCommand{
	public:
		virtual bool execute(std::shared_ptr<Model::Storage> storage);
	};

	class DocCommand {
	public:
		virtual ~DocCommand() {};
		virtual bool execute(const std::string & filename);
	};

	class ImportDocCommand : public DocCommand {
	public:
		bool execute(const std::string & filename) override;
	};

	class ExportDocCommand : public DocCommand {
	public:
		bool execute(const std::string & filename) override;
	};

	class NewDocCommand : public DocCommand {
	public:
		bool execute(const std::string & filename) override;
	};

	class Renderer {
	public:
		void fill(const Model::coordinates &fillPoint, const Model::RGB &color);
		void draw(const std::shared_ptr<Model::Elem> elem);
	};

	class Logic : public std::enable_shared_from_this<Logic>{
	private:
		std::shared_ptr<Model::Storage> m_pS;
		std::shared_ptr<View::UI> m_pUI;

		// for ctrl Z ctrl Y
		std::stack<std::shared_ptr<Controller::DrawCommand>> cmdStack;

	public:
		Logic(std::shared_ptr<Model::Storage> pS, std::shared_ptr<View::UI> pUI) : m_pS(pS), m_pUI(pUI) {};

		bool run() {
			if (!m_pUI) {
				return false;
			}

			m_pUI->setController(shared_from_this());

			return true;
		};

		void handleMouseDown(int x, int y);
		void handleMouseUp(int x, int y);
		void handleMouseMove(int x, int y);

		void handleNewDoc();
		void handleImportDoc(const std::string& filename);
		void handleExportDoc(const std::string& filename);

		void execDrawCmd(std::shared_ptr<Controller::DrawCommand> pCmd);
	};

	std::shared_ptr<Model::Storage> createStorage() {
    	return std::make_shared<Model::Storage>();
	}

	std::shared_ptr<View::UI> createUI() {
		return std::make_shared<View::UI>();
	}

}
