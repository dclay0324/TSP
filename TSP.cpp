#include <iostream> 
#include <fstream>
#include <time.h>
#include <vector>
#include <queue>
#include <climits>
#define MAX 100
#define INF INT_MAX

using namespace std;

int ub_BF = INF, ub_BB = INF;
int ans_BF[MAX], ans_BB[MAX];

struct timespec diff(struct timespec start, struct timespec end) {
  struct timespec temp;
  if ((end.tv_nsec-start.tv_nsec)<0) {
    temp.tv_sec = end.tv_sec-start.tv_sec-1;
    temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec-start.tv_sec;
    temp.tv_nsec = end.tv_nsec-start.tv_nsec;
  }
  return temp;
}

class Node{
	public:
	    vector<int> path;
	    int newMatrix[MAX][MAX];
	    int cost;
	    int vertex;
	    int level;
};

Node* makenode(int parentMatrix[MAX][MAX], vector<int> const &path, int level, int i, int j, int N){            	
    Node* root = new Node;
    root->path = path;
    if (level != 0) root->path.push_back(i);
    memcpy(root->newMatrix, parentMatrix, sizeof root->newMatrix);

    for (int k = 0; level != 0 && k < N; k++){
       	root->newMatrix[i][k] = INF;
        root->newMatrix[k][j] = INF;
    }

    root->newMatrix[j][i] = INF;
    root->level = level;
    root->vertex = j;

    return root;
}

int calculateCost(int newMatrix[MAX][MAX], int N){
    int cost = 0;
    int row[N];
    int col[N];

    fill_n(row, N, INF);
    fill_n(col, N, INF);
    
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (newMatrix[i][j] < row[i])
                row[i] = newMatrix[i][j];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (newMatrix[i][j] != INF && row[i] != INF)
                newMatrix[i][j] -= row[i];
     
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (newMatrix[i][j] < col[j])
                col[j] = newMatrix[i][j];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (newMatrix[i][j] != INF && col[j] != INF)
                newMatrix[i][j] -= col[j];

    for (int i = 0; i < N; i++){
    	if (row[i] != INF) cost += row[i];
    	if (col[i] != INF) cost += col[i];
	}

    return cost;
}

void show(vector<int> const &list){
    for (int i = 0; i < list.size(); i++) ans_BB[i] = list[i];
}

void showPath(vector<int> const &list){
	cout << "Path: ";
    for (int i = 0; i < list.size(); i++) cout << list[i] << " ";
    cout << "\n";
}

struct comp {
    bool operator()(const Node* lhs, const Node* rhs) const
    {
        return lhs->cost > rhs->cost;
    }
};

void branch_and_bound(int input[MAX][MAX], int N){
    priority_queue<Node*, std::vector<Node*>, comp> pq;
    vector<int> v;

    Node* root = makenode(input, v, 0, -1, 0, N);
    root->cost = calculateCost(root->newMatrix, N);
    pq.push(root);

    while (!pq.empty()){
        Node* min = pq.top();
        pq.pop();
        int i = min->vertex;
        //cout << i << " " << min->level << " " << min->cost << "\n";
        //showPath(min->path);
        if (min->level == N){
            min->path.push_back(i);
            show(min->path);
            //showPath(min->path);
            ub_BB = min->cost;
            return;
        }

        for (int j = 0; j < N; j++){
            if (min->newMatrix[i][j] != INF){
                Node* child = makenode(min->newMatrix, min->path,
                    min->level + 1, i, j, N);
                child->cost = min->cost + min->newMatrix[i][j]
                            + calculateCost(child->newMatrix, N);
                pq.push(child);
            }
        }
        delete min;
    }
}

void brute(int input[MAX][MAX], int N, int *ch, int a, int b){
	fstream file;
	int i, j;
	if (a == b){
    	int lb = 0;
        for(int i = 0;i < N-1; i++){
            lb += input[ch[i]][ch[i+1]]; 	
		}
		lb += input[ch[N-1]][ch[0]];
		if (lb < ub_BF){
			ub_BF = lb;
			for(int i = 0; i < N; i++) ans_BF[i] = ch[i];	
		}
    } else if (ch[0] == 0){
    	for(int j = a; j < N; j++){     
            swap(ch[a], ch[j]);
            brute(input, N, ch, a+1, b);
        	swap(ch[a], ch[j]);
        }
	}
}

int main(){
	fstream file;
	int input[MAX][MAX], list[MAX];
	int ch;
	int i = 0, j = 0;
	file.open("input.txt", ios::in);
	file >> ch;
	int N = ch;
	//cout << N << endl;
	for (int i = 0; i < N; i++) list[i] = i;
	while (file >> ch){
		input[i][j] = ch;
		if (input[i][j] == -1) input[i][j] = INF;
		j++;
		if (j == N){
			i++;
			j = 0;
		} 
	}
	file.close();

	struct timespec start, end, temp;
  	double time_BF, time_BB;
  	clock_gettime(CLOCK_MONOTONIC, &start);
  	brute(input, N, list, 0, N-1);
  	clock_gettime(CLOCK_MONOTONIC, &end);
  	temp = diff(start, end);
  	time_BF = temp.tv_sec + (double) temp.tv_nsec / 1000000000.0;
  	
  	clock_gettime(CLOCK_MONOTONIC, &start);
  	branch_and_bound(input, N);
  	clock_gettime(CLOCK_MONOTONIC, &end);
  	temp = diff(start, end);
  	time_BB = temp.tv_sec + (double) temp.tv_nsec / 1000000000.0;
  	
	file.open("BF.txt", ios::out);
	file << "Solution : ";
	for(int i = 0; i < N; i++) file << ans_BF[i] << " - ";
	file << ans_BF[0] << endl;
	file << "Cost     : " << ub_BF << endl;
	file << "Time     : " << time_BF << " s";
	file.close();
	
	file.open("BB.txt", ios::out);
	file << "Solution : ";
	for(int i = 0; i < N; i++) file << ans_BB[i] << " - ";
	file << ans_BB[0] << endl;
	file << "Cost     : " << ub_BB << endl;
	file << "Time     : " << time_BB << " s";
	file.close();
	
	cout << "Solution : ";
	for(int i = 0; i < N; i++) cout << ans_BF[i] << " - ";
	cout << ans_BF[0] << endl;
	cout << "Cost     : " << ub_BF << endl;
	cout << "Time     : " << time_BF << " s" << endl;
	
	cout << "Solution : ";
	for(int i = 0; i < N; i++) cout << ans_BB[i] << " - ";
	cout << ans_BB[0] << endl;
	cout << "Cost     : " << ub_BB << endl;
	cout << "Time     : " << time_BB << " s" << endl;
	
	return 0;
}
