export XPL_OBJECT_TYPE="LINUX-X86"
export XPL_PROJECT_DIR=$(pwd)
export XPL_FEATURE_DIR=$XPL_PROJECT_DIR/feature
export XPL_PROTOCOL_DIR=$XPL_PROJECT_DIR/protocol
export CCACHE_PATH=/usr/bin:$CCACHE_PATH
export CC="gcc"
export AR="gcc-ar"
export CXX="g++"
export CPP="gcc"
export AS="as"
export GDB=gdb
export STRIP=strip
export RANLIB=ranlib
export OBJCOPY=objcopy
export OBJDUMP=objdump
. /opt/fsl-qoriq/2.0/environment-setup-aarch64-fsl-linux


