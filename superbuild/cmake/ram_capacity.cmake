# Sets RAM_SIZE_GB to be the RAM capacity of the current machine
function(get_ram_capacity OUT_VAR total_ram)
  if (APPLE)
    execute_process(COMMAND sysctl -n hw.memsize
                    OUTPUT_VARIABLE MEM_SIZE 
                    OUTPUT_STRIP_TRAILING_WHITESPACE) 
    math(EXPR RAM_SIZE_GB "${MEM_SIZE} / (1024 * 1024 * 1024)")
    set(${OUT_VAR} ${RAM_SIZE_GB} PARENT_SCOPE)

  elseif(LINUX)
    execute_process(COMMAND grep MemTotal /proc/meminfo
                    OUTPUT_VARIABLE MEMTOTAL_LINE
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX MATCH "^MemTotal: +([0-9]+) kB" _ ${MEMTOTAL_LINE})
    math(EXPR RAM_SIZE_GB "${CMAKE_MATCH_1} / (1024 * 1024)")
    set(${OUT_VAR} ${RAM_SIZE_GB} PARENT_SCOPE) 

  elseif(WIN32)
    # TODO: Implement WMIC parsing to calculate RAM in GB
    set(${OUT_VAR} "" PARENT_SCOPE)  # Set empty for now
    
  else()
    message(FATAL_ERROR "Unsupported platform for RAM detection")
    set(${OUT_VAR} "" PARENT_SCOPE)
  endif()
endfunction()
