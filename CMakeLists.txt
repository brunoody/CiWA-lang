# CMakeList.txt: projeto de CMake para CiWA-lang, incluir origem e definir
# lógica específica a um projeto aqui.
#
cmake_minimum_required (VERSION 3.8)

project ("CiWA-lang")

# Adicione a origem ao executável deste projeto.
add_executable (CiWA-lang "CiWA-lang.cpp" "CiWA-lang.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CiWA-lang PROPERTY CXX_STANDARD 20)
endif()

# TODO: Adicione testes e instale destinos, se necessário.
