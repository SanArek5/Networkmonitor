# CMakeList.txt: projekt CMake dla elementu Network monitor, dołącz źródło i zdefiniuj
# tutaj logikę specyficzną dla projektu.
#
cmake_minimum_required (VERSION 3.10)

# Włącz przeładowywanie kodu XAML na gorąco dla kompilatorów MSVC, jeśli jest to obsługiwane.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Network monitor")

# Dodaj źródło do pliku wykonywalnego tego projektu.
add_executable (CMakeTarget "Network monitor.cpp" "Network monitor.h")
target_link_libraries(CMakeTarget PRIVATE iphlpapi ws2_32)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CMakeTarget PROPERTY CXX_STANDARD 20)
endif()

# TODO: Dodaj testy i zainstaluj elementy docelowe w razie potrzeby.
