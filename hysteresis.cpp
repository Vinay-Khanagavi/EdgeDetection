#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <string.h>

using namespace std;

int width, height;

string path = "image.ppm";
vector<vector<vector<int>>> grid;
vector<vector<int>> gridg;
vector<vector<int>> gridm;
vector<vector<int>> gridh;
int lthresh = 75;
int hthresh = 150;

int maskX(int i, int j) {
    int mask[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int subsect[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
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
    if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
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

void areaFill(int i, int j) {
    if (i < 0 || i >= height || j < 0 || j >= width || gridh[i][j] == 3 || gridh[i][j] == 0) {
        return;
    }
    if (gridh[i][j] == 2 || gridh[i][j] == 1) {
        gridh[i][j] = 3;
        areaFill(i - 1, j);
        areaFill(i + 1, j);
        areaFill(i, j - 1);
        areaFill(i, j + 1);
        areaFill(i - 1, j - 1);
        areaFill(i - 1, j + 1);
        areaFill(i + 1, j - 1);
        areaFill(i + 1, j + 1);
    }
}

void hysteresis(int argc, char** argv) {
    for (int i = 0; i < argc-1; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            path = argv[i + 1];
        } else if (strcmp(argv[i], "-l") == 0) {
            lthresh = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-h") == 0) {
            hthresh = atoi(argv[i + 1]);
        }
    }

    // read image.ppm
    ifstream infile;
    infile.open(path);
    string temp;
    infile >> temp; // get P3
    // get size
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
    gridg.resize(height);
    for (int i = 0; i < height; i++) {
        gridg[i].resize(width);
        for (int j = 0; j < width; j++) {
            int intensity = 0;
            for (int k = 0; k < 3; k++) {
                intensity += grid[i][j][k];
            }
            intensity /= 3;
            gridg[i][j] = intensity;
        }
    }

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
    gridm.resize(height);
    for (int i = 0; i < height; i++) {
        gridm[i].resize(width);
        for (int j = 0; j < width; j++) {
            gridm[i][j] = sqrt(gridgx[i][j] * gridgx[i][j] + gridgy[i][j] * gridgy[i][j]);
        }
    }

    // double threshold
    gridh.resize(height);
    for (int i = 0; i < height; i++) {
        gridh[i].resize(width);
        for (int j = 0; j < width; j++) {
            if (gridm[i][j] > hthresh) {
                gridh[i][j] = 2;
            } else if (gridm[i][j] > lthresh) {
                gridh[i][j] = 1;
            } else {
                gridh[i][j] = 0;
            }
        }
    }

    // hysteresis
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (gridh[i][j] == 2) {
                areaFill(i, j);
            }
        }
    }

    // output
    ofstream outfile;
    outfile.open("hysteresis.ppm");
    outfile << "P3" << endl;
    outfile << width << " " << height << endl;
    outfile << "1" << endl;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (gridh[i][j] == 3) {
                outfile << "1 1 1 ";
            } else {
                outfile << "0 0 0 ";
            }
        }
        outfile << endl;
    }
    outfile.close();
}

int main(int argc, char** argv) {
    hysteresis(argc, argv);
    return 0;
}