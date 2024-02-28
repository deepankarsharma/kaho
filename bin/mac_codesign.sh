#!/bin/bash

codesign --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app/Contents/MacOS/ggml-metal.metal
codesign --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app/Contents/MacOS/server
codesign --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app/Contents/Frameworks/QtCore.framework
codesign --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app/Contents/Frameworks/QtGui.framework
codesign --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app/Contents/Frameworks/QtSvg.framework
codesign --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app/Contents/Frameworks/QtDBus.framework
codesign --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app/Contents/Frameworks/QtWidgets.framework
codesign --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app/Contents/Frameworks/QtNetwork.framework
codesign --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app/Contents/MacOS/kaho
