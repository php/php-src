Rules for building FBSQL
------------------------

The fbsql project contains 2 configurations.

To build this extension you must first download and
install FrontBase. The default instalation path would
be c:\usr\FrontBase. If you install it in another location 
you need to change the include path in the project before 
compiling.

Start Visual Studio, load php_modules.dsw, select the fbsql projects,
configuration and build it.

Finaly copy php_fbsql.dll to your extension directory and enable it
by adding the following line tp php.ini

extension=php_fbsql.dll

or by calling dl() in each script

dl("php_fbsql.dll");