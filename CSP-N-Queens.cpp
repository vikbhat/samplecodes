#include<cstdio>
#include<cstdlib>
#define max 100
int placeQ(int q[max],int s,int n)
{
    int i,j;
    if(s==n)
    {
        printf("\n%d-Queens problem has one of many sol: \n",n);
        for(i=0; i<n; i++)
        {
            for(j=0; j<n; j++)
            {
                if(q[i]==j)
                    printf("Q ");
                else printf("_ ");
            }
            printf("\n");
        }
        return(1);
    }
    else
    {
        for(i=0; i<n; i++)
        {
            bool GOOD=true;
            for(j=0; j<s; j++)
            {
                if((q[j]==i)||((q[j]+s-j)==i)||((q[j]-s+j)==i))
                {
                    GOOD=false;
                    break;
                }
            }
            if(GOOD)
            {
                q[s]=i;
                int r=placeQ(q,s+1,n);
                if(r)
                    return(r);
            }
        }
    }
    return(0);
}
int N_Q(int n)
{
    int q[max];
    for(int i=0; i<n; i++)
    {
        q[i]=-1;
    }
    if(!placeQ(q,0,n))
        printf("\nNO FEASIBLE SOLUTION");
}

int main()
{
    int n;
    printf("Constraint Satisfaction Problem (N-Queens):-\nEnter no of Queens to place: ");
    scanf("%d",&n);
    N_Q(n);
    return(0);
}
