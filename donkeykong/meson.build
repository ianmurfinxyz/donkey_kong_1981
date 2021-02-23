project('donkeykong', 'cpp', default_options: ['cpp_std=c++17', 'warning_level=2'])

cc = meson.get_compiler('cpp')

project_dir = meson.current_source_dir()
lib_pixiretro_dir = join_paths(project_dir, 'lib')
lib_pixiretro = cc.find_library('pixiretro', dirs: lib_pixiretro_dir)

donkeykong_inc = include_directories('include')

donkeykong_src = [
  'source/Animation.cpp',
  'source/AnimationFactory.cpp',
  'source/DonkeyKong.cpp',
  'source/GameLevel.cpp',
  'source/GameProp.cpp',
  'source/GamePropFactory.cpp',
  'source/Main.cpp',
  'source/PlayState.cpp',
  'source/Transition.cpp'
]

executable('donkeykong', 
           donkeykong_src, 
           dependencies: [lib_pixiretro],
           include_directories: donkeykong_inc)
