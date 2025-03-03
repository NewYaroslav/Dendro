# Dendro

Dendro is a command-line tool for generating and visualizing project directory structures. It provides filtering options, customizable output formats, and clipboard support.

## Features

- Recursively generates a tree-like structure of directories and files.
- Supports including/excluding specific directories and files.
- Customizable output file location.
- Clipboard support for quick sharing.
- Configurable root directory and visibility.
- Supports filtering by file extensions.
- Fast and minimalistic implementation.

## Installation

### Clone the Repository

```sh
git clone https://github.com/NewYaroslav/Dendro.git
cd Dendro
```

### Build

**ПОКА НЕ РЕАЛИЗОВАНО**

Ensure you have a C++17-compatible compiler and [Consolix](https://github.com/NewYaroslav/Consolix) installed.

```sh
mkdir build && cd build
cmake ..
make
```

## Usage

Run `dendro` without arguments to generate `project_structure.txt` for the current directory:

```sh
./dendro
```

### Available Flags

| Flag               | Description                                          |
| ------------------ | ---------------------------------------------------  |
| `-i, --include`    | Specify directories to include in the structure      |
| `-e, --exclude`    | Exclude specific directories or files                |
| `-o, --output`     | Define output file location                          |
| `-c, --clipboard`  | Copy the result to the clipboard                     |
| `--allow-ext`      | Include only files with specified extensions         |
| `--exclude-ext`    | Exclude files with specified extensions              |
| `--root-path`      | Set a custom root directory for structure generation |
| `--show-root`      | Display the root directory in the output             |
| `-h, --help`       | Show available commands and options                  |

### Example Usage

Generate a structure including only `src` and `include` directories, excluding `build` and saving to `structure.txt`:

```sh
./dendro -i src include -e build -o structure.txt
```

Copy project structure to the clipboard:

```sh
./dendro -c
```

Generate a structure starting from `projects/myrepo` and display the root folder:

```sh
./dendro --root-path projects/myrepo --show-root
```

Filter files by extensions, allowing only `.cpp` and `.hpp` files:

```sh
./dendro --allow-ext cpp hpp
```

Exclude log and temporary files:

```sh
./dendro --exclude-ext log tmp
```

## Example Output

```
┌── docs/
│   ├── README.md
│   ├── INSTALL.md
│   ├── API_REFERENCE.md
│   └── assets/
│       ├── logo.png
│       └── banner.jpg
└── build/
    ├── CMakeLists.txt
    ├── Makefile
    └── bin/
        ├── project_executable
        ├── libcore.so
        └── libgraphics.so
```

## License

Dendro is licensed under the MIT License.

## Author

Developed by [NewYaroslav](https://github.com/NewYaroslav).

