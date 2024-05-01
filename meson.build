project(
	'gester',
	'cpp',
	version: '0.0',
	default_options : [
		'cpp_std=c++17',
		'optimization=2',
        'werror=true',
        'warning_level=3',
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

executable(
   'gest',
   'gester.cpp',
   dependencies : [gester_dep],
  )