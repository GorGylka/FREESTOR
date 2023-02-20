cmake_minimum_required(VERSION 3.14)

project(
	curl_test
	VERSION 0.1.0
	DESCRIPTION "PSP cURL test"
	LANGUAGES CXX
)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# set CFlags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__GLIBC_USE\(...\)=0")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (PLATFORM_PSP)

	set(BUILD_PRX 1)

	set(DEFAULT_STUBS
			pspdebug 
			pspdisplay 
			pspge 
			pspctrl 
			pspnet 
			pspnet_apctl
    )

	set(LIBS
		stdc++
		psphttp
		pspgum
		pspgu
		curl
	)

	target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS} ${DEFAULT_STUBS})

	create_pbp_file(
		TARGET ${PROJECT_NAME}
		ICON_PATH NULL
		BACKGROUND_PATH NULL
		PREVIEW_PATH NULL
		TITLE ${PROJECT_DESCRIPTION}
	)

	add_custom_target( 
		RUN_PSP
		DEPENDS ${PROJECT_NAME}
		COMMAND PPSSPPSDL ./EBOOT.PBP
	)

else()

	message(FATAL_ERROR "This project can only be built for the PSP")

endif()
