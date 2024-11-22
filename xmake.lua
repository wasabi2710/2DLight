add_rules("mode.debug", "mode.release")

add_requires("libsdl >2.30")

target("2dlight")
set_kind("binary")
add_files("src/*.c")
add_packages("libsdl")
