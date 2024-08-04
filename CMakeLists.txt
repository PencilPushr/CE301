# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("i80386")

if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Od")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Od")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Od")
endif()

#compile 3rdParty               _______________________

#set variables
set(IMGUI_INCLUDES thirdparty/imgui/ thirdparty/imgui/backends)
set(ZYDIS_INCLUDE thirdparty/zydis)

#D3D11 + WIN32
set(BACKENDS_DIR thirdparty/imgui/backends)
file(GLOB DX_FILES 
        ${BACKENDS_DIR}/imgui_impl_dx11.cpp
        ${BACKENDS_DIR}/imgui/imgui_impl_win32.cpp
)

# Register Zydis
option(ZYDIS_BUILD_TOOLS "" OFF)
option(ZYDIS_BUILD_EXAMPLES "" OFF)

include_directories(${IMGUI_INCLUDES} src)

#note: we only need to include the imgui cmakelist unlike the glfw
add_subdirectory(${IMGUI_INCLUDES})
add_subdirectory(${ZYDIS_INCLUDE})

#end compile 3rdParty           _______________________

#compile my files
file(GLOB source_files CONFIGURE_DEPENDS
        "*.h"
        "*.cpp"
            "src/*.cpp"
            "src/*.h"
                "src/cpu/*.h"
                "src/cpu/*.cpp"
                "src/dx11/*.h"
                "src/dx11/*.cpp"
                "src/imgooey/*.h"
                "src/imgooey/*.cpp"
                "src/mem/*.h"
                "src/mem/*.cpp"
                "src/PlatformWin/*.h"
                "src/PlatformWin/*.cpp"
            # Do not need to include thirdparty, the above code in include_dir and add_sub includes the imgui, dx11 and win32 files.
            "util/*.h"
            "util/*.cpp"
        )

add_executable(${PROJECT_NAME} ${source_files})

#target_link_libraries(BadTwitter ${OPENGL_LIBRARIES} OpenGL::GL glfw imgui)
#find_package(D3DX11_LIBRARY)
#find_library(D3DX11_LIBRARY d3d11.lib "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22000.0\um\x64")
#find_file(D3D11 d3d11.lib "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22000.0\um\x64")
target_link_libraries(${PROJECT_NAME} ${D3D11} d3d11.lib d3d11 dxguid imgui Zydis)

 set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()


# Include sub-projects.
#add_subdirectory ("i80386")
