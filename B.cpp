#include <iostream>
#include <string>
#include <vector>

using namespace std;

int lengths[] = {5, 7, 5, 7, 7};
int num_lengths = sizeof(lengths)/sizeof(int);

bool is_tanku(const vector<string>& words, int i)
{
  vector<int> lens(lengths, lengths + num_lengths);

  int k = 0;
  while (i < words.size()) {
    if (words[i].size() > lens[k]) {
      return false;
    } else if (words[i].size() < lens[k]) {
      lens[k] -= words[i].size();
      i++;
    } else {
      lens[k] -= words[i].size();
      i++;
      k++;
      if (k == lens.size()) {
        return true;
      }
    }
  }
  
  return false;
}

int main()
{
  int n;
  while (cin >> n) {
    if (n == 0) {
      break;
    }

    vector<string> words(n);
    for (int i = 0; i < n; i++) {
      cin >> words[i];
    }

    for (int i = 0 ; i < n; i++) {
      if (is_tanku(words, i)) {
        cout << i+1 << endl;
        break;
      }
    }
  }
  
  return 0;
}
