cmake_minimum_required(VERSION 3.16)
project(connect_6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set Qt6 installation path
set(Qt6_DIR "/opt/homebrew/Cellar/qt/6.9.0/lib/cmake/Qt6")
set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qt/6.9.0")

# Find Qt packages
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# iOS specific settings
if(IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0")
    set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "YOUR_TEAM_ID") # Replace with your team ID
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")
    set(CMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2") # 1=iPhone, 2=iPad
    set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "14.0")
    
    # Enable automatic provisioning
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Automatic")
    set(CMAKE_XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.yourdomain.connect6") # Replace with your bundle ID
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/gui/MainWindow.cpp
    src/gui/GameBoardWidget.cpp
    src/gui/MainMenuWidget.cpp
    src/core/GameBoard.cpp
    src/ai/GameAI.cpp
)

# Header files
set(HEADERS
    include/gui/MainWindow.h
    include/gui/GameBoardWidget.h
    include/gui/MainMenuWidget.h
    include/core/GameBoard.h
    include/core/Player.h
    include/core/GameState.h
    include/ai/GameAI.h
)

# iOS specific source files
if(IOS)
    set(SOURCES ${SOURCES}
        src/gui/GameBoardWidgetIOS.mm
    )
    set(HEADERS ${HEADERS}
        include/gui/GameBoardWidgetIOS.h
    )

    # Create iOS icons
    set(ICON_SIZES "40" "60" "58" "87" "80" "120" "180")
    set(ICON_FILES "")
    
    foreach(SIZE ${ICON_SIZES})
        set(ICON_FILE "${CMAKE_BINARY_DIR}/ios/Assets.xcassets/AppIcon.appiconset/Icon-${SIZE}.png")
        list(APPEND ICON_FILES ${ICON_FILE})
        
        # Create icon directory if it doesn't exist
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/ios/Assets.xcassets/AppIcon.appiconset")
        
        # Generate icon using sips (macOS command-line tool)
        add_custom_command(
            OUTPUT ${ICON_FILE}
            COMMAND sips -z ${SIZE} ${SIZE} ${CMAKE_SOURCE_DIR}/resources/app_icon.png --out ${ICON_FILE}
            DEPENDS ${CMAKE_SOURCE_DIR}/resources/app_icon.png
        )
    endforeach()
    
    # Add icons to the build
    add_custom_target(app_icons DEPENDS ${ICON_FILES})
endif()

# Create executable
if(IOS)
    add_executable(connect_6 MACOSX_BUNDLE ${SOURCES} ${HEADERS})
    add_dependencies(connect_6 app_icons)
else()
    add_executable(connect_6 ${SOURCES} ${HEADERS})
endif()

# Include directories
target_include_directories(connect_6 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link Qt libraries
target_link_libraries(connect_6 PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# iOS specific settings for the app bundle
if(IOS)
    set_target_properties(connect_6 PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourdomain.connect6"
        MACOSX_BUNDLE_BUNDLE_NAME "Connect 6"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/ios/Info.plist.in"
    )
endif() 