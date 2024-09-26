# GabHTTP

GabHTTP is a simple HTTP server library written in C++.

## Dependencies

### Windows
- Windows SDK (includes `winsock2.h` and `ws2tcpip.h`)

### Unix-like systems (Linux, macOS)
- POSIX standard libraries (includes `unistd.h` and `arpa/inet.h`)

## Building the Project

### Prerequisites
- CMake 3.29 or higher
- A C++20 compatible compiler

### Steps

1. Clone the repository:
    ```sh
    git clone https://github.com/ImLimiteds/GabHTTP.git
    cd GabHTTP
    ```

2. Create a build directory and navigate into it:
    ```sh
    mkdir build
    cd build
    ```

3. Run CMake to configure the project:
    ```sh
    cmake ..
    ```

4. Build the project:
    ```sh
    cmake --build .
    ```

## Usage

Here is a simple example of how to use the GabHTTP library:

```cpp
#include "GabHttp.hpp"

int main() {
    GabHttp::Server server(8000);

    server.route("/", [](GabHttp::Request& req, GabHttp::Response& res) {
        res.set_body("Hello, world!");
    });

    server.start();
    return 0;
}
```

**GabHTTP is still in development and is not yet ready for production use. Please note that GabHTTP has only been tested
on Windows 11 23H2**

Version: **1.0.0**

![Linux](https://github.com/ImLimiteds/GabHTTP/actions/workflows/linux.yml/badge.svg)
![Windows](https://github.com/ImLimiteds/GabHTTP/actions/workflows/windows.yml/badge.svg)