#ifndef LIGHTS_H
#define LIGHTS_H

#include <cmath>
#include <string>

#include "json.hpp"

/**
 * @struct Light
 * @brief Represents a smart light with its properties and state
 *
 * This struct encapsulates all the information about a light device,
 * including its identification, location, and current operational state.
 * It also provides conversion methods between JSON and Light objects.
 */
struct Light {
  /** @brief Unique identifier for the light */
  std::string id;

  /** @brief Display name of the light */
  std::string name;

  /** @brief Room location where the light is installed */
  std::string room;

  /** @brief Current power state of the light (true = on, false = off) */
  bool on;

  /** @brief Current brightness level (0-100%) */
  int8_t brightness;

  /**
   * @brief Constructor for creating a light with all properties
   * @param id Unique identifier for the light
   * @param name Display name of the light
   * @param room Room location where the light is installed
   * @param on Power state of the light (true = on, false = off)
   * @param brightness Brightness level (0-100%)
   */
  Light(std::string id, std::string name, std::string room, bool on,
        int brightness)
      : id(id), name(name), room(room), on(on), brightness(brightness) {}

  /**
   * @brief Converts the light object to a JSON representation
   * @return JSON object containing all light properties
   */
  nlohmann::json ToJsonFull() const {
    nlohmann::json j;
    j["id"] = id;
    j["name"] = name;
    j["room"] = room;
    j["on"] = on;
    j["brightness"] = brightness;
    return j;
  }

  /**
   * @brief Creates a Light object from full API JSON format
   * @param j JSON object containing all light properties
   * @return A new Light object initialized with the provided properties
   *
   * Creates a Light object with all properties from the API's full JSON format.
   * Converts the API brightness scale (0-255) to percentage (0-100).
   */
  static Light FromJsonApi(const nlohmann::json &j) {
    std::string id = j.value("id", "");
    std::string name = j.value("name", "");
    std::string room = j.value("room", "");
    bool on = j.value("on", false);
    // api brightness / 255 * 100
    int api_brightness = j.value("brightness", 0);
    int brightness =
        static_cast<int>(std::round((api_brightness / 255.0) * 100));
    return Light(id, name, room, on, brightness);
  }
};

#endif // LIGHTS_h
