workspace "Wrenderer"
configurations { "Debug", "Release"}
project "Wrenderer"
    language "C"
    targetname "Wrenderer"
    architecture "x64"
    kind "StaticLib"
    outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

    toolset "clang"
    cdialect "C99"

    targetdir("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")
    objdir("%{wks.location}/Binaries/Intermediates/" .. outputdir .. "/%{prj.name}")
    files { "**.h", "**.c" }
    libdirs { "./libs/" }
    includedirs { "./include/" }
    includedirs { "./include/libs/" }
    includedirs { os.getenv("VULKAN_SDK") .. "/Include" }
    buildoptions { "-Wextra -Wall" }
    links { "vulkan-1", "glfw3" }
    removefiles { "test/**.**" }
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        sanitize { "Address", "Fuzzer" }
    filter "configurations:Release"
        optimize "On"

    filter "system:windows"
        links { "user32", "msvcrt", "gdi32", "shell32", "libcmt" }
        defines { "VK_USE_PLATFORM_WIN32_KHR" }

    filter "system:linux"
        defines { "VK_USE_PLATFORM_XLIB_KHR" }

    filter "action:gmake"
    prebuildcommands {

        "mkdir -p" .. " %{wks.location}/Binaries/",
        "mkdir -p" .. " %{wks.location}/Binaries/Intermediates/",

        "mkdir -p" .. " %{wks.location}/Binaries/" .. outputdir,
        "mkdir -p" .. " %{wks.location}/Binaries/Intermediates/" .. outputdir,
    }
    filter "not action:gmake"
        prebuildcommands {

            "{MKDIR}" .. " %{wks.location}/Binaries/",
            "{MKDIR}" .. " %{wks.location}/Binaries/Intermediates/",

            "{MKDIR}" .. " %{wks.location}/Binaries/" .. outputdir,
            "{MKDIR}" .. " %{wks.location}/Binaries/Intermediates/" .. outputdir,
        }
    project "WrenTest"
        architecture "x64"
        kind "ConsoleApp"  
        language "C"   

        files { "**.h", "test/**.c" }
        outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

        toolset "clang"
        cdialect "C99"
    
        targetdir("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")
        objdir("%{wks.location}/Binaries/Intermediates/" .. outputdir .. "/%{prj.name}")

        libdirs { "./libs/" }
        includedirs { "./include/" }
        includedirs { "./include/libs/" }
        includedirs { os.getenv("VULKAN_SDK") .. "/Include" }
        links { "Wrenderer" }
        buildoptions { "-Wextra -Wall" }
        links { "vulkan-1", "glfw3" }

        filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        sanitize { "Address", "Fuzzer" }

        filter "configurations:Release"
            optimize "On"

        filter "system:windows"
            links { "user32", "msvcrt", "gdi32", "shell32", "libcmt" }
            defines { "VK_USE_PLATFORM_WIN32_KHR" }

        filter "system:linux"
            defines { "VK_USE_PLATFORM_XLIB_KHR" }
        filter "action:gmake"
            prebuildcommands {

                "mkdir -p" .. " %{wks.location}/Binaries/",
                "mkdir -p" .. " %{wks.location}/Binaries/Intermediates/",

                "mkdir -p" .. " %{wks.location}/Binaries/" .. outputdir,
                "mkdir -p" .. " %{wks.location}/Binaries/Intermediates/" .. outputdir,
            }
            filter "not action:gmake"
                prebuildcommands {

                    "{MKDIR}" .. " %{wks.location}/Binaries/",
                    "{MKDIR}" .. " %{wks.location}/Binaries/Intermediates/",

                    "{MKDIR}" .. " %{wks.location}/Binaries/" .. outputdir,
                    "{MKDIR}" .. " %{wks.location}/Binaries/Intermediates/" .. outputdir,
                }
newaction {
    trigger     = "clean",
    description = "clean the software",
    execute     = function ()
       print("Cleaning")
       os.rmdir("./Binaries")
       os.remove("./Lib/*.lib")
       os.remove("*.make")
       os.remove("Makefile")
       os.remove("*.vcxproj")
       os.remove("*.vcxproj.filters")
       os.remove("*.vcxproj.user")
       os.remove("*.sln")
       print("done.")
    end
}
