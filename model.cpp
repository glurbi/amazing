#include <vector>
#include <set>
#include <bitset>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <random>

struct Cell {

	int x;
	int y;
	bool wall;

	struct Comp	{
		bool operator() (const Cell& c1, const Cell& c2) const {
			if (c1.x < c2.x) {
				return true;
			} else if (c1.x == c2.x)
				return c1.y < c2.y;
			else {
				return false;
			}
		}
	};

};

class Model {

private:

	int width;
	int height;
	std::vector<Cell> cells;
	unsigned seed;

public:

    Model(int width_, int height_)
		: width(width_), height(height_),
		  cells(width*height, Cell())
	{
		seed = std::chrono::system_clock::now().time_since_epoch().count();
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				getCell(i,j).x = i;
				getCell(i,j).y = j;
				getCell(i,j).wall = true;
			}
		}
	}

	inline Cell& getCell(int x, int y) {
		Cell& cell = cells[x+y*width];
		return cell;
	}

    void createMaze() {
        std::set<Cell, Cell::Comp> visited;
        visit(getCell(1,1), visited);
    }

	inline bool contains(Cell& c, std::set<Cell, Cell::Comp>& visited) {
		return visited.find(c) != visited.end();
	}

    void visit(Cell& c, std::set<Cell, Cell::Comp>& visited) {
		c.wall = false;
        visited.insert(c);
        std::vector<Cell*> neighbors;
		if ((c.x > 2) && !contains(getCell(c.x-2, c.y), visited)) neighbors.push_back(&getCell(c.x-2, c.y));
		if ((c.x < width-2) && !contains(getCell(c.x+2, c.y), visited)) neighbors.push_back(&getCell(c.x+2, c.y));
		if ((c.y > 2) && !contains(getCell(c.x, c.y-2), visited)) neighbors.push_back(&getCell(c.x, c.y-2));
		if ((c.y < height-2) && !contains(getCell(c.x, c.y+2), visited)) neighbors.push_back(&getCell(c.x, c.y+2));
		std::random_shuffle(neighbors.begin(), neighbors.end());
        for (auto& n : neighbors) {
			if (visited.find(*n) != visited.end()) {
                continue;
            }
			int wx = (n->x + c.x) / 2;
			int wy = (n->y + c.y) / 2;
			getCell(wx, wy).wall = false;
            visit(*n, visited);
        }
    }
};

/*
int main(int argc, char** argv) {
	int w = 55;
	int h = 55;
	Model m(w,h);
	m.createMaze();
	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			if (m.getCell(i, j).wall) {
				std::cout << "0";
			} else {
				std::cout << " ";
			}
		}
		std::cout << std::endl;
	}
	std::cin.ignore();
	return 0;
}
*/