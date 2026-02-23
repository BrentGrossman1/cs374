# Grayscale Image Converter with Multithreading

## Building

Command line:
* `make` to build. An executable called `grayscaler` will be produced.
* `make clean` to remove the executable.

## Files

* `grayscaler.c`: Main program that handles threading and image processing
* `ppm.h`: Header file for PPM image library
* `ppm.c`: PPM image library implementation
* `Makefile`: Build configuration

## Data

The main data structures are:

* `struct ppm`: Represents a PPM image with width, height, and pixel data
* `struct thread_data`: Holds per-thread information including:
  * Image pointer (shared across all threads)
  * Starting row and number of rows for this thread's slice
  * RGB sum accumulators for computing averages

The image is divided into horizontal slices, with each thread processing its assigned rows.

## Functions

* `main()`
  * `validate_input()`: Validates command line arguments
  * `ppm_read()`: Reads input PPM image file
  * Thread setup and creation loop: Configures data for each thread and launches them
  * Thread join loop: Waits for all threads to complete
  * `compute_and_print_averages()`: Computes and displays average RGB values
  * `ppm_write()`: Writes grayscale output image
  * `ppm_free()`: Cleans up image memory
* `process_slice()`: Thread running function
  * Processes assigned image rows
  * Converts each pixel to grayscale using ITU-R recommendation BT.601 luma standard formula
  * Accumulates RGB sums for averaging
  * Uses `ppm_get_pixel()` and `ppm_set_pixel()` from PPM library
* `validate_input()`: Checks command line argument count
* `compute_and_print_averages()`: Aggregates thread results and calculates final averages

## Usage
```bash
./grayscaler <num_threads> <input.ppm> <output.ppm>
```

Where:
- `<num_threads>` is the number of worker threads to use
- `<input.ppm>` is the input color PPM image
- `<output.ppm>` is the output grayscale PPM image

Example with 3 threads:
```bash
./grayscaler 3 goat.ppm graygoat.ppm
```

Output format:
```
Thread 0: <start_row> <num_rows>
Thread 1: <start_row> <num_rows>
Thread 2: <start_row> <num_rows>
...
Average R: <value>
Average G: <value>
Average B: <value>
```

Example output (3 threads, 71-pixel tall image):
```
Thread 0: 0 23
Thread 1: 23 23
Thread 2: 46 25
Average R: 119
Average G: 108
Average B: 94
```

## Notes

* The grayscale conversion uses the formula: `gray = (299*r + 587*g + 114*b) / 1000`
* The last thread handles any remainder rows from the division
* PPM files can be converted to PNG using ImageMagick: `convert output.ppm output.png`
* All threads share the same image data structure; each modifies its own slice