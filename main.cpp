#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

// Schimba doar acest ID pentru a rula input1.txt, input2.txt, input3.txt etc.
#define TEST_ID "1"

// Implementeaza cerintele problemei folosind cin / cout
void solve() {

}

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

#ifdef LOCAL
    string input_path = string(IO_DIR) + "/input" + TEST_ID + ".txt";
    string output_path = string(IO_DIR) + "/my_output.txt";

    if (argc >= 3) {
        input_path = argv[1];
        output_path = argv[2];
    }

    freopen(input_path.c_str(), "r", stdin);
    freopen(output_path.c_str(), "w", stdout);
#endif

    solve();

    return 0;
}