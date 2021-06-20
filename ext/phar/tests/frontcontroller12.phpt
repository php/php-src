--TEST--
Phar front controller mime type unknown int
--INI--
default_charset=UTF-8
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller12.php
REQUEST_URI=/frontcontroller12.php/a.php
PATH_INFO=/a.php
--FILE_EXTERNAL--
files/frontcontroller6.phar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECTF--
Fatal error: Uncaught PharException: Unknown mime type specifier used, only Phar::PHP, Phar::PHPS and a mime type string are allowed in %sfrontcontroller12.php:2
Stack trace:
#0 %sfrontcontroller12.php(2): Phar::webPhar('whatever', 'index.php', NULL, Array)
#1 {main}
  thrown in %sfrontcontroller12.php on line 2
