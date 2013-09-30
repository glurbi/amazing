#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <random>

#include "model.hpp"

bool Cell::Comp::operator() (const Cell& c1, const Cell& c2) const {
	if (c1.x < c2.x) {
		return true;
	} else if (c1.x == c2.x)
		return c1.y < c2.y;
	else {
		return false;
	}
}

MazeModel::MazeModel(int width_, int height_) :
    width(width_), height(height_), cells(width*height, Cell())
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

void MazeModel::create() {
    std::set<Cell, Cell::Comp> visited;
    visit(getCell(1,1), visited);
}

void MazeModel::visit(Cell& c, std::set<Cell, Cell::Comp>& visited) {
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

MazeGeometryBuilder2D ::MazeGeometryBuilder2D(MazeModel& model_) : model(model_) {}

std::shared_ptr<Geometry2D> MazeGeometryBuilder2D::build() {
    std::vector<float> v;
    for (auto& cell : model.getCells()) {
        if (cell.wall) {
            v.push_back(cell.x-0.5f); v.push_back(cell.y-0.5f);
            v.push_back(cell.x+0.5f); v.push_back(cell.y-0.5f);
            v.push_back(cell.x+0.5f); v.push_back(cell.y+0.5f);
            v.push_back(cell.x-0.5f); v.push_back(cell.y+0.5f);
        }
    }
    auto mazeGeom = std::make_shared<Geometry2D>(Geometry2D(v.size()/2));
    mazeGeom->SetVertexPositions(&v[0], v.size() * sizeof(float));
    return mazeGeom;
}

MazeGeometryBuilder3D ::MazeGeometryBuilder3D(MazeModel& model_) : model(model_) {}

std::shared_ptr<Geometry3D> MazeGeometryBuilder3D::build() {
    BufferObjectBuilder<float> posb;
    BufferObjectBuilder<float> norb;
    for (auto& cell : model.getCells()) {
        if (cell.wall) {
            // top
            posb << cell.x-0.5f << cell.y-0.5f << 1.0f;
            posb << cell.x+0.5f << cell.y-0.5f << 1.0f;
            posb << cell.x+0.5f << cell.y+0.5f << 1.0f;
            posb << cell.x-0.5f << cell.y+0.5f << 1.0f;
            TIMES(4, norb << 0.0f << 0.0f << 1.0f);
            // right
            posb << cell.x+0.5f << cell.y-0.5f << 1.0f;
            posb << cell.x+0.5f << cell.y-0.5f << 0.0f;
            posb << cell.x+0.5f << cell.y+0.5f << 0.0f;
            posb << cell.x+0.5f << cell.y+0.5f << 1.0f;
            TIMES(4, norb << 1.0f << 0.0f << 0.0f);
            // front
            posb << cell.x-0.5f << cell.y-0.5f << 1.0f;
            posb << cell.x+0.5f << cell.y-0.5f << 1.0f;
            posb << cell.x+0.5f << cell.y-0.5f << 0.0f;
            posb << cell.x-0.5f << cell.y-0.5f << 0.0f;
            TIMES(4, norb << 0.0f << -1.0f << 0.0f);
        }
    }
    auto mazeGeom = std::make_shared<Geometry3D>(Geometry3D(posb.GetSize()/3));
    mazeGeom->SetVertexPositions(posb.Build());
    mazeGeom->SetVertexNormals(norb.Build());
    return mazeGeom;
}
