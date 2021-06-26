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
    int x_d,x_u,y_r,y_l,a,c;
    int i=1;
    int distance;




    // constructor from coordinates
    Position(int _x, int _y, int dist):Point(_x, _y),distance(dist){}

    // constructor from base ecn::Point
    Position(ecn::Point p) : Point(p.x, p.y) {}

    int distToParent()
    {   return distance;

        // in cell-based motion, the distance to the parent is always 1

    }

    void direc(int i){
        //int a,b;
        if (i == 0) {a= -1;c = 0;}
        if (i == 1){a= 1; c=0;}
        if (i == 2){a= 0; c=-1;}
        if (i == 3){a= 0; c= 1;}
    }

    bool is_corridor(int i, int j)
        {
            if(!maze.isFree(i, j))
                return false;
            double cell_free = (maze.isFree(i+1, j) + maze.isFree(i-1, j)
                    + maze.isFree(i, j+1) + maze.isFree(i, j-1));
            return cell_free== 2.0;
        }

    std::vector<PositionPtr> children()
    {

        // this method should return  all positions reachable from this one
        std::vector<PositionPtr> generated;

        for (int i=0; i<4;i++){
                int k = 1;
                direc(i);
                while(is_corridor(x+a*k, y+c*k)) k++;
                if(!maze.isFree(x+a*k, y+c*k)) k--;
                if(k) generated.push_back(std::make_unique<Position>(x+a*k, y+c*k, k));

        }
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
    Position::maze.saveSolution("line");
    cv::waitKey(0);

}






