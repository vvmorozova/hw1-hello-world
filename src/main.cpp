#include "lib.h"

#include <memory>
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <string>

enum class currElemTypeToDraw {
	Line,
	Circle,
	Rectangle
};

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

		void setController(std::shared_ptr<class Controller::Logic> controller)
		{
			m_controller = controller;
		}
	};	

}

namespace Model {
	struct coordinates {
		int x;
		int y;
	};

	struct RGB {
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};


	class Elem {
	private:
		coordinates basePoint;
		RGB color;
		int layer;

	public:
		virtual ~Elem();

		void setBasePoint(const coordinates &newBasePoint);
		coordinates getBasePoint() const;

		void setColor(const RGB &newColor);
		RGB getColor() const;

		void setLayer(const int newLayer);
		int getLayer() const;
	};

	class Line: public Elem {
	private:
		coordinates secondPoint;

	public:
		void setSecondPoint(const coordinates &newSecondPoint);
		coordinates getSecondPoint() const;
	};

	class Circle: public Elem {
	private:
		int radius;

	public:
		void setRadius(const int &newRadius);
		int getRadius() const;
	};

	class Rectangle: public Elem {
	private:
		coordinates secondPoint;

	public:
		void setSecondPoint(const coordinates &newSecondPoint);
		coordinates getSecondPoint() const;
	};

	class Storage {
	private:
		currElemTypeToDraw currType;
		// vector of already drawed elems
		std::vector<std::shared_ptr<Elem>> painted;
		// cached last opened/exported file's names
		std::queue<std::string> lastFiles;
	public:
		void setCurrElemTypeToDraw(const currElemTypeToDraw &type);
		currElemTypeToDraw getCurrElemTypeToDraw() const;
	
		void addElem(std::shared_ptr<Elem>);
		void removeElem(std::shared_ptr<Elem>);
	
	};
}

namespace Controller {
	class DrawCommand {
	public:
		virtual ~DrawCommand();
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

	void handleMouseDown(int x, int y);
	void handleMouseUp(int x, int y);
	void handleMouseMove(int x, int y);

	void handleNewDoc();
	void handleImportDoc(const std::string& filename);
	void handleExportDoc(const std::string& filename);


	class Renderer {
	public:
		void fill(const Model::coordinates &fillPoint, const Model::RGB &color);
		void draw(const std::shared_ptr<Model::Elem> elem);
	};

	class Logic {
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

			m_pUI->setController(std::make_shared<Controller::Logic>(*this));

			return true;
		};
	};

}

int main(int, char **) {
	auto s = std::make_shared<Model::Storage>;
	auto ui = std::make_shared<View::UI>;

	auto l = std::make_shared<Controller::Logic>(s, ui);
	l->run();
	return 0;
} 


