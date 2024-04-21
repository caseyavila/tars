#include "led-matrix.h"
#include "graphics.h"

#include "media.h"

#include <unistd.h>
#include <csignal>

#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>

#define SIZE 4
#define NAME "animation-memory"

using namespace rgb_matrix;

volatile bool interrupt_received = false;

static void InterruptHandler(int signo) {
    interrupt_received = true;
}

static int usage(const char *progname) {
    fprintf(stderr, "usage: %s\n <directory path> <delay in microseconds>", progname);
    fprintf(stderr, "Display an animation of frames.\n");
    rgb_matrix::PrintMatrixFlags(stderr);
    return 1;
}

int main(int argc, char *argv[]) {
    int shm_fd;
    int *animation_select;
    int curr_animation = 0;
    int fps;
    float frame_interval;

    // initialize shared mem
    shm_fd = shm_open(NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
     	perror("Error creating shared memory");
        return EXIT_FAILURE;
    }
    ftruncate(shm_fd, SIZE);
    animation_select = (int*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (animation_select == MAP_FAILED) {
        perror("Error mapping shared memory");
        return EXIT_FAILURE;
    }
    // default to 0
    *animation_select = curr_animation;

    // set up matrix
    RGBMatrix::Options matrix_options;
    rgb_matrix::RuntimeOptions runtime_opt;

    if (!rgb_matrix::ParseOptionsFromFlags(&argc, &argv, &matrix_options, &runtime_opt)) {
        return usage(argv[0]);
    }

    RGBMatrix *matrix = RGBMatrix::CreateFromOptions(matrix_options, runtime_opt);

    if (matrix == NULL) {
        return usage(argv[0]);
    }

    rgb_matrix::FrameCanvas *canvas = matrix->CreateFrameCanvas();
    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);

    fps = std::stoi(argv[argc - 1]);
    frame_interval = 1.0f / (float)fps;

    bool running = true;
    bool update = true;

    std::vector<std::vector<pixel>> idle = parse_animation("../frames/tars/idle/out/");
    std::vector<std::vector<pixel>> shake_head = parse_animation("../frames/tars/shake-head/out/");
    std::vector<std::vector<pixel>> blink = parse_animation("../frames/tars/blink/out/");
    std::vector<std::vector<pixel>> listen = parse_animation("../frames/tars/listen/out/");
    std::vector<std::vector<pixel>> question = parse_animation("../frames/tars/question/out/");
    std::vector<std::vector<pixel>> sad = parse_animation("../frames/tars/sad/out/");
    std::vector<std::vector<pixel>> happy = parse_animation("../frames/tars/happy/out/");
    std::vector<std::vector<pixel>> smiley = parse_animation("../frames/tars/smiley/out/");

    std::vector<std::vector<pixel>> animations[8] = {
	idle,
	shake_head,
	blink,
	listen,
	question,
	sad,
	happy,
	smiley
    };

    unsigned int frame_index = 0;

    bool blinking = false;

    while (!interrupt_received && running) {
        if (update) {
	    // check for new animation
	    if (*animation_select != curr_animation) {
		frame_index = 0;
		curr_animation = *animation_select;
		blinking = false;
	    }
	    // blink
	    if (curr_animation == 0) {
		int random_int = rand() % 100;
		if (random_int < 2) {
		    blinking = true;
		    curr_animation = 2;
		    *animation_select = 2;
		}
	    }
	    // update canvas
            canvas->Clear();
            for (pixel px : animations[curr_animation][frame_index]) {
                canvas->SetPixel(px.x, px.y, px.red, px.green, px.blue);
            }
            frame_index = (frame_index + 1) % animations[curr_animation].size();
            canvas = matrix->SwapOnVSync(canvas);
            usleep(frame_interval * 1000000);

	    if (blinking) {
	        // return to idle animation
	        curr_animation = 0;
		*animation_select = 0;
		blinking = false;
	    }
        }
    }

    // Finished. Shut down the RGB matrix.
    delete matrix;

    // close shared mem
    shm_unlink(NAME);
    munmap(animation_select, SIZE);
    close(shm_fd);

    printf("\n");
    return 0;
}
