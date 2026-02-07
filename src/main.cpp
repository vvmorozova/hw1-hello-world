#include "lib.h"

#include <memory>
#include <iostream>
#include <vector>
#include <stack>
#include <queue>

enum class currElemTypeToDraw {
	Line,
	Circle,
	Rectangle
};

namespace View {
	class UI {

	};

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

		virtual int draw();

		void setBasePoint(coordinates &newBasePoint);
		coordinates getBasePoint();

		void setColor(RGB newColor);
		RGB getColor();

		void setLayer(int newLayer);
		int getLayer();

		virtual bool isPointInside(coordinates &point);
		virtual void fill();
	};

	class Line: public Elem {
	private:
		coordinates secondPoint;

	public:
		int draw() override;

		void setSecondPoint(coordinates &newSecondPoint);
		coordinates getSecondPoint();
	};

	class Circle: public Elem {
	private:
		int radius;

	public:
		int draw() override;

		void setRadius(coordinates &newRadius);
		coordinates getRadius();

		bool isPointInside(coordinates &point) override;
		void fill() override;
	};

	class Rectangle: public Elem {
	private:
		coordinates SecondPoint;

	public:
		int draw() override;

		void setSecondPoint(coordinates &newSecondPoint);
		coordinates getSecondPoint();

		bool isPointInside(coordinates &point) override;
		void fill() override;
	};

}

namespace Model {
	class Storage {
	private:
		currElemTypeToDraw currType;
		// vector of already drawed elems
		std::vector<std::shared_ptr<View::Elem>> drawed;
		// cached last opened/exported file's names
		std::queue<std::string> lastFiles;
		// for ctrl Z ctrl Y
		std::stack<int> cmdStack;
	public:
		void setCurrElemTypeToDraw(currElemTypeToDraw type);
		currElemTypeToDraw getCurrElemTypeToDraw();

		void addElem(std::shared_ptr<View::Elem>);
		void removeElem(std::shared_ptr<View::Elem>);

	};
}

namespace Controller {
	class Logic {
	private:
		std::shared_ptr<Model::Storage> m_pS;
		std::shared_ptr<View::UI> m_pUI;
	public:
		Logic(std::shared_ptr<Model::Storage> pS, std::shared_ptr<View::UI> pUI) : m_pS(pS), m_pUI(pUI) {};

		int run();
	};

	class DrawCommand {
	private:
		int cmdId;
	public:
		int execute(View::Elem element);
	};

	class DocCommand {
	private:
		std::string m_filename;
	public:
		DocCommand (std::string filename) : m_filename(filename) {};
		virtual int execute();

		void setFilename();
		std::string getFilename();
	};

	class ImportDocCommand : public DocCommand {
	public:
		ImportDocCommand (std::string filename) : DocCommand(filename) {};
		int execute() override;
	};

	class ExportDocCommand : public DocCommand {
	public:
		ExportDocCommand (std::string filename) : DocCommand(filename) {};
		int execute() override;
	};

	class NewDocCommand : public DocCommand {
	public:
		NewDocCommand (std::string filename) : DocCommand(filename) {};
		int execute() override;
	};

}

int main(int, char **) {
	std::cout << "Version: " << version() << std::endl;
	std::cout << "Hello, world!" << std::endl;
	return 0;
} 


