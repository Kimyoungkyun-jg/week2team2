workspace "week2team2"
    architecture "x64"
    configurations { "Debug", "Release" }
    location "build"

project "Project1"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    location "build"

    targetdir ("bin/" .. "%{cfg.buildcfg}")
    objdir ("bin-int/" .. "%{cfg.buildcfg}")

    files {
        "Project1/**.h",
        "Project1/**.hpp",
        "Project1/**.cpp"
    }

    includedirs {
        "Project1"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"