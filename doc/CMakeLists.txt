cmake_minimum_required(VERSION 3.20.0)
project(DAWN_documentation CXX)

include(Doxygen)
Doxygen(Document_DawnApp ../app)
