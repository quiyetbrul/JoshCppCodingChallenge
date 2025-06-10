#include "light/light_monitor.h"

#include <iostream>

#include "cmdparser.hpp"

/**
 * @brief Configures the command line argument parser
 * @param parser Reference to the parser object to be configured
 *
 * Sets up the accepted command line arguments including their
 * default values, short names, and help descriptions.
 */
void configure_parser(cli::Parser &parser);

int main(int argc, char **argv) {
  cli::Parser parser(argc, argv);
  configure_parser(parser);
  parser.run_and_exit_if_error();

  std::string host = parser.get<std::string>("host");
  int port = parser.get<int>("port");

  LightMonitor light_monitor(host, port);

  // Start polling for changes
  std::cout << "Polling " << host << ":" << port << std::endl;
  while (true) {
    light_monitor.Poll();
    // Poll every 2 seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  return 0;
}

void configure_parser(cli::Parser &parser) {
  parser.set_optional<std::string>("host", "h", "localhost",
                                   "Specify host address");
  parser.set_optional<int>("port", "p", 8080, "Specify port number");
}
