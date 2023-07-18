cmake_minimum_required(VERSION 3.22.1)
project(clock)
add_executable(${PROJECT_NAME}
	../src/main.cpp
)
target_include_directories(${PROJECT_NAME}
	PRIVATE
	${CMAkE_CURRENT_SOURCE_DIR}/../inc
)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
