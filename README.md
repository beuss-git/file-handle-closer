# File Handle Closer

File Handle Finder is a command-line tool to search and close file handles for a specified file or pattern on Windows.

## Features

- Search for file handles by file name or pattern using `*` and `?` wildcards
    - `*` matches any sequence of characters
    - `?` matches any single character
- Close file handles (with a confirmation prompt or automatic confirmation option)
- Supports enabling debug privilege for the process

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

## Usage

### Basic usage

Search for file handles by file name or pattern:

```bash
file-handle-closer.exe "file.txt"
```

### Closing file handles

To close the found file handles, use the `-c` or `--close` option:

```bash
file-handle-closer.exe "file.txt" -c
```

### Automatic confirmation

To close the found file handles without a confirmation prompt, use the `-y` or `yes` option:

```bash
file-handle-closer.exe "file.txt" -c -y
```

## Disclaimer

This tool is provided as-is without any warranty. Use at your own risk.
Close file handles with caution this could lead to data loss or other unwanted side effects.

# License

MIT License. See [LICENSE](LICENSE) for more information.