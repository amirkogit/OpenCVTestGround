### Plugin_Basics
Contains example codes from Chapter 3 of the book: Computer Vision with OpenCV3 + Qt

The example demonstrates how to create a Plugin using Qt framework.

The main code for plugin is in the folder 'median_filter_plugin'
The folder 'Plugin_User' contains the client code how this plugin is used.

## Usage:
1. Build the project 'median_filter_plugin'
2. Build the project 'Plugin_User'

The file generated in Step 1 (median_filter_plugin.dll) should be copied into debug or release folder under the sub-folder 'filter_plugins' because Qt will search under this folder for all plugins.
