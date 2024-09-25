workspace "Wrenderer"
configurations { "Debug", "Release" }
project "Wrenderer"
    kind "ConsoleApp"
    language "C"
    targetname "main"
    architecture "x64"
    outputdir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

    toolset "clang"
    cdialect "C99"

    buildcommands {
        "{MKDIR} -p" .. "%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}",
        "{MKDIR} -p" .. "%{wks.location}/Binaries/Intermediates/" .. outputdir .. "/%{prj.name}"
    }


    targetdir("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")
    objdir("%{wks.location}/Binaries/Intermediates/" .. outputdir .. "/%{prj.name}")
    files { "**.h", "**.c" }
    libdirs { "./libs/" }
    includedirs { "./include/" }
    includedirs { "./include/libs/" }
    includedirs { os.getenv("VULKAN_SDK") .. "/Include" }
    buildoptions { "-Wextra -Wall" }
    links { "vulkan-1", "glfw3" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
    filter "configurations:Release"
        optimize "On"

    filter "system:windows"
        links { "user32", "msvcrt", "gdi32", "shell32", "libcmt" }
        defines { "VK_USE_PLATFORM_WIN32_KHR" }
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
       os.remove("*.sln")
       print("done.")
    end
}
