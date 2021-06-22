#if defined(__linux__)
// https://linux.die.net/man/3/malloc_usable_size
#include <malloc.h>
size_t portable_ish_malloced_size(const void *p) {
    return malloc_usable_size((void*)p);
}
#elif defined(__APPLE__)
// https://www.unix.com/man-page/osx/3/malloc_size/
#include <malloc/malloc.h>
size_t portable_ish_malloced_size(const void *p) {
    return malloc_size(p);
}
#elif defined(_WIN32)
// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/msize
#include <malloc.h>
size_t portable_ish_malloced_size(const void *p) {
    return _msize((void *)p);
}
#else
#error "oops, I don't know this system"
#endif

#include "CLI11.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <cmath>
#include <unistd.h>
#include <charconv>
#include <string_view>

void run_allocator(uint32_t, uint32_t);
void main_loop(uint32_t, uint32_t, int);

// int work(uint32_t baseline, uint32_t magnitude, int p) {
// 	uint64_t allocated_memory;
// 	const int tick_interval = 500;
// 	char * buffer = NULL;

// 	int32_t  current_tick = 0;

// 	do {
// 		std::cout << "Tick: " << current_tick << std::endl;
// 		std::cout.flush();
// 		//calculate new value

// 		current_tick = ++current_tick % p;
// 	} while (true);
// }

// void child(uint32_t a, uint32_t s) {
// 	// start timing
// 	const std::chrono::time_point<std::chrono::steady_clock> start =
// 	std::chrono::steady_clock::now();

// 	// malloc memory
// 	buffer = (char*) calloc (a, sizeof(char));
// 	if (buffer==NULL) exit (1);
// 	size_t true_length = portable_ish_malloced_size(buffer);
// 	std::cout << "Allocated " << true_length << " bytes." << std::endl;
// 	std::cout.flush();		
// 	// end timing
// 	const auto end = std::chrono::steady_clock::now();

// 	// sleep remainder of tick_interval
// 	const auto runtime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
// 	std::this_thread::sleep_for(std::chrono::milliseconds(tick_interval)-runtime);

// 	// free allocated memory
// 	if (buffer != NULL) {
// 		std::cout << "Freeing memory." << std::endl;
// 		std::cout.flush();
// 		free (buffer);
// 	}
//     exit(EXIT_SUCCESS);
// }

int main(int argc, char **argv) {
	CLI::App app{"App description"};
	auto allocate = app.add_subcommand("allocate", "This is a subcommand");

	// Define options
	uint32_t b = 10 * 1 << 20;
	uint32_t m = 5 * 1 << 20;
	int p = 20;
	uint32_t s = 0;
	uint32_t a = 0;

	app.add_option("-b", b, "Baseline with units")->transform(CLI::AsSizeValue(false));
	app.add_option("-m", m, "Magnitude with units")->transform(CLI::AsSizeValue(false));
	app.add_option("-p", p, "Period in ticks")->check(CLI::PositiveNumber);

	allocate->add_option("-s", s, "sleep time")->required(true)->check(CLI::PositiveNumber);
	allocate->add_option("-a", a, "allocation in bytes")->required(true)->transform(CLI::AsSizeValue(false));

    CLI11_PARSE(app, argc, argv);
	
	if (app.got_subcommand(allocate)) {
		run_allocator(a, s);
	} else {
		main_loop(b, m, p);
	}

	return 0;

	// if (s>0 && a>0) {
	// 	child(a,s);
	// 	return 0;
	// }
	// work(b,m,p);
	// return 0;
}

pid_t spawnChild(const char* program, char * const *arg_list)
{
    pid_t ch_pid = fork();
    if (ch_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (ch_pid > 0) {
        std::cout << "spawn child with pid - " << ch_pid << std::endl;
        return ch_pid;
    } else {
        execve(program, arg_list, nullptr);
        perror("execve");
        exit(EXIT_FAILURE);
    }
}

void getStringView(std::string_view strView){}


void run_allocator(uint32_t alloc_size, uint32_t sleep_time) {

    std::string_view program_name{"main"};

	const char *temp[] = {
		program_name.data(),
		"allocate",
		"-a", std::to_string(alloc_size),
		"-s", std::to_string(sleep_time),
		NULL
	};

	std::cout << "run_allocator:"
			  << " alloc_size: " << alloc_size 
	     	  << " sleep_time: " << sleep_time
	          << std::endl;
	std::cout.flush();

    pid_t child_pid = spawnChild(program_name.data(), arg_list);

	while ((child_pid = wait(nullptr)) > 0) {
    	std::cout << "child " << child_pid << " terminated" << std::endl;
	}
}

void main_loop(uint32_t baseline_bytes, uint32_t magnitude_bytes, int period) {

	std::cout << "main_loop:"
			  << " baseline_bytes: " << baseline_bytes 
	     	  << " magnitude_bytes: " << magnitude_bytes
	 		  << " period: " << period
	          << std::endl;
	std::cout.flush();

}