# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.22.1)

project("myapplication")

# Creates your game shared library. The name must be the same as the
# one used for loading in your Kotlin/Java or AndroidManifest.txt files.
add_library(myapplication SHARED
        main.cpp
        AndroidOut.cpp
        AndroidHelper.cpp
        VulkanRenderer.cpp)

# Import native_app_glue for NativeActivity support
add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

target_include_directories(native_app_glue PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue)

# Export ANativeActivity_onCreate for NativeActivity
set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

# Configure libraries CMake uses to link your target library.
target_link_libraries(myapplication
        # Native app glue for NativeActivity
        native_app_glue

        # Vulkan
        vulkan

        # Android libraries
        jnigraphics
        android
        log)

# Include directories for headers
target_include_directories(myapplication PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/stb-master
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/tiny_obj_loader
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm-1.0.3
        ${ANDROID_NDK}/sources/android/native_app_glue)
