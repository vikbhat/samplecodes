/* TIC-TAC-TOE Game AI using MINI-MAX Algorithm
   By
   Name: Vikram Bhat
   Div: BE-H
   RollNo: 16
   Batch: 1
*/

#include<stdio.h>
#include<stdlib.h>
char board[3][3];
int win_pos[9]= {0};
void initialize()
{
    int i,j;
    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
        {
            board[i][j]='_';
            win_pos[i*3+j]=0;
        }
    }
}

int END(int turn)
{
    int i;
    if(turn>2)
    {
        for( i=0; i<8; i++)
        {
            if(win_pos[i]==60)
                return(0);
            if(win_pos[i]==-30)
                return(1);
        }
        if(turn==9)
            return(2);
    }
    return(-1);
}

void addpos(int move,int f)
{
    int k,m;
    k=move/3;
    m=move%3;

    win_pos[k]+=f;
    win_pos[m+3]+=f;
    if(k==m)
    {
        win_pos[6]+=f;
    }
    if(k==2-m)
    {
        win_pos[7]+=f;
    }
}

void disp_board(int player,int move)
{
    char p;
    int i,j,k,m,f;
    if(player==0)
    {
        p='X';
        f=20;
    }
    else
    {
        p='O';
        f=-10;
    }

    if(move>=0)
    {
        board[move/3][move%3]=p;
        addpos(move,f);
    }

    printf("\nBoard Status: \n");

    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
        {
            printf("|%c",board[i][j]);
        }
        printf("\n");
    }

}

int calpos()
{
    int pos=0,i;
    for(i=0; i<8; i++)
    {
        if(win_pos[i]==60)
            return(10);
        if(win_pos[i]==-30) return(-10);
        if(win_pos[i]>0)
        {
            if((win_pos[i]/10)%2==0)
                pos=pos+1;
        }
        else if(win_pos<0)
        {
            pos=pos-1;
        }
    }
    return(pos);
}

int user_move()
{
    while(1)
    {
        int c;
        printf("\nPlease enter the next move: (0-8):\n");
        scanf("%d",&c);
        if(board[c/3][c%3]=='_')
            return(c);
        else printf("\nThe position is occupied already!!\n");
    }
}


int chose_move(int player,int ply,int s,int turn,int prefact)
{
    int i=0,k,m,currfact[2]= {-8,8};
    int minmax[2],f=0,next=0,cminmax[2],curr;
    char p='_';
    minmax[0]=-100;
    minmax[1]=100;
    curr=calpos();
    if(ply==s||turn==9||curr==-10||curr==10)
    {
//printf("\nply%d %d",ply,curr);
        return(curr);
    }

    for(i=0; i<9; i++)
    {
        k=i/3;
        m=i%3;
        if(board[k][m]=='_')
        {
//printf("\nply%d entering %d",s,i);
            if(player==0)
            {
                f=20;
                p='X';
            }
            else
            {
                f=-10;
                p='O';
            }
            board[k][m]=p;
            addpos(i,f);
            next=chose_move(!player,ply,s+1,turn+1,currfact[player]);
//printf("\nply%d child%d value %d",ply,i,next);
            board[k][m]='_';
            addpos(i,-f);
            if(player==0&&next==10&&s==0)
                return(i);
            if(player==1&&next==-10&&s==0)
                return(i);
            if(minmax[1]>next)
            {
                currfact[1]=minmax[1]=next;
                cminmax[1]=i;
            }
            if(minmax[0]<next)
            {
                currfact[0]=minmax[0]=next;
                cminmax[0]=i;
            }
        }
        if(player==0&&prefact<=currfact[player])
        {
            break;
        }
        if(player==1&&prefact>=currfact[player])
        {
            break;
        }
    }
    if(s==0)
    {
        return(cminmax[player]);
    }
    return(minmax[player]);
}

int r_move()
{
    int r;
    printf("\nCOMP is making a move ....\n");
    sleep(1);
    srand(time(0));
    do
    {
        r=rand()%9;
    }
    while(board[r/3][r%3]!='_');
    printf("%d",r);
    return(r);
}

int main()
{
    char p1,p2;
    int end=-1,turn,human=-1,next_move=-1,ply_max=8,ch,player,inifact[2]= {8,-8};
    while(1)
    {
        printf("\n1.select X\n2.select O\n3.simulation\n4.quit\n");
        scanf("%d",&ch);
        if(ch==4)
        {
            printf("\nthank u for playing game :) ");
            break;
        }
        initialize();
        turn=0;
        human=-1;
        if(ch==1)
        {
            human=0;
        }
        else if(ch==2)
        {
            human=1;
        }
        disp_board(-1,-1);
        while((end=END(turn))<0)
        {
            player=turn%2;
            if(human==player)
                next_move=user_move();
            else
            {
                printf("\nCOMP%d is choosing move....",player);
                sleep(0.5);
                next_move=chose_move(player,ply_max,0,turn,inifact[player]);
                printf("\nmove choosen: %d",next_move);
            }
            disp_board(player,next_move);
            turn++;
        }

        if(end==0)
        {
            if(human==0)
                printf("\nCongratulations you win !!!!");
            else printf("\nCOMP1 wins the game");
        }
        else if(end==1)
        {
            if(human==1)
                printf("\nCongratulations you win !!!!");
            else printf("\nCOMP2 wins the game");
        }
        else printf("The game ended as a draw");
    }
    return(0);
}

/*
OUTPUT:
1.select X
2.select O
3.simulation
4.quit
3

Board Status:
|_|_|_
|_|_|_
|_|_|_

COMP0 is choosing move....
move choosen: 0
Board Status:
|X|_|_
|_|_|_
|_|_|_

COMP1 is choosing move....
move choosen: 4
Board Status:
|X|_|_
|_|O|_
|_|_|_

COMP0 is choosing move....
move choosen: 1
Board Status:
|X|X|_
|_|O|_
|_|_|_

COMP1 is choosing move....
move choosen: 2
Board Status:
|X|X|O
|_|O|_
|_|_|_

COMP0 is choosing move....
move choosen: 6
Board Status:
|X|X|O
|_|O|_
|X|_|_

COMP1 is choosing move....
move choosen: 3
Board Status:
|X|X|O
|O|O|_
|X|_|_

COMP0 is choosing move....
move choosen: 5
Board Status:
|X|X|O
|O|O|X
|X|_|_

COMP1 is choosing move....
move choosen: 7
Board Status:
|X|X|O
|O|O|X
|X|O|_

COMP0 is choosing move....
move choosen: 8
Board Status:
|X|X|O
|O|O|X
|X|O|X
The game ended as a draw
1.select X
2.select O
3.simulation
4.quit
4
*/

