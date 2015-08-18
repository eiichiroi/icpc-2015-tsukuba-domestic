#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>

using namespace std;

struct Hole {
  Hole() : x_0(0), y_0(0), x_1(0), y_1(0), z(0) {
  }

  void getAllX(vector<int>& x) const {
    x.push_back(x_0-1);
    x.push_back(x_1+1);
  }
  
  void getAllY(vector<int>& y) const {
    y.push_back(y_0-1);
    y.push_back(y_1+1);
  }

  int x_0, y_0, x_1, y_1, z;
};

struct Point {
  Point() : x(0), y(0), z(0), cost(0) {
  }

  Point(int x, int y, int z, int cost = 0) : x(x), y(y), z(z), cost(cost) {
  }

  Point getClosestPointOnHole(const Hole& hole) const {
    Point p;
    if (x < hole.x_0) {
      p.x = hole.x_0;
    } else if (x > hole.x_1) {
      p.x = hole.x_1;
    } else {
      p.x = x;
    }
    if (y < hole.y_0) {
      p.y = hole.y_0;
    } else if (y > hole.y_1) {
      p.y = hole.y_1;
    } else {
      p.y = y;
    }
    p.z = hole.z;
    p.cost = cost + getManhattanDistance(p) + kCostToMoveToUpstair;
    return p;
  }

  int getManhattanDistance(const Point& p) const {
    return abs(x - p.x) + abs(y - p.y);
  }

  bool isOnHole(const Hole& hole) const {
    return z == hole.z &&
      (hole.x_0 <= x && x <= hole.x_1) &&
      (hole.y_0 <= y && y <= hole.y_1);
  }
  
  int x, y, z;
  int cost;

  static const int kCostToMoveToUpstair = 100;
};

struct PointEqual {
  PointEqual(const Point& p) : p(p) {}
  
  bool operator()(const Point& q) const {
    return p.x == q.x && p.y == q.y && p.z == q.z;
  }

  const Point& p;
};

struct Robot {
  void moveToTopFloor(size_t M, size_t K, size_t R,
                      const vector<Hole>& holes) {
    vector<Point> current_floor;
    
    current_floor.push_back(start);
    for (size_t k = start.z+1; k < K; k++) {
      vector<Point> next_floor;
      for (size_t m = 0; m < M; m++) {
        if (holes[m].z != k) {
          continue;
        }

        for (size_t i = 0; i < current_floor.size(); i++) {
          const Point& current_point = current_floor[i];
          const Point next_point = current_point.getClosestPointOnHole(holes[m]);

          vector<Point>::iterator it = find_if(next_floor.begin(), next_floor.end(), PointEqual(next_point));
          if (it == next_floor.end()) {
            next_floor.push_back(next_point);
          } else {
            if (it->cost > next_point.cost) {
              it->cost = next_point.cost;
            }
          }
        }
      }
      current_floor.swap(next_floor);
    }

    top_floor.swap(current_floor);
  }

  void getAllX(vector<int>& x) const {
    for (size_t i = 0; i < top_floor.size(); i++) {
      x.push_back(top_floor[i].x);
    }
  }

  void getAllY(vector<int>& y) const {
    for (size_t i = 0; i < top_floor.size(); i++) {
      y.push_back(top_floor[i].y);
    }
  }

  int getMinCostToGoal(const Point& goal) const {
    int min_cost = numeric_limits<int>::max();
    for (size_t i = 0; i < top_floor.size(); i++) {
      int cost = top_floor[i].cost + top_floor[i].getManhattanDistance(goal);
      if (min_cost > cost) {
        min_cost = cost;
      }
    }
    return min_cost;
  }

  Point start;
  vector<Point> top_floor;
};

int solve(size_t N, size_t M, size_t K, size_t R,
          vector<Robot>& robots,
          vector<Hole>& holes)
{
  for (size_t n = 0; n < N; n++) {
    robots[n].moveToTopFloor(M, K, R, holes);
  }
  vector<int> x, y;
  for (size_t n = 0; n < N; n++) {
    robots[n].getAllX(x);
    robots[n].getAllY(y);
  }
  for (size_t m = 0; m < M; m++) {
    if (holes[m].z != K-1) {
      continue;
    }
    holes[m].getAllX(x);
    holes[m].getAllY(y);
  }

  sort(x.begin(), x.end());
  x.erase(unique(x.begin(), x.end()), x.end());
  sort(y.begin(), y.end());
  y.erase(unique(y.begin(), y.end()), y.end());

  int min_cost = numeric_limits<int>::max();
  for (size_t i = 0; i < x.size(); i++) {
    if (x[i] < 0 || x[i] >= R) {
      continue;
    }
    for (size_t j = 0; j < y.size(); j++) {
      if (y[j] < 0 || y[j] >= R) {
        continue;
      }
      Point goal(x[i], y[j], K-1);
      
      bool isGoalOnHole = false;
      for (size_t m = 0; m < M; m++) {
        if (goal.isOnHole(holes[m])) {
          isGoalOnHole = true;
          break;
        }
      }
      if (isGoalOnHole) {
        continue;
      }
      
      int cost = 0;
      for (size_t n = 0; n < N; n++) {
        cost += robots[n].getMinCostToGoal(goal);
      }

      if (min_cost > cost) {
        min_cost = cost;
      }
    }
  }

  return min_cost;
}

int main()
{
  size_t N;
  while (cin >> N) {
    if (N == 0) {
      break;
    }
    
    size_t M, K, R;
    cin >> M >> K >> R;
    
    vector<Robot> robots(N);
    for (size_t n = 0; n < N; n++) {
      cin >> robots[n].start.x >> robots[n].start.y >> robots[n].start.z;
      robots[n].start.x--;
      robots[n].start.y--;
      robots[n].start.z--;
    }
    
    vector<Hole> holes(M);
    for (size_t m = 0; m < M; m++) {
      cin >> holes[m].x_0 >> holes[m].y_0
          >> holes[m].x_1 >> holes[m].y_1
          >> holes[m].z;
      holes[m].x_0--; holes[m].y_0--;
      holes[m].x_1--; holes[m].y_1--;
      holes[m].z--;
    }

    cout << solve(N, M, K, R, robots, holes) << endl;
  }
  
  return 0;
}
