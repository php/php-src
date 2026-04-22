--TEST--
Phar front controller $_SERVER munging failure 2 tar-based
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller19.phar.php
REQUEST_URI=/frontcontroller19.phar.php/
PATH_INFO=/
--FILE_EXTERNAL--
files/frontcontroller10.phar.tar
--EXPECTF--
Fatal error: Uncaught ValueError: Phar::mungServer(): Argument #1 ($variables) must have at most 4 elements in %sfrontcontroller19.phar.php:2
Stack trace:
#0 %sfrontcontroller19.phar.php(2): Phar::mungServer(Array)
#1 {main}
  thrown in %sfrontcontroller19.phar.php on line 2
