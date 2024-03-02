#!/bin/bash

#codesign --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app

codesign --deep --force --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app

#codesign --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app/Contents/MacOS/ggml-metal.metal
#codesign --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app/Contents/MacOS/server
#codesign --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app/Contents/MacOS/kaho
#codesign --deep --force --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app/Contents/Frameworks/Sparkle.framework
