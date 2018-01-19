#!/bin/bash

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    HOMEBREW_NO_AUTO_UPDATE=1 brew install pcre
fi

if [[ ! -f "dependencies" ]]; then
    mkdir -p dependencies;
fi

install_boost () {
    wget -O boost_$1_$2_$3.tar.gz http://sourceforge.net/projects/boost/files/boost/$1.$2.$3/boost_$1_$2_$3.tar.gz/download && tar xzf boost_$1_$2_$3.tar.gz
    (
        cd boost_$1_$2_$3/;
        ./bootstrap.sh --with-libraries=date_time,filesystem,program_options,system,test;
        ./b2 link=shared threading=multi variant=release > /dev/null;
        ./b2 install --prefix=../dependencies/boost > /dev/null;
    )
}

install_cmake_linux () {
    if [[ ! -f "cmake-$1.$2.$3-Linux-x86_64/bin/cmake" ]]; then
        echo "*** install cmake"
        wget --no-check-certificate http://cmake.org/files/v$1.$2/cmake-$1.$2.$3-Linux-x86_64.tar.gz && tar -xzf cmake-$1.$2.$3-Linux-x86_64.tar.gz;
    fi
    export PATH="${PWD}/cmake-$1.$2.$3-Linux-x86_64/bin:${PATH}"
}

install_cmake_osx () {
    if [[ ! -f "cmake-$1.$2.$3-Darwin-x86_64/CMake.app/Contents/bin/cmake" ]]; then
        echo "*** install cmake"
        wget --no-check-certificate http://cmake.org/files/v$1.$2/cmake-$1.$2.$3-Darwin-x86_64.tar.gz && tar -xzf cmake-$1.$2.$3-Darwin-x86_64.tar.gz;
    fi
    export PATH="${PWD}/cmake-$1.$2.$3-Darwin-x86_64/CMake.app/Contents/bin:${PATH}"
}

# Install CMake
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    install_cmake_linux 3 4 3
    echo "*** cmake installed ($PATH)"
elif [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    install_cmake_osx 3 4 3
    echo "*** cmake installed ($PATH)"
fi

# Install Boost
if [[ ! -d "dependencies/boost" ]]; then
    echo "*** build boost"
    install_boost 1 61 0
    echo "*** built boost successfully"
fi

# Update library search paths
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    sudo ldconfig ${PWD}/dependencies
    export LD_LIBRARY_PATH=${PWD}/dependencies/boost/lib:$LD_LIBRARY_PATH
elif [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    export DYLD_FALLBACK_LIBRARY_PATH=${PWD}/dependencies/boost/lib:$LD_LIBRARY_PATH
fi
