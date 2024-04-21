#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

#include "media.h"

std::vector<pixel> parse_frame(std::string filename) {
    std::vector<pixel> pixels;

    std::ifstream frame_stream(filename);
    std::string line;

    while (std::getline(frame_stream, line)) {
        std::istringstream iline(line);
        std::string item;
        struct pixel px;
        for (int i = 0; std::getline(iline, item, ' '); i++) {
            switch (i) {
                case 0:
                    px.x = stoi(item);
                    break;
                case 1:
                    px.y = stoi(item);
                    break;
                case 2:
                    px.red = stoi(item);
                    break;
                case 3:
                    px.green = stoi(item);
                    break;
                case 4:
                    px.blue = stoi(item);
                    break;
                default:
                    break;
            }
        }
        pixels.push_back(px);
    }

    return pixels;
}

std::vector<std::vector<pixel>> parse_animation(std::string directory) {
    std::vector<std::vector<pixel>> animation;

    for (int i = 0; std::filesystem::exists(directory + "frame" + std::to_string(i)); i++) {
        animation.push_back(parse_frame(directory + "frame" + std::to_string(i)));
    }

    return animation;
}
