#include "light_monitor.h"

void LightMonitor::Poll() {

  httplib::Result res = http_client_.Get("/lights");

  if (!HandleHttpResponse(res, "/lights")) {
    return;
  }

  try {
    nlohmann::json j_array = nlohmann::json::parse(res->body);
    std::unordered_map<std::string, Light> new_lights;

    // Populate new_lights map and detect new lights/changes
    for (const auto &j_light : j_array) {
      Light light = Light::FromJsonApiConcise(j_light);
      if (!FetchSingleLightDetails(light.id, light)) {
        continue;
      }
      new_lights.emplace(light.id, light);

      // Check if new_lights[light.id] exists in current_lights_.
      // Add if it is a new light; otherwise, look for any updates
      auto it = current_lights_.find(light.id);
      if (it == current_lights_.end()) {
        std::cout << light.ToJsonFull().dump(4) << std::endl;
      } else {
        const Light &oldLight = it->second;
        ReportPropertyChange(light.id, "name", oldLight.name, light.name);
        ReportPropertyChange(light.id, "room", oldLight.room, light.room);
        ReportPropertyChange(light.id, "on", oldLight.on, light.on);
        ReportPropertyChange(light.id, "brightness", oldLight.brightness,
                             light.brightness);
      }
    }

    DetectRemovedLights(new_lights);

    // Update currentLights map for the next poll
    current_lights_ = std::move(new_lights);

  } catch (const nlohmann::json::parse_error &e) {
    std::cerr << "JSON parse error during polling: " << e.what() << std::endl;
  }
}

bool LightMonitor::FetchSingleLightDetails(const std::string &id, Light &light,
                                           int attempt) {

  std::string endpoint = "/lights/" + id;

  if (attempt > MAX_ATTEMPTS) {
    std::cerr << "HTTP GET " << endpoint << " failed: max attempt reached"
              << std::endl;
    return false;
  }

  httplib::Result light_res = http_client_.Get(endpoint);
  if (!HandleHttpResponse(light_res, endpoint)) {
    // Exponential backoff
    std::this_thread::sleep_for(
        std::chrono::milliseconds(100 * (1 << attempt)));
    return FetchSingleLightDetails(id, light, attempt + 1);
  }

  try {
    nlohmann::json light_state = nlohmann::json::parse(light_res->body);
    light = Light::FromJsonApiFull(light_state);
    return true;
  } catch (const nlohmann::json::parse_error &e) {
    std::cerr << "JSON parse error for light " << id << ": " << e.what()
              << std::endl;
  }

  return false;
}

void LightMonitor::DetectRemovedLights(
    const std::unordered_map<std::string, Light> &new_lights) const {

  // Identify lights that were previously tracked but no longer exist
  for (const auto &pair : current_lights_) {
    const std::string &id = pair.first;
    const Light &oldLight = pair.second;

    if (new_lights.find(id) == new_lights.end()) {
      std::cout << oldLight.name << " (" << oldLight.id << ") has been removed"
                << std::endl;
    }
  }
}

bool LightMonitor::HandleHttpResponse(const httplib::Result &res,
                                      const std::string &endpoint) const{

  // Check HTTP response status and log errors
  if (!res || res->status != 200) {
    std::cerr << "HTTP GET " << endpoint << " failed: "
              << (res ? std::to_string(res->status) : "Connection Error")
              << std::endl;
    return false;
  }
  return true;
}
