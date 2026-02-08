#pragma once
#include <memory>
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <string>

namespace Model {
	enum class currElemTypeToDraw {
		Line,
		Circle,
		Rectangle
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
		virtual ~Elem() {};

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