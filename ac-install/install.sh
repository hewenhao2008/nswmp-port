#!/bin/sh

tar -xvf goahead-3.3.5-src.tgz
cd goahead-3.3.5
make
make install
echo "/usr/local/lib/goahead/latest/bin" >> /etc/ld.so.conf.d/ns_ac.conf
ldconfig


cd -
tar -xvf libevent-2.0.21-stable.tar.gz
cd libevent-2.0.21-stable
./configure
make 
make install
echo "/usr/local/lib" >> /etc/ld.so.conf.d/ns_ac.conf
ldconfig

cd -

mv natshell /usr/local
chmod a+x /usr/local/natshell/ac/bin/ac
chmod a+x /usr/local/natshell/ap/ap
chmod a+x /usr/local/natshell/ap/acaddr
