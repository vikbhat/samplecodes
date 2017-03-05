#include<iostream>
#include<cstdio>
#include<vector>
#include<algorithm>
#include<cstdlib>
#include<cmath>
#include<fstream>
#include<cstring>
using namespace std;
int total_class = 0;
int dim;
vector<float**> references[40];
vector<float**> test_seq[40];
vector<int> ref_len[40];
vector<int> test_len[40];
char train_file[50];
char test_file[50];
int total_test;


float distance(float* a,float* b) {	
	float res = 0.0;
	for(int i=0;i<dim;i++) {
		res += abs(a[i]-b[i]);
	}
	return res;
}

float dtw(float** seq,float** ref,int n,int m,int bw) {
	float* distc = new float[m+1];
	float* distp = new float[m+1];
	float minp;
	float inf = 100000000;
	float* t;
	distp[0] = distance(seq[0],ref[0]);
	//cout<<distp[0]<<endl;
	for(int i=1;i<m;i++) {
		distp[i] = distp[i-1]+distance(seq[0],ref[i]);
		//cout<<distp[i]<<endl;
	}
	for(int i=1;i<n;++i) {
		for(int j=0;j<m;++j) {
			if(abs(i-j)<=bw) {
				distc[j] = distance(seq[i],ref[j]);
				float minp = inf;
			if(abs(i-j)<bw)
				minp = distp[j];	
			if(j>0) {
				minp = min(minp,min(distc[j-1],distp[j-1]));
			}
			distc[j] += minp;
			if(distc[j]>inf) {
				distc[j] = inf;
			}
			}
			else {
				distc[j] = inf;
			}
		}
		t = distp;
		distp = distc;
		distc = t;
	}
	return distp[m-1]/(n+m);
	//return 0;
}

void read_data() {
	ifstream file1(train_file);
	int current_c  = 0;
	int cols,rows;
	int arr;
	string line;
	int cnt = 0;
	while(file1>>current_c) {
		file1>>cols;
		file1>>rows;
		//cout<<current_c<<" "<<cols<<" "<<rows<<endl;
		float** arr = new float*[rows];
		for(int i=0; i<rows;i++)
    			arr[i] = new float[cols];
		for(int i = 0;i<rows;i++) {
			for(int j=0;j<cols;j++) {
				file1>>arr[i][j];
			}
		}
		references[current_c].push_back(arr);
		ref_len[current_c].push_back(rows);
		cnt++;
	}
	//cout<<current_c<<endl;
	cout<<cnt<<endl;
	file1.close();
	ifstream file2(test_file);
	current_c  = 0;
	cnt=0;
	while(file2>>current_c) {
		file2>>cols;
		file2>>rows;
		float** arr = new float*[rows];
		for(int i=0; i<rows;i++)
    			arr[i] = new float[cols];
		for(int i = 0;i<rows;i++) {
			for(int j=0;j<cols;j++) {
				file2>>arr[i][j];
			}
		}
		test_seq[current_c].push_back(arr);
		test_len[current_c].push_back(rows);
		cnt++;
	}
	cout<<cnt;
	total_test = cnt;
	file2.close();
	//cout<<current_c<<endl;*/
}

void classify() {
	int confusion[total_class][total_class]= {0};
	float bests[total_class];
	int predicted;
	int accuracy = 0;
	float minsc = 0;
	for(int i=0;i<total_class;i++) {
		for(int j=0;j<test_seq[i].size();j++) {
			for(int k=0;k<total_class;k++) {
				float bestsc = 10000000.0;;
				float cursc;
				for(int m=0;m<references[k].size();m++) {
					cursc = dtw(references[k][m],test_seq[i][j],ref_len[k][m],test_len[i][j],500);
					if(bestsc>cursc) {
						bestsc = cursc;
					}
				}
				bests[k] = bestsc;
			}
			predicted = 0;
			minsc = bests[0];
			for(int k=1;k<total_class;k++) {
				if(minsc>bests[k]) {
					minsc = bests[k];
					predicted = k;
				}
			}
			accuracy = accuracy+(i==predicted);
			confusion[i][predicted]++;
			
		}
		
	}
	total_test = 0;
	for(int i=0;i<total_class;i++) {
		for(int j=0;j<total_class;j++) {
			cout<<confusion[i][j]<<" ";
			total_test = total_test+confusion[i][j];
		}
		cout<<endl;
	}
	cout<<"accuracy : "<<(double)accuracy/total_test;
}

int main(int argc,char* argv[]) {
	strcpy(train_file,argv[1]);
	strcpy(test_file,argv[2]);
	total_class = atoi(argv[3]);
	//cout<<train_file; 
	dim = 38;
	read_data();
	classify();
	return 0;
}

