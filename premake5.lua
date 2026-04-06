workspace "cv"
    architecture "ARM64"
    configurations { "Debug", "Release" }
    startproject "cv"

project "cv"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++23"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin-int/%{cfg.buildcfg}"

    buildoptions { "-std=c++23" }

    -- Source files
    files {
        "src/**.cpp",
        "src/headers/**.hpp"
    }
    includedirs {
        "src/headers"
    }
    pchheader "src/headers/pch.hpp"
    pchsource "src/controller/pch.cpp"
    excludes { "src/headers/*.gch" }
    -- Platform-specific settings
    filter "system:macosx"
        libdirs { "/opt/homebrew/Cellar/sfml/3.0.1/lib" }
        links {
            "sfml-graphics",
            "sfml-window",
            "sfml-system",
            "OpenGL.framework"
        }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter {}
