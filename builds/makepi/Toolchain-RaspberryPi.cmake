# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
SET(CMAKE_C_COMPILER /home/francois/x-tools/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER /home/francois/x-tools/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-g++)

# where is the target environment
SET(CMAKE_FIND_ROOT_PATH /home/francois/local/x-tools/arm-unknown-linux-gnueabi)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(CMAKE_CROSSCOMPILE YES)
