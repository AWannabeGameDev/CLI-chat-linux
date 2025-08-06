workspace "CLI-chat"
    configurations {"debug", "release"}

    filter "configurations:debug"
        defines {"DEBUG"}
        symbols "On"
        optimize "Off"

    filter "configurations:release"
        defines {"NDEBUG"}
        symbols "Off"
        optimize "On"

project "util"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    targetname "util_%{cfg.buildcfg}"
    targetdir "util/lib"

    includedirs {"util/include"}
    objdir "util/interms"

    files {"util/src/**.cpp"}

project "server"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetname "server_%{cfg.buildcfg}"
    targetdir "server/bin"

    includedirs {"util/include"}
    objdir "server/interms"

    dependson {"util"}
    links {"util"}

    files {"server/src/**.cpp"}

project "client"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetname "client_%{cfg.buildcfg}"
    targetdir "client/bin"

    includedirs {"util/include"}
    objdir "client/interms"

    dependson {"util"}
    links {"util"}

    files {"client/src/**.cpp"}