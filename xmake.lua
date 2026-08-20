includes("lib/commonlibf4")
add_requires("nlohmann_json")

set_project("ShieldFramework")
set_version("1.0.0")
set_license("MIT")
set_languages("c++23")
set_warnings("allextra")

add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")
add_defines("COMMONLIB_RUNTIMECOUNT=3")

target("ShieldFramework")
	add_packages("nlohmann_json")
    add_rules("commonlibf4.plugin", {
        name = "ShieldFramework",
        author = "jarari",
        description = "Physical shield framework for Fallout 4",
        plugin_template = path.join(os.projectdir(), "res/commonlibf4-plugin.cpp.in"),
    })
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/PCH.h")
