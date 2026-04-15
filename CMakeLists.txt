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
    src/common/StringUtils.cpp
    src/core/DatabaseManager.cpp
    src/modules/folder/FolderRepository.cpp
    src/modules/folder/FolderService.cpp
    src/modules/request/HttpService.cpp
    src/modules/request/RequestRepository.cpp
    src/modules/request/RequestService.cpp
    src/ui/MainWindowController.cpp
    src/ui/MainWindow.cpp
    src/ui/RequestWorkspaceWidget.cpp
    src/ui/SidebarWidget.cpp
    resources/resources.qrc
)

set(HEADERS
    include/common/StringUtils.h
    include/core/DatabaseManager.h
    include/modules/folder/FolderModel.h
    include/modules/folder/FolderRepository.h
    include/modules/folder/FolderService.h
    include/modules/request/HttpService.h
    include/modules/request/RequestModel.h
    include/modules/request/RequestRepository.h
    include/modules/request/RequestService.h
    include/modules/request/SavedRequestModel.h
    include/ui/MainWindowController.h
    include/ui/MainWindow.h
    include/ui/RequestWorkspaceWidget.h
    include/ui/SidebarWidget.h
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
