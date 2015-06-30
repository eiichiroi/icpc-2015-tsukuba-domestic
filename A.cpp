#include <iostream>
#include <vector>

using namespace std;

int main()
{
  int m, n_min, n_max;

  while (cin >> m >> n_min >> n_max) {
    if (m == 0 && n_min == 0 && n_max == 0) {
      break;
    }

    vector<int> points(m);
    for (size_t i = 0; i < m; i++) {
      cin >> points[i];
    }

    int n_ans = n_min;
    int gap_max = 0;
    for (int n = n_min; n <= n_max; n++) {
      int gap = points[n-1] - points[n];
      if (gap >= gap_max) {
        gap_max = gap;
        n_ans = n;
      }
    }

    cout << n_ans << endl;
  }

  return 0;
}
