cmake_minimum_required(VERSION 3.7)
project(cast-nano-rpi-client)
add_subdirectory(deps/libdatachannel)
if(POLICY CMP0079)
	cmake_policy(SET CMP0079 NEW)
endif()

set(CLIENT_SOURCES
	main.cpp
	parse_cl.cpp
	parse_cl.h
)

set(GETOPT_SOURCES
	getopt.cpp
	getopt.h
)

if(WIN32)
	add_executable(media-receiver-client ${CLIENT_SOURCES} ${GETOPT_SOURCES})
	target_compile_definitions(media-receiver-client PUBLIC STATIC_GETOPT)
else()
	add_executable(media-receiver-client ${CLIENT_SOURCES})
endif()

set_target_properties(media-receiver-client PROPERTIES
	CXX_STANDARD 17
	OUTPUT_NAME media-receiver-client)

set_target_properties(media-receiver-client PROPERTIES
	XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER com.github.paullouisageneau.libdatachannel.examples.client.benchmark)

find_package(Threads REQUIRED)
target_link_libraries(media-receiver-client LibDataChannel::LibDataChannel Threads::Threads nlohmann_json::nlohmann_json)

if(MSVC)
	add_custom_command(TARGET media-receiver-client POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"$<TARGET_FILE_DIR:datachannel>/datachannel.dll"
		$<TARGET_FILE_DIR:media-receiver-client>
	)
endif()