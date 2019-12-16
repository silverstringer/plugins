#!/bin/bash
# ---------------------------------------------------------------------
# Plugins startup script.
# ---------------------------------------------------------------------
clear


#COLOR
BLACK='\033[0;30m'
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
GRAY='\033[0;37m'

BOLD='\033[1m'      
DBOLD='\033[2m'    
NBOLD='\033[22m'   
UNDERLINE='\033[4m'    
NUNDERLINE='\033[4m'     
BLINK='\033[5m'      
NBLINK='\033[5m'       
INVERSE='\033[7m'    
NINVERSE='\033[7m'     
BREAK='\033[m'       
NORMAL='\033[0m'


#Help
if [ -n "$1" ]
      then
      if [[ $1 = "-h" ]];then
      echo -e "${GREEN} Help:"
      echo -e "${NORMAL}" "-package, create package ";
      echo -e  "-h, help";
      exit
fi
fi



echo -e "${BOLD} \t Plugin help script"
echo -e "${GREEN} \t Install requered package: ${NORMAL}"
echo "-----------------------------------------------"
apt-get install doxygen
apt-get install graphviz
apt-get install libboost-all-dev

# Build project
 
cd library
env -i git submodule update --init --recursive
cd ..
mkdir build
cd build
cmake ..
make -j4


if [ -n "$1" ]
	then
      if [[ $1 = "-package" ]]; then
	echo -e "${GREEN} \t CREATE_PACKAGE: ${NORMAL}"
	cpack
	fi
fi

echo "-----------------------------------------------"
echo -e "${GREEN}${BOLD}Done ${NORMAL}"

