@echo off
rem Directory to boost root
set boost_dir=boost-1.78.0

rem Clonning boost repository
echo git clone --progress --origin github -v "https://github.com/boostorg/boost.git" "%boost_dir%"
echo git checkout "%boost_dir%"
echo git submodule update --init libs/algorithm
echo git submodule update --init libs/align
echo git submodule update --init libs/any
echo git submodule update --init libs/array
echo git submodule update --init libs/assert
echo git submodule update --init libs/atomic
echo git submodule update --init libs/bind
echo git submodule update --init libs/chrono
echo git submodule update --init libs/core
echo git submodule update --init libs/concept_check
echo git submodule update --init libs/config
echo git submodule update --init libs/container
echo git submodule update --init libs/container_hash
echo git submodule update --init libs/crc
echo git submodule update --init libs/date_time
echo git submodule update --init libs/detail
echo git submodule update --init libs/exception
echo git submodule update --init libs/filesystem
echo git submodule update --init libs/foreach
echo git submodule update --init libs/format
echo git submodule update --init libs/function
echo git submodule update --init libs/headers
echo git submodule update --init libs/integer
echo git submodule update --init libs/intrusive
echo git submodule update --init libs/io
echo git submodule update --init libs/iterator
echo git submodule update --init libs/lexical_cast
echo git submodule update --init libs/math
echo git submodule update --init libs/move
echo git submodule update --init libs/mpl
echo git submodule update --init libs/multi_index
echo git submodule update --init libs/numeric
echo git submodule update --init libs/optional
echo git submodule update --init libs/predef
echo git submodule update --init libs/preprocessor
echo git submodule update --init libs/program_options
echo git submodule update --init libs/property_tree
echo git submodule update --init libs/range
echo git submodule update --init libs/ratio
echo git submodule update --init libs/regex
echo git submodule update --init libs/serialization
echo git submodule update --init libs/smart_ptr
echo git submodule update --init libs/spirit
echo git submodule update --init libs/static_assert
echo git submodule update --init libs/system
@rem echo git submodule update --init libs/thread
echo git submodule update --init libs/throw_exception
echo git submodule update --init libs/tokenizer
echo git submodule update --init libs/tuple
echo git submodule update --init libs/type_index
echo git submodule update --init libs/type_traits
echo git submodule update --init libs/utility
echo git submodule update --init libs/uuid
echo git submodule update --init libs/winapi
echo git submodule update --init tools/*
pause
if not exist "%~dp0\%boost_dir%\tools\build\bootstrap.bat" @(@echo Error: boost build submodule not updated!&@pause&@exit /b 1)

rem What toolset to use when building boost.
call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
rem call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"
set INCLUDE="%VCToolsInstallDir%include";%INCLUDE%
set PATHi386="%VCToolsInstallDir%bin\HostX86\x86";%PATH%
set LIBi386="%VCToolsInstallDir%lib\x86";%LIB%
set PATHx86_64="%VCToolsInstallDir%bin\HostX86\x64";%PATH%
set LIBx86_64="%VCToolsInstallDir%lib\x64";%LIB%

@rem https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B#Internal_version_numbering
rem Visual Studio 2012 -> set msvcver=msvc-11.0
rem Visual Studio 2013 -> set msvcver=msvc-12.0
rem Visual Studio 2015 -> set msvcver=msvc-14.0
rem Visual Studio 2017 -> set msvcver=msvc-14.1
rem Visual Studio 2019 -> set msvcver=msvc-14.2
rem Visual Studio 2022 -> set msvcver=msvc-14.3
set msvcver=msvc-14.3

rem Number of cores to use when building boost
set cores=%NUMBER_OF_PROCESSORS%

rem Start building boost
echo Building %boost_dir% with %cores% cores using toolset %msvcver%.
%~d0
cd "%~dp0\%boost_dir%"
call bootstrap.bat

rem Most libraries can be static libs
set PATH=%PATHi386%
set LIB=%LIBi386%
b2 -j%cores% toolset=%msvcver% address-model=32 architecture=x86 link=static threading=multi runtime-link=shared --build-type=minimal stage --stagedir=stage/i386
b2 -j%cores% toolset=%msvcver% address-model=32 architecture=x86 link=shared threading=multi runtime-link=shared --build-type=minimal stage --stagedir=stage/i386
set PATH=%PATHx86_64%
set LIB=%LIBx86_64%
b2 -j%cores% toolset=%msvcver% address-model=64 architecture=x86 link=static threading=multi runtime-link=shared --build-type=minimal stage --stagedir=stage/x86_64
b2 -j%cores% toolset=%msvcver% address-model=64 architecture=x86 link=shared threading=multi runtime-link=shared --build-type=minimal stage --stagedir=stage/x86_64

rem Removing intermediate files generated while building boost
rd /S /Q bin.v2

pause