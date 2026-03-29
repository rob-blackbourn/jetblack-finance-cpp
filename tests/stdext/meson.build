# Catch2 header-only dependency
catch2_inc = include_directories('../../external')

# Common include directories
src_inc = include_directories('../../src')

# Tests for stdext
test_match = executable('test_match', 'test_match.cpp',
  include_directories: [src_inc, catch2_inc],
  dependencies: [lib_stdext]
)
test('match', test_match)

test_namespace = executable('test_namespace', 'test_namespace.cpp',
  include_directories: [src_inc, catch2_inc],
  dependencies: [lib_stdext]
)
test('namespace', test_namespace)