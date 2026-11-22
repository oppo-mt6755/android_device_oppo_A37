#!/bin/bash
set -eu
cd "$(dirname "$0")/../../../.."
export ANDROID_BUILD_TOP="$PWD"
export OUT_DIR="${OUT_DIR:-out-sony-restart}"
case "$OUT_DIR" in
    /*) export OUT="$OUT_DIR/target/product/A37" ;;
    *) export OUT="$PWD/$OUT_DIR/target/product/A37" ;;
esac
export LINEAGE_BUILD=A37 TARGET_PRODUCT=lineage_A37
export TARGET_BUILD_VARIANT="${TARGET_BUILD_VARIANT:-userdebug}" TARGET_BUILD_TYPE=release
export JAVA_HOME="$PWD/tools/a37-host/jdk8u512-b09"
export PATH="$PWD/tools/a37-host/bin:$JAVA_HOME/bin:$PATH"
export M4="$PWD/tools/a37-host/bin/m4"
export LD_LIBRARY_PATH="$PWD/tools/a37-host/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export LC_ALL=C
export ANDROID_JACK_VM_ARGS="-Dfile.encoding=UTF-8 -XX:+TieredCompilation -Xmx10g -Djava.security.properties=$PWD/device/oppo/A37/tools/jack-java.security"
exec build/soong/soong_ui.bash --make-mode -j"${JOBS:-8}" "$@"
