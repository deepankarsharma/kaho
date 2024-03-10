#!/bin/bash

clang++ -o kaho_oss -I include public_main.cpp -L kaho_libs -L third_party_libs -lkaho_static -lqmarkdowntextedit \
  -L /opt/kaho/Release/lib -lQt6Core -lQt6BundledFreetype -lQt6Network -lQt6BundledHarfbuzz -lQt6OpenGL \
  -lQt6BundledLibjpeg -lQt6BundledLibjpeg12bits -lQt6BundledLibjpeg16bits -lQt6BundledLibpng -lQt6BundledPcre2 \
  -lQt6BundledZLIB -lQt6Concurrent -lQt6Core -lQt6DBus -lQt6DeviceDiscoverySupport -lQt6ExampleIcons -lQt6FbSupport \
  -lQt6Gui -lQt6OpenGLWidgets -lQt6PrintSupport -lQt6Sql -lQt6Svg -lQt6SvgWidgets -lQt6Test -lQt6Widgets -lQt6Xml \
  -lqmarkdowntextedit -framework AppKit -framework Carbon -framework Foundation -framework ApplicationServices \
  -framework IOKit -framework Metal -framework GSS -framework Security -framework Sparkle \
  -framework QuartzCore -framework Opengl \
  -framework SystemConfiguration -framework CoreVideo -framework IOSurface -F third_party_libs -lresolv \
  /opt/kaho/Release/lib/objects-Release/Widgets_resources_1/.rcc/qrc_qstyle.cpp.o \
  /opt/kaho/Release/lib/objects-Release/Widgets_resources_2/.rcc/qrc_qstyle1.cpp.o \
  /opt/kaho/Release/lib/objects-Release/Widgets_resources_3/.rcc/qrc_qmessagebox.cpp.o \
  /opt/kaho/Release/lib/objects-Release//QCocoaIntegrationPlugin_resources_1/.rcc/qrc_qcocoaresources.cpp.o \
  /opt/kaho/Release/lib/objects-Release//PrintSupport_resources_1/.rcc/qrc_qprintdialog.cpp.o \
  /opt/kaho/Release/lib/objects-Release//ExampleIconsPrivate_resources_1/.rcc/qrc_example_icons.cpp.o \
  /opt/kaho/Release/lib/objects-Release//PrintSupport_resources_2/.rcc/qrc_qprintdialog1.cpp.o \
  /opt/kaho/Release/lib/objects-Release//Gui_resources_1/.rcc/qrc_qpdf.cpp.o \
  /opt/kaho/Release/lib/objects-Release//Gui_resources_2/.rcc/qrc_gui_shaders.cpp.o \
  /opt/kaho/Release/plugins/platforms/objects-Release/QCocoaIntegrationPlugin_init/QCocoaIntegrationPlugin_init.cpp.o \
  -L /opt/kaho/Release/plugins/platforms \
  -lqcocoa \
  -Wl -rpath @loader_path/third_party_libs