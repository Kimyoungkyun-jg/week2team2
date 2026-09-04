workspace "week2team2"
    architecture "x64"
    configurations { "Debug", "Release" }
    location "build"

project "Project1"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    location "build"

    targetdir ("bin/" .. "%{cfg.buildcfg}")
    objdir ("bin-int/" .. "%{cfg.buildcfg}")

    files {
        "Project1/**.h",
        "Project1/**.hpp",
        "Project1/**.cpp",
        "Project1/**.hlsl"
    }

    filter "files:Project1/**.hlsl"
        buildaction "None"

    filter {}

    includedirs {
        "Project1"
    }

    postbuildcommands {
        "{COPYFILE} %{wks.location}/../Project1/ShaderW0.hlsl %{cfg.targetdir}/ShaderW0.hlsl"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"