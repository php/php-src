####################
 Visual Studio Code
####################

.. note::

   These instructions have been tested on Linux. macOS should mostly work the same. For Windows,
   ymmv.

An IDE can make navigating large code bases tremendously easier. Visual Studio Code is a popular and
free IDE that is well-suited for C development. It contains syntax highlighting, navigation,
auto-completion and a debugger. Check the `official website <https://code.visualstudio.com/>`__ for
installation instructions.

.. note::

   The ``settings.json`` file referenced below can be opened in the Settings page by pressing the
   "Open Settings (JSON)" button in the top right corner. Most of these settings can also be
   adjusted through the GUI.

*****************
 C/C++ extension
*****************

The `C/C++ extension`_ provides most of the features we'll need for php-src development. You can
find it in the extensions marketplace. You will also need ``gcc`` or ``clang`` installed. The
extension will mostly work out of the box, but it is advisable to use the ``compile_commands.json``
file. It contains a list of all compiled files, along with the commands used to compile them. It
provides the extension with the necessary information about include paths and other compiler flags.

.. _c/c++ extension: https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools

To generate the ``compile_commands.json`` file, you can use the compiledb_ tool. Install it using
``pip``, and then prefix your ``make`` command accordingly:

.. _compiledb: https://github.com/nickdiego/compiledb

.. code:: bash

   # Install compiledb
   pip install compiledb
   # Compile php-src and generate compile_commands.json
   compiledb make -j8

To tell the C/C++ extension to use the ``compile_commands.json`` file, add the following to your
``settings.json`` file:

.. code:: json

   {
       "C_Cpp.default.compileCommands": "${workspaceFolder}/compile_commands.json"
   }

********
 clangd
********

The C/C++ extension usually works well enough. Some people find that ``clangd`` works better.
``clangd`` is a language server built on top of the ``clang`` compiler. It only provides navigation
and code completion but no syntax highlighting and no debugger. As such, it should be used in
conjunction with the C/C++ extension. For the two extensions not to clash, add the following to your
``settings.json`` file:

.. code:: json

   {
       "C_Cpp.intelliSenseEngine": "disabled"
   }

Follow the `official installation instructions for clangd
<https://clangd.llvm.org/installation.html>`__, and then install the `clangd extension`_.
Alternatively, you can let the extension install ``clangd`` for you. ``clangd`` requires a
``compile_commands.json`` file, so make sure to follow the instructions from the previous section.
By default, ``clangd`` will auto-include header files on completion. php-src headers are somewhat
peculiar, so you might want to disable this option in your ``settings.json`` file:

.. _clangd extension: https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd

.. code:: json

   {
       "clangd.arguments": [
           "-header-insertion=never"
       ]
   }

*****
 gdb
*****

The C/C++ extension provides the ability to use Visual Studio Code as a frontend for ``gdb``. Of
course, you will need ``gdb`` installed on your system, and php-src must be compiled with the
``--enable-debug`` configure flag. Copy the following into your projects ``.vscode/launch.json``
file:

.. code:: json

   {
       "version": "0.2.0",
       "configurations": [
           {
               "name": "(gdb) Launch",
               "type": "cppdbg",
               "request": "launch",
               "program": "${workspaceFolder}/sapi/cli/php",
               "args": [
                   // Any options you want to test with
                   // "-dopcache.enable_cli=1",
                   "${relativeFile}",
               ],
               "stopAtEntry": false,
               "cwd": "${workspaceFolder}",
               // Useful if you build with --enable-address-sanitizer
               "environment": [
                   { "name": "USE_ZEND_ALLOC", "value": "0" },
                   { "name": "USE_TRACKED_ALLOC", "value": "1" },
                   { "name": "LSAN_OPTIONS", "value": "detect_leaks=0" },
               ],
               "externalConsole": false,
               "MIMode": "gdb",
               "setupCommands": [
                   { "text": "source ${workspaceFolder}/.gdbinit" },
               ]
           }
       ]
   }

Set any breakpoint in your C code, open a ``php`` (or ``phpt``) file and start debugging from the
"Run and Debug" tab in the sidebar.

..
   _todo: lldb should work mostly the same, I believe. It's available by default on macOS, and as such might be more convenient.
