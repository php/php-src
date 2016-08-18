--TEST--
Phar front controller mime type not string/int zip-based
--INI--
default_charset=UTF-8
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller13.phar.php
REQUEST_URI=/frontcontroller13.phar.php/a.php
PATH_INFO=/a.php
--FILE_EXTERNAL--
files/frontcontroller7.phar.zip
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECTF--
Fatal error: Uncaught PharException: Unknown mime type specifier used (not a string or int), only Phar::PHP, Phar::PHPS and a mime type string are allowed in %sfrontcontroller13.phar.php:2
Stack trace:
#0 %sfrontcontroller13.phar.php(2): Phar::webPhar('whatever', 'index.php', '', Array)
#1 {main}
  thrown in %sfrontcontroller13.phar.php on line 2