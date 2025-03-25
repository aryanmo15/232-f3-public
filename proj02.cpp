#include "ppmio.h"
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <cmath>

// Ensure pixel values remain in range 0-255
inline unsigned char Clipvalue(int val) {
    return static_cast<unsigned char>(std::max(0, std::min(255, val)));
}

// Convert image to grayscale using an alternative formula
// AI used to generate idea for function (link in history.txt)
void Grayscale(std::vector<std::vector<RGB>> &img) {
    for (auto &line : img) {
        for (auto &pix : line) {
            int gray = (pix.r * 0.299 + pix.g * 0.587 + pix.b * 0.114);
            pix.r = pix.g = pix.b = gray;
        }
    }
}

// Apply color inversion using bitwise operations
// AI used to generate idea for function (link in history.txt)
void Inversion(std::vector<std::vector<RGB>> &img) {
    for (auto &line : img) {
        for (auto &pix : line) {
            pix.r = ~pix.r;
            pix.g = ~pix.g;
            pix.b = ~pix.b;
        }
    }
}

// Enhance contrast using a more aggressive transformation
// AI used to generate idea for function (link in history.txt)
void Contrast(std::vector<std::vector<RGB>> &img, double factor = 1.6) {
    for (auto &line : img) {
        for (auto &pix : line) {
            pix.r = Clipvalue((pix.r - 128) * factor + 128);
            pix.g = Clipvalue((pix.g - 128) * factor + 128);
            pix.b = Clipvalue((pix.b - 128) * factor + 128);
        }
    }
}

// Blur image using a weighted kernel instead of averaging
// AI used to generate idea for function (link in history.txt)
void Blur(std::vector<std::vector<RGB>> &img) {
    std::vector<std::vector<RGB>> blurred(img);
    int h = img.size(), w = (h > 0) ? img[0].size() : 0;
    int kernel[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
    int kernelSum = 16;

    for (int i = 1; i < h - 1; ++i) {
        for (int j = 1; j < w - 1; ++j) {
            int rSum = 0, gSum = 0, bSum = 0;
            for (int di = -1; di <= 1; ++di) {
                for (int dj = -1; dj <= 1; ++dj) {
                    rSum += img[i + di][j + dj].r * kernel[di + 1][dj + 1];
                    gSum += img[i + di][j + dj].g * kernel[di + 1][dj + 1];
                    bSum += img[i + di][j + dj].b * kernel[di + 1][dj + 1];
                }
            }
            blurred[i][j].r = rSum / kernelSum;
            blurred[i][j].g = gSum / kernelSum;
            blurred[i][j].b = bSum / kernelSum;
        }
    }
    img.swap(blurred);
}

// Flip image by swapping elements instead of reversing
// AI used to generate idea for function (link in history.txt)
void Mirror(std::vector<std::vector<RGB>> &img) {
    for (auto &line : img) {
        for (size_t i = 0, j = line.size() - 1; i < j; ++i, --j) {
            std::swap(line[i], line[j]);
        }
    }
}

// Compress image by averaging every 2x2 block into one pixel
// AI used to generate idea for function (link in history.txt)
void Compress(std::vector<std::vector<RGB>> &img) {
    std::vector<std::vector<RGB>> reduced;
    for (size_t r = 0; r + 1 < img.size(); r += 2) {
        std::vector<RGB> newRow;
        for (size_t c = 0; c + 1 < img[r].size(); c += 2) {
            RGB avgPix;
            avgPix.r = (img[r][c].r + img[r][c + 1].r + img[r + 1][c].r + img[r + 1][c + 1].r) / 4;
            avgPix.g = (img[r][c].g + img[r][c + 1].g + img[r + 1][c].g + img[r + 1][c + 1].g) / 4;
            avgPix.b = (img[r][c].b + img[r][c + 1].b + img[r + 1][c].b + img[r + 1][c + 1].b) / 4;
            newRow.push_back(avgPix);
        }
        reduced.push_back(std::move(newRow));
    }
    img.swap(reduced);
}

// Main function to handle user commands and process images
// AI used to aid the stucture of main (link in history.txt)
int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input.ppm> <output.ppm> [-g] [-i] [-x] [-b] [-m] [-c]\n";
        return 1;
    }

    std::string inputFile = argv[1], outputFile = argv[2];
    std::vector<std::string> options(argv + 3, argv + argc);

    try {
        auto imgData = readPPM(inputFile);

        for (const auto &option : options) {
            if (option == "-g") Grayscale(imgData);
            else if (option == "-i") Inversion(imgData);
            else if (option == "-x") Compress(imgData);
            else if (option == "-b") Blur(imgData);
            else if (option == "-m") Mirror(imgData);
            else if (option == "-c") Compress(imgData);
            else {
                std::cerr << "Unknown option: " << option << "\n";
                return 1;
            }
        }
        
        writePPM(outputFile, imgData);
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
