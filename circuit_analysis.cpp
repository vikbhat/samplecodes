#include<iostream>
#include<cstdlib>
#include<vector>
#include<algorithm>
#include<utility>
#include<cmath>
using namespace std;
void circuit_eval(int N,double V,vector<pair<int,double> >* in,vector<pair<int,double> >* out,vector<pair<int,double> >* currents) {
	double A[N-2][N-1];
	double Vs[N];
	Vs[0] = V;
	Vs[1] = 0.0;
	for(int i=0;i<N-2;i++) {
		double tc = 0;
		for(int j=0;j<N-1;j++) {
			A[i][j] = 0.0;
		}
		for(int j=0;j<in[i+2].size();j++) {
			pair<int,double> p = in[i+2][j];
			//cout<<"in : "<<p.first<<" "<<p.second<<endl;
			tc-=p.second;
			if(p.first>=2) 
				A[i][p.first-2] += p.second;
			else {
				A[i][N-2]-=(Vs[p.first]*p.second);
			} 
		}
		for(int j=0;j<out[i+2].size();j++) {
			pair<int,double> p = out[i+2][j];
			//cout<<"out : "<<p.first<<" "<<p.second<<endl;
			tc-=p.second;
			if(p.first>=2) 
				A[i][p.first-2] += p.second;
			else {
				A[i][N-2]-=(Vs[p.first]*p.second);
			} 
		}
		A[i][i] = tc;	
		/*for(int j=0;j<N-1;j++) {
				cout<<A[i][j]<<" ";
		}*/
		//cout<<endl;
	}
	for(int i=0;i<N-2;i++) {
		double max = abs(A[i][i]);
		int k = i;
		for(int j=i+1;j<N-2;j++) {
			if(max<abs(A[j][i])) {
				max = abs(A[j][i]);
				k = j;
			}
		}
		//cout<<max<<" "<<k<<endl;
		for(int j=0;j<N-1;j++) {
			double t = A[k][j];
			A[k][j] = A[i][j];
			A[i][j] = t;
		}
		if(max>0) {
			for(int j=i+1;j<N-2;j++) {
				double mult = A[j][i]/A[i][i]; 
				for(int m=0;m<N-1;m++) {
					A[j][m] -= mult*A[i][m];
				}	
			}
		}
	}
	/*for(int i=0;i<N-2;i++) {
		for(int j=0;j<N-1;j++) 
			cout<<A[i][j]<<" ";
		cout<<endl;
	}*/
	for(int i=N-3;i>=0;i--) {
		double sum = 0;
		for(int j=i+1;j<N-2;j++) {
			sum+=(Vs[j+2]*A[i][j]);
		}
		Vs[i+2] = (A[i][N-2]-sum)/(A[i][i]);
	}
	/*for(int i=0;i<N;i++) {
		cout<<Vs[i]<<endl;
	}*/
	for(int i=0;i<N;i++) {
		for(int j=0;j<in[i].size();j++) {
			pair<int,double> p = in[i][j];
			int k = p.first;
			double curr = abs(Vs[i]-Vs[k])*p.second;
			if(Vs[i]>Vs[k]) {
				currents[i].push_back(make_pair(k,curr));
			}
			else {
				currents[k].push_back(make_pair(i,curr));
			}
		} 
	}
}

int main() {
	int N,M,u,v;
	double c,V;
	cin>>N>>M;
	cin>>V;
	vector<pair<int,double> > in[N];
	vector<pair<int,double> > out[N];
	for(int i=0;i<M;i++) {
		cin>>u>>v>>c;
		u--;
		v--;
		out[u].push_back(make_pair(v,c));
		in[v].push_back(make_pair(u,c));
	}
	vector<pair<int,double> > currents[N];
	circuit_eval(N,V,in,out,currents);
	cout<<"Currents :\n";
	for(int i=0;i<N;i++) {
		for(int j=0;j<currents[i].size();j++) {
			pair<int,double> p = currents[i][j];
			cout<<i+1<<"->"<<p.first+1<<" : "<<p.second<<endl;
		}
	}
	return 0;
}
