cmake_minimum_required(VERSION 3.16)

project(PostmanApp
    VERSION 0.1.0
    DESCRIPTION "A Qt6 desktop API client"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 auto-tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Network Sql Widgets)
qt_standard_project_setup()

# --- Sources ---
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/services/HttpService.cpp
    src/services/RequestRepository.cpp
    resources/resources.qrc
)

set(HEADERS
    include/MainWindow.h
    include/models/Folder.h
    include/models/Request.h
    include/models/SavedRequest.h
    include/services/HttpService.h
    include/services/RequestRepository.h
)

# --- Target ---
qt_add_executable(postman-app ${SOURCES} ${HEADERS})

target_include_directories(postman-app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(postman-app PRIVATE
    Qt6::Network
    Qt6::Sql
    Qt6::Widgets
)

# Install rules (useful for packaging later)
install(TARGETS postman-app
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
