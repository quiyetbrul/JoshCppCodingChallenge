#ifndef LIGHT_MONITOR_H
#define LIGHT_MONITOR_H

#include "light.h"

#include <string>
#include <unordered_map>

#include "httplib.h"

/**
 * @class LightMonitor
 * @brief Manages communication with a light simulator service
 *
 * This class handles fetching, monitoring, and reporting changes to lights
 * by communicating with a remote light simulator through HTTP requests.
 */
class LightMonitor {
private:
  /** @brief Maximum number of retry attempts for HTTP requests */
  static constexpr int MAX_ATTEMPTS = 3;

  /** @brief Hostname of the light simulator service */
  std::string simulator_host_;

  /** @brief Port number of the light simulator service */
  int simulator_port_;

  /** @brief Map of current lights indexed by their IDs */
  std::unordered_map<std::string, Light> current_lights_;

  /** @brief HTTP client for communicating with the simulator */
  httplib::Client http_client_;

public:
  /**
   * @brief Constructor for LightMonitor
   * @param host The hostname of the light simulator service
   * @param port The port number of the light simulator service
   */
  LightMonitor(const std::string &host, int port)
      : simulator_host_(host), simulator_port_(port), http_client_(host, port) {
    http_client_.set_connection_timeout(5); // seconds
    http_client_.set_read_timeout(5);       // seconds
    http_client_.set_keep_alive(true);
  }

  /**
   * @brief Polls for changes in the light configurations
   *
   * Periodically checks for updates to existing lights, additions of new
   * lights, and removals of existing lights from the simulator service.
   */
  void Poll();

private:
  /**
   * @brief Fetches detailed information for a specific light
   * @param id The unique identifier of the light
   * @param light Reference to the Light object to be populated with details
   * @param attempt Current retry attempt number (starts at 1)
   * @return true if successful, false otherwise
   */
  bool FetchSingleLightDetails(const std::string &id, Light &light,
                               int attempt = 1);

  /**
   * @brief Identifies lights that have been removed from the simulator
   * @param new_lights Map of lights currently available in the simulator
   *
   * Compares the newly fetched lights with the previously known lights
   * to detect any that have been removed.
   */
  void DetectRemovedLights(
      const std::unordered_map<std::string, Light> &new_lights) const;

  /**
   * @brief Handles HTTP response and implements retry logic
   * @param res The HTTP response from the simulator
   * @param endpoint The API endpoint that was requested
   * @param attempt Current retry attempt number
   * @return true if the response was successful, false otherwise
   */
  bool HandleHttpResponse(const httplib::Result &res,
                          const std::string &endpoint) const;

  /**
   * @brief Reports changes in a light's property
   * @tparam T The data type of the property value
   * @param id The unique identifier of the light
   * @param property_name The name of the property that changed
   * @param old_value The previous value of the property
   * @param new_value The new value of the property
   *
   * Outputs property changes as JSON to standard output when a value changes.
   */
  template <typename T>
  void ReportPropertyChange(const std::string &id,
                            const std::string &property_name,
                            const T &old_value, const T &new_value) {
    if (old_value != new_value) {
      nlohmann::json change;
      change["id"] = id;
      change[property_name] = new_value;
      std::cout << change.dump(4) << std::endl;
    }
  }
};

#endif // LIGHT_MONITOR_h
