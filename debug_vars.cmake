get_cmake_property(_normal_vars VARIABLES)
list(SORT _normal_vars)               # make the output deterministic

foreach(_var IN LISTS _normal_vars)
  # `${${_var}}` expands to the value of the variable whose name is stored
  # in `_var`.  If the variable is unset, this expands to an empty string,
  # which is fine for debugging.
  message(STATUS "${_var}=${${_var}}")
endforeach()

get_cmake_property(_cache_vars CACHE)
list(SORT _cache_vars)

foreach(_var IN LISTS _cache_vars)
  # Cache values are accessed via `get_property()`; no output redirection!
  get_property(_value CACHE ${_var} PROPERTY VALUE)
  message(STATUS "CACHE ${_var}=${_value}")
endforeach()
