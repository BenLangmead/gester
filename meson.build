project(
	'gester',
	'cpp',
	version: '0.0',
	default_options : [
		'cpp_std=c++17',
		'optimization=2',
        'warning_level=3',
		'werror=false',
	],
)

digest = subproject('digest')
digest_dep = digest.get_variable('digest_dep')
include_dirs = [digest.get_variable('include_dirs')]

install_headers(
	'gester.hpp'
)

gester_dep = declare_dependency(
	include_directories: include_dirs,
	dependencies: digest_dep,
)

thread_dep = dependency('threads')


executable(
   'gest',
   'gester.cpp',
   dependencies : [thread_dep, gester_dep],
  )
