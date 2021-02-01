# CMakeList.txt : CMake project for cookieVS, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.11)

# Add source to this project's executable.
set(CMAKE_TOOLCHAIN_FILE "D:\\vcpkg\\scripts\\buildsystemsvcpkg.cmake")
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(cryptopp CONFIG REQUIRED)

add_executable (testexe "base64.h" "Dumper.cpp" "Dumper.h" "Main.cpp" "Main.h" "Sender.cpp" "Sender.h")

target_link_libraries(testexe PRIVATE nlohmann_json nlohmann_json::nlohmann_json)
target_link_libraries(testexe PRIVATE unofficial::sqlite3::sqlite3)
target_link_libraries(testexe PRIVATE cryptopp-static)





# TODO: Add tests and install targets if needed.
