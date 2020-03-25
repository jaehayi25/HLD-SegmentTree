
/*
ID: jaehayi
TASK: test
LANG: C++                 
*/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <cstring>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <queue>
#include <map>
#include <stack>

using namespace std;

const int MAXN = 100005;

int N, Q;
int A[MAXN];
vector<int> nbrs[MAXN];

int sz[MAXN]; //size of subtree
int depth[MAXN]; //depth of node
int lca[MAXN][18]; 
int segtreeidx[MAXN]; //original array -> new array with contiguous subarray from parent to child
int topchain[MAXN];   //top of heavy chain

void dfs(int curr, int prev) {
	//cout<<curr<<endl;
	sz[curr] = 1;
	lca[curr][0] = prev;
	if (prev != -1) depth[curr] = depth[prev]+1;
	for (int adj: nbrs[curr]) {
		if (adj == prev) continue;
		dfs(adj, curr);
		sz[curr] += sz[adj];
	}
}

void dfs2(int curr, int prev, int& idx) {
	segtreeidx[curr] = idx++;
	//find heaviest child
	int maxWeight = 0;
	int heaviest = -1;
	for (int adj: nbrs[curr]) {
		if (adj == prev) continue;
		if (sz[adj] > maxWeight) {
			maxWeight = sz[adj];
			heaviest = adj;
		}
	}
	//prioritize heavy nodes
	if (heaviest == -1) return;
	topchain[heaviest] = topchain[curr];
	dfs2(heaviest, curr, idx);
	//visit light nodes
	for (int adj: nbrs[curr]) {
		if (adj == prev || adj == heaviest) continue;
		topchain[adj] = adj;
		dfs2(adj, curr, idx);
	}
}

int LCA(int a, int b) {
	if (depth[a] < depth[b]) swap(a, b); //a is deeper
	for (int i = 17; i >= 0; i--) {
		if (lca[a][i] == -1) continue;
		if (depth[lca[a][i]] >= depth[b]) {
			a = lca[a][i];
		}
	}
	if (a == b) return a;
	for (int i = 17; i >= 0; i--) {
		if (lca[a][i] == -1) continue;
		if (lca[a][i] != lca[b][i]) {
			a = lca[a][i];
			b = lca[b][i];
		}
	}
	return lca[a][0];
}

int segtree[MAXN*4];
int query(int l, int r, int qs, int qe, int i) {
	if (qs <= l && r <= qe) return segtree[i];
	int mid = (l+r)/2;
	if (qe <= mid) return query(l, mid, qs, qe, 2*i+1); //left child
	if (qs >= mid+1) return query(mid+1, r, qs, qe, 2*i+2); //right child
	return query(l, mid, qs, qe, 2*i+1)^query(mid+1, r, qs, qe, 2*i+2);
}
int query(int child, int parent) {
	return query(0, N-1, segtreeidx[parent], segtreeidx[child], 0);
}

void update(int l, int r, int pos, int val, int i) {
	int mid = (l+r)/2;
	if (l == r) {
		segtree[i] = val;
		return;
	}
	if (pos <= mid) update(l, mid, pos, val, 2*i+1); //left child
	if (pos >= mid+1) update(mid+1, r, pos, val, 2*i+2); //right child
	segtree[i] = segtree[2*i+1]^segtree[2*i+2];
}
void update(int original_idx, int val) {
	update(0, N-1, segtreeidx[original_idx], val, 0);
}

int traverse(int curr, int parent) {
	int ret = 0;
	while (curr != parent) {
		//light node
		if (topchain[curr] == curr) {
			ret^=A[curr];
			curr = lca[curr][0];
			continue;
		}
		if (depth[topchain[curr]] > depth[parent]) {
			ret^=query(curr, topchain[curr]);
			curr = lca[topchain[curr]][0];
		}
		else {
			ret^=query(curr, parent);
			ret^=A[parent];
			curr = parent;
			break;
		}
	}
	return ret;
}

int calc(int a, int b) {
	int c = LCA(a, b);
	return traverse(a, c) ^ A[c] ^ traverse(b, c);
}

int main() {
    ifstream fin ("cowland.in");
    ofstream fout ("cowland.out");
	
	fin>>N>>Q;
	
	for (int i = 0; i < N; i++) {
		fin>>A[i];
	}
	for (int i = 0; i < N-1; i++) {
		int a, b;
		fin>>a>>b; a--; b--;
		nbrs[a].push_back(b);
		nbrs[b].push_back(a);
	}
	dfs(0, -1);
	
	//set lca
	for (int i = 1; i <= 17; i++) 
		for (int j = 0; j < N; j++) {
			if (lca[j][i-1] == -1) {
				lca[j][i] = -1;
			}
			else {
				lca[j][i] = lca[lca[j][i-1]][i-1];
			}
		}
	
	//create new array with contiguous subarray from parent to child
	int idx = 0;
	dfs2(0, -1, idx);
	
	//construct segment tree 
	for (int i = 0; i < N; i++) {
		update(i, A[i]);
	}
	
	for (int n_q = 0; n_q < Q; n_q++) {
		int t;
		fin>>t;
		if (t == 1) { //update value
			int i, v;
			fin>>i>>v; i--; 
			A[i] = v;
			update(i, v);
		}
		if (t == 2) { //query
			int a, b;
			fin>>a>>b; a--; b--;
			//calculate from a to b
			fout<<calc(a, b)<<endl;
		}
	}
	
	return 0;
}
