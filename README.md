# Light Terminal Application

A terminal-based application that polls changes from a lights API and displays them in JSON format. The application detects and reports when lights are added, removed, or have property changes.

## Building the Application

### Pre-requisites

- C++14
- CMake 3.10 or higher

### Build Steps

1. Clone the repository
2. Create a build directory:

   ```bash
   mkdir build && cd build
   ```

3. Run CMake to configure the project:

   ```bash
   cmake ..
   ```

4. Build the project:

   ```bash
   make
   ```

5. Run the application:

   ```bash
   ./light_monitor

   # or with specific arguments
   ./light_monitor --host <host> --port <port_number>
   ```

Alternatively, you can run the application by using the script `makerun.sh` provided in the root directory. (This script was mainly for me so that I can quickly build and run the application without having to type the commands every time.)

Without specified arguments when running the application, it will use the default host `localhost` and port `8080`.

### Don't Want to Build it Yourself?

You can download the pre-built binary from the latest [releases](https://github.com/quiyetbrul/JoshCppCodingChallenge/releases) page, download the appropriate binary for your platform, and run it directly without building it yourself.

## Project Structure

- `libs/`: Contains header files.
  - `json.hpp`: Contains the nlohmann JSON library header files.
  - `cmdparser.hpp`: Contains the command line parser header files.
  - `httplib.h`: Contains the HTTP client library header files.
- `src/`: Contains the source code files.
  - `main.cpp`: The main entry point of the application.
  - `light/`: Contains the light-related classes and logic.
    - `light.h`: Header file for the Light struct data-bag.
    - `light_monitor.h`: Header file for the LightMonitor class.
    - `light_monitor.cpp`: Implementation of the LightMonitor class.

## Approach

- To get the default state of the lights:
  - The application requests `get lights/` from the API to retrieve concise information about the lights, which includes `id`, `name`, `room`.
  - The concise information is stored in a map<id, Light{}>. Since the API does not provide a way to get the full state of the lights, the application uses the `get light/<id>` endpoint to retrieve the full state of each light.
- Do the same steps for the current state of the lights.
- The application then compares the previous default state with the current state to detect changes:
  - If a light is present in the current state but not in the previous state, it is considered added.
  - If a light is present in the previous state but not in the current state, it is considered removed.
  - If a light is present in both states, it is checked for property changes.
- The current state map is then updated to the previous state map for the next iteration.
- The application prints the changes in JSON format, including added, removed, and changed lights.

## Error Handling

- The application requests `get /lights` until the user exits out of the program
- The application requests `get /lights/<id>` up to a maximum number of retries.
  - If the request fails after the maximum retries, it logs an error message and continues with the next request.
- If JSON parsing fails for an item, it skips to the next item and logs an error message.

## Other Notes

- I completely missed that `get lights/` returns a list of lights with only `id`, `name`, and `room` properties, so I had to make an additional request for each light to get the full state.
- I used ChatGPT to help with implementing GitHub Actions that creates and uploads a release artifact.
  - Based on a previous project I worked on, I used the `cmake` and `make` commands to build the project that was deployed on a Docker container.
  - I've also only run the binary specific to macOS arm64, so I hope it works on other platforms as well.
- `LightMonitor.Poll()` could potentially be optimized to reduce the number of requests made to the API, but I left it as is for simplicity.
  - Using promises or async/await could improve the performance, but I have not dabbed into that yet.
- The documentation/comments are Doxygen-style, so there's a possibility to generate documentation using Doxygen if needed.

## Total Hours Spent

- ~4 total hours
- ~3 hours, including reading the Simulator repo, implementing the application, and manually testing it.
  - This also includes figuring out why I was getting 0 for `brightness` and false for `on` :(
    - This took the time allotted for writing unit tests.
- ~0.25 hours reading library documentation; I have not used httplib and cmdparser before.
- ~0.75 hours writing the README.md file.
