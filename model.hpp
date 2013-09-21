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
    void createMaze();
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

#endif
