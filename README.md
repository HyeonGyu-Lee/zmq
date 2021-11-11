># 1. ZeroMQ (zmqpp)
>## 1.1 Install and Build of ZeroMQ for cpp
>~~~
>sudo apt update
>sudo apt upgrade
>sudo apt install libzmq3-dev
>~~~
>http://github.com/zeromq/zmqpp
>~~~
>git clone git://github.com/jedisct1/libsodium.git
>cd libsodium
>./autogen.sh 
>./configure && make check 
>sudo make install 
>sudo ldconfig
>cd ../
># Build, check, and install the latest version of ZeroMQ
>git clone git://github.com/zeromq/libzmq.git
>cd libzmq
>./autogen.sh 
>./configure --with-libsodium && make
>sudo make install
>sudo ldconfig
>cd ../
># Now install ZMQPP
>git clone git://github.com/zeromq/zmqpp.git
>cd zmqpp
>make
>make check
>sudo make install
>make installcheck
>~~~
>## 1.2 Environment setup
>-Setup the path
>~~~
>sudo cp -R /usr/local/lib/* /usr/lib
>~~~
