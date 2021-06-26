// input through terminal


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <maze.h>

using namespace ecn;
typedef struct
{
    int x, y; //Node position - little waste of memory, but it allows faster generation
    void *parent; //Pointer to parent node
    char c; //Character to be displayed
    char dirs; //Directions that still haven't been explored
} Node;

Maze *new_maze;
Node *nodes; //Nodes array
int width, height; //Maze dimensions
float P;
void *gen;


int init( )
{
    int i, j;
    Node *n;
    new_maze = new Maze(width,height);

    //Allocate memory for maze
    nodes = (Node *)calloc( width * height, sizeof( Node ) );
    if ( nodes == NULL ) return 1;

    //Setup crucial nodes
    for ( i = 0; i < width; i++ )
    {
        for ( j = 0; j < height; j++ )
        {
            n = nodes + i + j * width;
            if ( i * j % 2 )
            {
                n->x = i;
                n->y = j;
                n->dirs = 15; //Assume that all directions can be explored (4 youngest bits set)
                n->c = ' ';
            }
            else n->c = '#'; //Add walls between nodes
            if ((double)rand()/RAND_MAX < P && !(i==1 && j==1) && !(i==0 || j==0) && !(j==height-1 || i==width-1) && !(j==height-2 && i==width-2)) n->c = ' ';
        }
    }
    return 0;
}

Node *link( Node *n )
{
    //Connects node to random neighbor (if possible) and returns
    //address of next node that should be visited

    int x, y;
    char dir;
    Node *dest;

    //Nothing can be done if null pointer is given - return
    if ( n == NULL ) return NULL;

    //While there are directions still unexplored
    while ( n->dirs )
    {
        //Randomly pick one direction
        dir = ( 1 << ( rand( ) % 4 ) );

        //If it has already been explored - try again
        if ( ~n->dirs & dir ) continue;

        //Mark direction as explored
        n->dirs &= ~dir;

        //Depending on chosen direction
        switch ( dir )
        {
            //Check if it's possible to go right
            case 1:
                if ( n->x + 2 < width )
                {
                    x = n->x + 2;
                    y = n->y;
                }
                else continue;
                break;

            //Check if it's possible to go down
            case 2:
                if ( n->y + 2 < height )
                {
                    x = n->x;
                    y = n->y + 2;
                }
                else continue;
                break;

            //Check if it's possible to go left
            case 4:
                if ( n->x - 2 >= 0 )
                {
                    x = n->x - 2;
                    y = n->y;
                }
                else continue;
                break;

            //Check if it's possible to go up
            case 8:
                if ( n->y - 2 >= 0 )
                {
                    x = n->x;
                    y = n->y - 2;
                }
                else continue;
                break;
        }

        //Get destination node into pointer (makes things a tiny bit faster)
        dest = nodes + x + y * width;

        //Make sure that destination node is not a wall
        if ( dest->c == ' ' )
        {
            //If destination is a linked node already - abort
            if ( dest->parent != NULL ) continue;

            //Otherwise, adopt node
            dest->parent = n;

            //Remove wall between nodes
            nodes[n->x + ( x - n->x ) / 2 + ( n->y + ( y - n->y ) / 2 ) * width].c = ' ';

            //Return address of the child node
            return dest;
        }
    }

    //If nothing more can be done here - return parent's address
    return (Node *)n->parent;
}

void draw( )
{
    int i, j;

    //Outputs maze to terminal - nothing special
    for ( i = 0; i < height; i++ )
    {
        for ( j = 0; j < width; j++ )
        {
            printf( "%c", nodes[j + i * width].c );
            if (nodes[j+ i * width].c == ' ') new_maze->dig(j,i);
        }
        printf( "\n" );
    }
    new_maze->save();
}

int main( int argc, char **argv )
{

    Node *start, *last;
    std::cout<<"Width-- ";
    std::cin>>width;
    std::cout<<"height-- ";
    std::cin>>height;
    std::cout<<"Probability-- ";
    std::cin>>P;

    //Allow only odd dimensions
    if ( !( width % 2 ) || !( height % 2 ) )
    {
        fprintf( stderr, "%s: dimensions must be odd!\n", argv[0] );
        exit( 1 );
    }

    //Do not allow negative dimensions
    if ( width <= 0 || height <= 0 )
    {
        fprintf( stderr, "%s: dimensions must be greater than 0!\n", argv[0] );
        exit( 1 );
    }

    //Seed random generator
    srand( time( NULL ) );

    //Initialize maze
    if ( init( ) )
    {
        fprintf( stderr, "%s: out of memory!\n", argv[0] );
        exit( 1 );
    }

    //Setup start node
    start = nodes + 1 + width;
    start->parent = start;
    last = start;

    //Connect nodes until start node is reached and can't be left
    while ( ( last = link( last ) ) != start );
    draw( );
}






























/*
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <maze.h>
#include <point.h>
#include <cstdlib>
#include <ctime>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

using std::min;
using std::max;

using namespace ecn;

Maze maze1("maze1");

typedef struct
{
    int x, y; //Node position - little waste of memory, but it allows faster generation
    void *parent; //Pointer to parent node
    int c; //Integer to be displayed
    char dirs; //Directions that still haven't been explored
} Node;

Node *nodes; //Nodes array
int width, height; //Maze dimensions
double P;
std::vector<double>WT;
std::vector<double>WX;
std::vector<double>testVector;


int init( )
{
    int i, j;
    Node *n;

    //Allocate memory for maze
    nodes = (Node*)calloc( width * height, sizeof( Node ) );
    if ( nodes == NULL ) return 1;

    //Setup crucial nodes
    for ( i = 0; i < width; i++ )
    {
        for ( j = 0; j < height; j++ )
        {
            n = nodes + i + j * width;

            if ( i * j % 2 )
            {
                n->x = i;
                n->y = j;
                n->dirs = 15; //Assume that all directions can be explored (4 youngest bits set)
                n->c = 0;

            }
            else n->c = 255; //Add walls between nodes
            if ((double)rand()/RAND_MAX < P && !(i==1 && j==1) && !(i==0 || j==0) && !(j==height-1 || i==width-1) && !(j==height-2 && i==width-2)) n->c = 0;
            //if ((j==height-1 || i==width-1)) n->c = 255;            
        }

    }
    for ( i = 1; i < height;)
    {
        for ( j = 1; j < width;){
             n = nodes + j + i * width;

            int c=nodes[j + i * width].c;
            int r=nodes[(j+1) + i * width].c;
            int l=nodes[(j-1) + i * width].c;
            int d=nodes[j + (i+1) * width].c;
            int u=nodes[j + (i-1) * width].c;

            if (r==0){ n->c = 160; j=j+1;}
            else if (r!=0 && d==0){n->c = 160; i=i+1;}
            else if (r!=0 && d!=0 && l==0){n->c = 160; j=j-1;}
            else if (r!=0 && d!=0 && l!=0 && u==255){n->c = 160; i=i-1;}


        }
    }


    return 0;

}




Node *link( Node *n )
{
    //Connects node to random neighbor (if possible) and returns
    //address of next node that should be visited

    int x, y;
    char dir;
    Node *dest;

    //Nothing can be done if null pointer is given - return
    if ( n == NULL ) return NULL;

    //While there are directions still unexplored
    while ( n->dirs )
    {
        //Randomly pick one direction
        dir = ( 1 << ( rand( ) % 4 ) );

        //If it has already been explored - try again
        if ( ~n->dirs & dir ) continue;

        //Mark direction as explored
        n->dirs &= ~dir;

        //Depending on chosen direction
        switch ( dir )
        {
            //Check if it's possible to go right
            case 1:
                if ( n->x + 2 < width )
                {
                    x = n->x + 2;
                    y = n->y;
                }
                else continue;
                break;

            //Check if it's possible to go down
            case 2:
                if ( n->y + 2 < height )
                {
                    x = n->x;
                    y = n->y + 2;
                }
                else continue;
                break;

            //Check if it's possible to go left
            case 4:
                if ( n->x - 2 >= 0 )
                {
                    x = n->x - 2;
                    y = n->y;
                }
                else continue;
                break;

            //Check if it's possible to go up
            case 8:
                if ( n->y - 2 >= 0 )
                {
                    x = n->x;
                    y = n->y - 2;
                }
                else continue;
                break;
        }

        //Get destination node into pointer (makes things a tiny bit faster)
        dest = nodes + x + y * width;

        //Make sure that destination node is not a wall
        if ( dest->c == 0)
        {
            //If destination is a linked node already - abort
            if ( dest->parent != NULL ) continue;

            //Otherwise, adopt node
            dest->parent = n;

            //Remove wall between nodes
            nodes[n->x + ( x - n->x ) / 2 + ( n->y + ( y - n->y ) / 2 ) * width].c =0;

            //Return address of the child node
            return dest;
        }
    }

    //If nothing more can be done here - return parent's address
    return (Node*)n->parent;
}



void Image(std::vector<double>im){
    cv::Mat testMat = cv::Mat(500,1000,CV_8SC1);

    for(int e=0 ;e< im.size();e++){
        //std::cout<<im.at(e)<<' ';
        testVector.push_back(im.at(e));
        }

    testMat=cv::Mat(testVector).reshape(0,height);
    imshow("Maze",testMat);
    imwrite("/home/ecn/ecn_arpro/maze/testMat1.png",testMat);
    cv::waitKey(0);
}


void justprint(std::vector<double>im ){
    for ( int i = 0; i < height; i++ )
    {
        for ( int j = 0; j < width; j++ )
        {
            std::cout<<im.at(i + j * width)<<' ';


        }printf( "\n" );}}



void draw(std::vector<double>im )
{
    int i, j;


    //Outputs maze to terminal - nothing special
    for ( i = 0; i < height; i++ )
    {
        for ( j = 0; j < width; j++ )
        {
            int a=nodes[j + i * width].c;
            printf("%5d", a);
            if (a==0){ WT.push_back(255);im.push_back(255);}
            else if(a==255){ WT.push_back(0);im.push_back(0);}
            else{ WT.push_back(160);im.push_back(160);}


       }
        printf( "\n" );
    }
    Image(im);
    //justprint(im);
}







double huristic(int i ,int j){
    double hur = ((height - j)*(height - j))+((width - i)*(width - i));
    hur = sqrt(hur);
    return hur;
}





void Astar(){

    int i, j;
    Node *n;
    for ( i = 1; i < height;)
    {
        for ( j = 1; j < width;){
             n = nodes + j + i * width;

            int c=nodes[j + i * width].c;
            int r=nodes[(j+1) + i * width].c;
            int l=nodes[(j-1) + i * width].c;
            int d=nodes[j + (i+1) * width].c;
            int u=nodes[j + (i-1) * width].c;

            if (r==0){ n->c = 160; j=j+1; std::cout<<'A';}
            else if (r!=0 && d==0){n->c = 160; i=i+1;}
            else if (r!=0 && d!=0 && l==0){n->c = 160; j=j-1;}
            else if (r!=0 && d!=0 && l!=0 && u==255){n->c = 160; i=i-1;}


        }
    }
//draw(WX);
//Image(WX);
}



int main( int argc, char **argv )
{
    Node *start, *last;
    height=11;
    width=31;
    P =0.3;





    //Check argument count
    if ( argc < 3 )
    {
        fprintf( stderr, "%s: please specify maze dimensions!\n", argv[0] );
        exit( 1 );
    }

    //Read maze dimensions from command line arguments
    if ( sscanf( argv[1], "%d", &width ) + sscanf( argv[2], "%d", &height ) < 2 )
    {
        fprintf( stderr, "%s: invalid maze size value!\n", argv[0] );
        exit( 1 );
    }

    //Allow only odd dimensions
    if ( !( width % 2 ) || !( height % 2 ) )
    {
        fprintf( stderr, "%s: dimensions must be odd!\n", argv[0] );
        exit( 1 );
    }

    //Do not allow negative dimensions
    if ( width <= 0 || height <= 0 )
    {
        fprintf( stderr, "%s: dimensions must be greater than 0!\n", argv[0] );
        exit( 1 );
    }





    //Seed random generator
    srand( time( NULL ) );

    //Initialize maze
    if ( init( ) )
    {
        fprintf( stderr, "%s: out of memory!\n", argv[0] );
        exit( 1 );
    }

    //Setup start node
    start = nodes + 1 + width;
    start->parent = start;
    last = start;

    //Connect nodes until start node is reached and can't be left
    while ( ( last = link( last ) ) != start );

    draw(WX);





/*
cv::Mat testMat = cv::Mat(500,1000,CV_8SC1);

    for(int e=0 ;e< WT.size();e++){
        std::cout<<WT.at(e)<<' ';
        testVector.push_back(WT.at(e));
    }

    testMat=cv::Mat(testVector).reshape(0,height);
    imshow("testMat",testMat);
    imwrite("/home/ecn_arpro/maze/testMat.png",testMat);
    cv::waitKey(0);




}
*/


/*


    cv::Mat testMat1 = cv::Mat(500,1000,CV_8SC1);

        for(int e=0 ;e< WX.size();e++){
            std::cout<<WX.at(e)<<' ';
            testVector.push_back(WX.at(e));
        }

        testMat1=cv::Mat(testVector).reshape(0,height);
        imshow("testMat",testMat1);
        imwrite("/home/Desktop/testMat1.png",testMat1);
        cv::waitKey(0);

*/




/*
    Maze(height,width);
    cv::Mat testMat = cv::Mat(height, width, CV_8UC1, cv::Scalar(0));
    maze1.display("test",testMat);
*/








