

if("${BUILD_ARCH}" MATCHES "^arm")
    include_directories("${CMAKE_SOURCE_DIR}/include")
else()
    add_executable(test-wordexp test-wordexp.c)
endif()

add_executable(test-argv test-argv.c
        ${LIBRARY_FILES}
        ${DEPENDENCY_FILES}
        ${CMAKE_SOURCE_DIR}/src/app/args.c)

add_executable(test-readline test-readline.c)
target_link_libraries(test-readline edit)
# target_link_libraries(test-readline readline)

add_executable(test-editline test-editline.c)
target_link_libraries(test-editline edit)

include_directories(${CMAKE_SOURCE_DIR}/deps/linenoise)

add_executable(test-linenoise-app test-linenoise-app.c ${LINENOISE_FILES} ${LIBRARY_FILES})


add_executable(test-vm-region ${DEPENDENCY_FILES} ${LIBRARY_FILES} test-vm-region.c)
add_executable(test-vm-region-recurse ${DEPENDENCY_FILES} ${LIBRARY_FILES} test-vm-region-recurse.c)

add_executable(test-vm-segments ${DEPENDENCY_FILES} ${LIBRARY_FILES} test-vm-segments.c)

add_executable(test-dyld-info ${DEPENDENCY_FILES} ${LIBRARY_FILES} test-dyld-info.c)
#execute_process(COMMAND sudo chown root: test-dyld-info)
#execute_process(COMMAND sudo chmod u+s test-dyld-info)


add_executable(test-process-list test-process-list.c)
#add_custom_command(TARGET test-process-list POST_LINK COMMAND ldid -Sent.xml test-process-list)
#execute_process(COMMAND ldid -Sent.xml test-process-list)

add_executable(test-string-search ${DEPENDENCY_FILES} ${LIBRARY_FILES} test-string-search.c)

add_executable(test-memory-search ${DEPENDENCY_FILES} ${LIBRARY_FILES} test-memory-search.c)

add_executable(test-memory-dump ${DEPENDENCY_FILES} ${LIBRARY_FILES} test-memory-dump.c)

add_executable(test-memory-write ${DEPENDENCY_FILES} ${LIBRARY_FILES} test-memory-write.c)

add_executable(test-byte2hex ${DEPENDENCY_FILES} ${LIBRARY_FILES} test-byte2hex.c)

add_executable(test-hex2byte ${DEPENDENCY_FILES} ${LIBRARY_FILES} test-hex2byte.c)

add_executable(test-slist test-slist.c)
add_executable(test-list test-list.c)
add_executable(test-tailq test-tailq.c)


if(EXISTS ${CMAKE_SOURCE_DIR}/deps/duktape/duktape.c)
    add_executable(test-duktape-line-scripts test-duktape-line-scripts.c ${DUKTAPE_FILES})
endif()