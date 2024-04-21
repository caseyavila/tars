struct pixel {
    int x;
    int y;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

std::vector<pixel> parse_frame(std::string filename);

std::vector<std::vector<pixel>> parse_animation(std::string directory);
