#ifndef _model_hpp_
#define _model_hpp_

#include <vector>
#include <set>
#include <memory>

#include "geometry.hpp"

struct cell {
	int x;
	int y;
	bool wall;
	struct comp	{
		bool operator() (const cell& c1, const cell& c2) const;
	};
};

class maze_model {
public:
    maze_model(int width_, int height_);
    void create();
    int get_width();
    int get_height();
    inline cell& get_cell(int x, int y) { return cells[x+y*width]; }
    inline bool is_wall(int x, int y) { return (x < 0) || (x >= width) || (y < 0) || (y >= height) || get_cell(x, y).wall; }
    inline bool is_wall(float x, float y) { return is_wall((int)floor(x), (int)floor(y)); }
    inline std::vector<cell>& get_cells() { return cells; }
	inline bool contains(cell& c, std::set<cell, cell::comp>& visited) { return visited.find(c) != visited.end(); }
private:
    void visit(cell& c, std::set<cell, cell::comp>& visited, int& count);
    int width;
	int height;
	std::vector<cell> cells;
	unsigned seed;
};

class maze_geometry_builder_2d {
public:
    maze_geometry_builder_2d(maze_model& model_);
    std::shared_ptr<geometry<float>> build();
private:
    maze_model& model;
};

class maze_geometry_builder_3d {
public:
    maze_geometry_builder_3d(maze_model& model_);
    std::shared_ptr<geometry<float>> build();
private:
    maze_model& model;
};

class hero_builder_2d {
public:
    hero_builder_2d();
    std::shared_ptr<geometry<float>> build();
};

class multi_hero_builder_2d {
public:
    multi_hero_builder_2d();
    std::shared_ptr<geometry<float>> build();
};


#endif
