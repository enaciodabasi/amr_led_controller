cmake_minimum_required(VERSION 3.8)
project(amr_led_controller)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CXX_STANDARD 23)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(rclcpp REQUIRED)
find_package(ipc_handlers REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/MotorDriverInfo.msg"
  "msg/HardwareInfoArray.msg"
 )

rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")


add_executable(led_controller src/amr_led_controller.cpp)
# Link the libraries
ament_target_dependencies(led_controller
rclcpp
)
target_compile_features(
  led_controller
  PUBLIC cxx_std_23
)
target_link_libraries(led_controller
  ${ipc_handlers_LIBRARIES}
  ${cpp_typesupport_target}
)
target_include_directories(led_controller PUBLIC
  include/
  ${ipc_handlers_INCLUDE_DIRS}
)
 
 
 # Install the target
install(TARGETS led_controller
   DESTINATION DESTINATION lib/${PROJECT_NAME}/)

ament_package()
