#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

using namespace std;

vector<vector<vector<int>>> grid;
vector<vector<int>> gridg;
int threshold = 100;
string path = "image.ppm";

int maskX(int i, int j) {
    int mask[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int subsect[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    if (i == 0 || i == gridg.size() - 1 || j == 0 || j == gridg[0].size() - 1) {
        return 0;
    }
    for (int k = 0; k < 3; k++) {
        for (int l = 0; l < 3; l++) {
            subsect[k][l] = gridg[i + k - 1][j + l - 1];
        }
    }
    int sum = 0;
    for (int k = 0; k < 3; k++) {
        for (int l = 0; l < 3; l++) {
            sum += mask[k][l] * subsect[k][l];
        }
    }
    return sum;
}

int maskY(int i, int j) {
    int mask[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    int subsect[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    if (i == 0 || i == gridg.size() - 1 || j == 0 || j == gridg[0].size() - 1) {
        return 0;
    }
    for (int k = 0; k < 3; k++) {
        for (int l = 0; l < 3; l++) {
            subsect[k][l] = gridg[i + k - 1][j + l - 1];
        }
    }
    int sum = 0;
    for (int k = 0; k < 3; k++) {
        for (int l = 0; l < 3; l++) {
            sum += mask[k][l] * subsect[k][l];
        }
    }
    return sum;
}

void canny() {
    // read image.ppm
    ifstream infile;
    infile.open(path);
    string temp;
    infile >> temp; // get P3
    // get size
    int width, height;
    infile >> width >> height;
    infile >> temp; // get 255

    // read image
    grid.resize(height);
    for (int i = 0; i < height; i++) {
        grid[i].resize(width);
        for (int j = 0; j < width; j++) {
            grid[i][j].resize(3);
            for (int k = 0; k < 3; k++) {
                infile >> grid[i][j][k];
            }
        }
    }
    infile.close();

    // intensity
    ofstream outfile;
    outfile.open("grayscale.ppm");
    outfile << "P3" << endl;
    outfile << width << " " << height << endl;
    outfile << "255" << endl;
    gridg.resize(height);
    for (int i = 0; i < height; i++) {
        gridg[i].resize(width);
        for (int j = 0; j < width; j++) {
            int intensity = 0;
            for (int k = 0; k < 3; k++) {
                intensity += grid[i][j][k];
            }
            intensity /= 3;
            outfile << intensity << " " << intensity << " " << intensity << " ";
            gridg[i][j] = intensity;
        }
        outfile << endl;
    }
    outfile.close();

    // Gx and Gy
    vector<vector<int>> gridgx;
    gridgx.resize(height);
    vector<vector<int>> gridgy;
    gridgy.resize(height);
    for (int i = 0; i < height; i++) {
        gridgx[i].resize(width);
        gridgy[i].resize(width);
        for (int j = 0; j < width; j++) {
            gridgx[i][j] = maskX(i, j);
            gridgy[i][j] = maskY(i, j);
        }
    }

    // G
    outfile.open("canny.ppm");
    outfile << "P3" << endl;
    outfile << width << " " << height << endl;
    outfile << "1" << endl;
    vector<vector<int>> gridm;
    gridm.resize(height);
    for (int i = 0; i < height; i++) {
        gridm[i].resize(width);
        for (int j = 0; j < width; j++) {
            gridm[i][j] = sqrt(gridgx[i][j] * gridgx[i][j] + gridgy[i][j] * gridgy[i][j]);
            if (gridm[i][j] > threshold) {
                outfile << "1 1 1 ";
            } else {
                outfile << "0 0 0 ";
            }
        }
    }
    outfile.close();
}

int main() {
    canny();
    return 0;
}