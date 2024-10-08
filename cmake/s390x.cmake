if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(SystemZ|s390x)")
  message(STATUS "Forcing characters to be signed, as on x86_64.")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsigned-char")
  message(STATUS "Cannot build WAMR on this platform, so WASM support is disabled.")
  set(FLB_WASM OFF)
  if(FLB_LUAJIT)
    if(NOT FLB_PREFER_SYSTEM_LIB_LUAJIT)
      message(WARNING "LuaJIT is disabled, this platform does not support built-in LuaJIT. To use lua filter on this platform, install LuaJIT as system lib and build again with both FLB_LUAJIT and FLB_PREFER_SYSTEM_LIB_LUAJIT on.")
      set(FLB_LUAJIT OFF)
    endif()
  endif()
endif ()
