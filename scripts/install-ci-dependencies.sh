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

install_sundials () {
    wget http://computation.llnl.gov/projects/sundials/download/sundials-$1.$2.$3.tar.gz -P ${TRAVIS_BUILD_DIR}/tmp
    cd ${TRAVIS_BUILD_DIR}/tmp
    tar xzf sundials-$1.$2.$3.tar.gz
    cd sundials-$1.$2.$3
    mkdir build
    cd build
    if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
        cmake .. -DCMAKE_INSTALL_PREFIX=${TRAVIS_BUILD_DIR}/dependencies/sundials -DCMAKE_BUILD_TYPE=Release -DBUILD_CVODES=OFF -DBUILD_IDAS=OFF -DBUILD_SHARED_LIBS=OFF -DEXAMPLES_ENABLE=OFF -DEXAMPLES_INSTALL=OFF -DKLU_ENABLE=ON -DOPENMP_ENABLE=ON -DKLU_INCLUDE_DIR=/usr/include/suitesparse -DKLU_LIBRARY_DIR=/usr/lib/x86_64-linux-gnu
    elif [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
        cmake .. -DCMAKE_INSTALL_PREFIX=${TRAVIS_BUILD_DIR}/dependencies/sundials -DCMAKE_BUILD_TYPE=Release -DBUILD_CVODES=OFF -DBUILD_IDAS=OFF -DBUILD_SHARED_LIBS=OFF -DEXAMPLES_ENABLE=OFF -DEXAMPLES_INSTALL=OFF -DKLU_ENABLE=ON -DOPENMP_ENABLE=ON -DKLU_INCLUDE_DIR=/usr/include/suitesparse -DKLU_LIBRARY_DIR=/usr/lib/x86_64-linux-gnu
    fi
    make
    make install
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
export BOOST_ROOT=${TRAVIS_BUILD_DIR}/dependencies/boost}

# Install Sundials
if [[ ! -d "dependencies/sundials}" ]]; then
    install_sundials 2 7 0
fi
export SUNDIALS_ROOT=${TRAVIS_BUILD_DIR}/dependencies/sundials}

# Update library search paths
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    sudo ldconfig ${PWD}/dependencies
    export LD_LIBRARY_PATH=${PWD}/dependencies/boost/lib:$LD_LIBRARY_PATH
elif [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    export DYLD_FALLBACK_LIBRARY_PATH=${PWD}/dependencies/boost/lib:$LD_LIBRARY_PATH
fi
