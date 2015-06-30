#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>

using namespace std;

pair<int, int> solve(const vector<int>& prices)
{
  const int n = prices.size();
  const int max_money = 100 * 1000;
  const int invalid_num_500_coins = -1;
  const int invalid_total_payment = 1000000;
  
  vector<vector<int> > num_500_coins(n+1, vector<int>(max_money, invalid_num_500_coins));
  vector<vector<int> > total_payment(n+1, vector<int>(max_money, invalid_total_payment));

  num_500_coins[0][0] = 0;
  total_payment[0][0] = 0;
  for (int i = 0; i < n; i++) {
    // 1. Not buy prices[i]
    for (int money = 0; money < max_money; money++) {
      num_500_coins[i+1][money] = num_500_coins[i][money];
      total_payment[i+1][money] = total_payment[i][money];
    }
      
    // 2. Buy prices[i]
    int change = (prices[i] % 1000 == 0) ? 0 : (1000 - prices[i] % 1000);
    for (int current_money = 0; current_money < max_money; current_money++) {
      if (num_500_coins[i][current_money] == invalid_num_500_coins) { // Cannot reach this state
        continue;
      }

      int got_num_500_coins = (current_money + change < 500) ? 0 : 1; // We can get at most one 500 coin at once
      int next_money = current_money + change - (500 * got_num_500_coins);
      if (num_500_coins[i+1][next_money] < num_500_coins[i][current_money] + got_num_500_coins) {
        num_500_coins[i+1][next_money] = num_500_coins[i][current_money] + got_num_500_coins;
        total_payment[i+1][next_money] = total_payment[i][current_money] + prices[i];
      } else if (num_500_coins[i+1][next_money] == num_500_coins[i][current_money] + got_num_500_coins) {
        if (total_payment[i+1][next_money] > total_payment[i][current_money] + prices[i]) {
          total_payment[i+1][next_money] = total_payment[i][current_money] + prices[i];
        }
      }
    }
  }

  int max_num_500_coins = 0, min_total_payment = 0;
  for (int money = 0; money < max_money; money++) {
    if (max_num_500_coins < num_500_coins[n][money]) {
      max_num_500_coins = num_500_coins[n][money];
      min_total_payment = total_payment[n][money];
    } else if (max_num_500_coins == num_500_coins[n][money]) {
      if (min_total_payment > total_payment[n][money]) {
        min_total_payment = total_payment[n][money];
      }
    }
  }
  return make_pair(max_num_500_coins, min_total_payment);
}

int main()
{
  int n;
  while (cin >> n) {
    if (n == 0) {
      break;
    }

    vector<int> prices(n);
    for (size_t i = 0; i < prices.size(); i++) {
      cin >> prices[i];
    }

    pair<int, int> ans = solve(prices);
    cout << ans.first << " " << ans.second << endl;
  }
  return 0;
}

