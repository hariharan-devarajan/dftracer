#!/bin/bash

GIT_REPO=$1
DEPENDENCY_DIR=$2
INSTALL_DIR=$3
BRANCH=$4
NEED_INSTALL=$5
echo "${GIT_REPO} $DEPENDENCY_DIR $INSTALL_DIR $BRANCH $NEED_INSTALL"
echo "Removing cloned dir ${DEPENDENCY_DIR}"
rm -rf ${DEPENDENCY_DIR}
echo "cloning ${GIT_REPO}"
git clone ${GIT_REPO} ${DEPENDENCY_DIR}
pushd ${DEPENDENCY_DIR}
git checkout ${BRANCH}
git pull
rm -rf build
if [ $NEED_INSTALL = 1 ];
then
  echo "Installing ${DEPENDENCY_DIR}"
  mkdir build
  cmake . -B build -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DCMAKE_PREFIX_PATH=${INSTALL_DIR}
  cmake --build build -- -j
  cmake --install build
  popd
  echo "Clean source dir ${DEPENDENCY_DIR}"
  rm -rf ${DEPENDENCY_DIR}
else
  echo "Not installing ${DEPENDENCY_DIR}"
fi

