#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class UnionFind {
public:
  UnionFind(int n) : parent(n, -1), rank(n, 0) {
    for (int i = 0; i < n; i++) {
      parent[i] = i;
      rank[i] = 0;
    }
  }
  void UnionSet(int u, int v) {
    u = FindSet(u), v = FindSet(v);
    if (u == v) return ;
    if (rank[u] < rank[v]) {
      parent[u] = v;
    } else if (rank[u] > rank[v]) {
      parent[v] = u;
    } else {
      parent[v] = u;
      rank[u]++;
    }
  }
  int FindSet(int u) {
    if (parent[u] != u) {
      parent[u] = FindSet(parent[u]);
    }
    return parent[u];
  }
  
private:
  vector<int> parent, rank;
};

struct Proposal {
  Proposal(int u, int v, int w)
    : src(u), dst(v), cost(w) {
  }
  
  int src, dst;
  int cost;
};

struct ProposalLess {
  bool operator()(const Proposal& a, const Proposal& b) const {
    if (a.cost != b.cost) {
      return a.cost < b.cost;
    } else if (a.src != b.src) {
      return a.src < b.src;
    } else {
      return a.dst < b.dst;
    }
  }
};

struct EdgeIndexPair {
  EdgeIndexPair(int a_i, int b_i, int cost)
    : a_index(a_i), b_index(b_i), cost(cost) {
  }
  
  int a_index, b_index;
  int cost;
};

struct EdgeIndexPairCostLess {
  bool operator()(const EdgeIndexPair& e, const EdgeIndexPair& f) const {
    if (e.cost != f.cost) {
      return e.cost < f.cost;
    } else if (e.a_index != f.a_index) {
      return e.a_index < f.a_index;
    } else {
      return e.b_index < f.b_index;
    }
  }
};

int solve(int n, int m, int k,
          vector<Proposal>& a_proposals,
          vector<Proposal>& b_proposals)
{
  sort(a_proposals.begin(), a_proposals.end(), ProposalLess());
  sort(b_proposals.begin(), b_proposals.end(), ProposalLess());

  vector<bool> a_used(a_proposals.size(), false);
  vector<bool> b_used(b_proposals.size(), false);
  int num_a_proposals = 0;
  int num_b_proposals = 0;

  {
    UnionFind islands(n);
    for (size_t b_i = 0; b_i < b_proposals.size(); b_i++) {
      int u = islands.FindSet(b_proposals[b_i].src);
      int v = islands.FindSet(b_proposals[b_i].dst);
      if (u == v) continue;
      islands.UnionSet(u, v);
      b_used[b_i] = true;
      num_b_proposals++;
    }

    for (size_t a_i = 0; a_i < a_proposals.size(); a_i++) {
      int u = islands.FindSet(a_proposals[a_i].src);
      int v = islands.FindSet(a_proposals[a_i].dst);
      if (u == v) continue;
      islands.UnionSet(u, v);
      a_used[a_i] = true;
      num_a_proposals++;
    }
    
    if (num_a_proposals + num_b_proposals < n-1 || num_a_proposals > k) {
      return -1;
    }
  }

  while (num_a_proposals < k) {
    vector<EdgeIndexPair> edges;
    for (size_t b_i = 0; b_i < b_proposals.size(); b_i++) {
      if (!b_used[b_i]) continue;

      UnionFind islands(n);
      for (size_t b_j = 0; b_j < b_proposals.size(); b_j++) {
        if (b_i == b_j) continue;
        if (!b_used[b_j]) continue;
        int u = islands.FindSet(b_proposals[b_j].src);
        int v = islands.FindSet(b_proposals[b_j].dst);
        if (u == v) continue;
        islands.UnionSet(u, v);
      }
      for (size_t a_i = 0; a_i < a_proposals.size(); a_i++) {
        if (!a_used[a_i]) continue;
        int u = islands.FindSet(a_proposals[a_i].src);
        int v = islands.FindSet(a_proposals[a_i].dst);
        if (u == v) continue;
        islands.UnionSet(u, v);
      }

      for (size_t a_i = 0; a_i < a_proposals.size(); a_i++) {
        if (a_used[a_i]) continue;
        int u = islands.FindSet(a_proposals[a_i].src);
        int v = islands.FindSet(a_proposals[a_i].dst);
        if (u == v) continue;
        edges.push_back(EdgeIndexPair(a_i, b_i, a_proposals[a_i].cost - b_proposals[b_i].cost));
        break;
      }
    }

    if (edges.empty()) break;
    sort(edges.begin(), edges.end(), EdgeIndexPairCostLess());
    a_used[edges[0].a_index] = true;
    b_used[edges[0].b_index] = false;
    num_a_proposals++;
    num_b_proposals--;
  }
  
  if (num_a_proposals < k) {
    return -1;
  } else {
    int total_cost = 0;
    for (size_t a_i = 0; a_i < a_used.size(); a_i++) {
      if (a_used[a_i]) {
        total_cost += a_proposals[a_i].cost;
      }
    }
    for (size_t b_i = 0; b_i < b_used.size(); b_i++) {
      if (b_used[b_i]) {
        total_cost += b_proposals[b_i].cost;
      }
    }
    return total_cost;
  }
}

int main()
{
  int n, m, k;
  while (cin >> n >> m >> k) {
    if (n == 0 && m == 0 && k == 0) {
      break;
    }

    int u, v, w;
    char l;
    vector<Proposal> a_proposals, b_proposals;
    for (int i = 0; i < m; i++) {
      cin >> u >> v >> w >> l;
      u--, v--;
      if (l == 'A') {
        a_proposals.push_back(Proposal(u, v, w));
      } else {
        b_proposals.push_back(Proposal(u, v, w));
      }
    }

    cout << solve(n, m, k, a_proposals, b_proposals) << endl;
  }
  return 0;
}
