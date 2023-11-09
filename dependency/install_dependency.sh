#!/bin/bash

GIT_REPO=$1
DEPENDENCY_DIR=$2
INSTALL_DIR=$3
BRANCH=$4
NEED_INSTALL=$5
TYPE=$6
EXTRA=$7
echo "${GIT_REPO} $DEPENDENCY_DIR $INSTALL_DIR $BRANCH $NEED_INSTALL"
echo "Removing cloned dir ${DEPENDENCY_DIR}"
rm -rf ${DEPENDENCY_DIR}
echo "cloning ${GIT_REPO}"
git clone ${GIT_REPO} ${DEPENDENCY_DIR}
pushd ${DEPENDENCY_DIR}
if [[ "$TYPE" = "branch" ]];
then
  git checkout ${BRANCH}
else
  git checkout tags/${BRANCH} -b ${BRANCH}
fi
git pull
rm -rf build
if [ $NEED_INSTALL = 1 ];
then
  echo "Installing ${DEPENDENCY_DIR}"
  mkdir build
  echo cmake . -B build -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DCMAKE_PREFIX_PATH=${INSTALL_DIR} $EXTRA
  cmake . -B build -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DCMAKE_PREFIX_PATH=${INSTALL_DIR} $EXTRA
  cmake --build build -- -j
  cmake --install build
  popd
  echo "Clean source dir ${DEPENDENCY_DIR}"
  rm -rf ${DEPENDENCY_DIR}
else
  echo "Not installing ${DEPENDENCY_DIR}"
fi

