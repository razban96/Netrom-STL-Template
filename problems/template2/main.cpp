#include <algorithm>
#include <cmath>
#include <deque>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <ranges>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

// Seteaza "" pentru input.txt, "1" pentru input1.txt, "2" pentru input2.txt etc.
#define TEST_ID "1"

#ifdef LOCAL
struct IoRedirector
{
    ifstream fin;
    ofstream fout;
    streambuf* orig_cin = nullptr;
    streambuf* orig_cout = nullptr;

    bool init(const string& in_path, const string& out_path)
    {
        fin.open(in_path);
        if (!fin.is_open())
        {
            cerr << "Eroare: nu s-a putut deschide fisierul de input: " << in_path << '\n';
            return false;
        }

        fout.open(out_path);
        if (!fout.is_open())
        {
            cerr << "Eroare: nu s-a putut deschide fisierul de output: " << out_path << '\n';
            return false;
        }

        orig_cin = cin.rdbuf(fin.rdbuf());
        orig_cout = cout.rdbuf(fout.rdbuf());
        return true;
    }

    ~IoRedirector()
    {
        if (orig_cin)
        {
            cin.rdbuf(orig_cin);
        }
        if (orig_cout)
        {
            cout.rdbuf(orig_cout);
        }
    }
};
#endif

// Implementeaza cerintele problemei folosind cin / cout
void solve()
{
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

#ifdef LOCAL
    string input_path = string(IO_DIR) + "/input" + TEST_ID + ".txt";
    string output_path = string(IO_DIR) + "/my_output.txt";

    if (argc >= 3)
    {
        input_path = argv[1];
        output_path = argv[2];
    }

    IoRedirector io;
    if (!io.init(input_path, output_path))
    {
        return 1;
    }
#endif

    solve();

    return 0;
}