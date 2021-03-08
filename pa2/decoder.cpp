#include "decoder.h"
#include "queue.h"
#include "stack.h"
#include "treasureMap.h"
#include <cmath>
using namespace std;

decoder::decoder(const PNG & tm, pair<int,int> s)
   :start(s),mapImg(tm) {

    vector<vector<bool>> visit(mapImg.width(), vector<bool> (mapImg.height(), false));
    vector<vector<int>> distance(mapImg.width(), vector<int> (mapImg.height(), 0));
    vector<vector<pair<int, int>>> prev(mapImg.width(), vector<pair<int, int>> (mapImg.height()));

    int max = 0;

    pair<int, int> treasure;
    Queue<pair<int, int>> locations;

    locations.enqueue(start);
    visit[start.first][start.second] = true;
    distance[start.first][start.second] = 0;

    while(!locations.isEmpty()) {
        pair<int, int> curr = locations.peek();
        vector<pair<int,int>> neighbor = neighbors(curr);
        visit[curr.first][curr.second] = true;
        locations.dequeue();
        for(int i = 0; i < 4; i++){
            if(good(visit, distance, curr, neighbor[i])){
                distance[neighbor[i].first][neighbor[i].second] = distance[curr.first][curr.second] + 1;
                if(distance[neighbor[i].first][neighbor[i].second] > max){
                    max = distance[neighbor[i].first][neighbor[i].second];
                    treasure = neighbor[i];
                }
                prev[neighbor[i].first][neighbor[i].second] = curr;
                locations.enqueue(neighbor[i]);
            }
        }
    }

    pathPts.push_back(treasure);
    pair<int, int> curr = treasure;

    while(curr != start){
        curr = prev[curr.first][curr.second];
        pathPts.insert(pathPts.begin(), prev[curr.first][curr.second]);
    }

}

PNG decoder::renderSolution(){

    for(int i = 0; i < pathPts.size(); i++){
        RGBAPixel *pixel = mapImg.getPixel(pathPts[i].first, pathPts[i].second);
        pixel->r = 255;
        pixel->g = 0;
        pixel->b = 0;
    }

}

PNG decoder::renderMaze(){

    PNG copy = mapImg;
    vector<vector<bool>> visit(copy.width(), vector<bool> (copy.height(), false));
    vector<vector<int>> distance(copy.width(), vector<int> (copy.height()));
    Queue<pair<int, int>> locations;



    locations.enqueue(start);
    visit[start.first][start.second] = true;
    distance[start.first][start.second] = 0;
    setGrey(start);

    while(!locations.isEmpty()) {
        pair<int, int> curr = locations.peek();
        vector<pair<int,int>> neighbor = neighbors(curr);
        visit[curr.first][curr.second] = true;
        locations.dequeue();
        for(int i = 0; i < 4; i++){
            if(good(visit, distance, curr, neighbor[i])){
                setGrey(copy, neighbor[i]);
                distance[neighbor[i].first][neighbor[i].second] = distance[curr.first][curr.second] + 1;
                locations.enqueue(neighbor[i]);
            }
        }
    }

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

    return copy;
}


void decoder::setGrey(PNG & im, pair<int,int> loc){

    int x = loc.first;
    int y = loc.second;
    RGBAPixel *pixel = im.getPixel(x,y);
    pixel->r = 2*((pixel->r)/4);
    pixel->g = 2*((pixel->g)/4);
    pixel->b = 2*((pixel->b)/4);
}

pair<int,int> decoder::findSpot(){
    return pathPts[pathPts.size()-1];
}

int decoder::pathLength(){
    return pathPts.size()-1;
}

bool decoder::good(vector<vector<bool>> & v, vector<vector<int>> & d, pair<int,int> curr, pair<int,int> next){
    if(next.first >= (int)mapImg.width() || next.second >= (int)mapImg.height() || next.first < 0 || next.second < 0){
        return false;
    } else if (v[next.first][next.second] == true){
        return false;
    } else {
        RGBAPixel *p_next = mapImg.getPixel(next.first,next.second);
        int dis = d[curr.first][curr.second];
        return compare(p_next, dis);
    }
}

vector<pair<int,int>> decoder::neighbors(pair<int,int> curr) {

    vector<pair<int,int>> n;
    int x = curr.first;
    int y = curr.second;
    n.push_back(pair<int,int>(x-1,y));
    n.push_back(pair<int,int>(x,y+1));
    n.push_back(pair<int,int>(x+1,y));
    n.push_back(pair<int,int>(x,y-1));

    return n;

}


bool decoder::compare(RGBAPixel *p, int d){
    vector<int> bin_r = ten_2(p->r,6);
    vector<int> bin_g = ten_2(p->g,6);
    vector<int> bin_b = ten_2(p->b,6);

    vector<int> final;
    final.push_back(bin_r[4]);
    final.push_back(bin_r[5]);
    final.push_back(bin_g[4]);
    final.push_back(bin_g[5]);
    final.push_back(bin_b[4]);
    final.push_back(bin_b[5]);

    int final_i = two_10(final);
    if(final_i == 0 && d == 63){
        return true;
    }else{
        return(final_i == d+1);
    }
}

int decoder::two_10(vector<int> a){
    int b = 0;
    for(int i= 0;i<a.size();i++){
        b += (pow(2,a.size()-i-1))*a[i];
    }
    return b;
}

vector<int> decoder::ten_2(int n, int digit){
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
