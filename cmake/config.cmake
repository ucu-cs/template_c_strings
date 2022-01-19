# Only one of Memory(MSAN), Address(ASAN), or Thread(TSan) sanitizers is applicable at the time
set(ENABLE_UBSan ON)

set(ENABLE_ASAN ON)
set(ENABLE_TSan ON)
set(ENABLE_MSAN OFF)

include(cmake/defaults/CompilerWarnings.cmake)
include(cmake/defaults/Errors.cmake)
include(cmake/defaults/PVSStudio.cmake)
include(cmake/defaults/Sanitizers.cmake)
include(cmake/defaults/Windows.cmake)
