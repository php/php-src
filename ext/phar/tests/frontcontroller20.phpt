--TEST--
Phar front controller $_SERVER munging failure 3
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller20.php
REQUEST_URI=/frontcontroller20.php/
PATH_INFO=/
--FILE_EXTERNAL--
files/frontcontroller11.phar
--EXPECTF--
Fatal error: Uncaught TypeError: Phar::mungServer(): Argument #1 ($variables) must be an array of strings, array given in %sfrontcontroller20.php:2
Stack trace:
#0 %sfrontcontroller20.php(2): Phar::mungServer(Array)
#1 {main}
  thrown in %sfrontcontroller20.php on line 2
