cmake_minimum_required(VERSION 3.0.2)
project(bottle_proj)


find_package( OpenCV REQUIRED )

add_compile_options(-std=c++14)

add_executable(${PROJECT_NAME}_program 
  src/main.cpp  
)

target_link_libraries(${PROJECT_NAME}_program 
 ${OpenCV_LIBS}
)
