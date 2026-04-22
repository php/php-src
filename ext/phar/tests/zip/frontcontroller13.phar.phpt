--TEST--
Phar front controller mime type not string/int zip-based
--INI--
default_charset=UTF-8
phar.require_hash=0
--EXTENSIONS--
phar
zlib
--ENV--
SCRIPT_NAME=/frontcontroller13.phar.php
REQUEST_URI=/frontcontroller13.phar.php/a.php
PATH_INFO=/a.php
--FILE_EXTERNAL--
files/frontcontroller7.phar.zip
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECTF--
Fatal error: Uncaught TypeError: Phar::webPhar(): Argument #4 ($mimeTypes) mime type specifier must be of type string|int, null given in %sfrontcontroller13.phar.php:2
Stack trace:
#0 %sfrontcontroller13.phar.php(2): Phar::webPhar('whatever', 'index.php', NULL, Array)
#1 {main}
  thrown in %sfrontcontroller13.phar.php on line 2
