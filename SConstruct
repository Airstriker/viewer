import os

env = Environment(ENV=os.environ,
                  CXX="g++-snapshot",
                  CXXFLAGS= [ "-O0", "-g3",
                              "-std=c++11",
                              # "-ansi",
                              # "-pedantic",
                              "-Wno-unused-variable",
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
env.Append(LIBS=["glut", "SDL_image", "cwiid"])
env.ParseConfig("sdl-config --libs --cflags | sed 's/-I/-isystem/'")
env.ParseConfig("pkg-config --libs --cflags  gl glu glew | sed 's/-I/-isystem/'")
env.ParseConfig("pkg-config --libs --cflags cairomm-1.0 gl glu | sed 's/-I/-isystem/'")
env.Program("viewer", Glob("src/*.cpp"))

# EOF #
