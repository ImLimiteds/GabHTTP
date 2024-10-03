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

Here are two simple examples of how to use GabHTTP:

```cpp
#include "GabHttp.hpp"

int main() {
   GabHttp:Server Gab(8000);
   
   Gab.Route("GET", "/", [](GabHttp::Request req, GabHttp::Response res) {
      res.set_body("Hello, World!");
      res.set_header("Content-Type", "text/plain");
   });
   
   Gab.Start()
   return 0;
}
```

With Headers

```cpp
#include "GabHttp.hpp"

int main() {
   GabHttp::Server Gab(8000);
   
   Gab.Route("GET", "/", [](GabHttp::Request req, GabHttp::Response res) {
      res.set_body("Hello, World!");
      res.set_header("Content-Type", "text/plain");
   }, {{"Authorization", "1234"}});
   
   Gab.Start()
    return 0;
}
```

**GabHTTP is still in development and is not yet ready for production use. Please note that GabHTTP has only been tested
on Windows 11 23H2**

Version: **1.0.0**

![Linux](https://github.com/ImLimiteds/GabHTTP/actions/workflows/linux.yml/badge.svg)
![Windows](https://github.com/ImLimiteds/GabHTTP/actions/workflows/windows.yml/badge.svg)