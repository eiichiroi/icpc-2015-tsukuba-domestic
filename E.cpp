#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct Acquirement {
  Acquirement(size_t k, size_t locked)
    : k(k), locked(locked) {
  }

  bool hasSameLocks(size_t k, size_t locked) const {
    return this->locked & locked;
  }

  bool isDeadLocked(size_t k, size_t locked) const {
    return blocks(k, locked) && isBlockedBy(k, locked);
  }

  bool isBlockedBy(size_t k, size_t locked) const {
    return locked & (1u << this->k);
  }

  bool blocks(size_t k, size_t locked) const {
    return this->locked & (1u << k);
  }

  size_t k;
  size_t locked;
};

string solve(const string& program)
{
  const size_t n = program.size();

  vector<vector<char> > acquire(10, vector<char>(1u<<10, false));
  {
    size_t locked = 0u;
    for (size_t i = 0; i < n; i++) {
      if (program[i] == 'u') {
        locked = 0u;
      } else {
        size_t k = program[i] - '0';
        if (locked & (1u << k)) { // Cannot acquire L_k recursively
          return "UNSAFE";
        }
        acquire[k][locked] = true;
        locked = locked | (1u << k);
      }
    }
  }

  vector<Acquirement> acq;
  for (size_t k = 0; k < acquire.size(); k++) {
    for (size_t locked = 0; locked < acquire[k].size(); locked++) {
      if (acquire[k][locked]) {
        acq.push_back(Acquirement(k, locked));
      }
    }
  }
  
  for (size_t t = 1; t < 10; t++) {
    for (size_t i = 0; i < acq.size(); i++) {
      for (size_t k = 0; k < acquire.size(); k++) {
        for (size_t locked = 0; locked < acquire[k].size(); locked++) {
          if (!acquire[k][locked]) continue;
          
          if (acq[i].hasSameLocks(k, locked)) {
            continue;
          }
          
          if (acq[i].isDeadLocked(k, locked)) {
            return "UNSAFE";
          }
          
          if (acq[i].blocks(k, locked)) {
            acquire[acq[i].k][acq[i].locked | locked] = true;
          } else if (acq[i].isBlockedBy(k, locked)) {
            acquire[k][acq[i].locked | locked] = true;
          }
        }
      }
    }
  }
  
  return "SAFE";
}

int main()
{
  size_t n;
  while (cin >> n) {
    if (n == 0) {
      break;
    }

    string program;
    cin >> program;

    cout << solve(program) << endl;
  }
  return 0;
}
