/* Robot Navigation System (A* Algorithm)
   By
   Name: Vikram Bhat
   Div: BE-H
   RollNo: 16
   Batch: 1
*/

#include <iostream>
#include <queue>
#include <iomanip>
#include <cstdlib>
#include <cmath>

using namespace std;



struct State
{
    int x,y;
    float gx,hx;
};

struct PrevState
{
    int px,py;
};

class CompareState
{
public:
    bool operator()(State& t1,State& t2)
    {
        if(t1.gx+t1.hx>t2.gx+t2.hx)
            return(true);
        else return(false);
    }
};

class RoboNav
{
public:
    int** grid;
    int ** status;
    PrevState** prev;
    int row,col;
    int start[2];
    int target[2];
    int nodecount;
    RoboNav()
    {

    }
    void get_grid(int,int);
    void print_grid();
    void find_path();
    void construct_path(State t);
private:
    int cal_heuristic(int,int);
    bool check_target(State);
    bool valid(int x,int y);
};

void RoboNav:: construct_path(State t )
{
    int currx=t.x,curry=t.y;
    int output[row][col];
    for(int i=0; i<row; i++)
    {
        for(int j=0; j<col; j++)
        {
            output[i][j]=0;
        }
    }
    do
    {
        output[currx][curry]=1;
//cout<<"path - ("<<currx<<","<<curry<<")";
        PrevState temp=prev[currx][curry];
        currx=temp.px;
        curry=temp.py;
    }
    while(!(currx==start[0]&&curry==start[1]));
    cout<<"\n\nThe Best path by A* is :\n";
    for(int i=0; i<row; i++)
    {
        cout<<"\n";
        for(int j=0; j<col; j++)
        {
            if(start[0]==i&&start[1]==j)
                cout<<"S ";
            else if(target[0]==i&&target[1]==j)
                cout<<"T ";
            else cout<<output[i][j]<<" ";
        }
    }
    cout<<"\nTotal nodes visited : "<<nodecount;
}

bool RoboNav:: check_target(State t)
{
    if(target[0]==t.x&&target[1]==t.y)
        return(true);
    return(false);
}

int RoboNav::cal_heuristic(int x,int y)
{
//return(abs(target[0]-x)+abs(target[1]-y));
    int dx=abs(target[0]-x);
    int dy=abs(target[1]-y);
    if(dx>dy)
        return(dx);
    return(dy);
}

void RoboNav:: get_grid(int m,int n)
{
    row=m;
    col=n;
    grid=new int*[m];
    status=new int*[m];
    prev=new PrevState*[m];
    nodecount=0;
    for(int i=0; i<n; i++)
    {
        grid[i]=new int[n];
        status[i]=new int[n];
        prev[i]=new PrevState[n];
    }
    srand(time(0));
    cout<<"\nEnter the Grid:\n";
    for(int i=0; i<m; i++)
    {
        for(int j=0; j<n; j++)
        {
            cin>>grid[i][j];
            status[i][j]=-1;
        }
    }
    cout<<"\nGrid generated\n";
    cout<<"\nEnter start node: ";
    cin>>start[0]>>start[1];
    cout <<"\nEnter target node: ";
    cin>>target[0]>>target[1];

}

void RoboNav:: print_grid()
{
    cout<<"\n Grid: \n";
    for(int i=0; i<row; i++)
    {
        cout<<"\n";
        for(int j=0; j<col; j++)
        {
            cout<<grid[i][j]<<" ";
        }
    }
}

bool RoboNav:: valid(int x,int y)
{
    if(x<0||x>=row||y<0||y>=col)
        return(false);
    if(status[x][y]==0||status[x][y]==1||grid[x][y]==1)
        return(false);
    return(true);
}

void RoboNav:: find_path()
{
    priority_queue<State, vector<State>, CompareState> openset;
    State startnode;
    startnode.x=start[0];
    startnode.y=start[1];
    startnode.hx=cal_heuristic(startnode.x,startnode.y);
    startnode.gx=0;
    status[start[0]][start[1]]=1;
    openset.push(startnode);
    while(!openset.empty())
    {
        cout<<"\nEntering.... ";
        State curr=openset.top();
        cout<<"("<<curr.x<<","<<curr.y<<"): "<<curr.gx<<" "<<curr.hx<<"\n";
        if(check_target(curr))
        {
            construct_path(curr);
            return;
        }
        int xs=curr.x-1;
        int ys=curr.y-1;
        for(int i=0; i<=8; i=i+1)
        {
            int xk=xs+i/3,yk=ys+i%3;
            if(valid(xk,yk))
            {
                nodecount++;
                cout<<"nb: ("<<xk<<","<<yk<<")\n";
                State nb;
                nb.x=xk;
                nb.y=yk;
                nb.hx=cal_heuristic(xk,yk);
                nb.gx=curr.gx+1;
                prev[xk][yk].px=curr.x;
                prev[xk][yk].py=curr.y;
                openset.push(nb);
                status[nb.x][nb.y]=1;
            }
        }
        status[curr.x][curr.y]=0;
        openset.pop();
    }
}


int main()
{
    priority_queue<State, vector<State>, CompareState> pq;
    RoboNav rn;
    int n,m;
    cout<<"Enter grid dimensions: ";
    cin>>n>>m;
    rn.get_grid(n,m);
    rn.print_grid();
    rn.find_path();

    return 0;
}

/*
OUTPUT:-
Enter grid dimensions: 5 5

Enter the Grid:
0 0 0 0 0
0 1 1 1 0
0 0 0 1 0
0 0 1 1 0
0 0 0 0 0

Grid generated

Enter start node: 4 4

Enter target node: 2 2

 Grid:

0 0 0 0 0
0 1 1 1 0
0 0 0 1 0
0 0 1 1 0
0 0 0 0 0
Entering.... (4,4): 0 2
nb: (3,4)
nb: (4,3)

Entering.... (3,4): 1 2
nb: (2,4)

Entering.... (4,3): 1 2
nb: (4,2)

Entering.... (2,4): 2 2
nb: (1,4)

Entering.... (4,2): 2 2
nb: (3,1)
nb: (4,1)

Entering.... (3,1): 3 1
nb: (2,0)
nb: (2,1)
nb: (2,2)
nb: (3,0)
nb: (4,0)

Entering.... (2,2): 4 0


The Best path by A* is :

0 0 0 0 0
0 0 0 0 0
0 0 T 0 0
0 1 0 0 0
0 0 1 1 S

*/

