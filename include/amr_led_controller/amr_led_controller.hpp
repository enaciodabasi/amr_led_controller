/**
 * @file amr_led_controller.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef AMR_LED_CONTROLLER_HPP
#define AMR_LED_CONTROLLER_HPP

#include <queue>
#include <string_view>
#include <string>
#include <expected>
#include <chrono>

#include <rclcpp/rclcpp.hpp>

#include <nlohmann/json.hpp>

#include "amr_led_controller/msg/hardware_info_array.hpp"
#include "ipc_handlers/tcp_client_template.hpp"

struct IoRequest
{
  public:

  enum class RequestType {
    READ,
    WRITE
  };

  using Request = std::tuple<int, RequestType, std::optional<int>>;

  std::chrono::time_point<std::chrono::system_clock> timestamp;
  std::vector<Request> requests; 

  static std::string toJsonStr(const IoRequest& data) {
    nlohmann::json j;
    j["timestamp"] = std::chrono::system_clock::to_time_t(data.timestamp);
    for (const auto& [key, type, value] : data.requests) {
      nlohmann::json req;
      req["key"] = key;
      req["type"] = type == RequestType::READ ? "READ" : "WRITE";
      if (value) {
        req["value"] = *value;
      }
      j["requests"].push_back(req);
    }
    return j.dump();
  }

  static std::optional<IoRequest> fromStr(const std::string& str) {
    nlohmann::json j = nlohmann::json::parse(str);
    if(j.empty())
    {
      return std::nullopt;
    }

    IoRequest data;

    data.timestamp = std::chrono::system_clock::from_time_t(j["timestamp"].get<std::time_t>());
    for (const auto& req : j["requests"]) {
      RequestType type = req["type"] == "READ" ? RequestType::READ : RequestType::WRITE;
      std::optional<int> value;
      if (req.find("value") != req.end()) {
        value = req["value"].get<int>();
      }
      data.requests.push_back({req["key"].get<int>(), type, value});
    }
    return data;
  }

};

#endif // AMR_LED_CONTROLLER_HPP
