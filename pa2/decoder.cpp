#include "decoder.h"
#include "queue.h"
#include "stack.h"
#include "treasureMap.h"
#include <cmath>

using namespace std;

decoder::decoder(const PNG &tm, pair<int, int> s)
        : start(s), mapImg(tm) {

    vector<vector<bool>> visit(mapImg.width(), vector<bool>(mapImg.height(), false));
    vector<vector<int>> distance(mapImg.width(), vector<int>(mapImg.height(), 0));
    vector<vector<pair<int, int>>> prev(mapImg.width(), vector<pair<int, int>>(mapImg.height()));

    int max = 0;

    pair<int, int> treasure;
    Queue <pair<int, int>> locations;

    locations.enqueue(start);
    visit[start.first][start.second] = true;
    distance[start.first][start.second] = 0;

    while (!locations.isEmpty()) {

        pair<int, int> curr = locations.peek();
        visit[curr.first][curr.second] = true;
        vector<pair<int, int>> neighbor = neighbors(curr);
        locations.dequeue();

        for (int i = 0; i < 4; i++) {
            if (good(visit, distance, curr, neighbor[i])) {

                distance[neighbor[i].first][neighbor[i].second] = distance[curr.first][curr.second] + 1;
//                visit[neighbor[i].first][neighbor[i].second] = true;

                if (distance[neighbor[i].first][neighbor[i].second] > max) {
                    max = distance[neighbor[i].first][neighbor[i].second];
                    treasure = neighbor[i];
                }

                prev[neighbor[i].first][neighbor[i].second] = curr;
                locations.enqueue(neighbor[i]);
                cout << "added" << neighbor[i].first << " " << neighbor[i].second << endl;
            }
        }
//        cout << "for loop finished" << endl;
    }
    cout << "finish loop1" << endl;

    pair<int, int> now = treasure;
    this->pathPts.push_back(now);
    

    while (now != start) {
        now = prev[now.first][now.second];
        pathPts.insert(pathPts.begin(), prev[now.first][now.second]);
    }
    cout << "finish loop2"<<endl;
    for(int i=0; i<pathPts.size();i++){
        cout<<"("<<pathPts[i].first<<","<<pathPts[i].second<<")"<<endl;
    }
}

PNG decoder::renderSolution() {

    PNG copy = mapImg;
    for (int i = 1; i < pathPts.size(); i++) {
        RGBAPixel *pixel = copy.getPixel(pathPts[i].first, pathPts[i].second);
        pixel->r = 255;
        pixel->g = 0;
        pixel->b = 0;
    }
    return copy;

}

PNG decoder::renderMaze() {

    PNG copy = mapImg;
    vector<vector<bool>> visit(copy.width(), vector<bool>(copy.height(), false));
    vector<vector<int>> distance(copy.width(), vector<int>(copy.height()));
    Queue <pair<int, int>> locations;


    locations.enqueue(start);
    visit[start.first][start.second] = true;
    distance[start.first][start.second] = 0;
    setGrey(copy, start);

    cout << "begin loop3";
    while (!locations.isEmpty()) {
        pair<int, int> curr = locations.peek();
        vector<pair<int, int>> neighbor = neighbors(curr);
        visit[curr.first][curr.second] = true;
        locations.dequeue();
        for (int i = 0; i < 4; i++) {
            if (good(visit, distance, curr, neighbor[i])) {
                setGrey(copy, neighbor[i]);
                distance[neighbor[i].first][neighbor[i].second] = distance[curr.first][curr.second] + 1;
                locations.enqueue(neighbor[i]);
            }
        }
    }
    cout << "finish loop3";
    for (int i = start.first - 3; i <= start.first + 3; i++) {
        if (i >= 0 && i < copy.width()) {
            for (int j = start.second - 3; j <= start.second + 3; j++) {
                if (j >= 0 && j < copy.height()) {
                    RGBAPixel *pixel = copy.getPixel(i, j);
                    pixel->r = 255;
                    pixel->g = 0;
                    pixel->b = 0;
                }
            }
        }
    }

    return copy;
}


void decoder::setGrey(PNG &im, pair<int, int> loc) {

    int x = loc.first;
    int y = loc.second;
    RGBAPixel *pixel = im.getPixel(x, y);
    pixel->r = 2 * ((pixel->r) / 4);
    pixel->g = 2 * ((pixel->g) / 4);
    pixel->b = 2 * ((pixel->b) / 4);
}

pair<int, int> decoder::findSpot() {
    return pathPts[pathPts.size() - 1];
}

int decoder::pathLength() {
    return pathPts.size() - 1;
}

bool decoder::good(vector<vector<bool>> &v, vector<vector<int>> &d, pair<int, int> curr, pair<int, int> next) {
    if (next.first >= mapImg.width() || next.second >= mapImg.height() || next.first < 0 || next.second < 0) {
//        cout<<"case1"<<endl;
        return false;
    } else if (v[next.first][next.second]) {
//        cout<<"case2"<<endl;
        return false;
    } else {
//        cout<<"case3"<<endl;
        RGBAPixel *p_next = mapImg.getPixel(next.first, next.second);
        int dis = d[curr.first][curr.second];
        return compare(p_next, dis);
    }
}

vector<pair<int, int>> decoder::neighbors(pair<int, int> curr) {

    vector<pair<int, int>> n;
    int x = curr.first;
    int y = curr.second;
    n.push_back(pair<int, int>(x - 1, y));
    n.push_back(pair<int, int>(x, y + 1));
    n.push_back(pair<int, int>(x + 1, y));
    n.push_back(pair<int, int>(x, y - 1));

    return n;

}


bool decoder::compare(RGBAPixel *p, int d) {
    vector<int> bin_r = ten_2(p->r, 8);
    vector<int> bin_g = ten_2(p->g, 8);
    vector<int> bin_b = ten_2(p->b, 8);

    vector<int> final;
    final.push_back(bin_r[6]);
    final.push_back(bin_r[7]);
    final.push_back(bin_g[6]);
    final.push_back(bin_g[7]);
    final.push_back(bin_b[6]);
    final.push_back(bin_b[7]);

    int final_i = two_10(final);
//    cout<<"final_i="<<final_i<<" "<<"d = "<<d<<endl;
    if (final_i == 0 && d == 63) {
        return true;
    } else {
        return (final_i == d + 1);
    }
}

int decoder::two_10(vector<int> a) {
    int b = 0;
    for (int i = 0; i < a.size(); i++) {
        b += (pow(2, a.size() - i - 1)) * a[i];
    }
    return b;
}

vector<int> decoder::ten_2(int n, int digit) {
    vector<int> a;
    vector<int> result;

    while (n) {
        a.push_back(n % 2);
        n /= 2;
    }
    while (a.size() < digit) {
        a.push_back(0);
    }
    for (int i = a.size() - 1; i >= 0; i--)
        result.push_back(a[i]);
    return result;
}
