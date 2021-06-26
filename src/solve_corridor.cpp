#include <a_star.h>
#include <maze.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */


using namespace std;
using namespace ecn;

// a node is a x-y position, we move from 1 each time
class Position : public Point
{
    typedef std::unique_ptr<Position> PositionPtr;

public:
    int x_d,x_u,y_r,y_l,a,c;

    int distance;
    int x_g, y_g, x_s, y_s;



    void initial(Position start, Position goal)
    {
        x_s = start.x;
        y_s = start.y;
        x_g = goal.x;
        y_g = goal.y;
    }



    // constructor from coordinates
    Position(int _x, int _y, int dist):Point(_x, _y),distance(dist){}

    // constructor from base ecn::Point
    Position(ecn::Point p) : Point(p.x, p.y) {}

    int distToParent()
    {   return distance;

    }

    void direc(int i){
        //int a,b;
        if (i == 0) {a= -1;c = 0;}
        if (i == 1){a= 1; c=0;}
        if (i == 2){a= 0; c=-1;}
        if (i == 3){a= 0; c= 1;}
    }

    bool is_corridor(int _x, int _y, int &i, int &j)
    {   int x_p = (maze.isFree(_x+i, _y) +maze.isFree(_x, _y+1) +maze.isFree(_x, _y-1) );
        int y_p = (maze.isFree(_x, _y+j) + maze.isFree(_x+1, _y) +maze.isFree(_x-1, _y));
        if(i && x_p == 1)
        {
            if(maze.isFree(_x, _y-1))
            {
                i = 0;
                j = -1;
            }
            else if(maze.isFree(_x, _y+1))
            {
                i = 0;
                j = 1;
            }
            return true;
        }
        else if(j && y_p == 1)
        {
            if(maze.isFree(_x-1, _y))
            {
                i = -1;
                j = 0;
            }
            else if(maze.isFree(_x+1, _y))
            {
                i = 1;
                j = 0;
            }
            return true;
        }
        return false;
    }

    void print(const Point & parent)
        {
            if(parent.x || parent.y){
                std::vector<std::pair<int, int>> next;
                for (int i=0; i<4;i++){
                    direc(i);
                    next.clear();
                    int k = 0;
                    int x_n = x+a;
                    int y_n = y+c;
                    if(maze.isFree(x_n, y_n))
                    {
                        next.push_back({x_n, y_n});
                        k = 1;
                        while(is_corridor(x_n, y_n, a, c) && (x_n != x_s || y_n != y_s))
                        {
                            x_n += a;
                            y_n += c;
                            next.push_back({x_n, y_n});
                            k++;
                        }

                        if(x_n == parent.x && y_n == parent.y)
                        {
                            for(auto p = next.rbegin(); p!=next.rend(); p++)
                                maze.passThrough(p->first, p->second);
                                break;
                        }
                    }
                }
            }
            maze.passThrough(x, y);

        }



    void show(bool closed,const Point &parent)
        {
            const int b = closed?255:0, r = closed?0:255;
            if(parent.x || parent.y)
            {
                // look for path to parent
                std::vector<std::pair<int, int>> n_m;
                n_m.reserve(10);
                for (int i=0; i<4;i++){
                    direc(i);
                    int k = 0;
                    n_m.clear();
                    int x_n = x+a;
                    int y_n = y+c;

                    n_m.clear();

                    // follow corridor till end or crossing


                    if(maze.isFree(x_n, y_n))
                    {
                        n_m.push_back({x_n, y_n});
                        k = 1;
                        while(is_corridor(x_n, y_n, a, c) && (x_n != x_s || y_n != y_s))
                        {
                            x_n += a;
                            y_n += c;
                            n_m.push_back({x_n, y_n});
                            k++;
                        }

                        if(x_n == parent.x && y_n == parent.y)
                        {
                            for(const auto &p: n_m)
                                maze.write(p.first, p.second, r, 0, b, false);
                            break;
                        }
                    }
                }
            }
            maze.write(x, y, r, 0, b);
        }






    //typedef std :: unique_ptr < Position > PositionPtr ;
    std::vector<PositionPtr> children()
    {
        // this method should return  all positions reachable from this one
        std::vector<PositionPtr> generated;

        for (int i=0; i<4;i++)
        {       direc(i);
                int k=0;
                int x_n= x+a ,y_n = y+c;
                if(maze.isFree(x_n,y_n))
                {
                    k = 1;
                    while(is_corridor(x_n, y_n,a,c)&& (x_n != x_s || y_n != y_s))
                    {
                        x_n += a;
                        y_n +=c;
                        k++;
                        //std::cout<<x_n<<' '<<y_n<<' '<<k<<std::endl;
                    }
                    //std::cout<<" we are here"<<endl;
                    generated.push_back(std::make_unique<Position>(x_n, y_n, k));
                }

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
    start.initial(start, goal);

    // call A* algorithm
    ecn::Astar(start, goal);

    // save final image
    Position::maze.saveSolution("corridor");
    cv::waitKey(0);

}

