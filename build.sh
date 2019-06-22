#!/usr/bin/env bash
set -e
THISDIR="$(dirname -- "$(readlink -f -- "$0")")"
cd "$THISDIR"


#
# SETUP
#

QT_DIR="/opt/qtdev/dist"
export PATH="$QT_DIR/bin:$PATH"
export LD_LIBRARY_PATH="$QT_DIR/lib:$LD_LIBRARY_PATH"
export PYTHONDONTWRITEBYTECODE=1
export QML_DISABLE_DISK_CACHE=1
TMP_DIR="/tmp/build/gpu-fan-meister"
BUILD_DIR="$TMP_DIR/build"
DIST_DIR="$TMP_DIR/dist"
NPROC="$(nproc)"
QT_PLUGINS_DIR="qt.plugins"
QBS_PROFILE="qtopt"
PLUGINS=(
    nvidia-nvml
    test-manual
)
NO_REBUILD=
SIGN_KEY="062299952077C171"



#
# CLEAN
#

if [[ -z $NO_REBUILD ]]
then
    rm -rf "$BUILD_DIR" "$DIST_DIR"
fi

#
# DOCS
#

if [[ -z $NO_REBUILD ]]
then
    "$THISDIR/docs/src/build.sh"
fi

#
# PREPARE QBS PROFILES
#

if [[ -z $NO_REBUILD ]]
then
    qbs-setup-toolchains --detect
    qbs-setup-qt "$QT_DIR/bin/qmake" "$QBS_PROFILE"
fi

#
# BUILD
#

if [[ -z $NO_REBUILD ]]
then
    qbs build \
        -d "$BUILD_DIR/gpu-fan-meister" \
        -f "$THISDIR/gpu-fan-meister.qbs" \
        --jobs "$NPROC" \
        qbs.defaultBuildVariant:release \
        qbs.installRoot:"$DIST_DIR" \
        profile:"$QBS_PROFILE"
    for PLUGIN in "${PLUGINS[@]}"
    do
        qbs build \
            -d "$BUILD_DIR/gpufanmeister-$PLUGIN" \
            -f "$THISDIR/plugins/$PLUGIN/gpufanmeister-$PLUGIN.qbs" \
            --jobs "$NPROC" \
            qbs.defaultBuildVariant:release \
            qbs.installRoot:"$DIST_DIR" \
            profile:"$QBS_PROFILE"
    done
fi


#
# QT LIBS
#

mkdir -p "$DIST_DIR/lib"
cp -a \
    "$QT_DIR"/lib/libQt5Core.so* \
    "$QT_DIR"/lib/libQt5Charts.so* \
    "$QT_DIR"/lib/libQt5DBus.so* \
    "$QT_DIR"/lib/libQt5Gui.so* \
    "$QT_DIR"/lib/libQt5Network.so* \
    "$QT_DIR"/lib/libQt5Qml.so* \
    "$QT_DIR"/lib/libQt5Quick.so* \
    "$QT_DIR"/lib/libQt5QuickControls2.so* \
    "$QT_DIR"/lib/libQt5QuickTemplates2.so* \
    "$QT_DIR"/lib/libQt5Widgets.so* \
    "$QT_DIR"/lib/libQt5XcbQpa.so* \
    "$QT_DIR"/lib/libKF5SyntaxHighlighting.so* \
        "$DIST_DIR/lib/"

mkdir -p "$DIST_DIR/$QT_PLUGINS_DIR/platforms"
cp -a "$QT_DIR"/plugins/platforms/libqxcb.so "$DIST_DIR/$QT_PLUGINS_DIR/platforms/"

mkdir -p "$DIST_DIR/$QT_PLUGINS_DIR/platforminputcontexts"
cp -a "$QT_DIR"/plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so "$DIST_DIR/$QT_PLUGINS_DIR/platforminputcontexts/"
cp -a "$QT_DIR"/plugins/platforminputcontexts/libibusplatforminputcontextplugin.so "$DIST_DIR/$QT_PLUGINS_DIR/platforminputcontexts/"

mkdir -p "$DIST_DIR/$QT_PLUGINS_DIR/xcbglintegrations"
cp -a "$QT_DIR"/plugins/xcbglintegrations/libqxcb-glx-integration.so "$DIST_DIR/$QT_PLUGINS_DIR/xcbglintegrations/"



#
# QT CONF
#

cat > "$DIST_DIR/qt.conf" << EOF
[Paths]
Plugins = $QT_PLUGINS_DIR
EOF


#
# QML FILES
#

QML_FILES="$(QML2_IMPORT_PATH="$QT_DIR/qml" strace -f "$DIST_DIR/gpu-fan-meister.py" --load-qml 2>&1 | grep -Po '(openat|access)\(.*'"$QT_DIR/qml"'/\K[^"]+(?=.*=\s[^\-])')"
IFS=$'\n'; QML_FILES=($QML_FILES); unset IFS;
for f in "${QML_FILES[@]}"
do
    if [[ -f $QT_DIR/qml/$f ]]
    then
        DIRNAME="$(dirname -- "$f")"
        mkdir -p "$DIST_DIR/qml/$DIRNAME"
        cp -a "$QT_DIR/qml/$f" "$DIST_DIR/qml/$DIRNAME"
    fi
done


#
# APPIMAGE LINKS
#

ln -sf "gpu-fan-meister.py" "$DIST_DIR/AppRun"
ln -sf "assets/net.alkatrazstudio.gpufanmeister.desktop" "$DIST_DIR/"
ln -sf "assets/gpu-fan-meister.png" "$DIST_DIR/"
mkdir -p "$DIST_DIR/usr/share/metainfo/"
ln -sf "../../../assets/net.alkatrazstudio.gpufanmeister.appdata.xml" "$DIST_DIR/usr/share/metainfo/"
mkdir -p "$DIST_DIR/usr/share/applications/"
ln -sf "../../../assets/net.alkatrazstudio.gpufanmeister.desktop" "$DIST_DIR/usr/share/applications/"


#
# BUILD APPIMAGE
#

appimagetool "$DIST_DIR" --sign --sign-key "$SIGN_KEY"
