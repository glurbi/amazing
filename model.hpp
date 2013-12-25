#ifndef _model_hpp_
#define _model_hpp_

#include <vector>
#include <set>
#include <memory>

#include "geometry.hpp"

struct Cell {
	int x;
	int y;
	bool wall;
	struct Comp	{
		bool operator() (const Cell& c1, const Cell& c2) const;
	};
};

class MazeModel {
public:
    MazeModel(int width_, int height_);
    void create();
    int get_width();
    int get_height();
    inline Cell& getCell(int x, int y) { return cells[x+y*width]; }
    inline std::vector<Cell>& getCells() { return cells; }
	inline bool contains(Cell& c, std::set<Cell, Cell::Comp>& visited) { return visited.find(c) != visited.end(); }
private:
    void visit(Cell& c, std::set<Cell, Cell::Comp>& visited);
    int width;
	int height;
	std::vector<Cell> cells;
	unsigned seed;
};

class MazeGeometryBuilder2D {
public:
    MazeGeometryBuilder2D(MazeModel& model_);
    std::shared_ptr<Geometry<float>> build();
private:
    MazeModel& model;
};

class MazeGeometryBuilder3D {
public:
    MazeGeometryBuilder3D(MazeModel& model_);
    std::shared_ptr<Geometry<float>> build();
private:
    MazeModel& model;
};

#endif
