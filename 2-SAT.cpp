#include<iostream>
#include<vector>
#include<algorithm>
#include<cstdio>
#include<cmath>
#include<cstdlib>
#include<ctime>
#include<utility>
using namespace std;


inline int imap(int idx,int n) {
	idx = idx+n;
	if(idx>n)
		idx--;
	return idx;
}
inline int neg(int idx,int n) {
	if(idx<n) {
		return 2*n-idx-1;
	}	
	else return -(idx-2*n+1);
}

void dfs(int u,vector<int>* edges,int* comps,int cc) {
	comps[u] = cc;
	for(int i=0;i<edges[u].size();i++) {
		int v = edges[u][i];
		if(comps[v]==-1)
			dfs(v,edges,comps,cc);
	}
}

void dfs_sort(int u,vector<int>* edges,int* visited,vector<int>&sorted) {
	visited[u] = 1;
	for(int i=0;i<edges[u].size();i++) {
		int v = edges[u][i];
		if(!visited[v])
			dfs_sort(v,edges,visited,sorted);
	}
	sorted.push_back(u);
}

int SCC(vector<int>*edges,int* comps,int n) {
	int visited[n];
	for(int i=0;i<n;i++) {
		visited[i] = 0;
		comps[i] = -1;
	}
	vector<int> revedges[n];
	for(int i=0;i<n;i++) {
		for(int j=0;j<edges[i].size();j++) {
			int v = edges[i][j];
			revedges[v].push_back(i);
		}
	}
	int cc = 0;
	vector<int> sorted;
	for(int i=0;i<n;i++) {
		if(!visited[i]) 
			dfs_sort(i,edges,visited,sorted);
	}
	for(int i=sorted.size()-1;i>=0;i--) {
		if(comps[sorted[i]]==-1) {
			dfs(sorted[i],revedges,comps,cc);
			cc++;
		}
		//cout<<sorted[i]<<endl;
	}
	//for(int i=0;i<n;i++) 
	//	cout<<comps[i]<<endl;
	return cc;
}

void find_assign(int (*clauses)[2],int M,int* assign,int N) {
	while(1) {
		for(int i=0;i<N;i++) {
			assign[i] = rand()%2; 
		}
		/*for(int i=0;i<N;i++) {
			cout<<assign[i];
		}*/
		//cout<<endl;
		int u,v,c1,c2;
		for(int i=0;i<2*N*N;i++) {
			vector<int> invalids;
			for(int j=0;j<M;j++) {
				u = clauses[j][0];
				v = clauses[j][1];
				int c1 = assign[abs(u)-1];
				int c2 = assign[abs(v)-1];
				if(u<0)
					c1 = 1-c1;
				if(v<0)
					c2 = 1-c2;
				if(!(c1|c2)) {
					invalids.push_back(j);
				}
				
			}
			/*for(int j=0;j<N;j++) {
				cout<<assign[j];
			}
			cout<<endl;*/			
			if(invalids.size()==0) {
				//cout<<"returning..";
				return ;
			}
			int selc = invalids[rand()%invalids.size()];
			int var[2];
			var[0] = clauses[selc][0];
			var[1] = clauses[selc][1];
			int idx = rand()%2;
			int c = 1;
			if(var[idx]<0)
				c = 0;
			assign[abs(var[idx])-1] = c;
			
		}
	}
}

int find_another_assign(int (*clauses)[2],int M,int* valid_assign,int* another_assign,int N) {
	srand(time(NULL));
	for(int i=0;i<20;i++) {
		find_assign(clauses,M,another_assign,N);
		int flag = 0;
		for(int j=0;j<N;j++) {
			if(valid_assign[j]!=another_assign[j]) {
				flag = 1;
				break;
			}
		}
		if(flag) {
			return 1;
		}
	}
	return 0;
}

int find_assignments(vector<int>* sccs,vector<int>* scc_edges,int* scc_assign,int k,int scc,int* comps,int N,int more) {
	if(k<0) {
		for(int i=0;i<N;i++) {
			cout<<scc_assign[comps[N+i]];
			if(i<N-1)
				cout<<" ";
		}
		cout<<endl;
		return 1;
	}
	else {
		int mret = 0;
		if(scc_assign[k]==-1) {
			for(int m=1;m>=0;m--) {
				if(m==1) {
					int flag = 0;
					for(int j=0;j<scc_edges[k].size();j++) {
						int c = scc_edges[k][j];
						if(scc_assign[c]==0) {
							flag = 1;
							break;
						}
					}
					if(flag) 
						continue;
				}
				for(int j=0;j<sccs[k].size();j++) {
					int nc = comps[neg(sccs[k][j],N)];
					scc_assign[nc] = 1-m;
				}
				scc_assign[k]=m;
				int ret = find_assignments(sccs,scc_edges,scc_assign,k-1,scc,comps,N,more);
				mret = mret|ret;
				if(ret&&!more)
					return 1;
				scc_assign[k]=-1;
				for(int j=0;j<sccs[k].size();j++) {
					int nc = comps[neg(sccs[k][j],N)];
					scc_assign[nc] = -1;
				}
			}
			return mret;
		}
		else {
			for(int i=0;i<sccs[k].size();i++) {
				int nc = comps[neg(sccs[k][i],N)];
				if(scc_assign[nc]!=-1&&(scc_assign[nc]==scc_assign[k])) {
					return 0;
				}
			}
			if(scc_assign[k]) {
				for(int j=0;j<scc_edges[k].size();j++) {
						int c = scc_edges[k][j];
						if(scc_assign[c]==0) {
							return 0;
						}
				}
			}
			int ret = find_assignments(sccs,scc_edges,scc_assign,k-1,scc,comps,N,more);
			if(ret&&!more)
				return 1;
			return ret;
		}
	}
}




int main() {

	int N,M;
	int u,v;
	int cu,cv;
	cin>>N>>M;
	vector<int> edges[2*N];
	int clauses[M][2];
	for(int i=0;i<M;i++) {
		cin>>u>>v;
		clauses[i][0] = u;
		clauses[i][1] = v;
		cu = imap(u,N);
		cv = imap(v,N);
		//cout<<cu<<" "<<" "<<cv<<" "<<neg(cu,N)<<" "<<neg(cv,N)<<endl;
		edges[neg(cu,N)].push_back(cv);
		edges[neg(cv,N)].push_back(cu);	
	}

	int comps[2*N];
	int scc = SCC(edges,comps,2*N);
	cout<<"no of sccs : "<<scc<<endl;
	for(int i=0;i<N;i++) {
		if(comps[i]==comps[neg(i,N)]) {
			cout<<"\nThis 2-SAT is not satisfiable";
			exit(0);
		}
	}
	vector<int> sccs[scc];
	for(int i=0;i<2*N;i++) {
		//cout<<comps[i]<<endl;
		sccs[comps[i]].push_back(i);
	}
	int scc_assign[scc];
	vector<int> scc_edges[scc];
	int isedge[scc] = {0};
	for(int i=0;i<scc;i++) {
		for(int j=0;j<sccs[i].size();j++) {
			int u = sccs[i][j];
			for(int k=0;k<edges[u].size();k++) {
				int cu = comps[edges[u][k]];
				if(!isedge[cu]) {
					isedge[cu] = 1;
					scc_edges[i].push_back(cu);
					
				}
			}
		}
		for(int j=0;j<scc_edges[i].size();j++) {
			isedge[scc_edges[i][j]] = 0;
		}
		scc_assign[i] = -1;
	}
	cout<<"Following is an assignment : \n";
	find_assignments(sccs,scc_edges,scc_assign,scc-1,scc,comps,N,0);
	cout<<"enter an assignment : \n";
	int valid_assign[N];
	int another_assign[N];
	for(int i=0;i<N;i++) {
		cin>>valid_assign[i];
	}
	if(find_another_assign(clauses,M,valid_assign,another_assign,N)) {
		cout<<"Following is another assignment : \n";
		cout<<another_assign[0];
		for(int i=1;i<N;i++) {
			cout<<" "<<another_assign[i];
		}
		cout<<endl;
	}
	else {
		cout<<"There is only one assignment.\n";
	}
	cout<<"Following are all satisfying assignments :"<<endl;
	for(int i=0;i<scc;i++) {
		scc_assign[i] = -1;
	}
	find_assignments(sccs,scc_edges,scc_assign,scc-1,scc,comps,N,1);
	return 0;
}
