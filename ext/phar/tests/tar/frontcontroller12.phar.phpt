--TEST--
Phar front controller mime type unknown int tar-based
--INI--
default_charset=UTF-8
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller12.phar.php
REQUEST_URI=/frontcontroller12.phar.php/a.php
PATH_INFO=/a.php
--FILE_EXTERNAL--
files/frontcontroller6.phar.tar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECTF--
Fatal error: Uncaught PharException: Unknown mime type specifier used, only Phar::PHP, Phar::PHPS and a mime type string are allowed in %sfrontcontroller12.phar.php:2
Stack trace:
#0 %sfrontcontroller12.phar.php(2): Phar::webPhar('whatever', 'index.php', '', Array)
#1 {main}
  thrown in %sfrontcontroller12.phar.php on line 2