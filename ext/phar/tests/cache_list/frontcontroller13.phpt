--TEST--
Phar front controller mime type not string/int [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/frontcontroller13.php
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller13.php
REQUEST_URI=/frontcontroller13.php/a.php
PATH_INFO=/a.php
--FILE_EXTERNAL--
files/frontcontroller7.phar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECTF--
Fatal error: Uncaught exception 'PharException' with message 'Unknown mime type specifier used (not a string or int), only Phar::PHP, Phar::PHPS and a mime type string are allowed' in %sfrontcontroller13.php:2
Stack trace:
#0 %sfrontcontroller13.php(2): Phar::webPhar('whatever', 'index.php', '', Array)
#1 {main}
  thrown in %sfrontcontroller13.php on line 2