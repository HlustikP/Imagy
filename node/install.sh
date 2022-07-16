#!/usr/bin/env sh
npm i
node-gyp clean
node-gyp configure
node-gyp build
