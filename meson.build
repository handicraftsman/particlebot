project('particlebot', ['cpp'], default_options : ['cpp_std=gnu++17'])

cxx = meson.get_compiler('cpp')

deps = [
  dependency('threads'),
  dependency('guosh'),
  dependency('jsoncpp'),
  dependency('sqlite3'),
  dependency('lua51'),
  cxx.find_library('dl')
]

r = run_command('bash', meson.current_source_dir() + '/branch.sh')
conf_data = configuration_data()
conf_data.set('version', r.stdout().strip())
configure_file(input: 'config.hpp.in', output: 'config.hpp', configuration: conf_data)

lib = shared_library(
  'particlebot',
  [
    'lib/EventMachine.cpp',
    'lib/Event.cpp',
    'lib/User.cpp',
    'lib/PluginManager.cpp',
    'lib/Plugin.cpp',
    'lib/CPPPlugin.cpp',
    'lib/LuaPlugin.cpp',
    'lib/CommandInfo.cpp',
    'lib/Bot.cpp',
    'lib/IRCSocket.cpp'
  ],
  dependencies: deps,
  include_directories: include_directories(['.']),
  cpp_pch: 'pch/libs.hpp',
  cpp_args: ['-DPB_BIN']
)

executable(
  'particlebot',
  [
    'main.cpp'
  ],
  dependencies: deps,
  link_with: [ lib ],
  include_directories: include_directories(['.']),
  cpp_pch: 'pch/libs.hpp',
  cpp_args: ['-DPB_BIN']
)

shared_library(
  'pb-core',
  [
    'core/main.cpp',
    'core/debug.cpp',
    'core/help.cpp',
    'core/admin.cpp'
  ],
  dependencies: deps,
  link_with: [ lib ],
  include_directories: include_directories(['.']),
  cpp_pch: 'pch/libs.hpp',
  cpp_args: ['-DPB_PLUGIN']
)

shared_library(
  'pb-chanop',
  [
    'chanop/main.cpp',
  ],
  dependencies: deps,
  link_with: [ lib ],
  include_directories: include_directories(['.']),
  cpp_pch: 'pch/libs.hpp',
  cpp_args: ['-DPB_PLUGIN']
)

shared_library(
  'pb-fun',
  [
    'fun/main.cpp',
  ],
  dependencies: deps,
  link_with: [ lib ],
  include_directories: include_directories(['.']),
  cpp_pch: 'pch/libs.hpp',
  cpp_args: ['-DPB_PLUGIN']
)
