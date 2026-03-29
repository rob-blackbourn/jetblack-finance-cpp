# Catch2 header-only dependency
catch2_inc = include_directories('../../external')

# Common include directories
src_inc = include_directories('../../src')

test_arithmetic = executable('test_arithmetic', 'test_arithmetic.cpp',
  include_directories: [src_inc, catch2_inc],
  dependencies: [lib_dates, lib_stdext]
)
test('arithmetic', test_arithmetic)

test_business_days = executable('test_business_days', 'test_business_days.cpp',
  include_directories: [src_inc, catch2_inc],
  dependencies: [lib_dates, lib_stdext]
)
test('business_days', test_business_days)

test_calendars = executable('test_calendars', 'test_calendars.cpp',
  include_directories: [src_inc, catch2_inc],
  dependencies: [lib_dates, lib_stdext]
)
test('calendars', test_calendars)

test_daycount = executable('test_daycount', 'test_daycount.cpp',
  include_directories: [src_inc, catch2_inc],
  dependencies: [lib_dates, lib_stdext]
)
test('daycount', test_daycount)

test_schedules = executable('test_schedules', 'test_schedules.cpp',
  include_directories: [src_inc, catch2_inc],
  dependencies: [lib_dates, lib_stdext]
)
test('schedules', test_schedules)

test_tenors = executable('test_tenors', 'test_tenors.cpp',
  include_directories: [src_inc, catch2_inc],
  dependencies: [lib_dates, lib_stdext]
)
test('tenors', test_tenors)
