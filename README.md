# File Handle Closer

File Handle Finder is a command-line tool to search and close file handles for a specified file or pattern on Windows.

## Features

- Search for file handles using a pattern search or a regex search
- Close file handles

## Usage

```bash
file-handle-closer.exe <query> [-c/--close] [-y/--yes] [-r/--regex]
```

## Examples

### Pattern search (default)

Search for file handles with names that contain "file.txt".

```bash
file-handle-closer.exe "file.txt"
```

Search for Nvidia logs files

```bash
file-handle-closer.exe "NVIDIA*.log"
```

### Regex search

Search for file handles with names that match the regex pattern "file\.[a-zA-Z]{3}$". (e.g. file.txt, file.doc,
file.pdf, etc.)

```bash
file-handle-closer.exe "file\.[a-zA-Z]{3}$" -r
```

### Close file handles

Close file handles with names that end with "file.txt".

```bash
file-handle-closer.exe "file.txt" -c
```

### Kill processes

Kill file handle owner processes with names that end with "file.txt".

```bash
file-handle-closer.exe "file.txt" -k
```

### Automatically answer yes to prompts

Close file handles with names that end with "file.txt" and automatically answer yes to prompts.

```bash
file-handle-closer.exe "file.txt" -c -y
```

## Requirements

- Windows OS
- Visual Studio or a compatible C++ compiler

## Building

1. Clone the repository:

```bash
git clone https://github.com/beuss-git/file-handle-closer.git
```

2. Create a build directory and navigate to it:

```bash
mkdir file-handle-closer/build
cd file-handle-closer/build
```

3. Generate the build files:

```bash
cmake ..
```

4. Build the project:

```bash
cmake --build .
```

Alternatively, you can open the generated solution in Visual Studio or use your preferred C++ compiler.

## Disclaimer

This tool is provided as-is without any warranty. Use at your own risk.
Close file handles with caution this could lead to data loss or other unwanted side effects.

# License

MIT License. See [LICENSE](LICENSE) for more information.