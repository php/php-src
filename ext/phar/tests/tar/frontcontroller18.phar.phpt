--TEST--
Phar front controller $_SERVER munging failure tar-based
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller18.phar.php
REQUEST_URI=/frontcontroller18.phar.php/fronk.gronk
PATH_INFO=/fronk.gronk
--FILE_EXTERNAL--
files/frontcontroller9.phar.tar
--EXPECTF--
Fatal error: Uncaught PharException: No values passed to Phar::mungServer(), expecting an array of any of these strings: PHP_SELF, REQUEST_URI, SCRIPT_FILENAME, SCRIPT_NAME in %sfrontcontroller18.phar.php:2
Stack trace:
#0 %sfrontcontroller18.phar.php(2): Phar::mungServer(Array)
#1 {main}
  thrown in %sfrontcontroller18.phar.php on line 2
