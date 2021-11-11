># 1. ZeroMQ (cppzmq)
>## 1.1 Install and Build of ZeroMQ for cpp
>~~~
>sudo apt update
>sudo apt upgrade
>sudo apt purge libzmq3-dev
>sudo apt purge cmake
>~~~
>http://github.com/zeromq/cppzmq
>~~~
># Upgrade cmake (CMake 3.10 or higher is required)
>wget https://github.com/Kitware/CMake/archive/master.zip
>unzip master.zip
>rm master.zip
>cd CMake-master
>./bootstrap
>sudo make -j8
>sudo make install
>sudo ldconfig
>cmake --version
># restart terminal
>
>wget https://download.libsodium.org/libsodium/releases/libsodium-1.0.18-stable.tar.gz
>tar -xvf libsodium-*
>rm *libsodium-1.0.18-stable.tar.gz
>cd libsodium-stable
>./configure
>sudo make clean
>sudo make -j8
>sudo make install 
>sudo ldconfig
>cd ../
># Build, check, and install the latest version of ZeroMQ
>wget https://github.com/zeromq/libzmq/archive/master.zip
>unzip master.zip
>rm master.zip
>cd libzmq-master
>./autogen.sh 
>./configure --with-libsodium
>mkdir build
>cd build && cmake .. -DENABLE_DRAFTS=ON
>sudo make -j8 install
>sudo ldconfig
>cd ../
># Now install CPPZMQ
>wget https://github.com/zeromq/cppzmq/archive/master.zip
>unzip master.zip
>rm master.zip
>cd cppzmq-master
>mkdir build
>cd build && cmake .. -DENABLE_DRAFTS=ON
>sudo make -j8 install
>sudo ldconfig
>~~~
>## 1.2 Environment setup
>-Setup the path
>~~~
>sudo cp -R /usr/local/lib/* /usr/lib
>~~~
