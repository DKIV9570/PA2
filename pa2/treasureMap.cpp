#include "treasureMap.h"
#include "queue.h"
#include <cmath>
using namespace std;

treasureMap::treasureMap(const PNG & baseim, const PNG & mazeim, pair<int,int> s)
{
    start = s;
    base = baseim;
    maze = mazeim;
}

void treasureMap::setGrey(PNG & im, pair<int,int> loc){
    int x = loc.first;
    int y = loc.second;
    RGBAPixel *pixel = im.getPixel(x,y);
    pixel->r = 2*((pixel->r)/4);
    pixel->g = 2*((pixel->g)/4);
    pixel->b = 2*((pixel->b)/4);
}

void treasureMap::setLOB(PNG & im, pair<int,int> loc, int d){
    int x = loc.first;
    int y = loc.second;
    RGBAPixel *pixel = im.getPixel(x,y);

    vector<int> binary_d = ten_2(d%64, 6);
    vector<int> binary_r = ten_2(pixel->r, 8);
    vector<int> binary_g = ten_2(pixel->g, 8);
    vector<int> binary_b = ten_2(pixel->b, 8);

    binary_r[6] = binary_d[0];
    binary_r[7] = binary_d[1];
    binary_g[6] = binary_d[2];
    binary_g[7] = binary_d[3];
    binary_b[6] = binary_d[4];
    binary_b[7] = binary_d[5];

    pixel->r = two_10(binary_r);
    pixel->g = two_10(binary_g);
    pixel->b = two_10(binary_b);
}

int treasureMap::two_10(vector<int> a){
    int b = 0;
    for(int i= 0;i<a.size();i++){
        b += (pow(2,a.size()-i-1))*a[i];
    }
    return b;
}

vector<int> treasureMap::ten_2(int n, int digit){
    vector<int> a;
    vector<int> result;

    while(n)
    {
        a.push_back(n%2);
        n/=2;
    }
    while(a.size()<digit){
        a.push_back(0);
    }
    for(int i=a.size()-1;i>=0;i--)
        result.push_back(a[i]);
    return result;
}

PNG treasureMap::renderMap(){

    PNG copy = base;
    vector<vector<bool>> visit(copy.width(), vector<bool> (copy.height(), false));
    vector<vector<int>> distance(copy.width(), vector<int> (copy.height()));
    Queue<pair<int, int>> locations;

    visit[start.first][start.second] = true;
    distance[start.first][start.second] = 0;
    setLOB(copy, start, 0);
    locations.enqueue(start);

    while(!locations.isEmpty()) {
        pair<int, int> curr = locations.peek();
        vector<pair<int,int>> neighbor = neighbors(curr);
        locations.dequeue();
        for(int i = 0; i < 4; i++){
            if(good(visit, curr, neighbor[i])){
                visit[neighbor[i].first][neighbor[i].second] = true;
                distance[neighbor[i].first][neighbor[i].second] = distance[curr.first][curr.second] + 1;
                int dis = distance[neighbor[i].first][neighbor[i].second];
                setLOB(copy, neighbor[i], dis);
                locations.enqueue(neighbor[i]);
            }
        }
    }
    return copy;
}


PNG treasureMap::renderMaze(){

    PNG copy = base;
    vector<vector<bool>> visit(copy.width(), vector<bool> (copy.height(), false));
    Queue<pair<int, int>> locations;

    for(int i = start.first-3; i <= start.first+3; i++){
        if(i>=0 && i < copy.width()){
            for(int j = start.second-3; j <= start.second+3; j++){
                if(j>=0 && j < copy.height()){
                    RGBAPixel *pixel = copy.getPixel(i,j);
                    pixel->r = 255;
                    pixel->g = 0;
                    pixel->b = 0;
                }
            }
        }
    }

    visit[start.first][start.second] = true;

    while(!locations.isEmpty()) {
        pair<int, int> curr = locations.peek();
        vector<pair<int,int>> neighbor = neighbors(curr);
        visit[curr.first][curr.second] = true;
        locations.dequeue();
        for(int i = 0; i < 4; i++){
            if(good(visit, curr, neighbor[i])){
                setGrey(copy, neighbor[i]);
                locations.enqueue(neighbor[i]);
            }
        }
    }

    return copy;

}

bool treasureMap::good(vector<vector<bool>> & v, pair<int,int> curr, pair<int,int> next){
    if(next.first >= base.width() || next.second >= base.height() || next.first < 0 || next.second < 0){
        return false;
    } else if (v[next.first][next.second] == true){
        return false;
    } else {
        RGBAPixel *p_curr = maze.getPixel(curr.first,curr.second);
        RGBAPixel *p_next = maze.getPixel(next.first,next.second);

        return (p_curr->r == p_next->r) && (p_curr->g == p_next->g) && (p_curr->b == p_next->b);
    }
}

vector<pair<int,int>> treasureMap::neighbors(pair<int,int> curr) {
    vector<pair<int,int>> n;
    int x = curr.first;
    int y = curr.second;
    n.push_back(pair<int,int>(x-1,y));
    n.push_back(pair<int,int>(x,y+1));
    n.push_back(pair<int,int>(x+1,y));
    n.push_back(pair<int,int>(x,y-1));

    return n;
}


