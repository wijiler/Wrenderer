workspace "Wre2"
    configurations { "Debug", "Release"}
    architecture "x64"

include "Renderer"

include "Test"

newaction {
    trigger     = "clean",
    description = "clean the software",
    execute     = function ()
       print("Cleaning")
       os.rmdir("./Binaries")
       os.remove("./Lib/*.lib")
       os.remove("**.make")
       os.remove("Makefile")
       os.remove("**.vcxproj")
       os.remove("**.vcxproj.filters")
       os.remove("**.vcxproj.user")
       os.remove("**.sln")
       print("done.")
    end
}
