#ifndef _model_hpp_
#define _model_hpp_

#include <vector>
#include <set>

struct Cell {

	int x;
	int y;
	bool wall;

	struct Comp	{
		bool operator() (const Cell& c1, const Cell& c2) const;
	};

};

class Model {

public:

    Model(int width_, int height_);
    inline Cell& getCell(int x, int y) { return cells[x+y*width]; }
    void createMaze();
	inline bool contains(Cell& c, std::set<Cell, Cell::Comp>& visited) { return visited.find(c) != visited.end(); }
    void visit(Cell& c, std::set<Cell, Cell::Comp>& visited);

private:

	int width;
	int height;
	std::vector<Cell> cells;
	unsigned seed;

};

#endif
