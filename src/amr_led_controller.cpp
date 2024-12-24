#include "amr_led_controller/amr_led_controller.hpp"

template <typename E>
constexpr auto to_integral(E e) -> typename std::underlying_type<E>::type {
  return static_cast<typename std::underlying_type<E>::type>(e);
}

enum class LedMode : uint16_t {
  OFF = 0x0, // 000
  RED = 0x1, // 100
  GREEN = 0x2 , // 010
  YELLOW = 0x4, // 001
  BLUE = 0x3, // 110
  WHITE = 0x5, // 101
  WHITE_WITH_RED_ENDS = 0x6, // 011
  BOUNCING_BLUE = 0x7 // 111
};

enum class HardwareState : uint16_t {
  INITIAL_STATE = 0x2,
  OPERATION_ENABLED = 0x3,
  ERROR = 0x1,
  WARNING = 0x1
};

int main(int argc, char** argv)
{
  using DriverInfo = amr_led_controller::msg::MotorDriverInfo;
  using HardwareInfo = amr_led_controller::msg::HardwareInfoArray;

  rclcpp::init(argc, argv);
  
  std::shared_ptr<rclcpp::Node> ledControllerNode = std::make_shared<rclcpp::Node>("led_controller");
  HardwareState generalState = HardwareState::INITIAL_STATE;
  HardwareState previousState = generalState;
  std::shared_ptr<rclcpp::Subscription<HardwareInfo>> hardwareInfoSub = ledControllerNode->create_subscription<HardwareInfo>(
    "/hardware_info", 10, [&generalState](const HardwareInfo::SharedPtr msg) {
      for (const DriverInfo& driverInfo : msg->motor_driver_info) {
        HardwareState newState = HardwareState::INITIAL_STATE;
        if (driverInfo.status == DriverInfo::OPERATION_ENABLED)
        {
          newState = HardwareState::OPERATION_ENABLED;
        }
        else if(DriverInfo::FAULT || DriverInfo::FAULT_REACTION_ACTIVE)
        {
          newState = HardwareState::ERROR;
        }
        if(newState != generalState)
        {
          generalState = newState;
        }
      }
    }
  );
  std::cout << "aaaaaa\n";
  std::expected<tcp_client::TcpClient, tcp_client::ErrorCode> tcpClientExp = tcp_client::TcpClient::create("127.0.0.1", "3255");

  if(!tcpClientExp.has_value())
  {
    std::cout << tcp_client::ErrorCodeDescriptions.at(tcpClientExp.error()) << std::endl;
    return 2;
  }
  std::cout << "aaaaaa\n";
  auto tcpClient = std::move(tcpClientExp.value());
  std::cout << "aaaaaa\n";
  uint16_t ledMode = to_integral(generalState);
  IoRequest ledModeReq;
  ledModeReq.requests.resize(6);
  
  
  rclcpp::Rate sleepRate(10);
  while(rclcpp::ok())
  {
    rclcpp::spin_some(ledControllerNode);
    
    if(generalState != previousState)
    {
      ledMode = to_integral(generalState);

      previousState = generalState;
    }
    std::cout << "bbbb\n";
    ledModeReq.timestamp = std::chrono::system_clock::now(); 
    ledModeReq.requests[0] = std::make_tuple(0, IoRequest::RequestType::WRITE, (ledMode & 1));
    ledModeReq.requests[1] = std::make_tuple(1, IoRequest::RequestType::WRITE, ((ledMode & 2) >> 1));
    ledModeReq.requests[2] = std::make_tuple(2, IoRequest::RequestType::WRITE, (ledMode & 4) >> 2);
    
    ledModeReq.requests[3] = std::make_tuple(3, IoRequest::RequestType::WRITE, (ledMode & 1));
    ledModeReq.requests[4] = std::make_tuple(4, IoRequest::RequestType::WRITE, ((ledMode & 2) >> 1));
    ledModeReq.requests[5] = std::make_tuple(5, IoRequest::RequestType::WRITE, (ledMode & 4) >> 2);
    
    
  
    const std::string ledModeJsonReq = IoRequest::toJsonStr(ledModeReq);
    std::cout << ledModeJsonReq << std::endl;
    tcp_client::ErrorCode tcpSendRes = tcpClient.sendData(ledModeJsonReq.c_str(), ledModeJsonReq.length());
    std::cout << tcp_client::ErrorCodeDescriptions.at(tcpSendRes) << std::endl;

    sleepRate.sleep();
  }

  
  rclcpp::shutdown();
  return 0;

}
