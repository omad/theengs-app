#!/usr/bin/env bash

echo "> Theengs App packager (macOS x86_64)"

export APP_NAME="Theengs";
export APP_VERSION=1.3;
export GIT_VERSION=$(git rev-parse --short HEAD);

## CHECKS ######################################################################

if [ "$(id -u)" == "0" ]; then
  echo "This script MUST NOT be run as root" 1>&2
  exit 1
fi

#if [ ${PWD##*/} != "Theengs" ]; then
#  echo "This script MUST be run from the Theengs/ directory"
#  exit 1
#fi

## SETTINGS ####################################################################

use_contribs=false
make_install=false
create_package=false
upload_package=false

while [[ $# -gt 0 ]]
do
case $1 in
  -c|--contribs)
  use_contribs=true
  ;;
  -i|--install)
  make_install=true
  ;;
  -p|--package)
  create_package=true
  ;;
  -u|--upload)
  upload_package=true
  ;;
  *)
  echo "> Unknown argument '$1'"
  ;;
esac
shift # skip argument or value
done

## APP INSTALL #################################################################

if [[ $make_install = true ]] ; then
  echo '---- Running make install'
  make INSTALL_ROOT=bin/ install;

  #echo '---- Installation directory content recap:'
  #find bin/;
fi

## DEPLOY ######################################################################

if [[ $use_contribs = true ]] ; then
  export LD_LIBRARY_PATH=$(pwd)/contribs/src/env/macOS_x86_64/usr/lib/;
else
  export LD_LIBRARY_PATH=/usr/local/lib/;
fi

echo '---- Running macdeployqt'
macdeployqt bin/$APP_NAME.app -qmldir=qml/ -hardened-runtime -timestamp -appstore-compliant;

#echo '---- Installation directory content recap:'
#find bin/;

## PACKAGE #####################################################################

if [[ $create_package = true ]] ; then
  echo '---- Compressing package'
  cd bin/;
  zip -r -y -X $APP_NAME-$APP_VERSION-macos.zip $APP_NAME.app;
fi

## UPLOAD ######################################################################

if [[ $upload_package = true ]] ; then
  printf "---- Uploading to transfer.sh"
  curl --upload-file $APP_NAME*.zip https://transfer.sh/$APP_NAME.$APP_VERSION-git$GIT_VERSION-macOS.zip;
  printf "\n"
fi
