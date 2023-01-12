#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <string.h>

using namespace std;

int width, height;

string path = "image.ppm";
vector<vector<vector<int>>> grid; // original pixel values
vector<vector<int>> gridg; // grayscale
vector<vector<int>> gridm; // magnitude
vector<vector<int>> grida; // angle
vector<vector<int>> gridh; // hysteresis
vector<vector<int>> gridn; // non max suppression
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

int roundAngle(double angle) {
    // round to 0, 45, 90, 135, 180, -45, -90, -135, -180
    if (angle > -22.5 && angle <= 22.5) {
        return 0;
    } else if (angle > 22.5 && angle <= 67.5) {
        return 45;
    } else if (angle > 67.5 && angle <= 112.5) {
        return 90;
    } else if (angle > 112.5 && angle <= 157.5) {
        return 135;
    } else if (angle > 157.5 || angle <= -157.5) {
        return 180;
    } else if (angle > -157.5 && angle <= -112.5) {
        return -135;
    } else if (angle > -112.5 && angle <= -67.5) {
        return -90;
    } else if (angle > -67.5 && angle <= -22.5) {
        return -45;
    }
}

bool nonMaxSupp(int i, int j, int a) {
    if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
        return false;
    } else if (a == 0 || a == 180) {
        if (gridm[i][j] > gridm[i][j - 1] && gridm[i][j] > gridm[i][j + 1]) {
            return true;
        }
    } else if (a == 45 || a == -135) {
        if (gridm[i][j] > gridm[i - 1][j - 1] && gridm[i][j] > gridm[i + 1][j + 1]) {
            return true;
        }
    } else if (a == 90 || a == -90) {
        if (gridm[i][j] > gridm[i - 1][j] && gridm[i][j] > gridm[i + 1][j]) {
            return true;
        }
    } else if (a == 135 || a == -45) {
        if (gridm[i][j] > gridm[i - 1][j + 1] && gridm[i][j] > gridm[i + 1][j - 1]) {
            return true;
        }
    }
    return false;
}

void suppression(int argc, char** argv) {
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
    grida.resize(height);
    for (int i = 0; i < height; i++) {
        gridm[i].resize(width);
        grida[i].resize(width);
        for (int j = 0; j < width; j++) {
            gridm[i][j] = sqrt(gridgx[i][j] * gridgx[i][j] + gridgy[i][j] * gridgy[i][j]);
            double angle = atan2(gridgy[i][j], gridgx[i][j]);
            int roundedAngle = roundAngle(angle);
            grida[i][j] = roundedAngle;
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

    // non max suppression 
    gridn.resize(height);
    for (int i = 0; i < height; i++) {
        gridn[i].resize(width);
        for (int j = 0; j < width; j++) {
            if (nonMaxSupp(i, j, grida[i][j]) && gridh[i][j] == 3) {
                gridn[i][j] = 1;
            } else {
                gridn[i][j] = 0;
            }
        }
    }

    // output
    ofstream outfile;
    outfile.open("suppression.ppm");
    outfile << "P3" << endl;
    outfile << width << " " << height << endl;
    outfile << "1" << endl;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (gridn[i][j] == 1) {
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
    suppression(argc, argv);
    return 0;
}