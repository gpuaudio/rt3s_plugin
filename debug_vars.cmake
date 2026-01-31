get_cmake_property(_normal_vars VARIABLES)
list(SORT _normal_vars) # make the output deterministic

foreach(_var IN LISTS _normal_vars)
  message(STATUS "${_var}=${${_var}}")
endforeach()

get_cmake_property(_cache_vars CACHE)
list(SORT _cache_vars)

foreach(_var IN LISTS _cache_vars)
  get_property(_value CACHE ${_var} PROPERTY VALUE)
  message(STATUS "CACHE ${_var}=${_value}")
endforeach()
