# Header-only libraries
lib_dates = declare_dependency(include_directories: include_directories('dates'))
lib_maths = declare_dependency(include_directories: include_directories('maths'))
lib_nr = declare_dependency(include_directories: include_directories('NR'))
lib_options = declare_dependency(include_directories: include_directories('options'))
lib_stdext = declare_dependency(include_directories: include_directories('stdext'))

# Static library for rates
rates_sources = [
  'rates/accrued.cpp',
  'rates/bond.cpp',
  'rates/cashflow.cpp',
  'rates/deposit.cpp',
  'rates/instrument.cpp',
  'rates/ir_future.cpp',
  'rates/ir_swap_leg_fixed.cpp',
  'rates/ir_swap_leg_floating.cpp',
  'rates/ir_swap_leg.cpp',
  'rates/ir_swap.cpp',
  'rates/value.cpp',
  'rates/yield_curve.cpp'
]

openssl_dep = dependency('openssl')
lib_rates = static_library('rates', rates_sources,
    dependencies: openssl_dep,
    include_directories: include_directories('rates')
)