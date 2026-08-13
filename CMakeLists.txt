cmake_minimum_required (VERSION 3.20)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("Solitaire" VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_library (utfcpp INTERFACE)

target_include_directories (utfcpp 
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/external/utfcpp
)

add_library (portable_file_dialogs INTERFACE)

target_include_directories (portable_file_dialogs 
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/external/portablefiledialogs
)

add_library (stbimage INTERFACE)

target_include_directories (stbimage 
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/external/stbimage
)

add_library(imgui
    ${IMGUI_DIR}/imgui.cpp
	${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui
    PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)

target_link_libraries(imgui
    PUBLIC
        glfw
        OpenGL::GL
)

add_executable (Solitaire
	${SRC_DIR}/Deck.cpp
	${SRC_DIR}/SolitaireAlgorithm.cpp
	${SRC_DIR}/main.cpp
	${SRC_DIR}/MessageTransformation.cpp 
	${SRC_DIR}/FileManager.cpp
	${SRC_DIR}/ImageHeader.cpp
	${SRC_DIR}/SolitaireUI.cpp
	${SRC_DIR}/stb_image_impl.cpp
)

target_include_directories (Solitaire
	PRIVATE
		${IMGUI_DIR}/backends
)

target_link_libraries(Solitaire 
	PRIVATE 
		utfcpp
		portable_file_dialogs
		imgui
		stbimage
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Solitaire PROPERTY CXX_STANDARD 20)
endif()

install(TARGETS Solitaire
    RUNTIME DESTINATION .
)

install(FILES
    "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/bin/glfw3.dll"
    DESTINATION .
)
