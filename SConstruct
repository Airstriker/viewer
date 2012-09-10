import os

env = Environment(ENV=os.environ,
                  CXX="g++-snapshot",
                  CXXFLAGS= [ "-O3", "-g3",
                              "-std=c++11",
                              # "-ansi",
                              # "-pedantic",
                              "-Wall",
                              "-Wextra",
                              "-Wnon-virtual-dtor",
                              "-Weffc++",
                              # "-Wconversion",
                              "-Wold-style-cast",
                              "-Werror",
                              "-Wshadow",
                              "-Wcast-qual",
                              "-Winit-self", # only works with >= -O1
                              "-Wno-unused-parameter"])
env.Append(LIBS=["glut"])
env.ParseConfig("sdl-config --libs --cflags | sed 's/-I/-isystem/'")
env.ParseConfig("pkg-config --libs --cflags  gl glu glew | sed 's/-I/-isystem/'")
env.Program("viewer", Glob("src/*.cpp"))

# EOF #
