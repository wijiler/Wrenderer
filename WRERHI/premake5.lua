project "WRERHI"
    language "C"
    targetname "WRERHI"
    location "WRERHI"
    kind "StaticLib"
    outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

    cdialect "C99"

    targetdir("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")
    objdir("%{wks.location}/Binaries/Intermediates/" .. outputdir .. "/%{prj.name}")
    files { "include/**.h", "src/**.c" }
    libdirs { "../libs/", os.getenv("VULKAN_SDK") .. "/Lib/" }
    includedirs { "./include/"}
    includedirs { os.getenv("VULKAN_SDK") .. "/Include" }
    links { "vulkan-1", "glfw3" }
    defines { "WREUSEVULKAN" }
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        optimize "On"
        symbols "On"

    filter "configurations:Dist"
        optimize "On"

    filter "system:windows"
        defines { "VK_USE_PLATFORM_WIN32_KHR" }
        systemversion "latest"

    filter "system:linux"
        defines { "VK_USE_PLATFORM_XLIB_KHR" }

    filter "action:gmake"
    prebuildcommands {

        "mkdir -p" .. " %[%{wks.location}/Binaries/]",
        "mkdir -p" .. " %[%{wks.location}/Binaries/Intermediates/]",

        "mkdir -p" .. " %[%{wks.location}/Binaries/" .. outputdir .. "]",
        "mkdir -p" .. " %[%{wks.location}/Binaries/Intermediates/" .. outputdir .. "]",
    }

    filter "not action:gmake"
        prebuildcommands {

            "{MKDIR}" .. " %[%{wks.location}/Binaries/]",
            "{MKDIR}" .. " %[%{wks.location}/Binaries/Intermediates/]",

            "{MKDIR} %[%{wks.location}/Binaries/" .. outputdir .. "]",
            "{MKDIR} %[%{wks.location}/Binaries/Intermediates/" .. outputdir .. "]",
        }

    filter "action:gmake"
    toolset "clang"
    buildoptions {"-Wextra", "-Wall", "-mavx", "-mavx2"}

    filter "not action:gmake"
    toolset "msc"
    filter""


