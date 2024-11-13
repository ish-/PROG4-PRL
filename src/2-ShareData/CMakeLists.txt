project(PRL-2-ShareData)

add_executable(${PROJECT_NAME} ./2-ShareData.cpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${ROOT_SOURCE_DIR}/common/pch.hpp)
