project(PRL-3-SyncOps)

add_executable(${PROJECT_NAME} ./3-SyncOps.cpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${ROOT_SOURCE_DIR}/common/pch.hpp)
