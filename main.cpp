#include <algorithm>
#include <array>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdio>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#define REP(i, n) for (int i = 0; (i) < (int)(n); ++ (i))
#define REP3(i, m, n) for (int i = (m); (i) < (int)(n); ++ (i))
#define REP_R(i, n) for (int i = int(n) - 1; (i) >= 0; -- (i))
#define REP3R(i, m, n) for (int i = int(n) - 1; (i) >= (int)(m); -- (i))
#define ALL(x) begin(x), end(x)
#define dump(x) cerr << #x " = " << x << endl
#define unittest_name_helper(counter) unittest_ ## counter
#define unittest_name(counter) unittest_name_helper(counter)
#define unittest __attribute__((constructor)) void unittest_name(__COUNTER__) ()
using ll = long long;
using namespace std;
template <class T> using reversed_priority_queue = priority_queue<T, vector<T>, greater<T> >;
template <class T, class U> inline void chmax(T & a, U const & b) { a = max<T>(a, b); }
template <class T, class U> inline void chmin(T & a, U const & b) { a = min<T>(a, b); }
template <typename X, typename T> auto vectors(X x, T a) { return vector<T>(x, a); }
template <typename X, typename Y, typename Z, typename... Zs> auto vectors(X x, Y y, Z z, Zs... zs) { auto cont = vectors(y, z, zs...); return vector<decltype(cont)>(x, cont); }
template <typename T> ostream & operator << (ostream & out, vector<T> const & xs) { REP (i, int(xs.size()) - 1) out << xs[i] << ' '; if (not xs.empty()) out << xs.back(); return out; }


/*******************************************************************************
 * 整数幾何
 ******************************************************************************/

struct vec2 {
    int y, x;
};

inline vec2 make_vec2(int y, int x) { return (vec2) { y, x }; }
inline vec2 operator + (vec2 const & a, vec2 const & b) { return make_vec2(a.y + b.y, a.x + b.x); }
inline vec2 operator - (vec2 const & a, vec2 const & b) { return make_vec2(a.y - b.y, a.x - b.x); }
inline vec2 operator * (int a, vec2 const & b) { return make_vec2(a * b.y, a * b.x); }
inline int   dot(vec2 const & a, vec2 const & b) { return a.x * b.x + a.y * b.y; }
inline int cross(vec2 const & a, vec2 const & b) { return a.x * b.y - a.y * b.x; }
inline int ccw(vec2 const & a, vec2 const & b, vec2 const & c) { ll z = cross(b - a, c - a); return z > 0 ? 1 : z < 0 ? -1 : 0; }
inline bool operator == (vec2 const a, vec2 const & b) { return a.y == b.y and a.x == b.x; }
inline bool operator != (vec2 const a, vec2 const & b) { return a.y != b.y  or a.x != b.x; }

istream & operator >> (istream & in, vec2 & a) {
    return in >> a.x >> a.y;
}

ostream & operator << (ostream & out, vec2 const & a) {
    return out << a.x << ' ' << a.y;
}


/*******************************************************************************
 * 入出力用 構造体
 ******************************************************************************/

struct config_t {
    ll think_time;  // in msec
    int step_limit;
    int width, height;
    int vision;
};

istream & operator >> (istream & in, config_t & a) {
    in >> a.think_time;
    in >> a.step_limit;
    in >> a.width >> a.height;
    in >> a.vision;
    return in;
}

struct jockey_t {
    vec2 p, v;
};

istream & operator >> (istream & in, jockey_t & a) {
    return in >> a.p >> a.v;
}

enum cell_t : int8_t {
    PLAIN = 0,
    WALL = 1,
    WATER = 2,
    UNKNOWN = -1,
};

struct input_t {
    int step_number;
    ll time_left;  // in msec
    jockey_t jockey[2];
    vector<vector<cell_t> > field;
};

istream & operator >> (istream & in, input_t & a) {
    in >> a.step_number;
    in >> a.time_left;
    in >> a.jockey[0];
    in >> a.jockey[1];
    REP (y, a.field.size()) {  // the size of a.field and a.field[y] must be initialized
        REP (x, a.field[y].size()) {
            int value; cin >> value;
            a.field[y][x] = (cell_t)value;
        }
    }
    return in;
}

struct output_t {
    vec2 f;
};

ostream & operator << (ostream & out, output_t const & a) {
    return out << a.f;
}


/*******************************************************************************
 * simulator
 ******************************************************************************/

bool will_enter(jockey_t const & a, vec2 const & cell) {
    if (a.v == make_vec2(0, 0)) return a.p == cell;
    auto x = 2 * a.p;
    auto y = x + 2 * a.v;
    int cnt[3] = {};
    cnt[ccw(x, y, 2 * cell + make_vec2(-1, -1)) + 1] += 1;
    cnt[ccw(x, y, 2 * cell + make_vec2(-1, +1)) + 1] += 1;
    cnt[ccw(x, y, 2 * cell + make_vec2(+1, -1)) + 1] += 1;
    cnt[ccw(x, y, 2 * cell + make_vec2(+1, +1)) + 1] += 1;
    return not (cnt[0] == 4 or cnt[2] == 4);
}

bool will_move(jockey_t const a, vector<vector<cell_t> > const & field) {
    const int height = field.size();
    const int width = field.front().size();
    vec2 np = a.p + a.v;
    if (np.y < 0 or np.x < 0 or np.x >= width) return false;
    int ly, ry; tie(ly, ry) = minmax({ a.p.y, np.y });
    int lx, rx; tie(lx, rx) = minmax({ a.p.x, np.x });
    REP3 (y, ly, ry + 1) {
        if (y >= height) continue;
        REP3 (x, lx, rx + 1) {
            if (field[y][x] == WALL and will_enter(a, make_vec2(y, x))) {
                return false;
            }
        }
    }
    return true;
}

inline tuple<int, int, int, int> get_aabb(jockey_t const & a) {
    int ly, ry; tie(ly, ry) = minmax({ a.p.y, a.p.y + a.v.y });
    int lx, rx; tie(lx, rx) = minmax({ a.p.x, a.p.x + a.v.x });
    return tie(ly, ry, lx, rx);
}

bool will_conflict(jockey_t const & a, jockey_t const & b) {
    int x = ccw(a.p, a.p + a.v, b.p);
    int y = ccw(a.p, a.p + a.v, b.p + b.v);
    int z = ccw(b.p, b.p + b.v, a.p);
    int w = ccw(b.p, b.p + b.v, a.p + a.v);
    if (x * y == 0 and z * w == 0) {  // on the same line
        // check the AABBs
        int aly, ary, alx, arx; tie(aly, ary, alx, arx) = get_aabb(a);
        int bly, bry, blx, brx; tie(bly, bry, blx, brx) = get_aabb(b);
        if (ary < bly or bry < aly) return false;
        if (arx < blx or brx < alx) return false;
        return true;
    }
    return x * y <= 0 and z * w <= 0;
}

int get_priority(jockey_t const & a, jockey_t const & b) {
    assert (a.p != b.p);
    bool p = will_enter(a, b.p);
    bool q = will_enter(b, a.p);
    if (p and q) return -1;
    if (p) return 1;
    if (q) return 0;
    if (a.p.y < b.p.y) return 0;
    if (b.p.y < a.p.y) return 1;
    if (a.p.x < b.p.x) return 0;
    if (b.p.x < a.p.x) return 1;
    assert (false);
}

pair<bool, bool> resolve_confliction(jockey_t const & a, jockey_t const & b) {
    assert (a.p != b.p);
    if (not will_conflict(a, b)) {
        return make_pair(true, true);
    }
    switch (get_priority(a, b)) {
        case 0: return make_pair(true, false);
        case 1: return make_pair(false, true);
        case -1: return make_pair(false, false);
        default: assert (false);
    }
}


/*******************************************************************************
 * AI class
 ******************************************************************************/

class ai_t {
    const config_t config;

public:
    ai_t(config_t const & config_)
        : config(config_) {
    }

    output_t operator () (input_t const & input) {
        auto may_conflict = [&](jockey_t const & a) {
            REP3 (fy, -1, 2) {
                REP3 (fx, -1, 2) {
                    jockey_t b = input.jockey[1];
                    b.v = b.v + make_vec2(fy, fx);
                    if (not will_move(b, input.field)) continue;
                    if (not resolve_confliction(a, b).first) {
                        return true;
                    }
                }
            }
            return false;
        };

        output_t output = {};
        int highscore = INT_MIN;
        REP3 (fy, -1, 2) {
            REP3 (fx, -1, 2) {
                jockey_t a = input.jockey[0];
                a.v = a.v + make_vec2(fy, fx);
                if (fy == 0 and fx == 0 and a.v == make_vec2(0, 0)) continue;
                if (not will_move(a, input.field)) continue;
                bool conflicted = may_conflict(a);
                if (conflicted) continue;
                double score = fy - (conflicted ? 0.5 : 0);
                if (score > highscore) {
                    highscore = score;
                    output.f = make_vec2(fy, fx);
                }
            }
        }
        return output;
    }
};


/*******************************************************************************
 * main 関数
 ******************************************************************************/

int main() {
    config_t config;
    cin >> config;
    ai_t ai(config);
    cout << 0 << endl;
    cout.flush();
    while (true) {
        input_t input;
        input.field.resize(config.height, vector<cell_t>(config.width));
        cin >> input;
        output_t output = ai(input);
        cout << output << endl;
        cout.flush();
    }
    return 0;
}
