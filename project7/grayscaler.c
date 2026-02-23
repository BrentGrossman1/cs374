/*
Brent Grossman
02/22/2026
cs374
Project 7 - Multithreading
*/

#include "ppm.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct thread_data {
    struct ppm *image;
    int start_y;
    int num_rows;
    long long sum_r;
    long long sum_g;
    long long sum_b;
};

void *process_slice(void *args)
{
    struct thread_data *data = args;

    // initialize the sums to zero
    data->sum_r = 0;
    data->sum_g = 0;
    data->sum_b = 0;

    // loop through this thread's rows
    for (int y = data->start_y; y < data->start_y + data->num_rows; y++) {
	// loop through all columns (width of image)
	for (int x = 0; x < data->image->width; x++) {
	    // get the pixel at this x,y coordinate
	    int pixel = ppm_get_pixel(data->image, x, y);

	    // get the red, green, and blue values from the pixel
	    int r = PPM_PIXEL_R(pixel);
	    int g = PPM_PIXEL_G(pixel);
	    int b = PPM_PIXEL_B(pixel);

	    // add these values to our running sums (for calculating average later)
	    data->sum_r += r;
	    data->sum_g += g;
	    data->sum_b += b;

	    // convert to grayscale using the formula from instructions
	    int gray = (299 * r + 587 * g + 114 * b) / 1000;

	    // create a gray pixel (all three RGB values are the same for gray)
	    int gray_pixel = PPM_PIXEL(gray, gray, gray);

	    // set the pixel back into the image (modifying it to grayscale)
	    ppm_set_pixel(data->image, x, y, gray_pixel);
	}
    }
    return NULL;
}

void validate_input(int argc, char *prog_name)
{
    if (argc != 4) {
	fprintf(stderr, "Usage: %s <threads> <input.ppm> <output.ppm>\n",
		prog_name);
	exit(1);
    }
}

void compute_and_print_averages(struct thread_data *thread_data_array,
				int num_threads, struct ppm *image)
{
    // add up all the RGB sums from all the threads
    long long total_r = 0, total_g = 0, total_b = 0;

    for (int i = 0; i < num_threads; i++) {
	total_r += thread_data_array[i].sum_r;
	total_g += thread_data_array[i].sum_g;
	total_b += thread_data_array[i].sum_b;
    }

    // calculate total number of pixels in the image
    long long total_pixels = (long long) image->width * image->height;

    // compute the average by dividing total sum by number of pixels
    int avg_r = total_r / total_pixels;
    int avg_g = total_g / total_pixels;
    int avg_b = total_b / total_pixels;

    // print the results
    printf("Average R: %d\n", avg_r);
    printf("Average G: %d\n", avg_g);
    printf("Average B: %d\n", avg_b);
}

int main(int argc, char *argv[])
{
    // check that we got the right number of command line arguments
    validate_input(argc, argv[0]);

    // get the arguments from the command line
    int num_threads = atoi(argv[1]);	// convert string to int
    char *input_file = argv[2];
    char *output_file = argv[3];

    // read the input image file
    struct ppm *image = ppm_read(input_file);
    if (image == NULL) {
	fprintf(stderr, "Error reading image\n");
	return 1;
    }
    // figure out how many rows each thread should process
    int rows_per_thread = image->height / num_threads;
    int remainder = image->height % num_threads;	// leftover rows for last thread

    // create arrays to hold all our threads and their data
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    struct thread_data *thread_data_array =
	malloc(num_threads * sizeof(struct thread_data));

    // keep track of which row we're starting at
    int current_y = 0;

    // create and launch all the threads
    for (int i = 0; i < num_threads; i++) {
	// set up the data for this thread
	thread_data_array[i].image = image;
	thread_data_array[i].start_y = current_y;

	// last thread gets any remainder rows
	if (i == num_threads - 1) {
	    thread_data_array[i].num_rows = rows_per_thread + remainder;
	} else {
	    thread_data_array[i].num_rows = rows_per_thread;
	}

	// initialize sums to zero
	thread_data_array[i].sum_r = 0;
	thread_data_array[i].sum_g = 0;
	thread_data_array[i].sum_b = 0;

	// print info about this thread's slice
	printf("Thread %d: %d %d\n", i, thread_data_array[i].start_y,
	       thread_data_array[i].num_rows);

	// actually create and start the thread
	pthread_create(&threads[i], NULL, process_slice,
		       &thread_data_array[i]);

	// move to the next slice's starting row
	current_y += thread_data_array[i].num_rows;
    }

    // wait for all threads to finish their work
    for (int i = 0; i < num_threads; i++) {
	pthread_join(threads[i], NULL);
    }

    // now that all threads are done, compute and print the averages
    compute_and_print_averages(thread_data_array, num_threads, image);

    // free the memory we allocated for threads and data
    free(threads);
    free(thread_data_array);

    // write the now-grayscale image to the output file
    ppm_write(image, output_file);

    // free the image memory
    ppm_free(image);

    return 0;
}

