# Project 2: File Scanner

## Description
This program recursively scans a directory hierarchy and calculates the total size of all regular files within it. 
The program traverses through all subdirectories, ignoring symbolic links, and prints the total size (in bytes) for each directory in post-order (subdirectories before their parents).

## Compilation
To build the program, run:
```bash
make
```

## Usage
```bash
./filescanner [directory]
```

- If no directory is specified, the current directory (`.`) is scanned
- Example: `./filescanner /home/user/documents`

## Function Call Diagram
```
main()
  |
  ├── check_args()
  |
  └── scan_dir()
        |
        └── scan_dir() (recursive call for subdirectories)
```

## Implementation Details
- Uses `lstat()` to detect and skip symbolic links
- Uses `readdir()` to iterate through directory entries
- Skips special directory entries (`.` and `..`) to prevent infinite recursion
- Implements post-order printing (subdirectories print before parent directories)
- Recursively calculates sizes by having each `scan_dir()` call return its total

## Author
Brent Grossman - CS 374 Operating Systems
