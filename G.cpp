#include <iostream>
#include <iomanip>
#include <vector>
#include <complex>
#include <algorithm>
#include <cmath>

using namespace std;

typedef complex<double> point;
typedef vector<point> polygon;

const double eps = 1e-9;

double dot(point a, point b)
{
  return real(conj(a) * b);
}

double det(point a, point b)
{
  return imag(conj(a) * b);
}

point proj(point a, point b)
{
  if (norm(b) < eps) {
    return b;
  } else {
    return dot(a, b) / norm(b) * b;
  }
}

int ccw(point a, point b, point c)
{
  b -= a, c -= a;
  if (det(b, c) > +eps) return +1; // counter clockwise
  if (det(b, c) < -eps) return -1; // clockwise
  if (dot(b, c) < -eps) return -2; // c--a--b
  if (norm(b) < norm(c)-eps) return +2; // a--b--c
  return 0; // a--c--b
}

// (a-b, c-d)
bool intersect_ss(point a, point b, point c, point d)
{
  //  return ccw(a, b, c) * ccw(a, b, d) <= 0 &&
  //    ccw(c, d, a) * ccw(c, d, b) <= 0;
  return ccw(a, b, c) * ccw(a, b, d) < 0 &&
    ccw(c, d, a) * ccw(c, d, b) < 0;
}

// (a, b-c)
bool intersect_ps(point a, point b, point c)
{
  return abs(b-a) + abs(c-a) - abs(b-c) < eps;
}

// (a, -b-c-)
bool intersect_pl(point a, point b, point c)
{
  return abs(det(b-a, c-a)) < eps;
}

// (-a-b-, c-d)
bool intersect_ls(point a, point b, point c, point d)
{
  return det(b-a, c-a) * det(b-a, d-a) < -eps;
}

// intersection line -a-b- and line -c-d-
// !!!this function assume that these lines are crossed at one point.
point intersection_ll(point a, point b, point c, point d)
{
  point ab = b-a, cd = d-c;
  return a + ab * det(cd, c-a) / det(cd, ab);
}

// intersection line -a-b- and line -c-d-
// !!!this function assume that these lines are crossed at one point.
point intersection_ss(point a, point b, point c, point d)
{
  point ab = b-a, cd = d-c;
  double d1 = abs(det(cd, a-c)), d2 = abs(det(cd, b-c));
  return a + ab * d1 / (d1 + d2);
}

// cut convex and leave left side of line -a-b-
polygon cut_convex(const polygon& P, point a, point b)
{
  const size_t n = P.size();
  polygon Q;
  for (size_t i = 0; i < n; i++) {
    if (ccw(a, b, P[i]) != -1) {
      Q.push_back(P[i]);
    }
    if (intersect_ls(a, b, P[i], P[(i+1) % n])) {
      Q.push_back(intersection_ll(a, b, P[i], P[(i+1) % n]));
    }
  }
  return Q;
}

point rotate_90_degree(point a)
{
  return a * point(0, 1);
}

// (a, -b-c-)
double distance_pl(point a, point b, point c)
{
  return abs(det(a-c, b-c)) / abs(b-c);
}

point reflect_with_line(point p, point a, point b)
{
  point ab = b-a;
  point pa = a-p;
  point q = pa - proj(pa, ab);
  return p + q + q;
}

polygon reflect_with_line(const polygon& P, point a, point b)
{
  const size_t n = P.size();
  polygon Q;
  for (size_t i = 0; i < n; i++) {
    Q.push_back(reflect_with_line(P[i], a, b));
  }
  reverse(Q.begin(), Q.end());
  return Q;
}

enum {
  OUT = 0,
  ON = 1,
  IN = 2
};

// Polygon P must be simple polygon.
int point_in_polygon(point p, const polygon& P)
{
  const size_t n = P.size();
  double angle = 0.0;
  for (size_t i = 0; i < n; i++) {
    if (intersect_ps(p, P[i], P[(i+1)%n])) {
      return ON;
    }
    angle += arg((P[(i+1)%n]-p) / (P[i]-p));
  }
  return abs(angle) < eps ? OUT : IN;
}

void rotate_polygon_indices_with_line(polygon& P, point a, point b)
{
  const size_t n = P.size();
  for (size_t i = 0; i < n; i++) {
    if (intersect_pl(P[i], a, b) &&
        intersect_pl(P[(i+1)%n], a, b)) {
      rotate(P.begin(), P.begin()+i, P.end());
      break;
    }
  }
  return ;
}

polygon merge_convex(const polygon& P, const polygon& Q)
{
  const size_t n = P.size(), m = Q.size();
  polygon R;

  vector<int> P_pos(n), Q_pos(m);
  for (size_t i = 0; i < n; i++) {
    P_pos[i] = point_in_polygon(P[i], Q);
  }
  for (size_t j = 0; j < m; j++) {
    Q_pos[j] = point_in_polygon(Q[j], P);
  }

  R.push_back(P[0]);
  R.push_back(P[1]);
  size_t i = 1, j = 1;
  while (i < n && j < m) {
    if (intersect_ss(P[i], P[(i+1)%n], Q[j], Q[(j+1)%m])) {
      R.push_back(intersection_ss(P[i], P[(i+1)%n], Q[j], Q[(j+1)%m]));
    }

    int dir_p = ccw(Q[j], Q[(j+1)%m], P[(i+1)%n]);
    int dir_q = ccw(P[i], P[(i+1)%n], Q[(j+1)%m]);
    if (dir_p == 0 && dir_q == 0) {
      R.push_back(P[(i+1)%n]);
      i++;
      j++;
    } else if (dir_p == -1) {
      while (j < m && Q_pos[(j+1)%m] == IN) {
        j++;
      }
      do {
        R.push_back(P[(i+1)%n]);
        i++;
      } while (i < n &&
               ccw(Q[j], Q[(j+1)%m], P[i]) *
               ccw(Q[j], Q[(j+1)%m], P[(i+1)%n]) > 0);
    } else if (dir_q == -1) {
      while (i < n && P_pos[(i+1)%n] == IN) {
        i++;
      }
      do {
        R.push_back(Q[(j+1)%m]);
        j++;
      } while (j < m &&
               ccw(P[i], P[(i+1)%n], Q[j]) *
               ccw(P[i], P[(i+1)%n], Q[(j+1)%m]) > 0);
    } else if (dir_p == 0) {
      R.push_back(P[(i+1)%n]);
      i++;
    } else if (dir_q == 0) {
      R.push_back(Q[(j+1)%m]);
      j++;
    }

    while (R.size() > 3) {
      size_t last = R.size()-1;
      int dir = ccw(R[last-2], R[last-1], R[last]);
      if (dir == +2) {
        R[last-1] = R[last];
        R.pop_back();
      } else if (dir == 0) {
        R.pop_back();
      } else {
        break;
      }
    }
  }

  return R;
}

double perimeter(const polygon& P)
{
  const size_t n = P.size();
  double perimeter_of_P = 0.0;
  for (size_t i = 0; i < n; i++) {
    perimeter_of_P += abs(P[(i+1) % n] - P[i]);
  }
  return perimeter_of_P;
}

polygon get_folding_polygon(const polygon& P, size_t p_i, size_t p_j)
{
  point folding_line_a = (P[p_i] + P[p_j]) / 2.0;
  point folding_line_b = rotate_90_degree(P[p_i] - folding_line_a) + folding_line_a;

  polygon Q1 = cut_convex(P, folding_line_a, folding_line_b);
  polygon Q2 = cut_convex(P, folding_line_b, folding_line_a);
  polygon folded_Q2 = reflect_with_line(Q2, folding_line_a, folding_line_b);

  rotate_polygon_indices_with_line(Q1, folding_line_a, folding_line_b);
  rotate_polygon_indices_with_line(folded_Q2, folding_line_a, folding_line_b);

  polygon R = merge_convex(Q1, folded_Q2);
  return R;
}

double solve(size_t n, const polygon& P)
{
  size_t max_num_vertex = 0;
  double max_perimeter = 0.0;

  for (size_t i = 0; i < n; i++) {
    for (size_t j = i+1; j < n; j++) {
      polygon Q = get_folding_polygon(P, i, j);

      if (Q.size() > max_num_vertex) {
        max_num_vertex = Q.size();
        max_perimeter = perimeter(Q);
      } else if (Q.size() == max_num_vertex) {
        double perimeter_of_Q = perimeter(Q);
        if (max_perimeter < perimeter_of_Q) {
          max_perimeter = perimeter_of_Q;
        }
      }
    }
  }

  return max_perimeter;
}

int main()
{
  size_t n;
  while (cin >> n) {
    if (n == 0) {
      break;
    }

    int x, y;
    polygon P;
    for (size_t i = 0; i < n; i++) {
      cin >> x >> y;
      P.push_back(point(x, y));
    }

    cout << fixed << setprecision(6) << solve(n, P) << endl;
  }

  return 0;
}
