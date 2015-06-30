#include <iostream>
#include <string>
#include <vector>

using namespace std;

int eval_expr(const vector<string>& expr, size_t& i, size_t level)
{
  int value;
  if (expr[i][level] == '*') {
    value = 1;
    i++;
    value *= eval_expr(expr, i, level+1); // operand 1
    value *= eval_expr(expr, i, level+1); // operand 2
    // operand 3 or later
    while (i < expr.size()) {
      if (expr[i].size()-1 != level+1) {
        break;
      }
      value *= eval_expr(expr, i, level+1);
    }
  } else if (expr[i][level] == '+') {
    value = 0;
    i++;
    value += eval_expr(expr, i, level+1); // operand 1
    value += eval_expr(expr, i, level+1); // operand 2
    // operand 3 or later
    while (i < expr.size()) {
      if (expr[i].size()-1 != level+1) {
        break;
      }
      value += eval_expr(expr, i, level+1);
    }
  } else {
    value = expr[i][level] - '0';
    i++;
  }
  return value;
}

int eval(const vector<string>& expr)
{
  size_t i = 0, level = 0;
  return eval_expr(expr, i, level);
}

int main()
{
  int n;
  while (cin >> n) {
    if (n == 0) {
      break;
    }
    
    vector<string> expr(n);
    for (int i = 0 ; i < n; i++) {
      cin >> expr[i];
    }

    cout << eval(expr) << endl;
  }
  
  return 0;
}
