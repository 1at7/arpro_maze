#include <a_star.h>
#include <maze.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>

using namespace std;
using namespace ecn;

// a node is a x-y position, we move from 1 each time
class Position : public Point
{
    typedef std::unique_ptr<Position> PositionPtr;

public:
    // constructor from coordinates
    Position(int _x, int _y) : Point(_x, _y) {}

    // constructor from base ecn::Point
    Position(ecn::Point p) : Point(p.x, p.y) {}

    int distToParent()
    {
        // in cell-based motion, the distance to the parent is always 1
        return 1;
    }

    //typedef std :: unique_ptr < Position > PositionPtr ;
    std::vector<PositionPtr> children()
    {

        // this method should return  all positions reachable from this one
        std::vector<PositionPtr> generated;

        int x_d = x+1;
        int x_u = x-1;
        int y_r = y+1;
        int y_l = y-1;
        if (Position::maze.isFree(x_d,y)){
        generated.push_back(std::make_unique<Position>( x_d , y ));}
        if (Position::maze.isFree(x,y_r)){
        generated.push_back(std::make_unique<Position>( x , y_r ));}
        if (Position::maze.isFree(x,y_l)){
        generated.push_back(std::make_unique<Position>( x , y_l ));}
        if (Position::maze.isFree(x_u,y)){
        generated.push_back(std::make_unique<Position>( x_u , y ));}
        return generated;

    }

};



int main( int argc, char **argv )
{
    // load file
    std::string filename = "maze.png";
    if(argc == 2)
        filename = std::string(argv[1]);

    // let Point know about this maze
    Position::maze.load(filename);

    // initial and goal positions as Position's
    Position start = Position::maze.start(),
             goal = Position::maze.end();

    // call A* algorithm
    ecn::Astar(start, goal);

    // save final image
    Position::maze.saveSolution("cell");
    cv::waitKey(0);

}
