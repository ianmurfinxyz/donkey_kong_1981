project('donkeykong', 'cpp', default_options: ['cpp_std=c++17', 'warning_level=2'])

cc = meson.get_compiler('cpp')

project_dir = meson.current_source_dir()
lib_pixiretro_dir = join_paths(project_dir, 'pixiretro/bin')
lib_pixiretro = cc.find_library('pixiretro', dirs: lib_pixiretro_dir)

donkeykong_inc = include_directories('pixiretro/include')

donkeykong_src = [
  'source/game.cpp'
]

executable('donkeykong', 
           donkeykong_src, 
           dependencies: [lib_pixiretro],
           include_directories: donkeykong_inc)
