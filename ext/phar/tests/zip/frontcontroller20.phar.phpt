--TEST--
Phar front controller $_SERVER munging failure 3 zip-based
--INI--
phar.require_hash=0
--EXTENSIONS--
phar
zlib
--ENV--
SCRIPT_NAME=/frontcontroller20.phar.php
REQUEST_URI=/frontcontroller20.phar.php/
PATH_INFO=/
--FILE_EXTERNAL--
files/frontcontroller11.phar.zip
--EXPECTF--
Fatal error: Uncaught TypeError: Phar::mungServer(): Argument #1 ($variables) must be an array of strings, array given in %sfrontcontroller20.phar.php:2
Stack trace:
#0 %sfrontcontroller20.phar.php(2): Phar::mungServer(Array)
#1 {main}
  thrown in %sfrontcontroller20.phar.php on line 2
