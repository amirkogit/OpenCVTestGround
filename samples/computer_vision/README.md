### computer_vision:
This project demonstrates how to use Qt's Plugin Architecture to build structured computer vision applications using plugins.

## mainapp:
This is the main application where plugins will be loaded and it consists of other functionalities like changing the theme of the application, internationalization etc.

## <PluginName>_plugin:
There are several folders that follows the above naming convention. These are individual plugins that can be developed independently and deployed as a dynamic library. After building these plugins, it should be copied in the folder where the final executable application is located.

## General steps for building new plugins:
1. Copy the folder template_plugin.
2. Rename to the name of plugins that we need to develop. For example filter_plugin.
3. Rename .h, .cpp and other files for all instances where the word 'template' occurs with the name of the plugin that we are developing.
4. Modify existing files or add new files as needed.
5. Build the project.
6. From the 'release' folder copy the .dll or .so file to the location where the executable for the mainapp is located. For example: copy Filter_Plugin.dll file to \mainapp\release\Filter_Plugin.dll

All plugins are deployed as a shared dynamic link libraries. In order to support static linking, Qt and OpenCV needs to be built as Static Libraries. This can be configured in CMake during while building these libraries from source.
