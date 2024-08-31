# Silly

**Silly** is a simple C++ application that fetches and renders HTML from web pages using `curl`. It is designed to work primarily with older and simple websites. The project leverages the SDL2 and SDL2_ttf libraries for rendering the HTML content.

## Features

- Fetches HTML content from the web using `curl`.
- Renders the fetched HTML using SDL2.
- Supports basic text rendering with SDL2_ttf.
- Suitable for older, simpler websites.

## Requirements

- CMake 3.10 or higher
- C++17 compiler
- libcurl
- SDL2
- SDL2_ttf

## Building the Project

1. **Clone the repository:**

   ```bash
   git clone https://github.com/vigneshk5/Silly.git
   cd Silly
   ```

2. **Create a build directory and navigate into it:**

   ```bash
   mkdir build && cd build
   ```

3. **Configure the project using CMake:**

   ```bash
   cmake ..
   ```

4. **Build the project:**

   ```bash
   make
   ```

## Running Silly

After building the project, you can run **Silly** by executing the following command from the `build` directory:

```bash
./Silly
```

You can provide a URL and render its HTML:

```bash
./Silly
```

## Project Structure

- **src/**: Contains the source files for the project.
  - `main.cpp`: Entry point of the application.
  - `HTMLRenderer.cpp`: Handles rendering of HTML content.
- **include/**: Header files for the project.
- **CMakeLists.txt**: CMake configuration file.

---
# silly
