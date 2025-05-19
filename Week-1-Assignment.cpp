// Provides input and output functionality using streams:
#include <iostream>
// To use a vector, we have to include the <vector> header file:
#include <vector>
// Allow to perform mathematical tasks on numbers:
#include <cmath>
// C++ library that contains a superset of the traditional C functions, macros,
// and datatypes:
#include <cstdlib>
// Allow to measure dates and times:
#include <ctime>
// Provides the ability to create and manage threads, enabling 
// concurrent execution of code:
#include <thread>
// Used to deal with date and time:
#include <chrono>
// Provides numeric limits for fundamental data types (like int, float,
// double, etc.):
#include <limits>

// Checks if the code is being compiled on Windows: 
#ifdef _WIN32
// If true, include this header, W\which gives access to Windows-specific functions like
// Sleep() or console manipulation:
#include <windows.h>
#else
// Else, include this header, whish gives access to non-Windows platforms (like Linux, macOS)
// like usleep() or sleep():
#include <unistd.h>
#endif

// Telling the compiler to consider all the names in the std namespace as if they're in the global namespace:
using namespace std;

// Defines the number of columns in the console grid (like the X-axis):
const int WIDTH = 80;
// Defines the number of rows in the console grid (like the Y-axis): 
const int HEIGHT = 25;

// This code defines an array of ANSI escape color codes for the terminal,
// allowing the program to print colored text on the console:
const string COLORS[] = {
    "\033[31m", // red
    "\033[32m", // green
    "\033[33m", // yellow
    "\033[34m", // blue
    "\033[35m", // magenta
    "\033[36m", // cyan
    "\033[91m", // bright red
    "\033[92m", // bright green
};

// This code defines a constant ANSI escape sequence that resets terminal
// text formatting (like color, bold, underline) back to default settings:
const string RESET = "\033[0m";

// Dimensions of the 2-Dimensional Space and the cluster variable:
struct Point {
    int x, y;
    int cluster = 0;
};

// Dimensions of the Centroid:
struct Centroid {
    float x1, y1;
};

float distance(Point a, Centroid b) {
    return sqrt((a.x - b.x1) * (a.x - b.x1) + (a.y - b.y1) * (a.y - b.y1));
}

void assignClusters(vector<Point>& points, const vector<Centroid>& centroids) {
    for (auto& p : points) {
        float minDist = numeric_limits<float>::max();
        for (int i = 0; i < centroids.size(); ++i) {
            float dist = distance(p, centroids[i]);
            if (dist < minDist) {
                minDist = dist;
                p.cluster = i;
            }
        }
    }
}

bool updateCentroids(const vector<Point>& points, vector<Centroid>& centroids) {
    vector<float> sumX(centroids.size(), 0), sumY(centroids.size(), 0);
    vector<int> count(centroids.size(), 0);

    for (const auto& p : points) {
        sumX[p.cluster] += p.x;
        sumY[p.cluster] += p.y;
        count[p.cluster]++;
    }

    bool changed = false;
    for (int i = 0; i < centroids.size(); ++i) {
        if (count[i] == 0) continue;
        float newX = sumX[i] / count[i];
        float newY = sumY[i] / count[i];
        if (newX != centroids[i].x1 || newY != centroids[i].y1) {
            centroids[i].x1 = newX;
            centroids[i].y1 = newY;
            changed = true;
        }
    }

    return changed;
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[1;1H"; // ANSI clear + move cursor
#endif
}

void draw(const vector<Point>& points, const vector<Centroid>& centroids) {
    vector<string> grid(HEIGHT, string(WIDTH, ' '));

    for (const auto& p : points) {
        if (p.y >= 0 && p.y < HEIGHT && p.x >= 0 && p.x < WIDTH) {
            grid[p.y][p.x] = 'o';
        }
    }

    for (int i = 0; i < centroids.size(); ++i) {
        int cx = static_cast<int>(centroids[i].x1);
        int cy = static_cast<int>(centroids[i].y1);
        if (cy >= 0 && cy < HEIGHT && cx >= 0 && cx < WIDTH) {
            grid[cy][cx] = 'X';
        }
    }

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            bool isCentroid = false;
            for (int i = 0; i < centroids.size(); ++i) {
                if ((int)centroids[i].x1 == x && (int)centroids[i].y1 == y) {
                    cout << COLORS[i % 8] << 'X' << RESET;
                    isCentroid = true;
                    break;
                }
            }
            if (isCentroid) continue;

            for (const auto& p : points) {
                if (p.x == x && p.y == y) {
                    cout << COLORS[p.cluster % 8] << 'o' << RESET;
                    goto nextChar;
                }
            }
            cout << ' ';
        nextChar:;
        }
        cout << '\n';
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    int K, numPoints, delay;
    cout << "Enter number of clusters (K): ";
    cin >> K;

    cout << "Enter number of data points: ";
    cin >> numPoints;

    cout << "Enter animation delay (ms): ";
    cin >> delay;

    vector<Point> points(numPoints);
    for (auto& p : points) {
        p.x = rand() % WIDTH;
        p.y = rand() % HEIGHT;
    }

    vector<Centroid> centroids(K);
    cout << "Enter " << K << " initial centroids (x y):\n";
    for (int i = 0; i < K; ++i) {
        cin >> centroids[i].x1 >> centroids[i].y1;
    }

    bool changed;
    do {
        assignClusters(points, centroids);
        clearScreen();
        draw(points, centroids);
        this_thread::sleep_for(chrono::milliseconds(delay));
        changed = updateCentroids(points, centroids);
    } while (changed);

    cout << "\nClustering complete. Press Enter to exit...";
    cin.ignore();
    cin.get();
    return 0;
}
