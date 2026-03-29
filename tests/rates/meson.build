# Catch2 header-only dependency
catch2_inc = include_directories('../../external')

# Common include directories
src_inc = include_directories('../../src')

# Tests for rates
test_accrued = executable('test_accrued', 'test_accrued.cpp',
  include_directories: [src_inc, catch2_inc],
  link_with: lib_rates,
  dependencies: [lib_dates, lib_maths, lib_nr, lib_stdext]
)
test('accrued', test_accrued)

test_deposit = executable('test_deposit', 'test_deposit.cpp',
  include_directories: [src_inc, catch2_inc],
  link_with: lib_rates,
  dependencies: [lib_dates, lib_maths, lib_nr, lib_stdext]
)
test('deposit', test_deposit)

test_ir_future = executable('test_ir_future', 'test_ir_future.cpp',
  include_directories: [src_inc, catch2_inc],
  link_with: lib_rates,
  dependencies: [lib_dates, lib_maths, lib_nr, lib_stdext]
)
test('ir_future', test_ir_future)

test_ir_swap_leg_fixed = executable('test_ir_swap_leg_fixed', 'test_ir_swap_leg_fixed.cpp',
  include_directories: [src_inc, catch2_inc],
  link_with: lib_rates,
  dependencies: [lib_dates, lib_maths, lib_nr, lib_stdext]
)
test('ir_swap_leg_fixed', test_ir_swap_leg_fixed)

test_ir_swap_leg_floating = executable('test_ir_swap_leg_floating', 'test_ir_swap_leg_floating.cpp',
  include_directories: [src_inc, catch2_inc],
  link_with: lib_rates,
  dependencies: [lib_dates, lib_maths, lib_nr, lib_stdext]
)
test('ir_swap_leg_floating', test_ir_swap_leg_floating)

test_ir_swap = executable('test_ir_swap', 'test_ir_swap.cpp',
  include_directories: [src_inc, catch2_inc],
  link_with: lib_rates,
  dependencies: [lib_dates, lib_maths, lib_nr, lib_stdext]
)
test('ir_swap', test_ir_swap)

test_value = executable('test_value', 'test_value.cpp',
  include_directories: [src_inc, catch2_inc],
  link_with: lib_rates,
  dependencies: [lib_dates, lib_maths, lib_nr, lib_stdext]
)
test('value', test_value)

test_yield_curve = executable('test_yield_curve', 'test_yield_curve.cpp',
  include_directories: [src_inc, catch2_inc],
  link_with: lib_rates,
  dependencies: [lib_dates, lib_maths, lib_nr, lib_stdext]
)
test('yield_curve', test_yield_curve)
