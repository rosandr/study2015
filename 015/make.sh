#/bin/bash

SRVTAGET=tcpsrv
CNLTARGET=tcpcnl

cd srv
make
#if [ -x $SRVTARGET ] ; then 
#    mv $SRVTARGET ../
#fi

# echo `uname -a`
#`mv $(SRVTARGET) ../ `

cd ../cln
make
cd ..