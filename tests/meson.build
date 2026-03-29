# Catch2 header-only dependency
catch2_inc = include_directories('../external')

# Common include directories
src_inc = include_directories('../src')

# Tests for dates
test_arithmetic = executable('test_arithmetic', 'dates/test_arithmetic.cpp',
  include_directories: [src_inc, catch2_inc],
  dependencies: [lib_dates, lib_stdext]
)
test('arithmetic', test_arithmetic)

test_business_days = executable('test_business_days', 'dates/test_business_days.cpp',
  include_directories: [src_inc, catch2_inc],
  dependencies: [lib_dates, lib_stdext]
)
test('business_days', test_business_days)

test_calendars = executable('test_calendars', 'dates/test_calendars.cpp',
  include_directories: [src_inc, catch2_inc],
  dependencies: [lib_dates, lib_stdext]
)
test('calendars', test_calendars)

test_daycount = executable('test_daycount', 'dates/test_daycount.cpp',
  include_directories: [src_inc, catch2_inc],
  dependencies: [lib_dates, lib_stdext]
)
test('daycount', test_daycount)

test_schedules = executable('test_schedules', 'dates/test_schedules.cpp',
  include_directories: [src_inc, catch2_inc],
  dependencies: [lib_dates, lib_stdext]
)
test('schedules', test_schedules)

test_tenors = executable('test_tenors', 'dates/test_tenors.cpp',
  include_directories: [src_inc, catch2_inc],
  dependencies: [lib_dates, lib_stdext]
)
test('tenors', test_tenors)

# Tests for rates
test_accrued = executable('test_accrued', 'rates/test_accrued.cpp',
  include_directories: [src_inc, catch2_inc],
  link_with: lib_rates,
  dependencies: [lib_dates, lib_maths, lib_nr, lib_stdext]
)
test('accrued', test_accrued)

test_deposit = executable('test_deposit', 'rates/test_deposit.cpp',
  include_directories: [src_inc, catch2_inc],
  link_with: lib_rates,
  dependencies: [lib_dates, lib_maths, lib_nr, lib_stdext]
)
test('deposit', test_deposit)

test_ir_future = executable('test_ir_future', 'rates/test_ir_future.cpp',
  include_directories: [src_inc, catch2_inc],
  link_with: lib_rates,
  dependencies: [lib_dates, lib_maths, lib_nr, lib_stdext]
)
test('ir_future', test_ir_future)

test_ir_swap_leg_fixed = executable('test_ir_swap_leg_fixed', 'rates/test_ir_swap_leg_fixed.cpp',
  include_directories: [src_inc, catch2_inc],
  link_with: lib_rates,
  dependencies: [lib_dates, lib_maths, lib_nr, lib_stdext]
)
test('ir_swap_leg_fixed', test_ir_swap_leg_fixed)

test_ir_swap_leg_floating = executable('test_ir_swap_leg_floating', 'rates/test_ir_swap_leg_floating.cpp',
  include_directories: [src_inc, catch2_inc],
  link_with: lib_rates,
  dependencies: [lib_dates, lib_maths, lib_nr, lib_stdext]
)
test('ir_swap_leg_floating', test_ir_swap_leg_floating)

test_ir_swap = executable('test_ir_swap', 'rates/test_ir_swap.cpp',
  include_directories: [src_inc, catch2_inc],
  link_with: lib_rates,
  dependencies: [lib_dates, lib_maths, lib_nr, lib_stdext]
)
test('ir_swap', test_ir_swap)

test_value = executable('test_value', 'rates/test_value.cpp',
  include_directories: [src_inc, catch2_inc],
  link_with: lib_rates,
  dependencies: [lib_dates, lib_maths, lib_nr, lib_stdext]
)
test('value', test_value)

test_yield_curve = executable('test_yield_curve', 'rates/test_yield_curve.cpp',
  include_directories: [src_inc, catch2_inc],
  link_with: lib_rates,
  dependencies: [lib_dates, lib_maths, lib_nr, lib_stdext]
)
test('yield_curve', test_yield_curve)

# Tests for stdext
test_match = executable('test_match', 'stdext/test_match.cpp',
  include_directories: [src_inc, catch2_inc],
  dependencies: [lib_stdext]
)
test('match', test_match)

test_namespace = executable('test_namespace', 'stdext/test_namespace.cpp',
  include_directories: [src_inc, catch2_inc],
  dependencies: [lib_stdext]
)
test('namespace', test_namespace)